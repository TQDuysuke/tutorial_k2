ESP32 Telemetry Dashboard (buoi_3)

This folder contains a small Express + Socket.IO (v2) server and a web dashboard that shows telemetry from ESP32 and lets you send LED control commands.

Quick start

1. Install dependencies (run inside `buoi_3`):

```bash
npm install
```

2. Start the server:

```bash
npm start
```

3. Open the dashboard in your browser:

Default (server and frontend on same host):

```
http://localhost:3000
```

Specify a server IP (if dashboard is served from a different host or you want to point to a LAN IP):

```
http://localhost:3000/?server=http://192.168.1.36:3000
```

Replace `http://192.168.1.36:3000` with your server's IP and port.

ESP32 (client) requirements

- The ESP32 client should use `SocketIoClient` (compatible with Socket.IO v2).
- After connecting, the ESP must register so the server can forward messages to it:

```js
socket.emit('register', 'esp');
```

- Send telemetry using the `telemetry` event, payload can be a JSON string or object, e.g. `{"temp":25.5,"hum":60}`.
- Listen for `control` events on the ESP and implement LED control accordingly.

Server notes

- This project pins `socket.io@2.4.1` in `package.json` for ESP32 compatibility. If you upgrade Socket.IO on the server, the ESP32 `SocketIoClient` may not work correctly.
- If the server machine has a firewall, allow TCP port 3000.

Files of interest

- `js/socket_io_server.js` — Express + Socket.IO server that relays telemetry and control messages.
- `web/index.html` — frontend dashboard connecting to the server via Socket.IO. It accepts an optional `?server=` query parameter to override the connection target.

Troubleshooting

- If the dashboard shows no data, ensure the ESP32 and the server are on the same network and that the ESP sends `register` and `telemetry` events.
- To test quickly, open the dashboard with `?server=` pointing at the server IP; e.g. `http://localhost:3000/?server=http://192.168.1.36:3000`.

If you want, I can add a small UI enhancement to select which ESP device to control or add explicit ON/OFF buttons for the LED.
