# 🏭 Advanced Multi-ESP32 IoT Dashboard - Buổi 6

## 🌟 Tính năng nâng cao

### ✨ **Auto-Discovery & Dynamic Cards**
- 🔄 **Tự động tạo card** khi ESP32 gửi telemetry lần đầu
- 🎯 **Unique Device ID** cho từng ESP32 (ESP32_Kitchen, ESP32_Living, v.v.)
- 📱 **Responsive grid** tự động sắp xếp cards
- 🗑️ **Auto-cleanup** cards khi thiết bị offline

### 📊 **Individual Charts & Telemetry**
- 📈 **Chart riêng** cho từng ESP32 với Chart.js
- 🌡️ **Real-time data**: Nhiệt độ, độ ẩm, trạng thái LED
- 📉 **Change indicators**: Hiển thị xu hướng tăng/giảm
- 🔢 **Data counters**: Số lượng điểm dữ liệu nhận được

### 🎮 **Individual Device Control**
- 💡 **LED control** riêng biệt cho từng ESP32
- 🎯 **Targeted commands**: Chỉ thiết bị được chọn nhận lệnh
- ⚡ **Instant feedback**: Cập nhật trạng thái ngay lập tức
- 🔄 **3 control modes**: Toggle, ON, OFF

### 🔧 **Advanced Server Features**
- 🏗️ **DeviceManager class** quản lý thiết bị chuyên nghiệp
- 📚 **Telemetry history**: Lưu trữ 100 điểm dữ liệu/thiết bị
- 🛡️ **Connection monitoring**: Auto-detect offline devices
- 🌐 **RESTful APIs**: Endpoints cho debug và monitoring

## 🚀 Cài đặt và triển khai

### 1️⃣ Setup Server

```bash
# Clone hoặc tạo project
cd d:\Dev\Javascript\tutorial_k2\buoi_6

# Cài đặt dependencies
npm install

# Khởi chạy server
npm start

# Hoặc development mode với auto-reload
npm run dev
```

### 2️⃣ Cấu hình ESP32

#### Thay đổi Device ID (QUAN TRỌNG!)
```cpp
// Trong file esp32_telemetry.ino, dòng ~8
String DEVICE_ID = "ESP32_Kitchen";  // Thay đổi cho từng ESP32

// Ví dụ các device ID:
// ESP32_Living, ESP32_Bedroom, ESP32_Garden
// Sensor_01, Sensor_02, Kitchen_Monitor
// Workshop_Temp, Office_Climate, etc.
```

#### WiFi & Server Settings
```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* host = "192.168.1.36"; // IP của máy chạy server
```

#### Optional: Sử dụng MAC Address làm Device ID
```cpp
// Uncomment dòng này để dùng MAC address
#define USE_MAC_AS_DEVICE_ID
```

### 3️⃣ Mở Dashboard

- **Local**: http://localhost:3000
- **LAN**: http://192.168.1.36:3000
- **Custom Server**: http://localhost:3000/?server=http://IP:3000

## 📱 Dashboard Features

### 🏠 Header Dashboard
- **Device Counter**: Hiển thị tổng số thiết bị
- **Online Status**: Số thiết bị đang online
- **Connection Status**: Trạng thái kết nối server

### 📋 Device Cards

Mỗi ESP32 có 1 card riêng bao gồm:

#### 📊 **Metrics Section**
- 🌡️ **Temperature**: Giá trị hiện tại + xu hướng thay đổi
- 💧 **Humidity**: Giá trị hiện tại + xu hướng thay đổi  
- ⏰ **Last Update**: Thời gian nhận dữ liệu cuối
- 🔢 **Data Points**: Số lượng telemetry đã nhận

#### 🎮 **Control Section**
```
[Toggle LED] [LED ON] [LED OFF]
```
- **Toggle**: Đảo trạng thái LED hiện tại
- **LED ON**: Bật LED (độc lập với ESP32 khác)
- **LED OFF**: Tắt LED (chỉ ESP32 này)

#### 📈 **Individual Chart**
- **Dual-axis**: Nhiệt độ (trái) + Độ ẩm (phải)
- **Real-time updates**: Cập nhật khi có dữ liệu mới
- **30-point history**: Giữ 30 điểm dữ liệu gần nhất
- **Smooth animations**: Transitions mượt mà

