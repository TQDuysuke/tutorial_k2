# Buổi 4 — Dashboard with realtime charts

Ví dụ: realtime charts (Chart.js), current readings and LED control.

Quick start

   cd buoi_4
   npm install
   npm start

Open: http://localhost:3000 (or add `?server=http://<IP>:3000`)

ESP32 notes

- Emit `register` after connection and `telemetry` events with JSON payloads.
- Listen for `control` events to toggle LED.

Files

- `js/socket_io_server.js` — server
- `web/index.html` — frontend (uses Chart.js for realtime charts)

Troubleshooting

- Chart empty: check telemetry JSON format, open browser console.
- Connection issues: check server IP/port and network/firewall.
- Chart lag: increase telemetry interval or reduce max data points.
