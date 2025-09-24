#include <Arduino.h>
#include <WiFi.h>
#include <SocketIoClient.h>

// ================================
// DEVICE CONFIGURATION
// ================================
// Unique device identifier - CHANGE THIS for each ESP32!
String DEVICE_ID = "ESP32_Living";  // ESP32_Kitchen, ESP32_Garden, Sensor_01, etc.

// Option: Use MAC address as device ID (uncomment to use)
// #define USE_MAC_AS_DEVICE_ID

// WiFi credentials
const char* ssid = "Huyen Tran 2.4G";
const char* password = "nhu0918746369";

// Server configuration
const char* host = "192.168.1.36"; // Change to your server IP
const uint16_t port = 3000;

// Hardware pins
const int LED_PIN = 2;        // Built-in LED
const int EXTERNAL_LED = 4;   // Optional external LED
const int SENSOR_PIN = A0;    // Optional analog sensor

// ================================
// GLOBAL VARIABLES
// ================================
SocketIoClient socket;
bool ledState = false;
bool isConnected = false;
bool isRegistered = false;

// Telemetry timing
unsigned long lastTelemetry = 0;
const unsigned long TELEMETRY_INTERVAL = 3000; // 3 seconds

// Connection monitoring
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 30000; // 30 seconds
unsigned long connectionStartTime = 0;

// Data simulation
float currentTemp = 25.0;
float currentHum = 55.0;
int telemetryCount = 0;

// ================================
// SETUP FUNCTION
// ================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  printWelcomeMessage();
  setupHardware();
  setupDeviceId();
  connectToWiFi();
  connectToServer();
  
  Serial.println("🚀 ESP32 IoT Device ready!");
  blinkLED(3, 200); // Success indicator
}

void printWelcomeMessage() {
  Serial.println("\n" + String("=").substring(0, 50));
  Serial.println("🏭 ADVANCED ESP32 IOT DEVICE");
  Serial.println(String("=").substring(0, 50));
  Serial.println("Version: 2.0");
  Serial.println("Features: Auto-registration, Robust telemetry");
  Serial.println(String("=").substring(0, 50));
}

void setupHardware() {
  // LED pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(EXTERNAL_LED, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(EXTERNAL_LED, LOW);
  
  // Initialize random seed
  randomSeed(analogRead(0) + millis());
  
  Serial.println("✅ Hardware initialized");
}

void setupDeviceId() {
#ifdef USE_MAC_AS_DEVICE_ID
  // Use MAC address as device ID
  DEVICE_ID = "ESP32_" + WiFi.macAddress().substring(12);
  DEVICE_ID.replace(":", "");
#endif
  
  Serial.println("🆔 Device ID: " + DEVICE_ID);
}

void connectToWiFi() {
  Serial.print("📶 Connecting to WiFi: " + String(ssid));
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // Blink LED during connection
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW);
    Serial.println("\n✅ WiFi Connected!");
    Serial.println("   IP Address: " + WiFi.localIP().toString());
    Serial.println("   MAC Address: " + WiFi.macAddress());
    Serial.println("   Signal Strength: " + String(WiFi.RSSI()) + " dBm");
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
    Serial.println("🔄 Restarting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }
}

void connectToServer() {
  Serial.println("🔌 Connecting to server: " + String(host) + ":" + String(port));
  connectionStartTime = millis();
  
  socket.begin(host, port);
  
  // Connection successful
  socket.on("connect", [] (const char * payload, size_t length) {
    Serial.println("✅ Connected to IoT server!");
    isConnected = true;
    
    // Register device automatically
    registerDevice();
    
    // Send initial hello
    sendHelloMessage();
    
    // Connection success indicator
    blinkLED(2, 150);
  });
  
  // Registration confirmation
  socket.on("registration_success", [] (const char * payload, size_t length) {
    Serial.println("✅ Device registered successfully!");
    isRegistered = true;
    
    // Send initial telemetry
    sendTelemetry();
  });
  
  // Registration error
  socket.on("registration_error", [] (const char * payload, size_t length) {
    Serial.println("❌ Registration error: " + String(payload));
    isRegistered = false;
  });
  
  // Control commands from dashboard
  socket.on("control", [] (const char * payload, size_t length) {
    handleControlCommand(payload);
  });
  
  // Disconnection handler
  socket.on("disconnect", [] (const char * payload, size_t length) {
    Serial.println("❌ Disconnected from server");
    isConnected = false;
    isRegistered = false;
  });
}

