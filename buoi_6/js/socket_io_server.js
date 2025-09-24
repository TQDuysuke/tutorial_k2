const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server, {
  origins: "*:*",
  pingTimeout: 60000,
  pingInterval: 25000
});

const PORT = 3000;

// Serve static files
app.use(express.static(path.join(__dirname, '../web')));

// Device registry with enhanced data structure
class DeviceManager {
  constructor() {
    this.devices = new Map(); // deviceId -> deviceInfo
    this.webClients = new Set(); // web client socket IDs
    this.telemetryHistory = new Map(); // deviceId -> array of recent telemetry
    this.maxHistoryPoints = 100; // Keep last 100 data points per device
  }

  // Register a new ESP32 device
  registerDevice(socketId, deviceId) {
    const deviceInfo = {
      socketId: socketId,
      deviceId: deviceId,
      lastSeen: Date.now(),
      isOnline: true,
      telemetryCount: 0,
      ledState: false,
      registeredAt: Date.now()
    };

    this.devices.set(deviceId, deviceInfo);
    
    // Initialize telemetry history
    if (!this.telemetryHistory.has(deviceId)) {
      this.telemetryHistory.set(deviceId, []);
    }

    console.log(`📱 Device registered: ${deviceId} (${socketId})`);
    this.broadcastDeviceList();
    return deviceInfo;
  }

  // Register web client
  registerWebClient(socketId) {
    this.webClients.add(socketId);
    console.log(`🖥️  Web client registered: ${socketId}`);
  }

  // Update device telemetry
  updateTelemetry(deviceId, telemetryData) {
    if (!this.devices.has(deviceId)) {
      console.log(`⚠️  Telemetry from unregistered device: ${deviceId}`);
      return false;
    }

    const device = this.devices.get(deviceId);
    device.lastSeen = Date.now();
    device.isOnline = true;
    device.telemetryCount++;

    // Add to history
    const history = this.telemetryHistory.get(deviceId);
    const dataPoint = {
      ...telemetryData,
      timestamp: Date.now()
    };
    
    history.push(dataPoint);

    // Keep only recent data points
    if (history.length > this.maxHistoryPoints) {
      history.shift();
    }

    console.log(`📊 Telemetry from ${deviceId}: temp=${telemetryData.temp}°C, hum=${telemetryData.hum}%`);
    
    // Broadcast to web clients
    this.broadcastTelemetry(deviceId, telemetryData);
    return true;
  }

  // Send control command to specific device
  controlDevice(deviceId, command) {
    if (!this.devices.has(deviceId)) {
      console.log(`❌ Control failed: Device ${deviceId} not found`);
      return false;
    }

    const device = this.devices.get(deviceId);
    
    // Update LED state if it's a LED command
    if (command.cmd === 'on') {
      device.ledState = true;
    } else if (command.cmd === 'off') {
      device.ledState = false;
    } else if (command.cmd === 'toggle') {
      device.ledState = !device.ledState;
    }

    // Send to device room
    io.to(`device_${deviceId}`).emit('control', command);
    console.log(`🎮 Control sent to ${deviceId}:`, command);
    
    // Notify web clients about LED state change
    this.broadcastDeviceUpdate(deviceId);
    return true;
  }

  // Remove device (disconnect)
  removeDevice(socketId) {
    let removedDeviceId = null;
    
    for (const [deviceId, device] of this.devices) {
      if (device.socketId === socketId) {
        this.devices.delete(deviceId);
        removedDeviceId = deviceId;
        console.log(`🚫 Device removed: ${deviceId}`);
        break;
      }
    }

    if (removedDeviceId) {
      this.broadcastDeviceList();
      this.broadcastDeviceDisconnected(removedDeviceId);
    }

    // Remove web client
    this.webClients.delete(socketId);
  }

  // Mark device as offline
  markDeviceOffline(deviceId) {
    if (this.devices.has(deviceId)) {
      this.devices.get(deviceId).isOnline = false;
      console.log(`📴 Device marked offline: ${deviceId}`);
      this.broadcastDeviceUpdate(deviceId);
    }
  }

  // Get device list for web clients
  getDeviceList() {
    return Array.from(this.devices.entries()).map(([deviceId, device]) => ({
      deviceId: deviceId,
      isOnline: device.isOnline,
      lastSeen: device.lastSeen,
      telemetryCount: device.telemetryCount,
      ledState: device.ledState,
      registeredAt: device.registeredAt
    }));
  }

  // Get telemetry history for a device
  getTelemetryHistory(deviceId, limit = 20) {
    const history = this.telemetryHistory.get(deviceId) || [];
    return history.slice(-limit); // Return last N points
  }

  // Broadcast device list to all web clients
  broadcastDeviceList() {
    const deviceList = this.getDeviceList();
    this.webClients.forEach(webSocketId => {
      io.to(webSocketId).emit('device_list', deviceList);
    });
  }

  // Broadcast telemetry to web clients
  broadcastTelemetry(deviceId, telemetryData) {
    const payload = {
      deviceId: deviceId,
      data: telemetryData,
      timestamp: Date.now()
    };

    this.webClients.forEach(webSocketId => {
      io.to(webSocketId).emit('telemetry', payload);
    });
  }

