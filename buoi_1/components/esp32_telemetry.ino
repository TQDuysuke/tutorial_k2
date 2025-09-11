
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

void setup() {
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
}

void loop() {
  socket.loop();

  // Gửi telemetry mỗi 5 giây
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    String telemetry = "{\"temp\":25.5,\"hum\":60}";
    socket.emit("telemetry", telemetry.c_str());
    Serial.println("Sent telemetry");
  }
}