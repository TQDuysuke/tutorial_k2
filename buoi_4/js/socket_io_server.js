const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server, {
  origins: "*:*"
});

const PORT = 3000;

// serve static files from web directory
app.use(express.static(path.join(__dirname, '../web')));

// track connected clients by type
let clients = {
  esp: [],
  web: []
};

io.on('connection', (socket) => {
  console.log('Client connected:', socket.id);

  socket.on('register', (type) => {
    if (type === 'esp') {
      clients.esp.push(socket.id);
      console.log('ESP32 registered:', socket.id);
    } else if (type === 'web') {
      clients.web.push(socket.id);
      console.log('Web client registered:', socket.id);
    }
  });

  socket.on('telemetry', (data) => {
    console.log('Telemetry from ESP:', data);
    
    // forward telemetry to all web clients
    clients.web.forEach(webId => {
      io.to(webId).emit('telemetry', data);
    });
  });

  socket.on('control', (data) => {
    console.log('Control from web:', data);
    
    // forward control to all ESP clients
    clients.esp.forEach(espId => {
      io.to(espId).emit('control', data);
    });
  });

  socket.on('disconnect', () => {
    console.log('Client disconnected:', socket.id);
    
    // remove from client lists
    clients.esp = clients.esp.filter(id => id !== socket.id);
    clients.web = clients.web.filter(id => id !== socket.id);
  });
});

server.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});