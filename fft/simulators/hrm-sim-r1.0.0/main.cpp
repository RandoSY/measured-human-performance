/*
  FFT+ Heart Rate Monitor Simulator R1.0
  BBC micro:bit V2.21 / Nordic nRF52833

  Purpose
  -------
  A one-purpose Bluetooth SIG Heart Rate Service peripheral for testing the
  Fair Fitness Tester FFT+ micro:bit central/client path.

  Controls
  --------
  Button A       decrease BPM by one
  Button B       increase BPM by one
  Hold A or B    auto-repeat
  A + B          reset to 90 BPM

  BLE identity
  ------------
  Device name: FFT-HRM-SIM
  Service:     Heart Rate Service, 0x180D
  Notify:      Heart Rate Measurement, 0x2A37
  Read:        Body Sensor Location, 0x2A38 (Chest)

  This is a laboratory simulator. It does not measure a person's heart rate.
*/

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Adafruit_Microbit.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "hrm_sim_core.h"

#ifndef FFT_HRM_BUTTON_A_PIN
#define FFT_HRM_BUTTON_A_PIN 5
#endif
#ifndef FFT_HRM_BUTTON_B_PIN
#define FFT_HRM_BUTTON_B_PIN 11
#endif
#ifndef CONFIG_MAIN_TASK_STACK_SIZE
#define CONFIG_MAIN_TASK_STACK_SIZE 512
#endif

static constexpr const char* DEVICE_NAME = "FFT-HRM-SIM";
static constexpr const char* FW_VERSION = "FFT-HRM-SIM-R1.0.0";
static constexpr const char* HRS_UUID = "180D";
static constexpr const char* HRM_UUID = "2A37";
static constexpr const char* BODY_SENSOR_LOCATION_UUID = "2A38";
static constexpr uint16_t HEART_RATE_SENSOR_APPEARANCE = 833;
static constexpr uint8_t BODY_LOCATION_CHEST = 1;

static NimBLEServer* g_server = nullptr;
static NimBLECharacteristic* g_hrMeasurement = nullptr;
static NimBLECharacteristic* g_bodyLocation = nullptr;

static volatile bool g_connected = false;
static volatile bool g_subscribed = false;
static volatile bool g_justConnected = false;
static volatile bool g_justDisconnected = false;
static volatile bool g_subscriptionChanged = false;
static volatile uint16_t g_connectionHandle = BLE_HS_CONN_HANDLE_NONE;

static uint16_t g_bpm = fft_hrm_sim::kDefaultBpm;
static uint8_t g_buttonStep = 1;
static uint32_t g_notifyIntervalMs = 1000;
static bool g_contactDetected = true;
static uint32_t g_nextNotifyMs = 0;
static uint32_t g_notifySequence = 0;
static uint32_t g_notifySuccess = 0;
static uint32_t g_notifyFailure = 0;

static Adafruit_Microbit_Matrix g_matrix;

// ---------------------------------------------------------------------------
// Matrix display: cycle through the decimal digits so 30..220 is readable on
// the 5x5 matrix without blocking BLE service or button handling.
// ---------------------------------------------------------------------------
static const uint8_t DIGITS[10][5] = {
    {0x0E, 0x11, 0x11, 0x11, 0x0E}, {0x04, 0x0C, 0x04, 0x04, 0x0E},
    {0x0E, 0x11, 0x02, 0x04, 0x1F}, {0x1E, 0x01, 0x06, 0x01, 0x1E},
    {0x02, 0x06, 0x0A, 0x1F, 0x02}, {0x1F, 0x10, 0x1E, 0x01, 0x1E},
    {0x0E, 0x10, 0x1E, 0x11, 0x0E}, {0x1F, 0x01, 0x02, 0x04, 0x04},
    {0x0E, 0x11, 0x0E, 0x11, 0x0E}, {0x0E, 0x11, 0x0F, 0x01, 0x0E},
};