void registerDevice() {
  String deviceIdMsg = "\"" + DEVICE_ID + "\"";
  socket.emit("register_device", deviceIdMsg.c_str());
  Serial.println("📝 Registering device: " + DEVICE_ID);
}

void sendHelloMessage() {
  String helloMsg = "\"Advanced IoT Device " + DEVICE_ID + " ready for telemetry\"";
  socket.emit("hello", helloMsg.c_str());
}

// ================================
// MAIN LOOP
// ================================
void loop() {
  // Maintain socket connection
  socket.loop();
  
  // Check WiFi connection
  checkWiFiConnection();
  
  // Send telemetry periodically
  if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {
    if (isConnected && isRegistered) {
      sendTelemetry();
    }
    lastTelemetry = millis();
  }
  
  // Send heartbeat
  if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    lastHeartbeat = millis();
  }
  
  // Connection status indicator
  updateConnectionIndicator();
  
  // Small delay to prevent watchdog issues
  delay(10);
}

// ================================
// TELEMETRY FUNCTIONS
// ================================
void sendTelemetry() {
  // Generate realistic sensor data
  updateSensorData();
  
  // Create comprehensive telemetry payload
  String telemetry = "{";
  telemetry += "\"deviceId\":\"" + DEVICE_ID + "\",";
  telemetry += "\"temp\":" + String(currentTemp, 2) + ",";
  telemetry += "\"hum\":" + String(currentHum, 1) + ",";
  telemetry += "\"ledState\":" + String(ledState ? "true" : "false") + ",";
  telemetry += "\"uptime\":" + String(millis() / 1000) + ",";
  telemetry += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  telemetry += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  telemetry += "\"count\":" + String(telemetryCount) + ",";
  telemetry += "\"voltage\":" + String(readBatteryVoltage(), 2);
  telemetry += "}";
  
  socket.emit("telemetry", telemetry.c_str());
  telemetryCount++;
  
  Serial.println("📊 Telemetry #" + String(telemetryCount) + " sent:");
  Serial.println("   🌡️  Temperature: " + String(currentTemp, 1) + "°C");
  Serial.println("   💧 Humidity: " + String(currentHum, 1) + "%");
  Serial.println("   💡 LED State: " + String(ledState ? "ON" : "OFF"));
  Serial.println("   📶 RSSI: " + String(WiFi.RSSI()) + " dBm");
  Serial.println("   🧠 Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
}

void updateSensorData() {
  // Simulate realistic temperature changes (18-35°C)
  float tempChange = (random(-100, 101) / 1000.0); // ±0.1°C change
  currentTemp += tempChange;
  currentTemp = constrain(currentTemp, 18.0, 35.0);
  
  // Simulate realistic humidity changes (30-80%)
  float humChange = (random(-200, 201) / 1000.0); // ±0.2% change
  currentHum += humChange;
  currentHum = constrain(currentHum, 30.0, 80.0);
  
  // Add some correlation (higher temp = lower humidity tendency)
  if (currentTemp > 30.0 && random(100) < 30) {
    currentHum -= 0.5;
  } else if (currentTemp < 22.0 && random(100) < 30) {
    currentHum += 0.5;
  }
  
  currentHum = constrain(currentHum, 30.0, 80.0);
}

float readBatteryVoltage() {
  // Simulate battery voltage (3.0-4.2V for Li-ion)
  static float voltage = 3.7;
  voltage += (random(-10, 11) / 1000.0); // Small fluctuations
  voltage = constrain(voltage, 3.0, 4.2);
  return voltage;
}

void sendHeartbeat() {
  if (!isConnected) return;
  
  String heartbeat = "{";
  heartbeat += "\"deviceId\":\"" + DEVICE_ID + "\",";
  heartbeat += "\"type\":\"heartbeat\",";
  heartbeat += "\"timestamp\":" + String(millis()) + ",";
  heartbeat += "\"uptime\":" + String(millis() / 1000);
  heartbeat += "}";
  
  socket.emit("heartbeat", heartbeat.c_str());
  Serial.println("💓 Heartbeat sent (uptime: " + String(millis() / 1000) + "s)");
}

// ================================
// CONTROL FUNCTIONS
// ================================
void handleControlCommand(const char* payload) {
  Serial.println("🎮 Control received: " + String(payload));
  
  // Parse command
  String command = String(payload);
  command.toLowerCase();
  
  bool ledChanged = false;
  
  if (command.indexOf("\"toggle\"") >= 0 || command.indexOf("toggle") >= 0) {
    ledState = !ledState;
    ledChanged = true;
    Serial.println("💡 LED toggled to: " + String(ledState ? "ON" : "OFF"));
  }
  else if (command.indexOf("\"on\"") >= 0 || command.indexOf("\"true\"") >= 0) {
    ledState = true;
    ledChanged = true;
    Serial.println("💡 LED turned ON");
  }
  else if (command.indexOf("\"off\"") >= 0 || command.indexOf("\"false\"") >= 0) {
    ledState = false;
    ledChanged = true;
    Serial.println("💡 LED turned OFF");
  }
  else {
    Serial.println("❓ Unknown command: " + command);
  }
  
  // Apply LED state to hardware
  if (ledChanged) {
    digitalWrite(LED_PIN, ledState);
    digitalWrite(EXTERNAL_LED, ledState);
    
    // Send immediate status update
    sendStatusUpdate();
    
    // Visual feedback
    if (ledState) {
      blinkLED(1, 100); // Quick blink for ON
    }
  }
}

void sendStatusUpdate() {
  String status = "{";
  status += "\"deviceId\":\"" + DEVICE_ID + "\",";
  status += "\"ledState\":" + String(ledState ? "true" : "false") + ",";
  status += "\"timestamp\":" + String(millis());
  status += "}";
  
  socket.emit("status_update", status.c_str());
}

// ================================
// UTILITY FUNCTIONS
// ================================
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("📶 WiFi disconnected! Reconnecting...");
    isConnected = false;
    isRegistered = false;
    connectToWiFi();
    
    // Reconnect to server after WiFi restoration
    if (WiFi.status() == WL_CONNECTED) {
      connectToServer();
    }
  }
}

