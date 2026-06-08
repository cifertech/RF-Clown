# M5-Shark RF-Clown V2 Fix Notes

**Fixed by:** Aung Myat Thu [w01f]  
**Device:** M5-Shark RF-Clown V2  
**Board:** ESP32-D0WD-V3 + nRF24L01+ modules  
**Purpose:** Firmware debugging, UI fix, button fix, LED/status logging, and safe RF module validation

> This document is for authorized RF lab research, firmware debugging, and defensive hardware validation only. Do not use RF-active modes against public devices, neighbors, office devices, or unknown networks.

---

## 1. Summary

The M5-Shark RF-Clown unit originally showed only a simple:

```text
Active / Deactive
```

interface and did not expose the expected RF-Clown V2 mode UI.

After firmware modification and Arduino CLI compilation, the device now shows and navigates the expected modes:

```text
WiFi
Video TX
RC
BLE
Bluetooth
USB Wireless
Zigbee
NRF24
```

The fixes include:

```text
Compile fix
Button polling fix
SELECT Active/Deactive fix
Mode/state serial debug
Channel verification logging
Reduced serial flood
RF24 radio detection confirmation
```

---

## 2. Tested Hardware

Detected ESP32:

```bash
esptool --port /dev/ttyUSB0 chip-id
```

Observed:

```text
Chip type: ESP32-D0WD-V3
Crystal frequency: 40MHz
USB bridge: Silicon Labs CP210x UART Bridge
Port: /dev/ttyUSB0
Flash size: 16MB
```

---

## 3. Backup Commands

### 4MB Backup

```bash
cd ~/rf-clown-backup

sudo esptool --port /dev/ttyUSB0 --baud 115200 \
  read-flash 0x00000 0x400000 rf-clown-original-backup-4mb.bin
```

### 16MB Backup

```bash
cd ~/rf-clown-backup

sudo esptool --port /dev/ttyUSB0 --baud 115200 \
  read-flash 0x00000 0x1000000 rf-clown-original-backup-16mb-slow.bin
```

### Restore 4MB Backup

```bash
cd ~/rf-clown-backup

sudo esptool --chip esp32 --port /dev/ttyUSB0 --baud 115200 \
  write-flash 0x00000 rf-clown-original-backup-4mb.bin
```

### Restore 16MB Backup

```bash
cd ~/rf-clown-backup

sudo esptool --chip esp32 --port /dev/ttyUSB0 --baud 115200 \
  write-flash 0x00000 rf-clown-original-backup-16mb-slow.bin
```

---

## 4. Arduino CLI Setup

Install ESP32 platform and libraries:

```bash
arduino-cli config init
arduino-cli core update-index
arduino-cli core install esp32:esp32@2.0.17

arduino-cli lib install "U8g2"
arduino-cli lib install "RF24"
arduino-cli lib install "Adafruit NeoPixel"
```

Compile:

```bash
cd ~/rf-clown-backup/RF-Clown/RfClown

arduino-cli compile --fqbn esp32:esp32:esp32 .
```

Upload:

```bash
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 .
```

Do not use:

```bash
sudo arduino-cli upload
```

unless the ESP32 platform is also installed for the root user.

If serial permission fails:

```bash
sudo usermod -aG uucp,lock $USER
sudo chmod 666 /dev/ttyUSB0
```

---

## 5. Compile Fix

The source had multiple-definition linker errors because global variables were defined directly in header files.

Affected symbols included:

```text
u8g2
pixels
current_Mode
current
channelGroup_1
channelGroup_2
channelGroup_3
ChangeRequested
ChangeRequested1
ChangeRequested2
lastPressTime
neoPixelActive
oledBrightness
```

Fix:

```text
Move global definitions into globals.cpp
Keep extern declarations in config.h / setting.h
Add channel group length variables for extern arrays
```

Example header declaration:

```cpp
extern byte channelGroup_1[];
extern byte channelGroup_2[];
extern byte channelGroup_3[];

extern const size_t channelGroup_1_len;
extern const size_t channelGroup_2_len;
extern const size_t channelGroup_3_len;
```

Example `globals.cpp` definition:

```cpp
byte channelGroup_1[] = {2, 5, 8, 11};
byte channelGroup_2[] = {26, 29, 32, 35};
byte channelGroup_3[] = {80, 83, 86, 89};

const size_t channelGroup_1_len = sizeof(channelGroup_1);
const size_t channelGroup_2_len = sizeof(channelGroup_2);
const size_t channelGroup_3_len = sizeof(channelGroup_3);
```

---

## 6. Button Fix

Interrupt-based button handling was unreliable on this M5-Shark unit.

Fix:

```text
Disable attachInterrupt() button handling
Use polling with debounce
Print button activity to Serial
```

Expected mapping:

```text
LEFT   = previous mode
RIGHT  = next mode
SELECT = Active / Deactive
```

Expected serial logs:

```text
[BTN] LEFT / previous mode
[RF-Clown] changed | Mode: NRF24 | State: DEACTIVE

[BTN] RIGHT / next mode
[RF-Clown] changed | Mode: WiFi | State: DEACTIVE

[BTN] SELECT / active-deactive
[RF-Clown] changed | Mode: WiFi | State: ACTIVE
```

---

## 7. SELECT Toggle Fix

SELECT was detected but did not properly toggle `ACTIVE_MODE` / `DEACTIVE_MODE`.

Fix logic:

```cpp
if (lastS == HIGH && nowS == LOW) {
  current = (current == ACTIVE_MODE) ? DEACTIVE_MODE : ACTIVE_MODE;
  ChangeRequested2 = false;
  lastEventMs = now;
  Serial.println("[BTN] SELECT / active-deactive");
}
```