static const uint8_t HEART[5] = {0x0A, 0x1F, 0x1F, 0x0E, 0x04};
static const uint8_t ARROW_DOWN[5] = {0x04, 0x04, 0x15, 0x0E, 0x04};
static const uint8_t ARROW_UP[5] = {0x04, 0x0E, 0x15, 0x04, 0x04};

static uint32_t g_nextDisplayMs = 0;
static uint8_t g_displayDigitIndex = 0;
static uint32_t g_overrideUntilMs = 0;

static void matrixShow(const uint8_t rows[5]) {
  uint8_t copy[5];
  memcpy(copy, rows, sizeof(copy));
  g_matrix.show(copy);
}

static void showDigit(uint8_t digit) { matrixShow(DIGITS[digit % 10]); }

static void showTemporary(const uint8_t rows[5], uint32_t durationMs) {
  matrixShow(rows);
  g_overrideUntilMs = millis() + durationMs;
}

static uint8_t bpmDigits(uint16_t bpm, uint8_t out[3]) {
  if (bpm >= 100) {
    out[0] = static_cast<uint8_t>(bpm / 100);
    out[1] = static_cast<uint8_t>((bpm / 10) % 10);
    out[2] = static_cast<uint8_t>(bpm % 10);
    return 3;
  }
  out[0] = static_cast<uint8_t>(bpm / 10);
  out[1] = static_cast<uint8_t>(bpm % 10);
  return 2;
}

static void updateDisplay(uint32_t now) {
  if (static_cast<int32_t>(now - g_overrideUntilMs) < 0) return;
  if (static_cast<int32_t>(now - g_nextDisplayMs) < 0) return;

  uint8_t digits[3] = {};
  const uint8_t count = bpmDigits(g_bpm, digits);
  if (g_displayDigitIndex >= count) g_displayDigitIndex = 0;
  showDigit(digits[g_displayDigitIndex]);
  g_displayDigitIndex = static_cast<uint8_t>((g_displayDigitIndex + 1u) % count);
  g_nextDisplayMs = now + 650;
}

// ---------------------------------------------------------------------------
// Button handling with debounce and hold-repeat.
// ---------------------------------------------------------------------------
struct ButtonState {
  bool rawPressed = false;
  bool stablePressed = false;
  bool previousStable = false;
  uint32_t rawChangedMs = 0;
  uint32_t pressedMs = 0;
  uint32_t lastRepeatMs = 0;
};

static ButtonState g_buttonA;
static ButtonState g_buttonB;
static bool g_comboHandled = false;
static uint32_t g_comboStartedMs = 0;

static void sampleButton(ButtonState& button, bool pressed, uint32_t now) {
  button.previousStable = button.stablePressed;
  if (pressed != button.rawPressed) {
    button.rawPressed = pressed;
    button.rawChangedMs = now;
  }
  if (button.stablePressed != button.rawPressed && now - button.rawChangedMs >= 25) {
    button.stablePressed = button.rawPressed;
    if (button.stablePressed) {
      button.pressedMs = now;
      button.lastRepeatMs = now;
    }
  }
}

static void printStatus();

static void setBpm(int requested, const char* source) {
  const uint16_t next = fft_hrm_sim::clampBpm(requested);
  if (next == g_bpm) return;
  g_bpm = next;
  g_displayDigitIndex = 0;
  g_nextDisplayMs = millis();
  Serial.print("BPM source=");
  Serial.print(source);
  Serial.print(" value=");
  Serial.println(g_bpm);
}

static bool shouldRepeat(const ButtonState& button, uint32_t now) {
  return button.stablePressed && now - button.pressedMs >= 600 &&
         now - button.lastRepeatMs >= 120;
}

