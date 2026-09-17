/*
  BBC micro:bit V2 -> NimBLE accelerometer streamer
  Target: h2zero n-able Arduino core + NimBLE-Arduino 2.x

  BLE service: Nordic UART Service (NUS)
    RX: host -> micro:bit
    TX: micro:bit -> host notifications

  Stream format (ASCII, newline terminated):
    A,<x_mg>,<y_mg>,<z_mg>\n

  Commands:
    I       information
    P       ping
    S,1     streaming on
    S,0     streaming off
    R,<hz>  set stream rate, 10..100 Hz

  The firmware auto-detects the two motion sensors allowed by the
  micro:bit V2 hardware design:
    ST LSM303AGR at 0x19
    NXP FXOS8700CQ at 0x1F
*/

#include <Arduino.h>
#include <Wire.h>
#include <NimBLEDevice.h>

static const char* DEVICE_NAME = "MB2-AI-TIMER";
static const char* NUS_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* NUS_RX_UUID      = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* NUS_TX_UUID      = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

static NimBLEServer* server = nullptr;
static NimBLECharacteristic* txChr = nullptr;
static NimBLECharacteristic* rxChr = nullptr;

enum SensorType : uint8_t { SENSOR_NONE = 0, SENSOR_LSM303AGR, SENSOR_FXOS8700 };
static SensorType sensorType = SENSOR_NONE;
static volatile bool streaming = true;
static volatile uint16_t streamHz = 50;

static bool i2cWrite8(uint8_t address, uint8_t reg, uint8_t value) {
  Wire1.beginTransmission(address); Wire1.write(reg); Wire1.write(value);
  return Wire1.endTransmission() == 0;
}

static bool i2cRead(uint8_t address, uint8_t reg, uint8_t* dst, uint8_t len) {
  Wire1.beginTransmission(address); Wire1.write(reg);
  if (Wire1.endTransmission(false) != 0) return false;
  uint8_t got = Wire1.requestFrom(address, len);
  if (got != len) { while (Wire1.available()) (void)Wire1.read(); return false; }
  for (uint8_t i = 0; i < len; ++i) dst[i] = Wire1.read();
  return true;
}

static bool i2cRead8(uint8_t address, uint8_t reg, uint8_t& value) { return i2cRead(address, reg, &value, 1); }

static const char* sensorName() {
  switch (sensorType) {
    case SENSOR_LSM303AGR: return "LSM303AGR";
    case SENSOR_FXOS8700: return "FXOS8700";
    default: return "NO_IMU";
  }
}

static bool initLSM303AGR() {
  constexpr uint8_t ADDR = 0x19; uint8_t who = 0;
  if (!i2cRead8(ADDR, 0x0F, who) || who != 0x33) return false;
  if (!i2cWrite8(ADDR, 0x20, 0x57)) return false;
  if (!i2cWrite8(ADDR, 0x23, 0x88)) return false;
  delay(10); sensorType = SENSOR_LSM303AGR; return true;
}

static bool initFXOS8700() {
  constexpr uint8_t ADDR = 0x1F; uint8_t who = 0;
  if (!i2cRead8(ADDR, 0x0D, who) || who != 0xC7) return false;
  if (!i2cWrite8(ADDR, 0x2A, 0x00)) return false;
  if (!i2cWrite8(ADDR, 0x0E, 0x00)) return false;
  if (!i2cWrite8(ADDR, 0x2A, 0x19)) return false;
  delay(10); sensorType = SENSOR_FXOS8700; return true;
}

static bool initMotionSensor() {
  Wire1.begin(); Wire1.setClock(400000); delay(5);
  if (initLSM303AGR()) return true;
  if (initFXOS8700()) return true;
  sensorType = SENSOR_NONE; return false;
}

static bool readAccelerationMg(int16_t& x, int16_t& y, int16_t& z) {
  if (sensorType == SENSOR_LSM303AGR) {
    constexpr uint8_t ADDR = 0x19; uint8_t b[6];
    if (!i2cRead(ADDR, 0x28 | 0x80, b, 6)) return false;
    int16_t rx = (int16_t)((uint16_t)b[1] << 8 | b[0]);
    int16_t ry = (int16_t)((uint16_t)b[3] << 8 | b[2]);
    int16_t rz = (int16_t)((uint16_t)b[5] << 8 | b[4]);
    x = rx / 16; y = ry / 16; z = rz / 16; return true;
  }
  if (sensorType == SENSOR_FXOS8700) {
    constexpr uint8_t ADDR = 0x1F; uint8_t b[6];
    if (!i2cRead(ADDR, 0x01, b, 6)) return false;
    int16_t rx = (int16_t)((uint16_t)b[0] << 8 | b[1]);
    int16_t ry = (int16_t)((uint16_t)b[2] << 8 | b[3]);
    int16_t rz = (int16_t)((uint16_t)b[4] << 8 | b[5]);
    rx >>= 2; ry >>= 2; rz >>= 2;
    x = (int16_t)(((int32_t)rx * 1000L) / 4096L);
    y = (int16_t)(((int32_t)ry * 1000L) / 4096L);
    z = (int16_t)(((int32_t)rz * 1000L) / 4096L);
    return true;
  }
  return false;
}

