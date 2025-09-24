#include <Arduino.h>
#include <WiFi.h>
#include <SocketIoClient.h>

// Thông tin WiFi
const char* ssid = "Huyen Tran 2.4G";
const char* password = "nhu0918746369";

// Địa chỉ server Socket.IO
const char* host = "192.168.1.36"; // Thay bằng IP máy chủ
const uint16_t port = 3000;

// LED pin
const int ledPin = 2; // Built-in LED
bool ledState = false;

SocketIoClient socket;

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Kết nối WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Kết nối Socket.IO
  socket.begin(host, port);
  
  socket.on("connect", [] (const char * payload, size_t length) {
    Serial.println("Connected to Socket.IO server!");
    // Đăng ký là ESP client
    socket.emit("register", "\"esp\"");
    socket.emit("hello", "\"Hello from ESP32 - Buoi 4\"");
  });

  // Lắng nghe lệnh điều khiển từ dashboard
  socket.on("control", [] (const char * payload, size_t length) {
    Serial.print("Received control: ");
    Serial.println(payload);
    
    // Parse JSON command
    if (strstr(payload, "toggle")) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      Serial.println("LED toggled");
    }
    else if (strstr(payload, "\"on\"") || strstr(payload, "\"ON\"")) {
      ledState = true;
      digitalWrite(ledPin, HIGH);
      Serial.println("LED ON");
    }
    else if (strstr(payload, "\"off\"") || strstr(payload, "\"OFF\"")) {
      ledState = false;
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF");
    }
    
    Serial.print("LED State: ");
    Serial.println(ledState ? "ON" : "OFF");
  });

  socket.on("disconnect", [] (const char * payload, size_t length) {
    Serial.println("Disconnected from server");
  });
}

void loop() {
  socket.loop();

  // Gửi telemetry đều đặn mỗi 3 giây
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 3000) {
    lastSend = millis();
    
    // Tạo dữ liệu telemetry giả lập
    float temperature = 20.0 + random(0, 200) / 10.0; // 20-40°C
    float humidity = 40.0 + random(0, 400) / 10.0;    // 40-80%
    
    // Tạo JSON string
    String telemetry = "{\"temp\":" + String(temperature, 1) + 
                      ",\"hum\":" + String(humidity, 1) + 
                      ",\"time\":" + String(millis()) + "}";
    
    socket.emit("telemetry", telemetry.c_str());
    
    Serial.print("Sent telemetry - Temp: ");
    Serial.print(temperature);
    Serial.print("°C, Hum: ");
    Serial.print(humidity);
    Serial.print("%, LED: ");
    Serial.println(ledState ? "ON" : "OFF");
  }

  // Blink LED nhanh nếu đang kết nối
  static unsigned long lastBlink = 0;
  if (socket.isConnected() && millis() - lastBlink > 100) {
    lastBlink = millis();
    // Chỉ blink nếu LED đang OFF (để thấy trạng thái kết nối)
    if (!ledState) {
      digitalWrite(ledPin, !digitalRead(ledPin));
      delay(10);
      digitalWrite(ledPin, LOW);
    }
  }
}