static void updateButtons(uint32_t now) {
  sampleButton(g_buttonA, digitalRead(FFT_HRM_BUTTON_A_PIN) == LOW, now);
  sampleButton(g_buttonB, digitalRead(FFT_HRM_BUTTON_B_PIN) == LOW, now);

  if (g_buttonA.stablePressed && g_buttonB.stablePressed) {
    if (g_comboStartedMs == 0) g_comboStartedMs = now;
    if (!g_comboHandled && now - g_comboStartedMs >= 80) {
      g_comboHandled = true;
      setBpm(fft_hrm_sim::kDefaultBpm, "A+B_RESET");
      showTemporary(HEART, 500);
      printStatus();
    }
    return;
  }

  g_comboStartedMs = 0;
  g_comboHandled = false;

  const bool aPressedEdge = g_buttonA.stablePressed && !g_buttonA.previousStable;
  const bool bPressedEdge = g_buttonB.stablePressed && !g_buttonB.previousStable;

  if (aPressedEdge || shouldRepeat(g_buttonA, now)) {
    g_buttonA.lastRepeatMs = now;
    setBpm(static_cast<int>(g_bpm) - g_buttonStep, "BUTTON_A");
    showTemporary(ARROW_DOWN, 180);
  }

  if (bPressedEdge || shouldRepeat(g_buttonB, now)) {
    g_buttonB.lastRepeatMs = now;
    setBpm(static_cast<int>(g_bpm) + g_buttonStep, "BUTTON_B");
    showTemporary(ARROW_UP, 180);
  }
}

// ---------------------------------------------------------------------------
// BLE server callbacks.
// ---------------------------------------------------------------------------
class HrmServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* server, NimBLEConnInfo& connInfo) override {
    g_connected = true;
    g_justConnected = true;
    g_connectionHandle = connInfo.getConnHandle();
    server->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 200);
  }

  void onDisconnect(NimBLEServer*, NimBLEConnInfo&, int) override {
    g_connected = false;
    g_subscribed = false;
    g_connectionHandle = BLE_HS_CONN_HANDLE_NONE;
    g_justDisconnected = true;
    NimBLEDevice::startAdvertising();
  }
};

class HrmCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
  void onSubscribe(NimBLECharacteristic*, NimBLEConnInfo&, uint16_t subValue) override {
    g_subscribed = (subValue & 0x01u) != 0;
    g_subscriptionChanged = true;
    g_nextNotifyMs = millis();
  }

  void onStatus(NimBLECharacteristic*, int code) override {
    if (code != 0) {
      Serial.print("NOTIFY_STATUS code=");
      Serial.println(code);
    }
  }
};

static HrmServerCallbacks g_serverCallbacks;
static HrmCharacteristicCallbacks g_characteristicCallbacks;

static void setupBle() {
  NimBLEDevice::init(DEVICE_NAME);

  g_server = NimBLEDevice::createServer();
  g_server->setCallbacks(&g_serverCallbacks);

  NimBLEService* hrs = g_server->createService(HRS_UUID);
  g_hrMeasurement = hrs->createCharacteristic(HRM_UUID, NIMBLE_PROPERTY::NOTIFY, 3);
  g_hrMeasurement->setCallbacks(&g_characteristicCallbacks);

  g_bodyLocation = hrs->createCharacteristic(BODY_SENSOR_LOCATION_UUID, NIMBLE_PROPERTY::READ, 1);
  const uint8_t location = BODY_LOCATION_CHEST;
  g_bodyLocation->setValue(&location, 1);
  hrs->start();

  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->enableScanResponse(true);
  const bool serviceOk = advertising->addServiceUUID(HRS_UUID);
  const bool appearanceOk = advertising->setAppearance(HEART_RATE_SENSOR_APPEARANCE);
  const bool nameOk = advertising->setName(DEVICE_NAME);
  advertising->setMinInterval(160);  // 100 ms
  advertising->setMaxInterval(240);  // 150 ms
  const bool startOk = advertising->start();
  const bool active = advertising->isAdvertising();

  Serial.print("BLE_ADV service_uuid=");
  Serial.print(serviceOk ? 1 : 0);
  Serial.print(" appearance=");
  Serial.print(appearanceOk ? 1 : 0);
  Serial.print(" name=");
  Serial.print(nameOk ? 1 : 0);
  Serial.print(" start=");
  Serial.print(startOk ? 1 : 0);
  Serial.print(" active=");
  Serial.print(active ? 1 : 0);
  Serial.print(" device=");
  Serial.println(DEVICE_NAME);
}

