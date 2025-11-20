# Quick Start - Buổi 7 với Bảo Mật

## 🚀 Khởi Động Nhanh

### 1️⃣ Cài Đặt Dependencies

```bash
cd d:\Dev\Javascript\tutorial_k2\buoi_7
npm install
```

### 2️⃣ Cấu Hình

Tất cả API keys đã được thiết lập trong file `.env`:

```env
ESP32_API_KEY=esp32_secret_key_2024_buoi7
DASHBOARD_API_KEY=dashboard_secret_key_2024_buoi7
PORT=3000
```

### 3️⃣ Chạy Server

```bash
npm start
```

Hoặc auto-reload:
```bash
npm run dev
```

**Output:**
```
Multi-ESP32 Server running on http://localhost:3000
Waiting for ESP32 devices and web clients...
```

### 4️⃣ Mở Dashboard

Browser: `http://localhost:3000`

---

## 📡 Cấu Hình ESP32

Sửa trong `components/esp32_telemetry.ino`:

```cpp
// WiFi
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

// Server
const char* host = "192.168.1.36";  // IP của máy chạy Node.js
const uint16_t port = 3000;

// API Key (PHẢI KHỚP VỚI .env)
const String API_KEY = "esp32_secret_key_2024_buoi7";
```

---

## 🔐 Cơ Chế Xác Thực

### ESP32 Gửi Tín Hiệu Đăng Ký

```json
{
  "deviceId": "ESP32_001",
  "apiKey": "esp32_secret_key_2024_buoi7"
}
```

### Dashboard Gửi Tín Hiệu Đăng Ký

```
apiKey: "dashboard_secret_key_2024_buoi7"
```

### Server Kiểm Tra

✅ **Nếu key khớp**: Đăng ký thành công
❌ **Nếu key sai**: Gửi `auth_failed` và disconnect

---

## 📊 Dữ Liệu Real-time

Sau khi xác thực thành công:

```
ESP32 → Server: telemetry {temp, humidity, ledState, ...}
Server → Dashboard: telemetry {deviceId, data, timestamp}
Dashboard → Server: control_device {deviceId, command}
Server → ESP32: control {command}
```

---

## 🧪 Test Xác Thực

### Test 1: Kết Nối Đúng API Key

**Dự kiến**: ✅ Kết nối thành công, hiển thị "Connected"

### Test 2: Kết Nối Sai API Key

**Dự kiến**: ❌ Kết nối thất bại, hiển thị "Auth Failed"

### Cách Test:

1. Mở `index.html` với custom key:
   ```
   http://localhost:3000/?key=wrong_key
   ```
   Kết quả: `auth_failed`

2. Sửa API_KEY trong ESP32 code và upload
   Kết quả: Device không đăng ký được, thấy `auth_failed` trong Serial Monitor

---

## 📝 File Quan Trọng

| File | Mô Tả |
|------|-------|
| `.env` | Lưu trữ API keys (⚠️ không commit) |
| `js/socket_io_server.js` | Server với validation |
| `web/index.html` | Dashboard với xác thực |
| `components/esp32_telemetry.ino` | ESP32 code với auth |
| `SECURITY.md` | Tài liệu chi tiết về bảo mật |

---

## ✅ Checklist Kiểm Tra

- [ ] Dependencies cài đặt xong (`npm install`)
- [ ] Server chạy bình thường (`npm start`)
- [ ] Dashboard mở được tại `http://localhost:3000`
- [ ] Có thông báo "Connected" trên dashboard
- [ ] ESP32 code cấu hình đúng WiFi & Server IP
- [ ] ESP32 hiển thị "Device registered: ESP32_001"
- [ ] Dashboard nhận được dữ liệu từ ESP32 (temp, humidity)
- [ ] Có thể điều khiển LED từ dashboard

---

## 🐛 Debug

### Không kết nối được:

```bash
# Kiểm tra port 3000 có đang chạy không
netstat -ano | findstr :3000

# Kill process nếu cần
taskkill /PID <PID> /F
```

### Xem console.log:

- **Server**: Terminal chạy `npm start`
- **ESP32**: Serial Monitor tại 115200 baud
- **Dashboard**: Browser F12 → Console

---

## 📚 Tài Liệu Đầy Đủ

Xem `SECURITY.md` để hiểu chi tiết hơn về cơ chế bảo mật.

---

**Mọi thắc mắc, xem SECURITY.md hoặc hỏi trong nhóm!** 🎓
