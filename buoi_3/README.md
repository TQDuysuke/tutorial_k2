ESP32 Telemetry Dashboard (buoi_3)

This folder contains a small Express + Socket.IO (v2) server and a web dashboard that shows telemetry from ESP32 and lets you send LED control commands.

Quick start:

1. Install dependencies (in buoi_3 folder):

```bash
npm install
```

2. Start the server:

```bash
npm start
```

3. Open browser at http://localhost:3000

4. Make sure your ESP32 code connects to this server and emits `register` with `'esp'` or sends telemetry events. The server will forward telemetry to web clients and forward `control` events from web to ESP.

Notes:
- This project expects Socket.IO v2 for ESP32 `SocketIoClient` compatibility.
- If your server machine has a firewall, allow port 3000.