static void sendHeartRate(uint32_t now) {
  if (!g_connected || !g_subscribed || g_hrMeasurement == nullptr) return;
  if (static_cast<int32_t>(now - g_nextNotifyMs) < 0) return;
  g_nextNotifyMs = now + g_notifyIntervalMs;

  const fft_hrm_sim::HeartRateMeasurement packet =
      fft_hrm_sim::makeMeasurement(g_bpm, true, g_contactDetected);
  const bool ok = g_hrMeasurement->notify(packet.bytes, packet.length, g_connectionHandle);
  ++g_notifySequence;
  if (ok) ++g_notifySuccess;
  else ++g_notifyFailure;

  Serial.print("HR_NOTIFY seq=");
  Serial.print(g_notifySequence);
  Serial.print(" bpm=");
  Serial.print(g_bpm);
  Serial.print(" flags=0x");
  if (packet.bytes[0] < 16) Serial.print('0');
  Serial.print(packet.bytes[0], HEX);
  Serial.print(" contact=");
  Serial.print(g_contactDetected ? 1 : 0);
  Serial.print(" ok=");
  Serial.println(ok ? 1 : 0);
}

// ---------------------------------------------------------------------------
// Serial command interface.
// ---------------------------------------------------------------------------
static char g_command[96] = {};
static size_t g_commandLength = 0;

static void printHelp() {
  Serial.println("HELP commands=STATUS,VERSION,HELP,BPM_n,SET_BPM_n,STEP_n,INTERVAL_n,CONTACT_ON,CONTACT_OFF,RESET");
  Serial.println("HELP buttons=A_decrease,B_increase,HOLD_repeat,A+B_reset_90");
}

static void printStatus() {
  Serial.print("STATUS fw=");
  Serial.print(FW_VERSION);
  Serial.print(" device=");
  Serial.print(DEVICE_NAME);
  Serial.print(" bpm=");
  Serial.print(g_bpm);
  Serial.print(" step=");
  Serial.print(g_buttonStep);
  Serial.print(" interval_ms=");
  Serial.print(g_notifyIntervalMs);
  Serial.print(" connected=");
  Serial.print(g_connected ? 1 : 0);
  Serial.print(" subscribed=");
  Serial.print(g_subscribed ? 1 : 0);
  Serial.print(" contact_supported=1 contact=");
  Serial.print(g_contactDetected ? 1 : 0);
  Serial.print(" notify_ok=");
  Serial.print(g_notifySuccess);
  Serial.print(" notify_fail=");
  Serial.println(g_notifyFailure);
}

static void uppercase(char* text) {
  for (; *text; ++text) *text = static_cast<char>(toupper(static_cast<unsigned char>(*text)));
}

static char* skipSpaces(char* text) {
  while (*text == ' ' || *text == '\t') ++text;
  return text;
}

static bool parseIntegerArgument(char* command, const char* prefix, long& value) {
  const size_t length = strlen(prefix);
  if (strncmp(command, prefix, length) != 0) return false;
  char* argument = skipSpaces(command + length);
  if (*argument == '\0') return false;
  char* end = nullptr;
  value = strtol(argument, &end, 10);
  return end != argument && *skipSpaces(end) == '\0';
}

