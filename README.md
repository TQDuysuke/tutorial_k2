# ESP32 Telemetry Dashboard - tutorial_k2

This repository contains a set of example projects (buoi_1 .. buoi_6) that demonstrate a simple ESP32 -> Node.js (Socket.IO) -> Web dashboard pipeline for realtime telemetry and device control.

Structure
- `buoi_X/` - examples for each session (buoi = lesson). Each may contain:
  - `components/esp32_telemetry.ino` - Arduino sketch for ESP32
  - `js/socket_io_server.js` - Node.js Express + Socket.IO server
  - `web/index.html` - frontend dashboard (in some sessions)

Quick start (example: `buoi_5`)
1. Open a terminal and install dependencies:

   cd buoi_5
   npm install

2. Start the server:

   npm start

3. Open the dashboard in your browser:

   http://localhost:3000

   To point the dashboard to a remote server use `?server=`, for example:

   http://localhost:3000/?server=http://192.168.1.36:3000

ESP32 notes
- The ESP32 sketches use a Socket.IO client compatible with Socket.IO v2. The server in these examples pins `socket.io@2.4.1` for compatibility.
- The sketch should emit `register` (or `register_device`) to identify itself, and `telemetry` events with JSON payloads.

Where to look
- `buoi_3/` - simple single-device dashboard
- `buoi_4/` - realtime charts and LED control
- `buoi_5/` - multi-device dashboard with deviceId support (recommended)
- `buoi_6/` - advanced features, device manager, REST endpoints

If you want, I can:
- run quick smoke tests for each `buoi_X/js` server
- standardize package.json scripts across sessions (install/start)
- add a CONTRIBUTING.md or a short HOWTO for flashing the ESP32

---
Author: repository maintainer
