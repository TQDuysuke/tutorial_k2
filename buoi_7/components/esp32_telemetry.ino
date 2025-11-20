#include <Arduino.h>
#include <WiFi.h>
#include <SocketIoClient.h>

// ================================
// DEVICE CONFIGURATION
// ================================
// Thay đổi DEVICE_ID cho từng ESP32 khác nhau
const String DEVICE_ID = "ESP32_001";  // ESP32_001, ESP32_002, ESP32_003, etc.
// Hoặc dùng MAC address: WiFi.macAddress()

// WiFi Configuration
const char* ssid = "Huyen Tran 2.4G";
const char* password = "nhu0918746369";

// Server Configuration  
const char* host = "192.168.1.36"; // Thay bằng IP server
const uint16_t port = 3000;

// Security Configuration - API Key (phải khớp với .env trên server)
const String API_KEY = "esp32_secret_key_2024_buoi7";

// Hardware Configuration
const int ledPin = 2; // Built-in LED
const int sensorPin = A0; // Optional: analog sensor

// ================================
// GLOBAL VARIABLES
// ================================
SocketIoClient socket;
bool ledState = false;
unsigned long lastTelemetry = 0;
unsigned long lastHeartbeat = 0;
bool isConnectedToServer = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=================================");
  Serial.println("Multi-ESP32 IoT System");
  Serial.println("Device ID: " + DEVICE_ID);
  Serial.println("=================================");
  
  // Hardware initialization
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Connect to WiFi
  connectWiFi();
  
  // Connect to Socket.IO server
  connectToServer();
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: " + String(ssid));
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.println("IP Address: " + WiFi.localIP().toString());
    Serial.println("MAC Address: " + WiFi.macAddress());
  } else {
    Serial.println("\nWiFi Connection Failed!");
    ESP.restart();
  }
}

void connectToServer() {
  Serial.println("Connecting to server: " + String(host) + ":" + String(port));
  
  socket.begin(host, port);
  
  // Connection event
  socket.on("connect", [] (const char * payload, size_t length) {
    Serial.println("✅ Connected to Socket.IO server!");
    isConnectedToServer = true;
    
    // Register device with unique ID and API Key for authentication
    String registerMsg = "{\"deviceId\":\"" + DEVICE_ID + "\",\"apiKey\":\"" + API_KEY + "\"}";
    socket.emit("register_device", registerMsg.c_str());
    Serial.println("📡 Device registered: " + DEVICE_ID);
    Serial.println("🔐 Authentication key sent");
    
    // Send hello message
    String helloMsg = "\"Hello from " + DEVICE_ID + "\"";
    socket.emit("hello", helloMsg.c_str());
    
    // Blink LED to indicate connection
    blinkLED(3, 200);
  });
  
  // Authentication failure event
  socket.on("auth_failed", [] (const char * payload, size_t length) {
    Serial.println("🚨 Authentication FAILED!");
    Serial.println("Error: " + String(payload));
    isConnectedToServer = false;
  });
  
  // Control event handler
  socket.on("control", [] (const char * payload, size_t length) {
    Serial.println("🎮 Control received: " + String(payload));
    handleControlCommand(payload);
  });
  
  // Disconnect event
  socket.on("disconnect", [] (const char * payload, size_t length) {
    Serial.println("❌ Disconnected from server");
    isConnectedToServer = false;
  });
  
  // Error event
  socket.on("error", [] (const char * payload, size_t length) {
    Serial.println("🚨 Socket error: " + String(payload));
  });
}

void handleControlCommand(const char* payload) {
  String command = String(payload);
  command.toLowerCase();
  
  if (command.indexOf("toggle") >= 0) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    Serial.println("💡 LED toggled: " + String(ledState ? "ON" : "OFF"));
  }
  else if (command.indexOf("\"on\"") >= 0) {
    ledState = true;
    digitalWrite(ledPin, HIGH);
    Serial.println("💡 LED turned ON");
  }
  else if (command.indexOf("\"off\"") >= 0) {
    ledState = false;
    digitalWrite(ledPin, LOW);
    Serial.println("💡 LED turned OFF");
  }
  else {
    Serial.println("❓ Unknown command: " + command);
  }
}