static void notifyText(const char* text) {
  if (!txChr || !server || server->getConnectedCount() == 0) return;
  txChr->setValue(text); txChr->notify();
}

static void notifyInfo() {
  char msg[20]; snprintf(msg, sizeof(msg), "I,%s,%u\n", sensorName(), (unsigned)streamHz); notifyText(msg);
}

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
    pServer->updateConnParams(connInfo.getConnHandle(), 6, 12, 0, 180);
  }
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
    NimBLEDevice::startAdvertising();
  }
};

class RxCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    std::string value = pCharacteristic->getValue();
    while (!value.empty() && (value.back() == '\n' || value.back() == '\r' || value.back() == ' ')) value.pop_back();
    if (value == "I") { notifyInfo(); return; }
    if (value == "P") { notifyText("P,OK\n"); return; }
    if (value.rfind("S,", 0) == 0) {
      streaming = atoi(value.c_str() + 2) != 0;
      notifyText(streaming ? "S,1,OK\n" : "S,0,OK\n"); return;
    }
    if (value.rfind("R,", 0) == 0) {
      int hz = atoi(value.c_str() + 2);
      if (hz >= 10 && hz <= 100) {
        streamHz = (uint16_t)hz; char msg[20];
        snprintf(msg, sizeof(msg), "R,%u,OK\n", (unsigned)streamHz); notifyText(msg);
      } else notifyText("E,RANGE\n");
      return;
    }
    notifyText("E,CMD\n");
  }
};

static ServerCallbacks serverCallbacks;
static RxCallbacks rxCallbacks;

static void initBLE() {
  NimBLEDevice::init(DEVICE_NAME);
  server = NimBLEDevice::createServer(); server->setCallbacks(&serverCallbacks);
  NimBLEService* nus = server->createService(NUS_SERVICE_UUID);
  txChr = nus->createCharacteristic(NUS_TX_UUID, NIMBLE_PROPERTY::NOTIFY);
  rxChr = nus->createCharacteristic(NUS_RX_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
  rxChr->setCallbacks(&rxCallbacks); nus->start();
  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->setName(DEVICE_NAME); advertising->addServiceUUID(NUS_SERVICE_UUID);
  advertising->enableScanResponse(true); advertising->start();
}

void setup() {
  Serial.begin(115200); delay(100);
  bool sensorOK = initMotionSensor(); initBLE();
  Serial.printf("Device: %s\n", DEVICE_NAME);
  Serial.printf("Sensor: %s\n", sensorName());
  Serial.printf("BLE stream: %u Hz\n", (unsigned)streamHz);
  if (!sensorOK) Serial.println("ERROR: no supported motion sensor detected.");
}

void loop() {
  static uint32_t nextSampleUs = 0; static uint32_t lastErrorMs = 0;
  if (!server || server->getConnectedCount() == 0 || !streaming) {
    delay(10); nextSampleUs = micros(); return;
  }
  if (sensorType == SENSOR_NONE) {
    if ((uint32_t)(millis() - lastErrorMs) > 1000) { notifyText("E,NO_IMU\n"); lastErrorMs = millis(); }
    delay(20); return;
  }
  const uint32_t periodUs = 1000000UL / streamHz;
  const uint32_t nowUs = micros();
  if ((int32_t)(nowUs - nextSampleUs) < 0) { delay(1); return; }
  nextSampleUs += periodUs;
  if ((int32_t)(nowUs - nextSampleUs) > (int32_t)(periodUs * 4UL)) nextSampleUs = nowUs + periodUs;
  int16_t x, y, z;
  if (!readAccelerationMg(x, y, z)) { notifyText("E,I2C\n"); delay(10); return; }
  char msg[24]; snprintf(msg, sizeof(msg), "A,%d,%d,%d\n", x, y, z);
  txChr->setValue(msg); txChr->notify();
}
