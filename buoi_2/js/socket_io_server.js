
const io = require("socket.io")(3000, {
  origins: "*:*"
});

// Keep a simple toggle state for demo control
let ledState = false;

io.on("connection", (socket) => {
  console.log("ESP32 connected:", socket.id);

  socket.on("hello", (msg) => {
    console.log("ESP32 says:", msg);
  });

  socket.on("telemetry", (data) => {
    console.log("Telemetry from", socket.id, ":", data);
    // try parse telemetry and act upon conditions
    try {
      const t = JSON.parse(data);
      if (t && typeof t.temp === 'number') {
        // example: if temperature too high, force LED on
        if (t.temp > 30) {
          socket.emit("control", JSON.stringify({ led: true }));
          console.log("Sent control -> led ON to", socket.id);
        }
      }
    } catch (e) {
      // ignore parse errors
    }
  });
});

// Broadcast a control command to all clients every 15s (toggle)
setInterval(() => {
  ledState = !ledState;
  io.emit("control", JSON.stringify({ led: ledState }));
  console.log("Broadcast control led =", ledState);
}, 15000);

// npm install socket.io@2.4.1