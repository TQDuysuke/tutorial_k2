const io = require('socket.io-client');

// Device emulator for buoi_7 that sends token with register
const SERVER = 'http://localhost:3000';
const deviceId = process.argv[2] || 'esp32-manual2';
const token = process.argv[3] || 'devkey123';

const socket = io(SERVER, { transports: ['websocket'], reconnection: false });

socket.on('connect', () => {
  console.log('Connected as device', socket.id);
  // send object with deviceId and token so server validates
  socket.emit('register_device', { deviceId, token });
  console.log('Registered deviceId =', deviceId, 'token=', token);

  // send telemetry every 2s
  setInterval(() => {
    const payload = {
      temp: parseFloat((20 + Math.random() * 10).toFixed(1)),
      hum: Math.floor(40 + Math.random() * 40)
    };
    console.log('Sending telemetry', payload);
    socket.emit('telemetry', payload);
  }, 2000);
});

socket.on('control', (cmd) => {
  console.log('Received control:', cmd);
});

socket.on('disconnect', () => {
  console.log('Disconnected');
});

socket.on('connect_error', (err) => {
  console.error('Connect error', err.message || err);
});
