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

// Device management
const devices = new Map(); // deviceId -> { socketId, lastSeen, data }
const webClients = new Set();

io.on('connection', (socket) => {
  console.log('Client connected:', socket.id);

  // ESP32 registration with deviceId
  socket.on('register_device', (deviceId) => {
    if (!deviceId) {
      console.log('Device registration failed - no deviceId provided');
      return;
    }
    
    // Join device-specific room
    socket.join(`device_${deviceId}`);
    
    // Store device info
    devices.set(deviceId, {
      socketId: socket.id,
      lastSeen: Date.now(),
      data: null
    });
    
    console.log(`ESP32 registered: ${deviceId} (${socket.id})`);
    
    // Notify all web clients about new device
    webClients.forEach(webSocketId => {
      io.to(webSocketId).emit('device_list', Array.from(devices.keys()));
    });
  });

  // Web client registration
  socket.on('register_web', () => {
    webClients.add(socket.id);
    console.log('Web client registered:', socket.id);
    
    // Send current device list to new web client
    socket.emit('device_list', Array.from(devices.keys()));
    
    // Send current data for all devices
    devices.forEach((deviceInfo, deviceId) => {
      if (deviceInfo.data) {
        socket.emit('telemetry', {
          deviceId: deviceId,
          data: deviceInfo.data,
          timestamp: deviceInfo.lastSeen
        });
      }
    });
  });

  // Telemetry from ESP32
  socket.on('telemetry', (payload) => {
    // Find device by socketId
    let deviceId = null;
    for (const [id, info] of devices) {
      if (info.socketId === socket.id) {
        deviceId = id;
        break;
      }
    }
    
    if (!deviceId) {
      console.log('Telemetry from unregistered device:', socket.id);
      return;
    }
    
    console.log(`Telemetry from ${deviceId}:`, payload);
    
    // Update device data
    devices.get(deviceId).data = payload;
    devices.get(deviceId).lastSeen = Date.now();
    
    // Forward telemetry to all web clients with deviceId
    const telemetryData = {
      deviceId: deviceId,
      data: payload,
      timestamp: Date.now()
    };
    
    webClients.forEach(webSocketId => {
      io.to(webSocketId).emit('telemetry', telemetryData);
    });
  });

  // Control command from web to specific device
  socket.on('control_device', ({ deviceId, command }) => {
    console.log(`Control for ${deviceId}:`, command);
    
    if (devices.has(deviceId)) {
      const deviceInfo = devices.get(deviceId);
      // Send control to specific device room
      io.to(`device_${deviceId}`).emit('control', command);
      console.log(`Control sent to device ${deviceId}`);
    } else {
      console.log(`Device ${deviceId} not found`);
    }
  });

  // Handle disconnect
  socket.on('disconnect', () => {
    console.log('Client disconnected:', socket.id);
    
    // Remove from web clients
    webClients.delete(socket.id);
    
    // Find and remove device
    for (const [deviceId, info] of devices) {
      if (info.socketId === socket.id) {
        devices.delete(deviceId);
        console.log(`Device ${deviceId} removed`);
        
        // Notify web clients about device removal
        webClients.forEach(webSocketId => {
          io.to(webSocketId).emit('device_list', Array.from(devices.keys()));
          io.to(webSocketId).emit('device_disconnected', deviceId);
        });
        break;
      }
    }
  });

  // Get device status
  socket.on('get_devices', () => {
    const deviceList = Array.from(devices.entries()).map(([id, info]) => ({
      deviceId: id,
      lastSeen: info.lastSeen,
      connected: Date.now() - info.lastSeen < 10000, // 10 seconds timeout
      data: info.data
    }));
    
    socket.emit('device_status', deviceList);
  });
});

// Cleanup disconnected devices periodically
setInterval(() => {
  const now = Date.now();
  const timeout = 30000; // 30 seconds
  
  devices.forEach((info, deviceId) => {
    if (now - info.lastSeen > timeout) {
      console.log(`Device ${deviceId} timed out, removing...`);
      devices.delete(deviceId);
      
      // Notify web clients
      webClients.forEach(webSocketId => {
        io.to(webSocketId).emit('device_list', Array.from(devices.keys()));
        io.to(webSocketId).emit('device_disconnected', deviceId);
      });
    }
  });
}, 15000); // Check every 15 seconds

server.listen(PORT, () => {
  console.log(`Multi-ESP32 Server running on http://localhost:${PORT}`);
  console.log('Waiting for ESP32 devices and web clients...');
});