# 🧪 Hướng Dẫn Test Xác Thực

## ⚡ Test Nhanh (3 Bước)

### Bước 1: Chạy Server

```bash
cd d:\Dev\Javascript\tutorial_k2\buoi_7
npm install
npm start
```

Chờ đến khi thấy:
```
Multi-ESP32 Server running on http://localhost:3000
Waiting for ESP32 devices and web clients...
```

### Bước 2: Mở Dashboard (Đúng Key)

Browser → `http://localhost:3000/`

**Kết quả mong đợi**:
- ✅ Status: **Connected** (xanh)
- ✅ Có thể thấy thông báo "Authenticating..." rồi "Connected"
- ✅ Console log: "Connected to server, authenticating..."

### Bước 3: Test Sai Key

Browser → `http://localhost:3000/?key=wrong_key`

**Kết quả mong đợi**:
- ❌ Status: **Auth Failed** (đỏ)
- ❌ Alert: "Authentication failed: Invalid API Key"
- ❌ Console log: "Authentication failed: Invalid API Key"

---

## 🔐 Các API Key Hiện Tại

| Loại | Key |
|------|-----|
| **ESP32** | `esp32_secret_key_2024_buoi7` |
| **Dashboard** | `dashboard_secret_key_2024_buoi7` |

Lưu trong file: `.env`

---

## 📱 Test Với ESP32 (Khi Có Hardware)

### Bước 1: Cấu Hình ESP32

File: `components/esp32_telemetry.ino`

```cpp
// ============ THAY ĐỔI CÁC THÔNG SỐ NÀY ============

// WiFi
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_PASSWORD";

// Server (IP của máy chạy Node.js)
const char* host = "192.168.1.36";  // 👈 THAY BẰNG IP CỦA BẠN
const uint16_t port = 3000;

// API Key (PHẢI KHỚP VỚI .env)
const String API_KEY = "esp32_secret_key_2024_buoi7";
```

### Bước 2: Upload lên ESP32

Dùng Arduino IDE hoặc VS Code + PlatformIO

### Bước 3: Xem Serial Monitor

Mở Serial Monitor ở 115200 baud

**Kết quả (Nếu Key Đúng)**:
```
✅ Connected to Socket.IO server!
📡 Device registered: ESP32_001
🔐 Authentication key sent
📊 Telemetry sent:
  Temperature: 25.3°C
  Humidity: 55.2%
```

**Kết quả (Nếu Key Sai)**:
```
❌ Connected to Socket.IO server!
🚨 Authentication FAILED!
Device registration FAILED - Invalid API Key
```

---

## 🔍 Kiểm Tra Terminal

### Server Terminal

**Khi Dashboard kết nối (Đúng Key)**:
```
Web client registered: abcd1234... - Auth OK
```

**Khi Dashboard kết nối (Sai Key)**:
```
Web client registration FAILED - Invalid API Key from abcd1234...
```

**Khi ESP32 kết nối (Đúng Key)**:
```
ESP32 registered: ESP32_001 (socket_id...) - Auth OK
```

**Khi ESP32 kết nối (Sai Key)**:
```
Device registration FAILED - Invalid API Key from socket_id...
```

---

## 🧩 Test Case Chi Tiết

### Test Case 1: Dashboard Đúng + ESP32 Đúng

```
1. npm start
2. Mở http://localhost:3000/
3. (Sau này) Upload ESP32 code với key đúng

✅ Expected:
   - Dashboard: Status = "Connected"
   - Terminal: "Web client registered - Auth OK"
   - Terminal: "ESP32 registered - Auth OK"
   - Dashboard: Hiển thị device "ESP32_001"
   - Dashboard: Nhận dữ liệu temperature & humidity
```

### Test Case 2: Dashboard Sai + ESP32 Đúng

```
1. npm start
2. Mở http://localhost:3000/?key=wrong
3. Upload ESP32 code với key đúng

❌❌ Expected:
   - Dashboard: Status = "Auth Failed"
   - Terminal: "Web client registration FAILED"
   - Terminal: "ESP32 registered - Auth OK"
   - Dashboard: KHÔNG hiển thị dữ liệu (không kết nối)
```

### Test Case 3: Dashboard Đúng + ESP32 Sai

```
1. npm start
2. Mở http://localhost:3000/
3. Upload ESP32 code với key sai

✅❌ Expected:
   - Dashboard: Status = "Connected"
   - Terminal: "Web client registered - Auth OK"
   - Terminal: "Device registration FAILED"
   - Dashboard: Không có device nào (ESP32 không đăng ký)
```

### Test Case 4: Dashboard Sai + ESP32 Sai

```
1. npm start
2. Mở http://localhost:3000/?key=wrong1
3. Upload ESP32 code với key khác = wrong2

❌❌ Expected:
   - Dashboard: Status = "Auth Failed"
   - Terminal: "Web client registration FAILED"
   - Terminal: "Device registration FAILED"
   - Không có kết nối nào hoạt động
```

---

## 🐛 Debugging Tips

### 1. Kiểm Tra Server Terminal

Nếu không thấy log gì:
- Server có chạy không?
- Có lỗi JavaScript không? (Xem terminal)
- Port 3000 có bị chiếm không?

```bash
# Kiểm tra port 3000
netstat -ano | findstr :3000
```

### 2. Kiểm Tra Browser Console

F12 → Console tab

**Có lỗi gì không?**
- "Connected to server"
- "Authentication failed"
- Network errors?

### 3. Kiểm Tra ESP32 Serial

Tại 115200 baud

**Có output gì không?**
- WiFi connected?
- Socket.IO connected?
- "Auth OK" hay "Auth FAILED"?

---

## 📝 Ghi Chú

- **API Keys**: Lưu trong `.env`, không public
- **URL Parameter**: `?key=...` để test custom keys
- **Xác thực**: Server kiểm tra trước khi đăng ký
- **Sai Key**: Disconnect ngay (không cho kết nối)

---

## ✅ Checklist Test

- [ ] Server chạy thành công
- [ ] Dashboard mở với correct key
- [ ] Dashboard fail với wrong key
- [ ] Terminal log hiển thị auth messages
- [ ] Browser console không có errors
- [ ] (Optional) ESP32 kết nối và gửi dữ liệu

---

## 💡 Mẹo

**Để test nhiều keys khác nhau:**

1. Mở tab 1: `http://localhost:3000/?key=correct_key`
2. Mở tab 2: `http://localhost:3000/?key=wrong_key`
3. Xem tab 1 kết nối thành công, tab 2 fail

**Để test nhiều ESP32:**

1. Upload code với DEVICE_ID = "ESP32_001"
2. Upload code khác với DEVICE_ID = "ESP32_002"
3. Cả 2 sẽ đăng ký nếu API_KEY đúng

---

**Happy Testing!** 🚀
