#include <WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "MohsenZiadi";
const char* WIFI_PASS = "123456789";
const char* TB_SERVER = "mqtt.thingsboard.cloud";
const int   TB_PORT   = 1883;
const char* TB_TOKEN  = "aQjAGGDdFy0mIPrlRhD2";

WiFiClient espClient;
PubSubClient client(espClient);

const int LED_R = 25;
const int LED_G = 26;
const int LED_B = 27;
const int BUZZER_PIN = 32;
const int MAG_SENSOR_PIN = 14;
const int BTN_INACTIVE_PIN = 12;
const int BTN_ACTIVE_PIN = 13;
const int BTN_INACTIVE_PIN_RE = 15;
const int BTN_ACTIVE_PIN_RE = 2;

// --- جدید ---
const int LED_WIFI = 33;    // LED وضعیت WiFi
const int LED_TB = 4;       // LED وضعیت ThingsBoard
const int BTN_CONNECT = 5;  // دکمه اتصال دستی (با Pull-up)

bool systemActive = false;
bool alarmActive = false;

unsigned long lastSensorChange = 0;
unsigned long sensorDebounceMs = 50;
unsigned long lastBtnChange = 0;
unsigned long btnDebounceMs = 50;
unsigned long lastAlarmToggle = 0;
unsigned long alarmToggleInterval = 200;
unsigned long buzzerToggleTime = 0;
bool buzzerCurrentlyOn = false;
bool alarmVisualState = false;
unsigned long lastMqttSend = 0;
unsigned long mqttInterval = 2000;

void setLED(bool r, bool g, bool b) {
  digitalWrite(LED_R, r ? HIGH : LOW);
  digitalWrite(LED_G, g ? HIGH : LOW);
  digitalWrite(LED_B, b ? HIGH : LOW);
}

void activateSystem() {
  systemActive = true;
  alarmActive = false;
  noTone(BUZZER_PIN);
  setLED(false, true, false);
}

void deactivateSystem() {
  systemActive = false;
  alarmActive = false;
  noTone(BUZZER_PIN);
  setLED(false, false, true);
}

void activateAlarm() {
  alarmActive = true;
  lastAlarmToggle = millis();
  buzzerToggleTime = millis();
  buzzerCurrentlyOn = false;
  alarmVisualState = false;
}

void deactivateAlarm() {
  alarmActive = false;
  noTone(BUZZER_PIN);
  setLED(false, true, false);
}

void handleAlarmBlinkAndBuzz() {
  unsigned long now = millis();
  if (now - lastAlarmToggle >= alarmToggleInterval) {
    lastAlarmToggle = now;
    alarmVisualState = !alarmVisualState;
    setLED(alarmVisualState, false, false);
  }
  static unsigned long buzzerOnMs = 150;
  static unsigned long buzzerOffMs = 200;
  if (buzzerCurrentlyOn) {
    if (now - buzzerToggleTime >= buzzerOnMs) {
      noTone(BUZZER_PIN);
      buzzerCurrentlyOn = false;
      buzzerToggleTime = now;
    }
  } else {
    if (now - buzzerToggleTime >= buzzerOffMs) {
      tone(BUZZER_PIN, 2000);
      buzzerCurrentlyOn = true;
      buzzerToggleTime = now;
    }
  }
}

// ---------------- اتصال WiFi و ThingsBoard ----------------
void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  digitalWrite(LED_WIFI, LOW);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI, !digitalRead(LED_WIFI));
    delay(300);
  }
  digitalWrite(LED_WIFI, HIGH); // بعد از اتصال کامل روشن بماند
}

void connectToThingsBoard() {
  client.setServer(TB_SERVER, TB_PORT);
  digitalWrite(LED_TB, LOW);
  while (!client.connected()) {
    if (client.connect("ESP32", TB_TOKEN, NULL)) {
      digitalWrite(LED_TB, HIGH); // بعد از اتصال روشن بماند
    } else {
      digitalWrite(LED_TB, !digitalRead(LED_TB));
      delay(1000);
    }
  }
  client.subscribe("v1/devices/me/rpc/request/+");
}

void sendTelemetry() {
  if (millis() - lastMqttSend > mqttInterval) {
    lastMqttSend = millis();
    String payload = "{";
    payload += "\"systemActive\":" + String(systemActive ? "true" : "false") + ",";
    payload += "\"alarmActive\":" + String(alarmActive ? "true" : "false");
    payload += "}";
    client.publish("v1/devices/me/telemetry", payload.c_str());
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  if (msg.indexOf("method") != -1 && msg.indexOf("setSystem") != -1) {
    if (msg.indexOf("true") != -1) {
      activateSystem();
    } else {
      deactivateSystem();
    }
  }
}

// ---------------- setup و loop ----------------
void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MAG_SENSOR_PIN, INPUT_PULLUP);
  pinMode(BTN_INACTIVE_PIN, INPUT_PULLUP);
  pinMode(BTN_ACTIVE_PIN, INPUT_PULLUP);
  pinMode(BTN_INACTIVE_PIN_RE, INPUT);
  pinMode(BTN_ACTIVE_PIN_RE, INPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_TB, OUTPUT);
  pinMode(BTN_CONNECT, INPUT_PULLUP);

  setLED(false, false, false);
  digitalWrite(LED_WIFI, LOW);
  digitalWrite(LED_TB, LOW);

  client.setCallback(callback);
  deactivateSystem();
}

void loop() {
  // اگر MQTT قطع شد، دوباره وصل شو
  if (WiFi.status() == WL_CONNECTED && !client.connected()) connectToThingsBoard();
  client.loop();
  sendTelemetry();

  // دکمه اتصال دستی
  if (digitalRead(BTN_CONNECT) == LOW) {
    connectToWiFi();
    connectToThingsBoard();
  }

  // کنترل دکمه‌ها
  int btnInactive = digitalRead(BTN_INACTIVE_PIN);
  int btnActive = digitalRead(BTN_ACTIVE_PIN);
  int btnInactivere = digitalRead(BTN_INACTIVE_PIN_RE);
  int btnActivere = digitalRead(BTN_ACTIVE_PIN_RE);

  if (millis() - lastBtnChange > btnDebounceMs) {
    lastBtnChange = millis();
    if (btnActive == LOW || btnActivere == HIGH) activateSystem();
    else if (btnInactive == LOW || btnInactivere == HIGH) deactivateSystem();
  }

  // عملکرد دزدگیر
  if (systemActive) {
    int magState = digitalRead(MAG_SENSOR_PIN);
    if (millis() - lastSensorChange > sensorDebounceMs) {
      lastSensorChange = millis();
      if (magState == HIGH && !alarmActive) activateAlarm();
    }
    if (alarmActive) handleAlarmBlinkAndBuzz();
  }
}