static void handleCommand(char* line) {
  while (*line == ' ' || *line == '\t') ++line;
  size_t length = strlen(line);
  while (length && (line[length - 1] == ' ' || line[length - 1] == '\t')) line[--length] = '\0';
  uppercase(line);
  if (*line == '\0') return;

  if (!strcmp(line, "HELP")) {
    printHelp();
    return;
  }
  if (!strcmp(line, "STATUS")) {
    printStatus();
    return;
  }
  if (!strcmp(line, "VERSION")) {
    Serial.print("VERSION ");
    Serial.println(FW_VERSION);
    return;
  }
  if (!strcmp(line, "RESET")) {
    setBpm(fft_hrm_sim::kDefaultBpm, "SERIAL_RESET");
    printStatus();
    return;
  }
  if (!strcmp(line, "CONTACT ON") || !strcmp(line, "CONTACT_ON")) {
    g_contactDetected = true;
    Serial.println("OK contact=1");
    return;
  }
  if (!strcmp(line, "CONTACT OFF") || !strcmp(line, "CONTACT_OFF")) {
    g_contactDetected = false;
    Serial.println("OK contact=0");
    return;
  }

  long value = 0;
  if (parseIntegerArgument(line, "BPM", value) || parseIntegerArgument(line, "SET_BPM", value)) {
    setBpm(static_cast<int>(value), "SERIAL");
    printStatus();
    return;
  }
  if (parseIntegerArgument(line, "STEP", value)) {
    if (value < 1 || value > 20) {
      Serial.println("ERR code=RANGE field=STEP min=1 max=20");
    } else {
      g_buttonStep = static_cast<uint8_t>(value);
      Serial.print("OK step=");
      Serial.println(g_buttonStep);
    }
    return;
  }
  if (parseIntegerArgument(line, "INTERVAL", value) ||
      parseIntegerArgument(line, "SET_INTERVAL_MS", value)) {
    if (value < 100 || value > 5000) {
      Serial.println("ERR code=RANGE field=INTERVAL min=100 max=5000");
    } else {
      g_notifyIntervalMs = static_cast<uint32_t>(value);
      g_nextNotifyMs = millis();
      Serial.print("OK interval_ms=");
      Serial.println(g_notifyIntervalMs);
    }
    return;
  }

  Serial.print("ERR code=UNKNOWN_COMMAND command=");
  Serial.println(line);
}

static void processSerial() {
  while (Serial.available() > 0) {
    const char c = static_cast<char>(Serial.read());
    if (c == '\r') continue;
    if (c == '\n') {
      g_command[g_commandLength] = '\0';
      handleCommand(g_command);
      g_commandLength = 0;
      continue;
    }
    if (g_commandLength + 1 < sizeof(g_command)) {
      g_command[g_commandLength++] = c;
    } else {
      g_commandLength = 0;
      Serial.println("ERR code=COMMAND_TOO_LONG");
    }
  }
}

static void processBleEvents() {
  if (g_justConnected) {
    g_justConnected = false;
    Serial.print("CONNECTED handle=");
    Serial.println(g_connectionHandle);
    showTemporary(HEART, 500);
  }
  if (g_subscriptionChanged) {
    g_subscriptionChanged = false;
    Serial.print("SUBSCRIPTION hrs_notify=");
    Serial.println(g_subscribed ? 1 : 0);
  }
  if (g_justDisconnected) {
    g_justDisconnected = false;
    Serial.println("DISCONNECTED advertising_restarted=1");
    showTemporary(HEART, 500);
  }
}

void setup() {
  Serial.begin(115200);
  delay(120);
  Serial.println();
  Serial.println("FFT+ Heart Rate Monitor Simulator boot");

  pinMode(FFT_HRM_BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(FFT_HRM_BUTTON_B_PIN, INPUT_PULLUP);

  g_matrix.begin();
  g_matrix.clear();
  matrixShow(HEART);
  g_overrideUntilMs = millis() + 700;

  setupBle();

  Serial.print("STACK_CONFIG main_words=");
  Serial.println(static_cast<unsigned long>(CONFIG_MAIN_TASK_STACK_SIZE));
  Serial.print("BOOT fw=");
  Serial.print(FW_VERSION);
  Serial.print(" board_target=microbit_v2.21 hrs=0x180D hrm=0x2A37 body_location=CHEST matrix=1 bpm=");
  Serial.println(g_bpm);
  printHelp();
  printStatus();
}

void loop() {
  const uint32_t now = millis();
  processSerial();
  processBleEvents();
  updateButtons(now);
  sendHeartRate(now);
  updateDisplay(now);
  delay(1);
}
