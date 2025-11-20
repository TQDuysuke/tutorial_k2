# Buổi 7: Multi-ESP32 Dashboard với Cơ Chế Bảo Mật (Token Tĩnh)

## 📋 Tổng Quan

Buổi 7 nâng cao hệ thống Dashboard bằng cách thêm cơ chế bảo mật với **Token Tĩnh (Static API Keys)**. Mỗi ESP32 và Dashboard web đều phải gửi kèm một `secret key` khi kết nối với server.

## 🔐 Cơ Chế Bảo Mật

### Token Tĩnh (Static API Key)
- **ESP32 API Key**: `esp32_secret_key_2024_buoi7`
- **Dashboard API Key**: `dashboard_secret_key_2024_buoi7`

Các key này được lưu trữ trong file `.env` và được load vào server khi khởi động.

## 📁 Cấu Trúc Dự Án

```
buoi_7/
├── .env                           # File cấu hình (API Keys, Port)
├── package.json                   # Dependencies
├── components/
│   └── esp32_telemetry.ino       # Code ESP32 với authentication
├── js/
│   └── socket_io_server.js       # Server Socket.IO với validation
└── web/
    └── index.html                # Dashboard web với authentication
```

## ⚙️ Cách Cài Đặt

### 1. Cài Đặt Dependencies

```bash
cd buoi_7
npm install
```

Dependencies bao gồm:
- `express`: Web server framework
- `socket.io`: Real-time communication
- `dotenv`: Load biến môi trường từ `.env`

### 2. Cấu Hình File `.env`

File `.env` đã được tạo với nội dung:

```env
# Security Keys for buoi_7
ESP32_API_KEY=esp32_secret_key_2024_buoi7
DASHBOARD_API_KEY=dashboard_secret_key_2024_buoi7
PORT=3000
```

**⚠️ Bảo mật**: Không commit file `.env` lên Git! Thêm vào `.gitignore`:

```
.env
```

### 3. Cấu Hình ESP32

Mở file `components/esp32_telemetry.ino` và sửa các thông số:

```cpp
// WiFi Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Server Configuration  
const char* host = "YOUR_SERVER_IP"; // IP của máy chạy Node.js
const uint16_t port = 3000;

// Security Configuration (phải khớp với .env)
const String API_KEY = "esp32_secret_key_2024_buoi7";
```

## 🚀 Chạy Ứng Dụng

### Server Node.js

```bash
npm start
```

Hoặc dùng `nodemon` để auto-reload khi có thay đổi:

```bash
npm run dev
```

Server sẽ chạy tại: `http://localhost:3000`

### Dashboard Web

Mở browser và truy cập:
- `http://localhost:3000/` - Mở bằng API key mặc định
- `http://localhost:3000/?key=YOUR_CUSTOM_KEY` - Mở bằng custom API key

### ESP32

1. Upload code từ file `components/esp32_telemetry.ino` lên ESP32
2. Mở Serial Monitor để xem log

## 🔒 Quy Trình Xác Thực

### ESP32 Kết Nối

```
1. ESP32 tạo WiFi connection
2. ESP32 kết nối Socket.IO đến server
3. ESP32 gửi: register_device với {deviceId, apiKey}
4. Server kiểm tra apiKey
   - Nếu ĐÚNG ✅: Đăng ký device thành công
   - Nếu SAI ❌: Gửi auth_failed và disconnect
```

### Dashboard Kết Nối

```
1. Browser tạo kết nối Socket.IO đến server
2. Dashboard gửi: register_web với apiKey
3. Server kiểm tra apiKey
   - Nếu ĐÚNG ✅: Đăng ký web client thành công
   - Nếu SAI ❌: Gửi auth_failed và disconnect
```

## 📊 API Events

### Server → Client

| Event | Payload | Mô Tả |
|-------|---------|-------|
| `device_list` | `[deviceId1, deviceId2]` | Danh sách devices đã đăng ký |
| `telemetry` | `{deviceId, data, timestamp}` | Dữ liệu cảm biến từ ESP32 |
| `device_disconnected` | `deviceId` | Device ngắt kết nối |
| `auth_failed` | `{message}` | Xác thực thất bại |
| `device_status` | `[...]` | Danh sách trạng thái devices |

### Client → Server

| Event | Payload | Mô Tả |
|-------|---------|-------|
| `register_device` | `{deviceId, apiKey}` | ESP32 đăng ký |
| `register_web` | `apiKey` | Dashboard đăng ký |
| `telemetry` | `{temp, hum, ledState, ...}` | Dữ liệu từ ESP32 |
| `control_device` | `{deviceId, command}` | Điều khiển device |
| `get_devices` | - | Lấy danh sách devices |

## 🎯 Chức Năng

