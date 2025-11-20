# Multi-ESP32 IoT Dashboard - Buổi 5

## 🌟 Tính năng chính

✅ **Multi-Device Support**: Kết nối và quản lý nhiều ESP32 cùng lúc  
✅ **Device ID**: Mỗi ESP32 có deviceId riêng (ESP32_001, ESP32_002, v.v.)  
✅ **Dynamic Cards**: Dashboard tự động tạo card cho từng thiết bị  
✅ **Individual Control**: Điều khiển LED riêng biệt cho từng ESP32  
✅ **Real-time Monitoring**: Telemetry realtime với mini-charts  
✅ **Device Status**: Hiển thị trạng thái online/offline  
✅ **Room Management**: Sử dụng Socket.IO rooms để phân tách dữ liệu  

# Buổi 5 — Multi-ESP32 IoT Dashboard (tóm tắt)

Một ví dụ đơn giản cho hệ thống: nhiều ESP32 gửi telemetry đến một server Node.js (Socket.IO v2), server relay dữ liệu cho dashboard web realtime.

Ngắn gọn (Quick start)

1) Cài dependencies và chạy server (Windows cmd):

   cd buoi_5
   npm install
   npm start

2) Mở dashboard: http://localhost:3000

3) Nếu dashboard và server khác host: http://localhost:3000/?server=http://IP_SERVER:3000

ESP32 (sketch)
- File: `components/esp32_telemetry.ino`
- Thay `DEVICE_ID` cho mỗi module:

  String DEVICE_ID = "ESP32_001"; // đổi thành ESP32_002, ESP32_Kitchen, ...

- Cấu hình WiFi & server trong sketch:

  const char* ssid = "YOUR_WIFI_NAME";
  const char* password = "YOUR_WIFI_PASSWORD";
  const char* host = "192.168.1.36"; // IP server

- Khi kết nối thành công, ESP gửi event đăng ký và telemetry, ví dụ:

  socket.emit("register_device", "ESP32_001");
  socket.emit("telemetry", telemetryObject);

Dashboard features
- Tự động tạo card cho từng device
- Mini-chart và telemetry realtime
- Nút điều khiển LED (Toggle / ON / OFF) cho từng device
- Hiển thị trạng thái online/offline và last-seen

Troubleshooting
- Không thấy device: kiểm tra WiFi, IP server, port 3000, và serial logs của ESP32.
- Duplicate card: đảm bảo DEVICE_ID là duy nhất; restart server nếu cần.
- Control không hoạt động: kiểm tra server log khi nhấn nút và ESP có join đúng room hay chưa.

Gợi ý tiếp theo (tôi có thể làm nếu bạn muốn)
- Chuẩn hoá script npm (start/dev) trong mọi `buoi_X`
- Viết hướng dẫn flash ESP32 nhanh (esptool hoặc Arduino IDE)
- Viết script test nhỏ để giả lập nhiều ESP gửi telemetry

---
Author: tutorial_k2
License: MIT
### Environmental Monitoring
- Station_North: Trạm phía Bắc
- Station_South: Trạm phía Nam
- Station_Center: Trạm trung tâm

---

**Tác giả**: ESP32 IoT Team  
**Version**: 1.0.0  
**License**: MIT