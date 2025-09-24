# Multi-ESP32 IoT Dashboard - Buổi 5

## 🌟 Tính năng chính

✅ **Multi-Device Support**: Kết nối và quản lý nhiều ESP32 cùng lúc  
✅ **Device ID**: Mỗi ESP32 có deviceId riêng (ESP32_001, ESP32_002, v.v.)  
✅ **Dynamic Cards**: Dashboard tự động tạo card cho từng thiết bị  
✅ **Individual Control**: Điều khiển LED riêng biệt cho từng ESP32  
✅ **Real-time Monitoring**: Telemetry realtime với mini-charts  
✅ **Device Status**: Hiển thị trạng thái online/offline  
✅ **Room Management**: Sử dụng Socket.IO rooms để phân tách dữ liệu  

## 🚀 Cài đặt và chạy

### 1. Cài đặt Server
```bash
cd buoi_5
npm install
npm start
```

### 2. Mở Dashboard
- **Local**: http://localhost:3000
- **Remote**: http://localhost:3000/?server=http://192.168.1.36:3000

### 3. Cấu hình ESP32

#### Thay đổi Device ID
Trong file `esp32_telemetry.ino`, thay đổi dòng:
```cpp
const String DEVICE_ID = "ESP32_001";  // ESP32_002, ESP32_003, etc.
```

#### WiFi và Server
```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* host = "192.168.1.36"; // IP của server
```

## 📱 Dashboard Features

### Device Cards
Mỗi ESP32 có một card riêng hiển thị:
- **Device ID** và trạng thái (Online/Offline)
- **Telemetry hiện tại**: Nhiệt độ, độ ẩm
- **Mini-chart**: Biểu đồ realtime nhỏ gọn
- **Control buttons**: Toggle, LED ON, LED OFF
- **Last seen**: Thời gian cập nhật cuối

### Auto-Discovery
- Server tự động phát hiện ESP32 mới
- Dashboard tự động tạo card cho thiết bị mới
- Tự động xóa card khi thiết bị ngắt kết nối

### Device Management
- **Room-based**: Mỗi ESP32 join room riêng (`device_ESP32_001`)
- **Targeted Control**: Lệnh điều khiển gửi đúng thiết bị
- **Timeout**: Tự động xóa thiết bị offline sau 30s

## 🔧 ESP32 Code Features

### Device Registration
```cpp
// ESP32 tự đăng ký với server
socket.emit("register_device", "\"ESP32_001\"");
```

### Telemetry Data
```json
{
  "deviceId": "ESP32_001",
  "temp": 25.5,
  "hum": 60.2,
  "ledState": false,
  "uptime": 12345,
  "freeHeap": 234567,
  "rssi": -45
}
```

### Control Commands
ESP32 lắng nghe event `control`:
- `{"cmd": "toggle"}` - Đảo trạng thái LED
- `{"cmd": "on"}` - Bật LED
- `{"cmd": "off"}` - Tắt LED

### Connection Indicators
- LED nhấp nháy nhẹ khi kết nối thành công
- Serial monitor hiển thị logs chi tiết
- Tự động reconnect khi mất WiFi

## 🏗️ Kiến trúc hệ thống

### Server Architecture
```
ESP32_001 ──┐
ESP32_002 ──┤──► Socket.IO Server ──► Web Dashboard
ESP32_003 ──┘         (Rooms)              (Cards)
```

### Socket Events
- `register_device` - ESP32 đăng ký với deviceId
- `register_web` - Web client đăng ký
- `telemetry` - Dữ liệu sensor từ ESP32
- `control_device` - Lệnh điều khiển từ web
- `device_list` - Danh sách thiết bị active

### Room Management
```javascript
// ESP32 join room theo deviceId
socket.join(`device_${deviceId}`);

// Gửi control đến room cụ thể
io.to(`device_${deviceId}`).emit('control', command);
```

## 📊 Monitoring Features

### Device Status
- **Online**: Thiết bị gửi telemetry trong 10s qua
- **Offline**: Không nhận dữ liệu > 10s
- **Timeout**: Xóa thiết bị sau 30s offline

### Data Visualization
- **Real-time values**: Nhiệt độ, độ ẩm hiện tại
- **Mini charts**: Line chart 20 điểm dữ liệu gần nhất
- **Device count**: Số thiết bị kết nối
- **Connection status**: Trạng thái server

## 🔬 Testing với nhiều ESP32

### Setup Multiple Devices
1. **Flash code lên nhiều ESP32** với Device ID khác nhau:
   - ESP32 #1: `DEVICE_ID = "ESP32_001"`
   - ESP32 #2: `DEVICE_ID = "ESP32_002"`
   - ESP32 #3: `DEVICE_ID = "Kitchen_Sensor"`

2. **Cấp nguồn tất cả ESP32**

3. **Mở Dashboard** - sẽ thấy cards xuất hiện tự động

### Test Individual Control
- Click "LED ON" trên card ESP32_001 → chỉ LED của ESP32_001 sáng
- Click "Toggle" trên card ESP32_002 → chỉ LED của ESP32_002 đảo trạng thái
- Telemetry của mỗi thiết bị cập nhật độc lập

## 🛠️ Troubleshooting

### ESP32 không xuất hiện trên Dashboard
1. Kiểm tra WiFi connection (Serial monitor)
2. Kiểm tra IP server đúng không
3. Đảm bảo port 3000 không bị firewall chặn
4. Xem logs server: `npm start`

### Cards bị duplicate
- Đảm bảo mỗi ESP32 có DEVICE_ID khác nhau
- Restart server nếu cần: `Ctrl+C` → `npm start`

### Telemetry không cập nhật
- Kiểm tra format JSON trong ESP32 code
- Mở F12 Developer Tools xem errors
- Kiểm tra connection status trên dashboard

### Control không hoạt động
- Đảm bảo ESP32 đã đăng ký thành công
- Kiểm tra room join trong server logs
- Test với Serial monitor ESP32

## 📈 Mở rộng

### Thêm sensors
```cpp
float temperature = dht.readTemperature();
float humidity = dht.readHumidity();
int soilMoisture = analogRead(A0);
```

### Database integration
```javascript
// Lưu telemetry vào MongoDB/MySQL
socket.on('telemetry', (data) => {
  saveToDatabase(data.deviceId, data.data);
});
```

### Alert system
```javascript
// Cảnh báo khi nhiệt độ cao
if (telemetry.temp > 35) {
  io.emit('alert', { deviceId, message: 'High temperature!' });
}
```

## 🎯 Sử dụng thực tế

### Smart Home
- ESP32_Kitchen: Cảm biến bếp
- ESP32_Living: Cảm biến phòng khách  
- ESP32_Garden: Cảm biến vườn

### Industrial IoT
- Sensor_Line1: Dây chuyền 1
- Sensor_Line2: Dây chuyền 2
- Quality_Check: Kiểm tra chất lượng

### Environmental Monitoring
- Station_North: Trạm phía Bắc
- Station_South: Trạm phía Nam
- Station_Center: Trạm trung tâm

---

**Tác giả**: ESP32 IoT Team  
**Version**: 1.0.0  
**License**: MIT