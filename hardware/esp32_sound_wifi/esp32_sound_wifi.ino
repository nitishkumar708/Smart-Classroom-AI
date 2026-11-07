#include <WiFi.h>
#include <HTTPClient.h>

// ====== SET YOUR WI-FI NAME AND PASSWORD HERE ======
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// IPv4 address of the laptop running the backend (find it with `ipconfig`) and backend port
const char* SERVER_URL = "http://YOUR_LAPTOP_IP:5000/api/devices/esp32/sound";
// ===========================================================

// Pins
const int soundPin = 34;
const int greenLeds[3] = {25, 26, 27};
const int redLeds[2]   = {14, 13};
const int buzzerPin    = 12;

// Sound limits (peak-to-peak level, 0-4095 scale)
const int QUIET_MAX  = 100;
const int NORMAL_MAX = 750;

const unsigned long SAMPLE_MS = 200;    // window used to find the peak level
const unsigned long SEND_MS   = 2000;   // send to the backend every 2 seconds

int lastLevel = 0;
unsigned long lastSend = 0;
unsigned long lastWifiTry = 0;

void setLeds(int greens, int reds, bool buzzer) {
  for (int i = 0; i < 3; i++) digitalWrite(greenLeds[i], i < greens ? HIGH : LOW);
  for (int i = 0; i < 2; i++) digitalWrite(redLeds[i],   i < reds   ? HIGH : LOW);
  digitalWrite(buzzerPin, buzzer ? HIGH : LOW);
}

const char* stateFor(int level) {
  if (level <= QUIET_MAX)  return "QUIET";
  if (level <= NORMAL_MAX) return "NORMAL";
  return "LOUD";
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nWiFi connected. ESP32 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection FAILED (LEDs/buzzer still work locally)");
  }
}

void sendToBackend(int level, const char* state) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Send skipped: WiFi not connected");
    return;
  }
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(1500);
  String body = String("{\"level\":") + level + ",\"state\":\"" + state + "\"}";
  int code = http.POST(body);
  Serial.print("Backend reply code: ");
  Serial.println(code);   // 200 = OK, negative = could not reach the server
  http.end();
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) pinMode(greenLeds[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(redLeds[i], OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  setLeds(0, 0, false);
  connectWifi();
}

void loop() {
  // 1) Measure the peak-to-peak sound level over 200 ms
  int minV = 4095, maxV = 0;
  unsigned long start = millis();
  while (millis() - start < SAMPLE_MS) {
    int v = analogRead(soundPin);
    if (v < minV) minV = v;
    if (v > maxV) maxV = v;
  }
  int level = maxV - minV;
  lastLevel = level;
  const char* state = stateFor(level);

  // 2) LEDs + buzzer
  if (level <= QUIET_MAX)       setLeds(1, 0, false);   // QUIET  -> 1 green
  else if (level <= NORMAL_MAX) setLeds(3, 0, false);   // NORMAL -> 3 green
  else                          setLeds(0, 2, true);    // LOUD   -> 2 red + buzzer

  Serial.print("Level: ");
  Serial.print(level);
  Serial.print("  State: ");
  Serial.println(state);

  // 3) Send to the backend every 2 seconds
  if (millis() - lastSend >= SEND_MS) {
    lastSend = millis();
    sendToBackend(lastLevel, state);
  }

  // 4) If WiFi drops, retry every 10 seconds
  if (WiFi.status() != WL_CONNECTED && millis() - lastWifiTry > 10000) {
    lastWifiTry = millis();
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}