  // Broadcast device update (LED state, etc.)
  broadcastDeviceUpdate(deviceId) {
    if (this.devices.has(deviceId)) {
      const device = this.devices.get(deviceId);
      const payload = {
        deviceId: deviceId,
        ledState: device.ledState,
        isOnline: device.isOnline,
        lastSeen: device.lastSeen
      };

      this.webClients.forEach(webSocketId => {
        io.to(webSocketId).emit('device_update', payload);
      });
    }
  }

  // Broadcast device disconnection
  broadcastDeviceDisconnected(deviceId) {
    this.webClients.forEach(webSocketId => {
      io.to(webSocketId).emit('device_disconnected', deviceId);
    });
  }

  // Cleanup inactive devices
  cleanupInactiveDevices() {
    const now = Date.now();
    const timeout = 60000; // 60 seconds timeout

    this.devices.forEach((device, deviceId) => {
      if (now - device.lastSeen > timeout) {
        console.log(`🧹 Cleaning up inactive device: ${deviceId}`);
        this.devices.delete(deviceId);
        this.broadcastDeviceList();
        this.broadcastDeviceDisconnected(deviceId);
      }
    });
  }
}

// Initialize device manager
const deviceManager = new DeviceManager();

// Socket.IO connection handling
io.on('connection', (socket) => {
  console.log(`🔌 Client connected: ${socket.id}`);

  // ESP32 device registration
  socket.on('register_device', (deviceId) => {
    if (!deviceId || typeof deviceId !== 'string') {
      console.log('❌ Invalid device registration - deviceId required');
      socket.emit('registration_error', 'Device ID is required');
      return;
    }

    // Join device-specific room
    socket.join(`device_${deviceId}`);
    
    // Register with device manager
    deviceManager.registerDevice(socket.id, deviceId);
    
    // Send registration confirmation
    socket.emit('registration_success', { deviceId: deviceId });
  });

  // Web client registration
  socket.on('register_web', () => {
    deviceManager.registerWebClient(socket.id);
    
    // Send current device list and history
    const deviceList = deviceManager.getDeviceList();
    socket.emit('device_list', deviceList);
    
    // Send recent telemetry for all devices
    deviceList.forEach(device => {
      const history = deviceManager.getTelemetryHistory(device.deviceId, 10);
      if (history.length > 0) {
        history.forEach(point => {
          socket.emit('telemetry', {
            deviceId: device.deviceId,
            data: point,
            timestamp: point.timestamp
          });
        });
      }
    });
  });

  // Telemetry data from ESP32
  socket.on('telemetry', (payload) => {
    // Find device by socket ID
    let deviceId = null;
    for (const [id, device] of deviceManager.devices) {
      if (device.socketId === socket.id) {
        deviceId = id;
        break;
      }
    }

    if (!deviceId) {
      console.log(`⚠️  Telemetry from unregistered socket: ${socket.id}`);
      return;
    }

    // Parse telemetry data
    let telemetryData;
    try {
      telemetryData = typeof payload === 'string' ? JSON.parse(payload) : payload;
    } catch (error) {
      console.log(`❌ Invalid telemetry JSON from ${deviceId}:`, payload);
      return;
    }

    // Update device telemetry
    deviceManager.updateTelemetry(deviceId, telemetryData);
  });

  // Control command from web client
  socket.on('control_device', ({ deviceId, command }) => {
    console.log(`🎮 Control request: ${deviceId} -> ${JSON.stringify(command)}`);
    deviceManager.controlDevice(deviceId, command);
  });

  // Get telemetry history request
  socket.on('get_telemetry_history', ({ deviceId, limit }) => {
    const history = deviceManager.getTelemetryHistory(deviceId, limit || 50);
    socket.emit('telemetry_history', { deviceId, history });
  });

  // Get device statistics
  socket.on('get_device_stats', () => {
    const stats = {
      totalDevices: deviceManager.devices.size,
      onlineDevices: Array.from(deviceManager.devices.values()).filter(d => d.isOnline).length,
      webClients: deviceManager.webClients.size,
      totalTelemetryPoints: Array.from(deviceManager.telemetryHistory.values())
        .reduce((sum, history) => sum + history.length, 0)
    };
    socket.emit('device_stats', stats);
  });

  // Handle disconnection
  socket.on('disconnect', () => {
    console.log(`🔌 Client disconnected: ${socket.id}`);
    deviceManager.removeDevice(socket.id);
  });

  // Heartbeat from ESP32
  socket.on('heartbeat', (data) => {
    // Update last seen time for heartbeat
    for (const [deviceId, device] of deviceManager.devices) {
      if (device.socketId === socket.id) {
        device.lastSeen = Date.now();
        device.isOnline = true;
        break;
      }
    }
  });
});

// Periodic cleanup of inactive devices
setInterval(() => {
  deviceManager.cleanupInactiveDevices();
}, 30000); // Every 30 seconds

// API endpoints for debugging
app.get('/api/devices', (req, res) => {
  res.json(deviceManager.getDeviceList());
});

app.get('/api/telemetry/:deviceId', (req, res) => {
  const { deviceId } = req.params;
  const limit = parseInt(req.query.limit) || 50;
  const history = deviceManager.getTelemetryHistory(deviceId, limit);
  res.json(history);
});

// Start server
server.listen(PORT, () => {
  console.log(`🚀 Advanced Multi-ESP32 Server running on http://localhost:${PORT}`);
  console.log(`📊 API endpoints:`);
  console.log(`   GET /api/devices - List all devices`);
  console.log(`   GET /api/telemetry/:deviceId - Get telemetry history`);
  console.log(`🎯 Ready for ESP32 connections...`);
});