void updateConnectionIndicator() {
  static unsigned long lastBlink = 0;
  
  if (!isConnected) {
    // Fast blink when disconnected
    if (millis() - lastBlink > 200) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastBlink = millis();
    }
  } else if (!ledState) {
    // Slow pulse when connected but LED not manually controlled
    if (millis() - lastBlink > 2000) {
      digitalWrite(LED_PIN, HIGH);
      delay(50);
      digitalWrite(LED_PIN, LOW);
      lastBlink = millis();
    }
  }
}

void blinkLED(int times, int delayMs) {
  bool originalState = digitalRead(LED_PIN);
  
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(EXTERNAL_LED, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(EXTERNAL_LED, LOW);
    delay(delayMs);
  }
  
  // Restore original state
  digitalWrite(LED_PIN, originalState);
  digitalWrite(EXTERNAL_LED, ledState);
}

// ================================
// DEBUG & INFO FUNCTIONS
// ================================
void printDeviceInfo() {
  Serial.println("\n📋 DEVICE INFORMATION");
  Serial.println("Device ID: " + DEVICE_ID);
  Serial.println("MAC Address: " + WiFi.macAddress());
  Serial.println("IP Address: " + WiFi.localIP().toString());
  Serial.println("Chip Model: " + String(ESP.getChipModel()));
  Serial.println("CPU Frequency: " + String(ESP.getCpuFreqMHz()) + " MHz");
  Serial.println("Flash Size: " + String(ESP.getFlashChipSize()) + " bytes");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  Serial.println("Telemetry Count: " + String(telemetryCount));
}

// Call this function via Serial for debugging
void handleSerialCommands() {
  if (Serial.available()) {
    String cmd = Serial.readString();
    cmd.trim();
    cmd.toLowerCase();
    
    if (cmd == "info") {
      printDeviceInfo();
    }
    else if (cmd == "led on") {
      ledState = true;
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED turned ON via serial");
    }
    else if (cmd == "led off") {
      ledState = false;
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED turned OFF via serial");
    }
    else if (cmd == "restart") {
      Serial.println("Restarting device...");
      ESP.restart();
    }
    else if (cmd == "telemetry") {
      sendTelemetry();
    }
    else {
      Serial.println("Available commands: info, led on, led off, restart, telemetry");
    }
  }
}