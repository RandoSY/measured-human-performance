/*
  BBC micro:bit V2 -> Arduino/NimBLE AI motion streamer
  Arduino IDE, h2zero n-able core, NimBLE-Arduino 2.x

  BLE stream: A,<x_mg>,<y_mg>,<z_mg>\n
  Commands:
    I             info
    P             ping
    S,1 / S,0     stream on/off
    R,<hz>        10..100 Hz
    D,READY
    D,STREAM
    D,STOP
    D,RECORD,E    record animation + label initial
    D,CLASS,E     recognized class initial
    D,CHAR,E      arbitrary A-Z/0-9
    D,UNKNOWN
    D,OK
    D,ERROR
*/

#include <Arduino.h>
#include <Wire.h>
#include <NimBLEDevice.h>
#include <Adafruit_Microbit.h>
#include <ctype.h>
#include <string.h>

static const char* DEVICE_NAME = "MB2-AI-TIMER";
static const char* NUS_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* NUS_RX_UUID      = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* NUS_TX_UUID      = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

static NimBLEServer* server = nullptr;
static NimBLECharacteristic* txChr = nullptr;
static NimBLECharacteristic* rxChr = nullptr;

// ---------------------------------------------------------------------------
// 5x5 status display
// ---------------------------------------------------------------------------
static Adafruit_Microbit_Matrix matrix;

static const uint8_t ICON_ADVERTISING[5] = {0x04,0x0A,0x15,0x04,0x04};
static const uint8_t ICON_READY[5]       = {0x00,0x01,0x02,0x14,0x08};
static const uint8_t ICON_STREAM[5]      = {0x04,0x02,0x1F,0x02,0x04};
static const uint8_t ICON_RECORD[5]      = {0x0E,0x11,0x11,0x11,0x0E};
static const uint8_t ICON_STOP[5]        = {0x11,0x0A,0x04,0x0A,0x11};
static const uint8_t ICON_UNKNOWN[5]     = {0x0E,0x11,0x02,0x00,0x04};
static const uint8_t ICON_ERROR[5]       = {0x04,0x04,0x04,0x00,0x04};
static const uint8_t ICON_DOT[5]         = {0x00,0x00,0x04,0x00,0x00};

// 0-9 then A-Z.
static const uint8_t FONT_5X5[36][5] = {
  {0x0E,0x11,0x11,0x11,0x0E},{0x04,0x0C,0x04,0x04,0x0E},
  {0x0E,0x11,0x02,0x04,0x1F},{0x1E,0x01,0x06,0x01,0x1E},
  {0x02,0x06,0x0A,0x1F,0x02},{0x1F,0x10,0x1E,0x01,0x1E},
  {0x0E,0x10,0x1E,0x11,0x0E},{0x1F,0x01,0x02,0x04,0x04},
  {0x0E,0x11,0x0E,0x11,0x0E},{0x0E,0x11,0x0F,0x01,0x0E},
  {0x0E,0x11,0x1F,0x11,0x11},{0x1E,0x11,0x1E,0x11,0x1E},
  {0x0F,0x10,0x10,0x10,0x0F},{0x1E,0x11,0x11,0x11,0x1E},
  {0x1F,0x10,0x1E,0x10,0x1F},{0x1F,0x10,0x1E,0x10,0x10},
  {0x0F,0x10,0x17,0x11,0x0F},{0x11,0x11,0x1F,0x11,0x11},
  {0x0E,0x04,0x04,0x04,0x0E},{0x07,0x02,0x02,0x12,0x0C},
  {0x11,0x12,0x1C,0x12,0x11},{0x10,0x10,0x10,0x10,0x1F},
  {0x11,0x1B,0x15,0x11,0x11},{0x11,0x19,0x15,0x13,0x11},
  {0x0E,0x11,0x11,0x11,0x0E},{0x1E,0x11,0x1E,0x10,0x10},
  {0x0E,0x11,0x15,0x12,0x0D},{0x1E,0x11,0x1E,0x12,0x11},
  {0x0F,0x10,0x0E,0x01,0x1E},{0x1F,0x04,0x04,0x04,0x04},
  {0x11,0x11,0x11,0x11,0x0E},{0x11,0x11,0x11,0x0A,0x04},
  {0x11,0x11,0x15,0x1B,0x11},{0x11,0x0A,0x04,0x0A,0x11},
  {0x11,0x0A,0x04,0x04,0x04},{0x1F,0x02,0x04,0x08,0x1F}
};