#### 🔍 **Status Indicators**
- **🟢 Online**: Thiết bị active và gửi data
- **🔴 Offline**: Không nhận data > 10 giây  
- **Visual feedback**: Card mờ đi khi offline

## 🏗️ Kiến trúc hệ thống

### 📡 **Communication Flow**

```
ESP32_Kitchen ──┐
ESP32_Living  ──┤──► DeviceManager ──► WebSocket ──► Dashboard
ESP32_Garden  ──┘    (Server)           (Relay)      (Cards)
```

### 🔄 **Auto-Discovery Process**

1. **ESP32 connects** → Server tạo device entry
2. **ESP32 registers** → Server xác nhận device ID  
3. **ESP32 sends telemetry** → Dashboard tự tạo card
4. **Real-time updates** → Card cập nhật liên tục
5. **ESP32 disconnects** → Card marked offline → Auto-cleanup sau 60s

### 📚 **Data Structure**

#### Device Registry (Server)
```javascript
{
  "ESP32_Kitchen": {
    socketId: "abc123",
    deviceId: "ESP32_Kitchen", 
    lastSeen: 1640995200000,
    isOnline: true,
    telemetryCount: 245,
    ledState: false
  }
}
```

#### Telemetry History
```javascript
{
  "ESP32_Kitchen": [
    {
      temp: 25.5,
      hum: 60.2,
      ledState: false,
      uptime: 12345,
      timestamp: 1640995200000
    }
  ]
}
```

## 🔧 ESP32 Code Features

### 🚀 **Advanced Initialization**
```cpp
✅ Hardware setup (LEDs, sensors)
✅ WiFi connection with retry logic  
✅ Auto device ID generation
✅ Server registration with confirmation
✅ Initial telemetry transmission
```

### 📊 **Rich Telemetry Data**
```json
{
  "deviceId": "ESP32_Kitchen",
  "temp": 25.47,
  "hum": 58.3,
  "ledState": false,
  "uptime": 3600,
  "freeHeap": 234567,
  "rssi": -42,
  "count": 1200,
  "voltage": 3.85
}
```

### 🎮 **Control Command Handling**
- **Toggle**: `{"cmd": "toggle"}` → Đảo trạng thái LED
- **ON**: `{"cmd": "on"}` → Bật LED + external LED
- **OFF**: `{"cmd": "off"}` → Tắt LED + external LED
- **Status feedback**: Gửi confirmation về server

### 🔄 **Connection Management**
- **WiFi monitoring**: Auto-reconnect khi mất mạng
- **Socket reconnection**: Tự động kết nối lại server
- **Heartbeat**: Gửi ping 30s/lần để maintain connection
- **Visual indicators**: LED blink patterns cho connection status

### 🛠️ **Debug Features**
```cpp
// Serial commands for debugging
"info"      → Hiển thị device information
"led on"    → Bật LED qua serial
"led off"   → Tắt LED qua serial  
"restart"   → Restart ESP32
"telemetry" → Gửi telemetry test
```

## 🌐 RESTful API Endpoints

Server cung cấp các API endpoints để debugging:

### GET /api/devices
```json
[
  {
    "deviceId": "ESP32_Kitchen",
    "isOnline": true,
    "lastSeen": 1640995200000,
    "telemetryCount": 245,
    "ledState": false,
    "registeredAt": 1640991600000
  }
]
```

### GET /api/telemetry/:deviceId?limit=50
```json
[
  {
    "temp": 25.5,
    "hum": 60.2,
    "ledState": false,
    "timestamp": 1640995200000
  }
]
```

## 🧪 Testing Scenarios

### 🏠 **Smart Home Setup**
```
ESP32_Living    → Phòng khách (temp/hum + LED strip)
ESP32_Kitchen   → Bếp (temp/hum + exhaust fan control)  
ESP32_Bedroom   → Phòng ngủ (temp/hum + night light)
ESP32_Garden    → Vườn (soil moisture + irrigation pump)
```

### 🏭 **Industrial Monitoring**
```  
Sensor_Line1    → Dây chuyền 1 (nhiệt độ máy)
Sensor_Line2    → Dây chuyền 2 (độ ẩm kho)
Quality_Check   → Kiểm tra chất lượng
Safety_Monitor  → An toàn lao động
```

### 🔬 **Research Lab**
```
Lab_Incubator   → Tủ ấm (temp control)
Lab_Freezer     → Tủ đông (temp monitoring)  
Lab_Humidity    → Phòng sạch (humidity control)
Lab_Pressure    → Áp suất phòng
```

