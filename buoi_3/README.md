# Buổi 3 — ESP32 Telemetry Dashboard (simple)

Một ví dụ cơ bản: ESP32 gửi telemetry -> server Node.js (Socket.IO v2) -> dashboard web realtime.

Quick start

1. Mở terminal, vào thư mục `buoi_3` và cài dependencies:

	cd buoi_3
	npm install

2. Chạy server:

	npm start

3. Mở trình duyệt: http://localhost:3000

Notes for ESP32

- Use a Socket.IO v2-compatible client on the ESP32.
- After connect, emit a register event:

  socket.emit('register', 'esp');

- Send telemetry with event `telemetry` (JSON object/string). Listen for `control` events for LED commands.

Server notes

- `socket.io@2.4.1` is used for ESP32 compatibility. Upgrading Socket.IO on the server may break the native ESP32 client.
- Open port 3000 in firewall if devices are on different networks.

Files

- `js/socket_io_server.js` — server entry
- `web/index.html` — frontend dashboard (supports `?server=` query parameter)

Troubleshooting

- No data: check ESP is connected to same network, and that it emits `register` and `telemetry` events.
- Quick test: open `http://localhost:3000/?server=http://<SERVER_IP>:3000` to force a server IP.