enum DisplayMode : uint8_t {
  DISP_ADVERTISING, DISP_READY, DISP_STREAM, DISP_STOP,
  DISP_RECORD, DISP_CLASS, DISP_CHAR, DISP_UNKNOWN, DISP_OK, DISP_ERROR
};

static DisplayMode displayMode = DISP_ADVERTISING;
static char displayChar = 'E';
static uint32_t nextDisplayMs = 0;
static bool displayPhase = false;

static void matrixShow(const uint8_t rows[5]) {
  uint8_t copy[5];
  memcpy(copy, rows, 5);
  matrix.show(copy);
}

static bool matrixShowChar(char c) {
  c = (char)toupper((unsigned char)c);
  if (c >= '0' && c <= '9') { matrixShow(FONT_5X5[c-'0']); return true; }
  if (c >= 'A' && c <= 'Z') { matrixShow(FONT_5X5[10 + c-'A']); return true; }
  return false;
}

static void setDisplay(DisplayMode mode, char c = 0) {
  displayMode = mode;
  if (c) displayChar = (char)toupper((unsigned char)c);
  nextDisplayMs = 0;
  displayPhase = false;
}

static void updateDisplay(uint32_t now) {
  if ((int32_t)(now - nextDisplayMs) < 0) return;
  switch (displayMode) {
    case DISP_ADVERTISING:
      matrixShow(displayPhase ? ICON_ADVERTISING : ICON_DOT);
      displayPhase = !displayPhase; nextDisplayMs = now + 450; break;
    case DISP_READY:
    case DISP_OK:
      matrixShow(ICON_READY); nextDisplayMs = now + 1000; break;
    case DISP_STREAM:
      matrixShow(ICON_STREAM); nextDisplayMs = now + 1000; break;
    case DISP_STOP:
      matrixShow(ICON_STOP); nextDisplayMs = now + 1000; break;
    case DISP_RECORD:
      if (displayPhase || !matrixShowChar(displayChar)) matrixShow(ICON_RECORD);
      displayPhase = !displayPhase; nextDisplayMs = now + 300; break;
    case DISP_CLASS:
    case DISP_CHAR:
      if (!matrixShowChar(displayChar)) matrixShow(ICON_UNKNOWN);
      nextDisplayMs = now + 1000; break;
    case DISP_UNKNOWN:
      matrixShow(ICON_UNKNOWN); nextDisplayMs = now + 1000; break;
    case DISP_ERROR:
      matrixShow(ICON_ERROR); nextDisplayMs = now + 1000; break;
  }
}

static bool parseDisplayCommand(const std::string& value) {
  if (value.rfind("D,", 0) != 0) return false;
  std::string a = value.substr(2);
  if (a == "ADVERTISING") { setDisplay(DISP_ADVERTISING); return true; }
  if (a == "READY")       { setDisplay(DISP_READY); return true; }
  if (a == "STREAM")      { setDisplay(DISP_STREAM); return true; }
  if (a == "STOP")        { setDisplay(DISP_STOP); return true; }
  if (a == "UNKNOWN")     { setDisplay(DISP_UNKNOWN); return true; }
  if (a == "OK")          { setDisplay(DISP_OK); return true; }
  if (a == "ERROR")       { setDisplay(DISP_ERROR); return true; }
  if (a.rfind("RECORD,",0)==0 && a.size()>=8) { setDisplay(DISP_RECORD,a[7]); return true; }
  if (a.rfind("CLASS,",0)==0  && a.size()>=7) { setDisplay(DISP_CLASS,a[6]); return true; }
  if (a.rfind("CHAR,",0)==0   && a.size()>=6) { setDisplay(DISP_CHAR,a[5]); return true; }
  return false;
}

