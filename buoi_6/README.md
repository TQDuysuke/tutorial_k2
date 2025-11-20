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
# Buổi 6 — Advanced Multi-ESP32 Dashboard (reference)

Advanced example with auto-discovery, per-device history, DeviceManager and optional REST endpoints.

Quick start

   cd buoi_6
   npm install
   npm start

Open: http://localhost:3000 (or use `?server=` to point to a server IP)

Highlights

- Auto-create device cards when a new ESP registers
- Per-device mini-charts and telemetry history
- Targeted control commands using Socket.IO rooms (one room per device)
- Optional REST endpoints for debugging and telemetry retrieval

ESP32 notes

- Set a unique DEVICE_ID in `components/esp32_telemetry.ino` (recommended), or enable MAC-based ID.
- Configure WiFi and server IP in the sketch.
- Emit `register_device` and `telemetry` events; listen for `control` events.

Files of interest

- `js/socket_io_server.js` — main server + DeviceManager logic
- `web/index.html` — frontend dashboard
- `components/esp32_telemetry.ino` — ESP32 sketch

Deployment & production tips

- For production: run `npm install --production`, use PM2 or Docker, enable HTTPS and firewall rules.
- Default Socket.IO version in examples is v2 (2.4.1) for ESP32 client compatibility.

Troubleshooting

- Card not appearing: check ESP serial logs for successful register, and ensure DEVICE_ID is unique.
- Controls not working: confirm server logs show a room join for the device and watch network traffic.
- Performance: lower telemetry frequency, reduce stored history points, or move server off localhost.

If you'd like, I can:
- add example curl commands for REST endpoints
- add a small script to simulate multiple ESP devices for load testing
