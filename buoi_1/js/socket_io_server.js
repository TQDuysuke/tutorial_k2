
const io = require("socket.io")(3000, {
  origins: "*:*"
});

io.on("connection", (socket) => {
  console.log("ESP32 connected:", socket.id);

  socket.on("hello", (msg) => {
    console.log("ESP32 says:", msg);
  });

  socket.on("telemetry", (data) => {
    console.log("Telemetry:", data);
  });
});
// npm install socket.io@2.4.1