// ---------------------------------------------------------------------------
// Motion sensor
// ---------------------------------------------------------------------------
enum SensorType : uint8_t { SENSOR_NONE=0, SENSOR_LSM303AGR, SENSOR_FXOS8700 };
static SensorType sensorType = SENSOR_NONE;
static volatile bool streaming = true;
static volatile uint16_t streamHz = 50;

static bool i2cWrite8(uint8_t a, uint8_t r, uint8_t v) {
  Wire1.beginTransmission(a); Wire1.write(r); Wire1.write(v);
  return Wire1.endTransmission() == 0;
}

static bool i2cRead(uint8_t a, uint8_t r, uint8_t* dst, uint8_t len) {
  Wire1.beginTransmission(a); Wire1.write(r);
  if (Wire1.endTransmission(false) != 0) return false;
  if (Wire1.requestFrom(a, len) != len) { while (Wire1.available()) Wire1.read(); return false; }
  for (uint8_t i=0;i<len;i++) dst[i]=Wire1.read();
  return true;
}

static bool i2cRead8(uint8_t a,uint8_t r,uint8_t& v){ return i2cRead(a,r,&v,1); }

static const char* sensorName() {
  if (sensorType==SENSOR_LSM303AGR) return "LSM303AGR";
  if (sensorType==SENSOR_FXOS8700) return "FXOS8700";
  return "NO_IMU";
}

static bool initLSM303AGR() {
  constexpr uint8_t A=0x19; uint8_t who=0;
  if (!i2cRead8(A,0x0F,who) || who!=0x33) return false;
  if (!i2cWrite8(A,0x20,0x57)) return false;
  if (!i2cWrite8(A,0x23,0x88)) return false;
  delay(10); sensorType=SENSOR_LSM303AGR; return true;
}

static bool initFXOS8700() {
  constexpr uint8_t A=0x1F; uint8_t who=0;
  if (!i2cRead8(A,0x0D,who) || who!=0xC7) return false;
  if (!i2cWrite8(A,0x2A,0x00)) return false;
  if (!i2cWrite8(A,0x0E,0x00)) return false;
  if (!i2cWrite8(A,0x2A,0x19)) return false;
  delay(10); sensorType=SENSOR_FXOS8700; return true;
}

static bool initMotionSensor() {
  Wire1.begin(); Wire1.setClock(400000); delay(5);
  if (initLSM303AGR()) return true;
  if (initFXOS8700()) return true;
  return false;
}

static bool readAccelerationMg(int16_t& x,int16_t& y,int16_t& z) {
  if (sensorType==SENSOR_LSM303AGR) {
    uint8_t b[6]; if (!i2cRead(0x19,0x28|0x80,b,6)) return false;
    int16_t rx=(int16_t)((uint16_t)b[1]<<8|b[0]);
    int16_t ry=(int16_t)((uint16_t)b[3]<<8|b[2]);
    int16_t rz=(int16_t)((uint16_t)b[5]<<8|b[4]);
    x=rx/16; y=ry/16; z=rz/16; return true;
  }
  if (sensorType==SENSOR_FXOS8700) {
    uint8_t b[6]; if (!i2cRead(0x1F,0x01,b,6)) return false;
    int16_t rx=(int16_t)((uint16_t)b[0]<<8|b[1]);
    int16_t ry=(int16_t)((uint16_t)b[2]<<8|b[3]);
    int16_t rz=(int16_t)((uint16_t)b[4]<<8|b[5]);
    rx>>=2; ry>>=2; rz>>=2;
    x=(int16_t)(((int32_t)rx*1000L)/4096L);
    y=(int16_t)(((int32_t)ry*1000L)/4096L);
    z=(int16_t)(((int32_t)rz*1000L)/4096L);
    return true;
  }
  return false;
}

static void notifyText(const char* s) {
  if (!txChr || !server || server->getConnectedCount()==0) return;
  txChr->setValue(s); txChr->notify();
}

