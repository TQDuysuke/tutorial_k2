
#include <Arduino.h>
#include <WiFi.h>
#include <SocketIoClient.h>

// Thông tin WiFi
const char* ssid = "Huyen Tran 2.4G";
const char* password = "nhu0918746369";

// Địa chỉ server Socket.IO
const char* host = "192.168.1.36"; // IP máy chủ
const uint16_t port = 3000;

SocketIoClient socket;

const int LED_PIN = 2; // onboard LED pin

void handleControl(const char * payload, size_t length) {
  Serial.print("Received control: ");
  Serial.println(payload);
  String s(payload, length);
  if (s.indexOf("\"led\":true") >= 0) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
  } else if (s.indexOf("\"led\":false") >= 0) {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  socket.begin(host, port);
  socket.on("connect", [] (const char * payload, size_t length) {
    Serial.println("Connected to Socket.IO server!");
    socket.emit("hello", "\"Hello from ESP32\"");
  });

  socket.on("control", handleControl);
}

void loop() {
  socket.loop();

  // Gửi telemetry mỗi 5 giây (fake data)
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    float temp = random(200, 350) / 10.0; // 20.0 - 35.0
    float hum = random(400, 700) / 10.0;  // 40.0 - 70.0
    char buf[64];
    snprintf(buf, sizeof(buf), "{\"temp\":%.1f,\"hum\":%.1f}", temp, hum);
    socket.emit("telemetry", buf);
    Serial.print("Sent telemetry: ");
    Serial.println(buf);
  }
}