void loop() {
  socket.loop();
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("📶 WiFi disconnected, reconnecting...");
    connectWiFi();
  }
  
  // Send telemetry data every 5 seconds
  if (millis() - lastTelemetry > 5000) {
    sendTelemetry();
    lastTelemetry = millis();
  }
  
  // Send heartbeat every 30 seconds
  if (millis() - lastHeartbeat > 30000) {
    sendHeartbeat();
    lastHeartbeat = millis();
  }
  
  // Connection indicator (blink if connected)
  if (isConnectedToServer && !ledState) {
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 2000) {
      digitalWrite(ledPin, HIGH);
      delay(50);
      digitalWrite(ledPin, LOW);
      lastBlink = millis();
    }
  }
}

void sendTelemetry() {
  if (!isConnectedToServer) {
    Serial.println("🚫 Not connected to server, skipping telemetry");
    return;
  }
  
  // Generate sensor data (you can replace with real sensors)
  float temperature = generateTemperature();
  float humidity = generateHumidity();
  
  // Optional: read real analog sensor
  // int sensorValue = analogRead(sensorPin);
  // float voltage = sensorValue * (3.3 / 4095.0);
  
  // Create telemetry JSON
  String telemetry = "{";
  telemetry += "\"deviceId\":\"" + DEVICE_ID + "\",";
  telemetry += "\"temp\":" + String(temperature, 1) + ",";
  telemetry += "\"hum\":" + String(humidity, 1) + ",";
  telemetry += "\"ledState\":" + String(ledState ? "true" : "false") + ",";
  telemetry += "\"uptime\":" + String(millis() / 1000) + ",";
  telemetry += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  telemetry += "\"rssi\":" + String(WiFi.RSSI());
  telemetry += "}";
  
  socket.emit("telemetry", telemetry.c_str());
  
  Serial.println("📊 Telemetry sent:");
  Serial.println("  Temperature: " + String(temperature, 1) + "°C");
  Serial.println("  Humidity: " + String(humidity, 1) + "%");
  Serial.println("  LED State: " + String(ledState ? "ON" : "OFF"));
  Serial.println("  Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("  WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
}

void sendHeartbeat() {
  if (!isConnectedToServer) return;
  
  String heartbeat = "{\"deviceId\":\"" + DEVICE_ID + "\",\"type\":\"heartbeat\",\"timestamp\":" + String(millis()) + "}";
  socket.emit("heartbeat", heartbeat.c_str());
  Serial.println("💓 Heartbeat sent");
}

// Sensor simulation functions (replace with real sensor code)
float generateTemperature() {
  // Simulate temperature between 18-35°C with some randomness
  static float temp = 25.0;
  temp += (random(-20, 21) / 100.0); // ±0.2°C change
  
  // Keep in realistic range
  if (temp < 18.0) temp = 18.0;
  if (temp > 35.0) temp = 35.0;
  
  return temp;
}

float generateHumidity() {
  // Simulate humidity between 30-80% with some randomness
  static float hum = 55.0;
  hum += (random(-50, 51) / 100.0); // ±0.5% change
  
  // Keep in realistic range
  if (hum < 30.0) hum = 30.0;
  if (hum > 80.0) hum = 80.0;
  
  return hum;
}

void blinkLED(int times, int delayMs) {
  bool originalState = digitalRead(ledPin);
  
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(delayMs);
    digitalWrite(ledPin, LOW);
    delay(delayMs);
  }
  
  digitalWrite(ledPin, originalState);
}

// Debug function to print device info
void printDeviceInfo() {
  Serial.println("\n📋 Device Information:");
  Serial.println("Device ID: " + DEVICE_ID);
  Serial.println("MAC Address: " + WiFi.macAddress());
  Serial.println("IP Address: " + WiFi.localIP().toString());
  Serial.println("Chip Model: " + String(ESP.getChipModel()));
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
  Serial.println("Flash Size: " + String(ESP.getFlashChipSize()));
  Serial.println("CPU Frequency: " + String(ESP.getCpuFreqMHz()) + " MHz");
}