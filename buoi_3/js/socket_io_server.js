const express = require('express');
const http = require('http');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = require('socket.io')(server, { origins: '*:*' });

// serve static frontend
app.use(express.static(path.join(__dirname, '..', 'web')));

const PORT = 3000;
server.listen(PORT, () => console.log(`Server listening on http://localhost:${PORT}`));

// track connected sockets by role
const espSockets = new Set();
const webSockets = new Set();

io.on('connection', (socket) => {
  console.log('client connected', socket.id);

  socket.on('register', (role) => {
    if (role === 'esp') {
      espSockets.add(socket.id);
      console.log('ESP registered', socket.id);
    } else if (role === 'web') {
      webSockets.add(socket.id);
      console.log('WEB registered', socket.id);
    }
  });

  socket.on('hello', (msg) => {
    console.log('hello from', socket.id, msg);
  });

  socket.on('telemetry', (data) => {
    console.log('Telemetry from', socket.id, data);
    // forward telemetry to web clients only
    webSockets.forEach((id) => io.to(id).emit('telemetry', data));
  });

  socket.on('control', (data) => {
    console.log('Control from', socket.id, data);
    // forward control to ESP clients only
    espSockets.forEach((id) => io.to(id).emit('control', data));
  });

  socket.on('disconnect', () => {
    espSockets.delete(socket.id);
    webSockets.delete(socket.id);
    console.log('client disconnected', socket.id);
  });
});

// npm install socket.io@2.4.1 express