After this fix, SELECT correctly changes state:

```text
[BTN] SELECT / active-deactive
[RF-Clown] changed | Mode: BLE | State: ACTIVE

[BTN] SELECT / active-deactive
[RF-Clown] changed | Mode: BLE | State: DEACTIVE
```

---

## 8. Mode UI Fix

After the button polling patch, the OLED mode UI became usable.

Confirmed mode navigation:

```text
WiFi
Video TX
RC
BLE
Bluetooth
USB Wireless
Zigbee
NRF24
```

---

## 9. RF24 Radio Detection

Terminal inspection confirmed the nRF24L01+ radio initializes correctly:

```text
SPI Frequency           = 10 Mhz
Channel                 = 76 (~2476 MHz)
Model                   = nRF24L01+
RF Data Rate            = 2 MBPS
RF Power Amplifier      = PA_MAX
RF Low Noise Amplifier  = Enabled
Primary Mode            = TX
TX address              = 0xffffffffff
```

This confirms:

```text
ESP32 to nRF24 SPI communication = OK
nRF24 module detection           = OK
TX mode configuration            = OK
PA level configuration           = OK
```

---

## 10. Channel Verification

The old RF24 details output repeatedly showed:

```text
Channel = 76 (~2476 MHz)
```

This looked suspicious, so channel verification logging was added.

Expected channel debug output:

```text
[CHANNEL] Mode: NRF24 | requested=78 (~2478 MHz) | A=78 B=78 C=78
[CHANNEL] Mode: NRF24 | requested=76 (~2476 MHz) | A=76 B=76 C=76
[CHANNEL] Mode: NRF24 | requested=79 (~2479 MHz) | A=79 B=79 C=79
```

This confirms:

```text
RadioA channel update = OK
RadioB channel update = OK
RadioC channel update = OK
```

---

## 11. Reduce Serial Flood

Channel debug and RF24 `printDetails()` can flood the terminal.

Fix:

```text
Disable repeated printDetails()
Throttle channel verification logs
Print only useful button/mode/state logs
```

Expected clean output:

```text
[RF-Clown] Boot diagnostic patch active
[RF-Clown] boot | Mode: WiFi | State: DEACTIVE

[BTN] RIGHT / next mode
[RF-Clown] changed | Mode: BLE | State: DEACTIVE

[BTN] SELECT / active-deactive
[RF-Clown] changed | Mode: BLE | State: ACTIVE

[CHANNEL] Mode: BLE | requested=26 (~2426 MHz) | A=26 B=26 C=26
```

---

## 12. LED Notes

The bright blue LED near the USB port appears to be a hardware power/charging LED.

Observed behavior:

```text
USB connected = blue LED bright
Battery low   = blue LED dim
Power on      = blue LED on
```

This LED is not necessarily controlled by firmware.

If the firmware NeoPixel/status LED does not change, possible reasons:

```text
NeoPixel GPIO mismatch
M5-Shark board revision uses a different LED pin
Firmware LED setting disabled
Board only exposes a hardware power LED
```

This does not mean the RF module is broken.

---

## 13. Terminal Inspection

Open serial:

```bash
picocom -b 115200 /dev/ttyUSB0
```

Exit serial:

```text
Ctrl+A, then Ctrl+X
```

Check USB device:

```bash
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
lsusb | grep -i -E "silicon|cp210|uart|esp"
```

Kill anything using the serial port:

```bash
sudo fuser -k /dev/ttyUSB0 2>/dev/null || true
```

Stop ModemManager if needed:

```bash
sudo systemctl stop ModemManager 2>/dev/null || true
```

---

## 14. Current Working Status

```text
ESP32 detected              = OK
CP2102 USB serial           = OK
OLED mode UI                = OK
LEFT/RIGHT buttons          = OK
SELECT Active/Deactive      = OK
nRF24L01+ detected          = OK
RadioA channel update       = OK
RadioB channel update       = OK
RadioC channel update       = OK
Serial flood                = reduced
```

---

## 15. Limitations

Terminal logs can confirm:

```text
ESP32 boot
OLED UI
Button detection
Mode switching
Active/Deactive state
nRF24 SPI initialization
RF24 channel configuration
TX mode configuration
```

Terminal logs cannot fully confirm:

```text
Actual RF emission strength
Antenna quality
External spectrum activity
Effect on Bluetooth/WiFi devices
```

For passive RF verification, use:

```text
2.4GHz spectrum analyzer
HackRF or other SDR with 2.4GHz support
Another nRF24 scanner
nRF52840 BLE sniffer
```

---

## 16. Safety Notice

Use this firmware only for:

```text
Authorized lab testing
Firmware debugging
Hardware validation
Defensive RF research
Education in a controlled environment
```

Do not use RF-active modes against public devices, office devices, neighbors, or unknown networks.

---

## 17. Git Commands

Create branch:

```bash
cd ~/rf-clown-backup/RF-Clown

git switch -c fix/m5shark-rfclown-ui-led-debug
```

Add docs and source fixes:

```bash
git add RfClown/ docs/M5SHARK_RF_CLOWN_FIX.md
```

Commit:

```bash
git commit -m "Fix M5-Shark RF-Clown UI buttons and diagnostics"
```

Push:

```bash
git push -u origin fix/m5shark-rfclown-ui-led-debug
```

---

## 18. Final Note

This fix was prepared and tested by:

```text
Aung Myat Thu [w01f]
```

The main goal was to make the M5-Shark RF-Clown V2 firmware usable for controlled lab debugging by fixing:

```text
Compilation
OLED mode navigation
Button handling
Active/Deactive toggle
Serial diagnostics
RF24 channel verification
```