static void notifyInfo() {
  char b[20]; snprintf(b,sizeof(b),"I,%s,%u\n",sensorName(),(unsigned)streamHz); notifyText(b);
}

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s, NimBLEConnInfo& ci) override {
    s->updateConnParams(ci.getConnHandle(),6,12,0,180);
    setDisplay(DISP_READY);
  }
  void onDisconnect(NimBLEServer*, NimBLEConnInfo&, int) override {
    setDisplay(DISP_ADVERTISING);
    NimBLEDevice::startAdvertising();
  }
};

class RxCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* c, NimBLEConnInfo&) override {
    std::string v=c->getValue();
    while (!v.empty() && (v.back()=='\n'||v.back()=='\r'||v.back()==' ')) v.pop_back();
    if (v=="I") { notifyInfo(); return; }
    if (v=="P") { notifyText("P,OK\n"); return; }
    if (parseDisplayCommand(v)) { notifyText("D,OK\n"); return; }
    if (v.rfind("S,",0)==0) {
      streaming=atoi(v.c_str()+2)!=0;
      setDisplay(streaming?DISP_STREAM:DISP_STOP);
      notifyText(streaming?"S,1,OK\n":"S,0,OK\n"); return;
    }
    if (v.rfind("R,",0)==0) {
      int hz=atoi(v.c_str()+2);
      if (hz>=10 && hz<=100) {
        streamHz=(uint16_t)hz; char b[20]; snprintf(b,sizeof(b),"R,%u,OK\n",(unsigned)streamHz); notifyText(b);
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
  server=NimBLEDevice::createServer(); server->setCallbacks(&serverCallbacks);
  NimBLEService* nus=server->createService(NUS_SERVICE_UUID);
  txChr=nus->createCharacteristic(NUS_TX_UUID,NIMBLE_PROPERTY::NOTIFY);
  rxChr=nus->createCharacteristic(NUS_RX_UUID,NIMBLE_PROPERTY::WRITE|NIMBLE_PROPERTY::WRITE_NR);
  rxChr->setCallbacks(&rxCallbacks); nus->start();
  NimBLEAdvertising* a=NimBLEDevice::getAdvertising();
  a->setName(DEVICE_NAME); a->addServiceUUID(NUS_SERVICE_UUID); a->enableScanResponse(true); a->start();
}

void setup() {
  Serial.begin(115200); delay(100);
  matrix.begin(); matrix.clear(); setDisplay(DISP_ADVERTISING); updateDisplay(millis());
  bool sensorOK=initMotionSensor(); initBLE();
  Serial.printf("Device: %s\n",DEVICE_NAME);
  Serial.printf("Sensor: %s\n",sensorName());
  Serial.printf("BLE stream: %u Hz\n",(unsigned)streamHz);
  if (!sensorOK) { setDisplay(DISP_ERROR); Serial.println("ERROR: no supported motion sensor detected."); }
}

void loop() {
  static uint32_t nextSampleUs=0, lastErrorMs=0;
  updateDisplay(millis());
  if (!server || server->getConnectedCount()==0 || !streaming) {
    delay(5); nextSampleUs=micros(); return;
  }
  if (sensorType==SENSOR_NONE) {
    setDisplay(DISP_ERROR);
    if ((uint32_t)(millis()-lastErrorMs)>1000) { notifyText("E,NO_IMU\n"); lastErrorMs=millis(); }
    delay(10); return;
  }
  const uint32_t periodUs=1000000UL/streamHz;
  const uint32_t nowUs=micros();
  if ((int32_t)(nowUs-nextSampleUs)<0) { delay(1); return; }
  nextSampleUs+=periodUs;
  if ((int32_t)(nowUs-nextSampleUs)>(int32_t)(periodUs*4UL)) nextSampleUs=nowUs+periodUs;
  int16_t x,y,z;
  if (!readAccelerationMg(x,y,z)) { setDisplay(DISP_ERROR); notifyText("E,I2C\n"); delay(10); return; }
  char msg[24]; snprintf(msg,sizeof(msg),"A,%d,%d,%d\n",x,y,z);
  txChr->setValue(msg); txChr->notify();
}
