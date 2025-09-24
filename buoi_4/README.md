# ESP32 Dashboard with Realtime Charts - Buổi 4

Dashboard web hiển thị telemetry từ ESP32 với biểu đồ realtime và điều khiển LED.

## Tính năng

- ✅ Hiển thị nhiệt độ & độ ẩm hiện tại (text)
- ✅ Biểu đồ realtime (line chart, cập nhật liên tục)
- ✅ Nút điều khiển LED (Toggle/ON/OFF)
- ✅ Trạng thái kết nối realtime
- ✅ Chart.js để vẽ biểu đồ mượt mà

## Cài đặt và chạy

1. **Cài dependencies:**
```cmd
cd buoi_4
npm install
```

2. **Khởi chạy server:**
```cmd
npm start
```

3. **Mở dashboard:**
- Cùng máy: http://localhost:3000
- Từ máy khác: http://localhost:3000/?server=http://IP_SERVER:3000

## ESP32 Setup

ESP32 cần:
- Kết nối WiFi và server Socket.IO
- Gửi `socket.emit('register', 'esp')` khi kết nối
- Gửi telemetry: `socket.emit('telemetry', '{"temp":25.5,"hum":60}')`
- Lắng nghe event `'control'` để điều khiển LED

## Dashboard Features

### Current Readings
- Hiển thị giá trị nhiệt độ, độ ẩm và thời gian cập nhật cuối

### Realtime Chart
- Biểu đồ line chart với 2 trục Y (nhiệt độ bên trái, độ ẩm bên phải)
- Cập nhật realtime không animation để tăng performance
- Giữ tối đa 50 điểm dữ liệu gần nhất

### LED Control
- Toggle LED: đảo trạng thái
- LED ON: bật LED
- LED OFF: tắt LED

## Troubleshooting

1. **Chart không hiển thị:**
   - Kiểm tra ESP32 có gửi đúng format JSON không
   - Mở Developer Tools (F12) để xem lỗi

2. **Mất kết nối:**
   - Kiểm tra IP server và port 3000 có mở không
   - ESP32 và dashboard cùng mạng LAN

3. **Biểu đồ lag:**
   - Giảm tần suất gửi telemetry từ ESP32 (từ 5s lên 10s)
   - Giảm `maxDataPoints` trong code nếu cần

## Dependencies

- **express**: Web server
- **socket.io@2.4.1**: Tương thích với ESP32 SocketIoClient
- **Chart.js**: Thư viện vẽ biểu đồ (CDN)