### Dashboard Features
- ✅ Hiển thị danh sách ESP32 kết nối
- ✅ Theo dõi nhiệt độ & độ ẩm real-time
- ✅ Biểu đồ mini cho mỗi device
- ✅ Điều khiển LED từ xa (ON/OFF/Toggle)
- ✅ Trạng thái kết nối (ONLINE/OFFLINE)
- ✅ Thời gian cập nhật cuối cùng
- ✅ Responsive design

### ESP32 Features
- ✅ Đọc dữ liệu cảm biến (temp, humidity)
- ✅ Nhận lệnh điều khiển từ dashboard
- ✅ Gửi telemetry mỗi 5 giây
- ✅ Xác thực bằng API key
- ✅ Hiển thị thông tin debug qua Serial

## 🔄 Luồng Dữ Liệu

```
┌─────────────────┐
│   ESP32 #1      │
│  (Temperature)  │
└────────┬────────┘
         │ telemetry + apiKey
         │
         ├─────────────────────────┐
         │                         │
    ┌────▼─────┐         ┌────────▼────┐
    │ Validate │         │   Validate   │
    │ ESP32    │         │  Dashboard   │
    │  Key     │         │     Key      │
    └────┬─────┘         └────────┬─────┘
         │                       │
         │                       │
    ┌────▼──────────────────────▼────┐
    │   Node.js Socket.IO Server     │
    │  (socket_io_server.js)         │
    │  ✓ Device Management           │
    │  ✓ Message Routing             │
    │  ✓ Authentication             │
    └────┬───────────────────────┬───┘
         │                       │
         │ device_list           │ telemetry
         │ device_disconnected   │
         │ auth_failed           │
         │                       │
    ┌────▼────────────────┐ ┌───▼──────────────┐
    │   Web Dashboard     │ │   ESP32 #2       │
    │ (index.html)        │ │ (Control Cmds)   │
    │ ✓ Device Grid       │ │ ✓ LED Control    │
    │ ✓ Real-time Charts  │ │ ✓ Heartbeat      │
    └────────────────────┘ └──────────────────┘
```

## 📝 Log Console

### Server Logs

```
Device Authentication Success:
ESP32 registered: ESP32_001 (...) - Auth OK

Web Client Authentication Success:
Web client registered: aho2b3c1... - Auth OK

Authentication Failure:
Device registration FAILED - Invalid API Key from ...
Auth failed message sent and connection closed
```

### Serial Monitor (ESP32)

```
Connected to WiFi: 192.168.1.50
Connected to Socket.IO server!
Device registered: ESP32_001
Authentication key sent ✅

Telemetry sent:
  Temperature: 25.3°C
  Humidity: 55.2%
  Free Heap: 125000 bytes
```

## ⚠️ Lưu Ý Bảo Mật

1. **API Keys**: Giữ bí mật, không share công khai
2. **HTTPS**: Trong production, dùng SSL/TLS
3. **.env File**: Luôn thêm vào `.gitignore`
4. **Key Rotation**: Thay đổi keys định kỳ
5. **Environment-specific**: Dùng keys khác nhau cho dev/staging/production

## 🐛 Troubleshooting

### Dashboard không kết nối
```
❌ Kểm tra:
- API key trong URL có khớp với .env không?
- Server có đang chạy không?
- Firewall có chặn port 3000 không?
```

### ESP32 không đăng ký được
```
❌ Kiểm tra:
- API_KEY trong code có khớp với ESP32_API_KEY trong .env không?
- WiFi connection có ổn không?
- Server IP có chính xác không?
- Port 3000 có accessible không?
```

### auth_failed message
```
❌ API key không khớp:
- Kiểm tra cách ESP32 gửi API key
- Kiểm tra cách web client gửi API key
- Đảm bảo keys trong .env chính xác
```

## 📚 Tài Liệu Tham Khảo

- [Socket.IO Docs](https://socket.io/docs/)
- [dotenv](https://www.npmjs.com/package/dotenv)
- [Express.js](https://expressjs.com/)
- [ESP32 Arduino](https://github.com/espressif/arduino-esp32)

## 🎓 Học Tiếp

Buổi 7 là bước đầu trong việc thêm bảo mật cho IoT system. Các bước tiếp theo có thể bao gồm:

1. **JWT Tokens**: Thay vì static keys, dùng JWT với expiration
2. **OAuth2**: Xác thực qua Google, GitHub
3. **Encryption**: Mã hóa dữ liệu truyền tải
4. **Database**: Lưu trữ users, devices, API keys
5. **Rate Limiting**: Ngăn chặn brute force attacks

---

**Author**: K2 Tutorial Team  
**Date**: November 2024  
**Version**: 1.0.0