## 📈 Performance & Scalability

### ⚡ **Optimizations**
- **Chart updates**: No animation cho performance
- **Data limits**: 50 telemetry points per device
- **Memory management**: Auto-cleanup inactive devices  
- **Efficient rendering**: Only update changed elements

### 📏 **Scale Limits**
- **Recommended**: 10-15 ESP32 devices
- **Maximum tested**: 25+ devices  
- **Network**: Giới hạn bởi WiFi bandwidth
- **Browser**: Modern browsers handle 50+ charts tốt

### 🔧 **Tuning Parameters**
```javascript
// Server config
maxHistoryPoints: 100,        // Telemetry history per device
deviceTimeout: 60000,         // Device offline timeout (60s)
cleanupInterval: 30000,       // Cleanup check interval (30s)

// ESP32 config  
TELEMETRY_INTERVAL: 3000,     // Send telemetry every 3s
HEARTBEAT_INTERVAL: 30000,    // Send heartbeat every 30s
```

## 🛠️ Troubleshooting

### ❌ **Card không xuất hiện**
1. ✅ Kiểm tra ESP32 đã register thành công chưa (Serial monitor)
2. ✅ Đảm bảo Device ID unique (không trùng với ESP32 khác)
3. ✅ Kiểm tra telemetry JSON format hợp lệ
4. ✅ Mở F12 Developer Tools xem lỗi JavaScript

### ❌ **Control không hoạt động**
1. ✅ ESP32 đã join đúng room chưa (`device_${deviceId}`)
2. ✅ Kiểm tra server logs khi click nút control
3. ✅ Test bằng Serial commands trên ESP32
4. ✅ Đảm bảo socket connection ổn định

### ❌ **Chart không cập nhật**  
1. ✅ Kiểm tra telemetry data có temp/hum hợp lệ không
2. ✅ Chart.js library đã load chưa (check Console)
3. ✅ Browser memory đầy → restart browser
4. ✅ Quá nhiều data points → giảm history limit

### ❌ **Performance issues**
1. ✅ Giảm số ESP32 đồng thời (< 15 devices)
2. ✅ Tăng telemetry interval (5-10s thay vì 3s) 
3. ✅ Giảm chart data points (20 thay vì 50)
4. ✅ Dùng dedicated server (không localhost)

## 🔐 Security Considerations

### 🛡️ **Network Security**
- Dùng WPA3 WiFi với strong password
- Firewall rules cho port 3000
- VPN access cho remote monitoring
- HTTPS trong production environment

### 🔒 **Device Security**
- Unique Device IDs (tránh conflicts)
- Input validation cho telemetry data  
- Rate limiting để tránh spam
- Authentication cho critical commands

## 🚀 Production Deployment

### ☁️ **Cloud Deployment**
```bash
# Deploy lên cloud server (AWS, DigitalOcean, etc.)
git clone your-repo
cd buoi_6
npm install --production
npm start
```

### 🐳 **Docker Deployment**
```dockerfile
FROM node:16-alpine
WORKDIR /app
COPY package*.json ./
RUN npm install --production
COPY . .
EXPOSE 3000
CMD ["npm", "start"]
```

### ⚙️ **Process Management**
```bash
# Sử dụng PM2 cho production
npm install -g pm2
pm2 start js/socket_io_server.js --name "esp32-dashboard"
pm2 startup
pm2 save
```

## 🎯 Future Enhancements

### 📊 **Data Analytics**
- Historical data export (CSV, JSON)
- Alert system (email/SMS khi temp > threshold)
- Data visualization dashboard (trends, statistics)
- Machine learning predictions

### 📱 **Mobile Features**  
- PWA (Progressive Web App) support
- Push notifications cho alerts
- Offline capability với service workers
- Touch-friendly mobile interface

### 🔧 **Advanced Controls**
- Slider controls (dimmer, fan speed)
- Scheduling system (turn on/off at specific times)  
- Automation rules (if temp > 30°C then turn on fan)
- Remote firmware OTA updates

---

## 📞 Support & Contributing

**Tác giả**: Advanced IoT Team  
**Version**: 2.0.0  
**License**: MIT  
**Last Updated**: September 2025

Nếu có vấn đề hoặc suggestions, hãy tạo issue trên GitHub repo!

---

**🎉 Chúc bạn thành công với Advanced Multi-ESP32 Dashboard!** 🎉