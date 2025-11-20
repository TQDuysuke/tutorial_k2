# How to flash ESP32 (quick guide)

This file contains short instructions to flash the provided `components/esp32_telemetry.ino` sketches to an ESP32 module.

1) Using Arduino IDE (recommended for beginners)

- Open Arduino IDE
- Install ESP32 boards support (Board Manager: search "esp32" by Espressif)
- Open `components/esp32_telemetry.ino`
- Edit WiFi / server / DEVICE_ID constants at the top of the sketch
- Select correct board (e.g., "ESP32 Dev Module") and correct COM port
- Click Upload

2) Using esptool (command-line)

- Build the binary using platformio or Arduino CLI (not covered here).
- Erase flash (optional):

  esptool.py --port COM3 erase_flash

- Write firmware (example):

  esptool.py --chip esp32 --port COM3 write_flash -z 0x1000 firmware.bin

Replace `COM3` on Windows with the actual COM port.

3) Common troubleshooting

- Serial monitor shows garbage: set correct baud (usually 115200) and line ending to "Both NL & CR".
- Can't upload: check that the board is in bootloader mode (GPIO0), try holding BOOT while pressing Upload (Arduino IDE handles this for most boards).
- Device not appearing on dashboard: confirm WiFi credentials and that server IP/port are reachable from the device network. Check serial logs.
- Duplicate devices: ensure `DEVICE_ID` is unique for each module.

4) Quick test without hardware

- You can simulate telemetry by writing a small Node.js script that connects to the Socket.IO server and emits `register_device` and `telemetry` events. If you want, I can add an example script under `tools/`.
