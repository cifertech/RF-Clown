/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/rfclown
   ________________________________________ */
   
#include "setting.h"
#include "config.h"

void IRAM_ATTR handleButton() {
  unsigned long currentTime = millis();
  if (currentTime - lastPressTime > debounceDelay) {
    ChangeRequested = true;
    lastPressTime = currentTime;
  }
}

void IRAM_ATTR handleButton1() {
  unsigned long currentTime = millis();
  if (currentTime - lastPressTime > debounceDelay) {
    ChangeRequested1 = true;
    lastPressTime = currentTime;
  }
}

void IRAM_ATTR handleButton2() {
  unsigned long currentTime = millis();
  if (currentTime - lastPressTime > debounceDelay) {
    ChangeRequested2 = true;
    lastPressTime = currentTime;
  }
}

void configure_Radio(RF24 &radio, const byte *channels, size_t size) {
  configureNrf(radio);
  radio.printPrettyDetails();
  for (size_t i = 0; i < size; i++) {
    radio.setChannel(channels[i]);
    radio.startConstCarrier(RF24_PA_MAX, channels[i]);
  }
}

const char* modeName(OperationMode m) {
  switch (m) {
    case WiFi_MODULE:         return "WiFi";
    case VIDEO_TX_MODULE:     return "VideoTX";
    case RC_MODULE:           return "RC";
    case BLE_MODULE:          return "BLE";
    case Bluetooth_MODULE:    return "Bluetooth";
    case USB_WIRELESS_MODULE: return "USB-Wireless";
    case ZIGBEE_MODULE:       return "Zigbee";
    case NRF24_MODULE:        return "NRF24";
    default:                  return "Unknown";
  }
}

void printNrf24Status(byte curChannel) {
  const char* actStr = (current == ACTIVE_MODE) ? "ACTIVE" : "DEACTIVE";
  const char* pwrStr = modeSupportsPowerAdjust(current_Mode)
                       ? powerLevelToString(modePowerLevel[current_Mode])
                       : "MAX(fixed)";
  Serial.printf("[NRF24] mode=%s state=%s ch=%u power=%s paA=%d paB=%d paC=%d\n",
                modeName(current_Mode), actStr, (unsigned)curChannel, pwrStr,
                (int)RadioA.getPALevel(), (int)RadioB.getPALevel(), (int)RadioC.getPALevel());
}

extern bool nrf24InitOk;

bool setPowerForCurrentMode(PowerLevel p) {
  if (!modeSupportsPowerAdjust(current_Mode)) {
    Serial.printf("[PWR] %s 模式不支持手动调节（固定 MAX）\n", modeName(current_Mode));
    return false;
  }
  PowerLevel oldP = modePowerLevel[current_Mode];
  modePowerLevel[current_Mode] = p;
  if (current == ACTIVE_MODE) {
    if (p == POWER_OFF) {
      RadioA.stopConstCarrier();
      RadioB.stopConstCarrier();
      RadioC.stopConstCarrier();
      const char* ledCol = nrf24InitOk ? "green" : "yellow";
      setNeoPixelColour(ledCol);
      Serial.printf("[LED] OFF -> %s (init %s)\n", ledCol, nrf24InitOk ? "OK" : "FAIL");
    } else {
      restartCarriers(powerLevelToPa(p));
      setNeoPixelColour("red");
    }
  }
  Serial.printf("[PWR] %s -> %s%s\n", modeName(current_Mode), powerLevelToString(p),
                (current == ACTIVE_MODE) ? " (applied)" : " (will apply on ACTIVE)");
  return true;
}

void handleSerialCommand() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  line.toUpperCase();
  if (line.length() == 0) return;

  if (line == "MIN") {
    setPowerForCurrentMode(POWER_MIN);
  } else if (line == "HIGH") {
    setPowerForCurrentMode(POWER_HIGH);
  } else if (line == "MAX") {
    setPowerForCurrentMode(POWER_MAX);
  } else if (line == "OFF") {
    setPowerForCurrentMode(POWER_OFF);
  } else if (line == "STATUS" || line == "?") {
    Serial.println(F("---- NRF24 status ----"));
    Serial.printf("mode=%s state=%s\n", modeName(current_Mode),
                  (current == ACTIVE_MODE) ? "ACTIVE" : "DEACTIVE");
    if (modeSupportsPowerAdjust(current_Mode)) {
      Serial.printf("power(BLE/BT/WiFi adjustable)=%s\n",
                    powerLevelToString(modePowerLevel[current_Mode]));
    } else {
      Serial.printf("power=%s (this mode not adjustable)\n",
                    powerLevelToString(modePowerLevel[current_Mode]));
    }
    Serial.printf("BLE=%s Bluetooth=%s WiFi=%s\n",
                  powerLevelToString(modePowerLevel[BLE_MODULE]),
                  powerLevelToString(modePowerLevel[Bluetooth_MODULE]),
                  powerLevelToString(modePowerLevel[WiFi_MODULE]));
    Serial.println(F("----------------------"));
  } else if (line == "HELP") {
    Serial.println(F("S键: 可调模式下循环 MAX/HIGH/MIN/OFF; 串口: MIN/HIGH/MAX/OFF/status/help"));
  } else {
    Serial.printf("[?] unknown cmd '%s' (try: help)\n", line.c_str());
  }
}

bool nrf24InitOk = false;

void initialize_MultiMode(bool startCarrier = true) {
  bool okA = RadioA.begin();
  bool okB = RadioB.begin();
  bool okC = RadioC.begin();
  if (okA) configureNrf(RadioA);
  if (okB) configureNrf(RadioB);
  if (okC) configureNrf(RadioC);

  nrf24InitOk = (okA && okB && okC);
  if (nrf24InitOk) {
    Serial.println(F("[NRF24] all radios init OK -> green"));
    setNeoPixelColour("green");
  } else {
    Serial.printf("[NRF24] init fail: A=%d B=%d C=%d -> yellow\n", okA, okB, okC);
    setNeoPixelColour("yellow");
  }
  delay(1000); 

  if (startCarrier) {
    if (okA) {
      for (size_t i = 0; i < sizeof(channelGroup_1); i++) {
        RadioA.setChannel(channelGroup_1[i]);
        RadioA.startConstCarrier(RF24_PA_MAX, channelGroup_1[i]);
      }
    }
    if (okB) {
      for (size_t i = 0; i < sizeof(channelGroup_2); i++) {
        RadioB.setChannel(channelGroup_2[i]);
        RadioB.startConstCarrier(RF24_PA_MAX, channelGroup_2[i]);
      }
    }
    if (okC) {
      for (size_t i = 0; i < sizeof(channelGroup_3); i++) {
        RadioC.setChannel(channelGroup_3[i]);
        RadioC.startConstCarrier(RF24_PA_MAX, channelGroup_3[i]);
      }
    }
    Serial.println(F("[NRF24] carriers started"));
  }
}

void initialize_Radios() {
  if (current == ACTIVE_MODE) {
    initialize_MultiMode(true); 
    PowerLevel p = modeSupportsPowerAdjust(current_Mode)
                   ? modePowerLevel[current_Mode] : POWER_MAX;
    if (p == POWER_OFF) {
      RadioA.stopConstCarrier();
      RadioB.stopConstCarrier();
      RadioC.stopConstCarrier();
      const char* ledCol = nrf24InitOk ? "green" : "yellow";
      setNeoPixelColour(ledCol);
      Serial.printf("[RF] ACTIVE+OFF -> %s (init %s, not transmitting)\n", ledCol, nrf24InitOk ? "OK" : "FAIL");
    } else {
      restartCarriers(powerLevelToPa(p));
      setNeoPixelColour("red");
      Serial.printf("[RF] ACTIVE -> red (transmitting) mode=%s power=%s\n",
                    modeName(current_Mode), powerLevelToString(p));
    }
  } else if (current == DEACTIVE_MODE) {
    RadioA.powerDown();
    RadioB.powerDown();
    RadioC.powerDown();
    delay(100);
    const char* ledCol = nrf24InitOk ? "green" : "yellow";
    setNeoPixelColour(ledCol);
    Serial.printf("[RF] DEACTIVE -> %s (init %s)\n", ledCol, nrf24InitOk ? "OK" : "FAIL");
  }
}

void restartCarriers(rf24_pa_dbm_e paLevel) {
  RadioA.stopConstCarrier();
  RadioB.stopConstCarrier();
  RadioC.stopConstCarrier();
  delay(5);
  uint8_t chA = RadioA.getChannel();
  uint8_t chB = RadioB.getChannel();
  uint8_t chC = RadioC.getChannel();
  if (chA == 0xFF) chA = (current_Mode == BLE_MODULE) ? ble_channels[0] :
                          (current_Mode == WiFi_MODULE) ? WiFi_channels[0] :
                          (current_Mode == Bluetooth_MODULE) ? bluetooth_channels[0] : 76;
  if (chB == 0xFF) chB = (current_Mode == BLE_MODULE) ? ble_channels[0] :
                          (current_Mode == WiFi_MODULE) ? WiFi_channels[0] :
                          (current_Mode == Bluetooth_MODULE) ? bluetooth_channels[0] : 77;
  if (chC == 0xFF) chC = (current_Mode == BLE_MODULE) ? ble_channels[0] :
                          (current_Mode == WiFi_MODULE) ? WiFi_channels[0] :
                          (current_Mode == Bluetooth_MODULE) ? bluetooth_channels[0] : 78;
  RadioA.powerUp();
  RadioB.powerUp();
  RadioC.powerUp();
  delay(2);
  RadioA.startConstCarrier(paLevel, chA);
  RadioB.startConstCarrier(paLevel, chB);
  RadioC.startConstCarrier(paLevel, chC);
  Serial.printf("[RF] carrier restarted pa=%d ch=%u/%u/%u\n",
                (int)paLevel, (unsigned)chA, (unsigned)chB, (unsigned)chC);
}

static const uint8_t HEADER_H = 12;
static const uint8_t PAD      = 6;
static const uint8_t CARD_H   = 36;
static const uint8_t RADIUS   = 3;
static const uint8_t DOT_Y    = 64 - 6;

static const char* kMenuLabels[] = {
  "WiFi", "Video TX", "RC", "BLE", "Bluetooth", "USB Wireless", "Zigbee", "NRF24"
};
static const int kMenuCount = sizeof(kMenuLabels)/sizeof(kMenuLabels[0]);

static int menuIndexFromMode(OperationMode m) {
  switch (m) {
    case WiFi_MODULE:         return 0;
    case VIDEO_TX_MODULE:     return 1;
    case RC_MODULE:           return 2;
    case BLE_MODULE:          return 3;
    case Bluetooth_MODULE:    return 4;
    case USB_WIRELESS_MODULE: return 5;
    case ZIGBEE_MODULE:       return 6;
    case NRF24_MODULE:        return 7;
    default: return 0;
  }
}
static OperationMode modeFromMenuIndex(int idx) {
  switch (idx) {
    case 0: return WiFi_MODULE;
    case 1: return VIDEO_TX_MODULE;
    case 2: return RC_MODULE;
    case 3: return BLE_MODULE;
    case 4: return Bluetooth_MODULE;
    case 5: return USB_WIRELESS_MODULE;
    case 6: return ZIGBEE_MODULE;
    case 7: return NRF24_MODULE;
    default: return WiFi_MODULE;
  }
}

void spectrum() {
  const uint8_t HDR_H = 10;
  const int TOP = HDR_H + 12;
  const int BOT = SCREEN_H - 2;
  const int H   = BOT - TOP;
  const int STRIDE = 3;
  const int BINS   = SCREEN_W / STRIDE;
  const int BAR_W  = 1;
  static uint8_t h[BINS];
  static int8_t  v[BINS];
  static bool init = false;
  if (!init) {
    for (int i = 0; i < BINS; ++i) {
      h[i] = random(0, H + 1);
      v[i] = random(-2, 3);
    }
    init = true;
  }
  for (int i = 0; i < BINS; ++i) {
    int8_t a = (int8_t)random(-1, 2);
    v[i] += a;
    if (v[i] > 3) v[i] = 3;
    if (v[i] < -3) v[i] = -3;
    int16_t nh = (int16_t)h[i] + v[i];
    if (nh < 0) {
      nh = 0;
      v[i] = -(v[i] * 3) / 4;
      if (v[i] == 0) v[i] = 1;
    } else if (nh > H) {
      nh = H;
      v[i] = -(v[i] * 3) / 4;
      if (v[i] == 0) v[i] = -1;
    }
    h[i] = (uint8_t)nh;
  }
  for (int i = 0; i < BINS; ++i) {
    int x  = i * STRIDE;
    int bh = h[i];
    if (bh <= 0) continue;
    int y  = BOT - bh;
    u8g2.drawVLine(x, y, bh);
  }
  u8g2.sendBuffer();
}

static void drawHeaderBar() {
  const uint8_t W = 128;
  const uint8_t H = HEADER_H;
  const uint8_t TXT_Y = 2;
  u8g2.setDrawColor(1);
  u8g2.drawBox(0, 0, W, H);
  u8g2.setFont(FONT_SMALL);
  u8g2.setDrawColor(0);
  u8g2.drawStr(PAD, TXT_Y, "RF-ClownV2");
  const char* ver = "v2.1.0";
  int vw = u8g2.getStrWidth(ver);
  u8g2.drawStr(W - vw - 4, TXT_Y, ver);
  u8g2.setDrawColor(1);
  u8g2.drawHLine(0, H - 1, W);
}

static void drawPillOutline(int x,int y,int w,int h){
  if (h & 1) h--; if (y & 1) y--;
  u8g2.drawRFrame(x, y, w, h, h/2);
}
static void drawPillKnob(int x,int y,int w,int h,float pos){
  if (h & 1) h--; if (y & 1) y--;
  int r=h/2, cxL=x+r, cxR=x+w-r-1, cy=y+r;
  int cx=(int)lroundf(cxL + (cxR - cxL)*pos);
  int knob = max(2, r-3);
  u8g2.drawDisc(cx, cy, knob);
}
static void drawPillToggle(int x,int y,int w,int h,bool on){
  drawPillOutline(x,y,w,h);
  drawPillKnob(x,y,w,h, on?0.0f:1.0f);
}

static void drawPaginationDots(int active){
  int totalW = (kMenuCount * 6) - 2;
  int startX = (128 - totalW) / 2;
  for (int i=0;i<kMenuCount;i++) {
    int x = startX + i*6;
    if (i == active) u8g2.drawDisc(x, DOT_Y, 2);
    else             u8g2.drawCircle(x, DOT_Y, 2);
  }
}

static void drawCardAtCenter(int centerX, int menuIndex){
  const char* label = kMenuLabels[menuIndex];
  const int W = 128;
  const int w = W - PAD*2;
  const int x = centerX - w/2;
  const int y = HEADER_H + 4;
  u8g2.setDrawColor(0);
  u8g2.drawBox(x+1, y+1, w-2, CARD_H-2);
  u8g2.setDrawColor(1);
  u8g2.drawRFrame(x, y, w, CARD_H, RADIUS);
  u8g2.setFont(FONT_MEDIUM);
  u8g2.setCursor(x + 12, y + 4);
  u8g2.print(label);
  bool isThisFocus = (modeFromMenuIndex(menuIndex) == current_Mode);
  bool isActive    = (isThisFocus && current == ACTIVE_MODE);
  char statusBuf[16];
  if (isActive && modeSupportsPowerAdjust(modeFromMenuIndex(menuIndex))) {
    snprintf(statusBuf, sizeof(statusBuf), "ACTIVE %s",
             powerLevelToString(modePowerLevel[modeFromMenuIndex(menuIndex)]));
  } else {
    snprintf(statusBuf, sizeof(statusBuf), "%s", isActive ? "ACTIVE" : "DEACTIVE");
  }
  u8g2.setFont(FONT_SMALL);
  u8g2.setCursor(x + 12, y + 16);
  u8g2.print(statusBuf);
  bool isOn = (modeFromMenuIndex(menuIndex) == current_Mode) && (current == ACTIVE_MODE);
  const int pillW=24, pillH=14;
  const int pillY=(y + ((CARD_H-pillH)/2)) & ~1;
  drawPillToggle(x + w - pillW - 10, pillY, pillW, pillH, isOn);
}

static void renderStaticMenu(int focusIndex) {
  u8g2.clearBuffer();
  drawHeaderBar();
  drawCardAtCenter(128 / 2, focusIndex);
  drawPaginationDots(focusIndex);
  u8g2.sendBuffer();
}

static void animateToMenu(int fromIdx, int toIdx) {
  const int W = 128;
  const int STEPS = 14;
  const uint8_t DT = 10;
  const int dir = (toIdx > fromIdx) ? -1 : 1;
  for (int s = 0; s <= STEPS; s++) {
    float t = (float)s / (float)STEPS;
    float e = (t < 0.5f) ? 4.0f*t*t*t : 1.0f - powf(-2.0f*t + 2.0f, 3)/2.0f;
    int shift = (int)(e * W + 0.5f);
    int centerFrom = (W/2) + (-dir * shift);
    int centerTo   = (W/2) + ( dir * (W - shift));
    u8g2.clearBuffer();
    drawHeaderBar();
    drawCardAtCenter(centerFrom, fromIdx);
    drawCardAtCenter(centerTo,   toIdx);
    drawPaginationDots(toIdx);
    u8g2.sendBuffer();
    delay(DT);
  }
  renderStaticMenu(toIdx);
}

static void animateToggleKnobForFocus(int focusIdx, bool fromActive, bool toActive){
  float start = (fromActive ? 0.0f : 1.0f);
  float end   = (toActive   ? 0.0f : 1.0f);
  const int STEPS = 10;
  const uint8_t FRAME_MS = 12;
  for (int s=0; s<=STEPS; s++){
    float t = (float)s / (float)STEPS;
    float e = (t < 0.5f) ? 2.0f*t*t : 1.0f - powf(-2.0f*t + 2.0f, 2)/2.0f;
    float p = start + (end - start) * e;
    u8g2.clearBuffer();
    drawHeaderBar();
    const int W = 128;
    const int w = W - PAD*2;
    const int x = W/2 - w/2;
    const int y = HEADER_H + 4;
    u8g2.setDrawColor(0);
    u8g2.drawBox(x+1, y+1, w-2, CARD_H-2);
    u8g2.setDrawColor(1);
    u8g2.drawRFrame(x, y, w, CARD_H, RADIUS);
    u8g2.setFont(FONT_MEDIUM); u8g2.setCursor(x+12, y+4);  u8g2.print(kMenuLabels[focusIdx]);
    u8g2.setFont(FONT_SMALL);  u8g2.setCursor(x+12, y+16); u8g2.print("----");
    const int pillW=24, pillH=14;
    const int pillY=(y + ((CARD_H-pillH)/2)) & ~1;
    const int pillX=x + w - pillW - 10;
    drawPillOutline(pillX, pillY, pillW, pillH);
    drawPillKnob(pillX, pillY, pillW, pillH, p);
    drawPaginationDots(focusIdx);
    u8g2.sendBuffer();
    delay(FRAME_MS);
  }
  renderStaticMenu(focusIdx);
}

void update_OLED() {
  int focus = menuIndexFromMode(current_Mode);
  renderStaticMenu(focus);
}

void showPowerLevelToast() {
  PowerLevel p = modePowerLevel[current_Mode];
  const char* pwrStr = powerLevelToString(p);
  u8g2.clearBuffer();
  drawHeaderBar();
  u8g2.setFont(FONT_SMALL);
  const char* label = isPowerOff(p) ? "TRANSMIT" : "POWER";
  int lw = u8g2.getStrWidth(label);
  u8g2.setCursor((128 - lw) / 2, HEADER_H + 4);
  u8g2.print(label);
  if (isPowerOff(p)) {
    u8g2.setFont(FONT_MEDIUM);
    int pw = u8g2.getStrWidth(pwrStr);
    u8g2.setCursor((128 - pw) / 2, HEADER_H + 22);
    u8g2.print(pwrStr);
    u8g2.setFont(FONT_SMALL);
    u8g2.setCursor(28, HEADER_H + 42);
    u8g2.print("Press S to MIN");
  } else {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    int pw = u8g2.getStrWidth(pwrStr);
    u8g2.setCursor((128 - pw) / 2, HEADER_H + 20);
    u8g2.print(pwrStr);
    u8g2.setFont(FONT_SMALL);
  }
  const char* modeStr = modeName(current_Mode);
  int mw = u8g2.getStrWidth(modeStr);
  u8g2.setCursor((128 - mw) / 2, 56);
  u8g2.print(modeStr);
  u8g2.sendBuffer();
  Serial.printf("[PWR] %s -> %s\n", modeStr, pwrStr);
  delay(700);
  update_OLED();
}

bool handleSKeyPress() {
  if (current == DEACTIVE_MODE) {
    current = ACTIVE_MODE;
    return true;
  }
  if (modeSupportsPowerAdjust(current_Mode)) {
    PowerLevel oldP = modePowerLevel[current_Mode];
    //MAX -> HIGH -> MIN -> OFF -> MAX
    PowerLevel p;
    if      (oldP == POWER_MAX)  p = POWER_HIGH;
    else if (oldP == POWER_HIGH) p = POWER_MIN;
    else if (oldP == POWER_MIN)  p = POWER_OFF;
    else                         p = POWER_MAX;
    modePowerLevel[current_Mode] = p;
    if (p == POWER_OFF) {
      RadioA.stopConstCarrier();
      RadioB.stopConstCarrier();
      RadioC.stopConstCarrier();
      const char* ledCol = nrf24InitOk ? "green" : "yellow";
      setNeoPixelColour(ledCol);
      Serial.printf("[LED] OFF -> %s (init %s)\n", ledCol, nrf24InitOk ? "OK" : "FAIL");
    } else if (oldP == POWER_OFF) {
      restartCarriers(powerLevelToPa(p));
      setNeoPixelColour("red");
      Serial.printf("[LED] %s -> red (transmitting)\n", powerLevelToString(p));
    } else {
      restartCarriers(powerLevelToPa(p));
      setNeoPixelColour("red");
    }
    showPowerLevelToast();
    return false;
  }
  current = DEACTIVE_MODE;
  return true;
}

void menuPrev() {
  int fromIdx = menuIndexFromMode(current_Mode);
  int toIdx   = (fromIdx == 0) ? (kMenuCount - 1) : (fromIdx - 1);
  current_Mode = modeFromMenuIndex(toIdx);
  animateToMenu(fromIdx, toIdx);
}
void menuNext() {
  int fromIdx = menuIndexFromMode(current_Mode);
  int toIdx   = (fromIdx == (kMenuCount - 1)) ? 0 : (fromIdx + 1);
  current_Mode = modeFromMenuIndex(toIdx);
  animateToMenu(fromIdx, toIdx);
}
void menuToggleActive() {
  int focus = menuIndexFromMode(current_Mode);
  bool wasActive = (current == ACTIVE_MODE);
  current = wasActive ? DEACTIVE_MODE : ACTIVE_MODE;
  animateToggleKnobForFocus(focus, wasActive, !wasActive);
}

void checkMode() {
  if (ChangeRequested) {
    ChangeRequested = false;
    current_Mode = static_cast<OperationMode>((current_Mode == 0) ? 7 : (current_Mode - 1));
  } else if (ChangeRequested1) {
    ChangeRequested1 = false;
    current_Mode = static_cast<OperationMode>((current_Mode + 1) % 8);
  }
}

void scanI2C() {
  Serial.println(F("[I2C] scanning bus 1..127 ..."));
  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      Serial.printf("[I2C]  -> device found at 0x%02X (%d)\n", addr, addr);
      found++;
    }
  }
  if (found == 0) {
    Serial.println(F("[I2C]  !! no device found. "));
  } else {
    Serial.printf("[I2C] scan done, %d device(s) found.\n", found);
  }

  Serial.println(F("[I2C] SSD1306 typical addr = 0x3C (SA0=0) or 0x3D (SA0=1)"));
}

void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println();
  Serial.println(F("=== RF-Clown boot ==="));
  Serial.printf("[OLED] SDA=%d  SCL=%d\n", PIN_OLED_SDA, PIN_OLED_SCL);

  pixels.begin();
  pixels.clear();
  pixels.show();

  initialize_MultiMode(false);  

  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  Wire.setClock(400000);


  scanI2C();

  Serial.println(F("[OLED] calling u8g2.begin() ..."));
  uint8_t u8ret = u8g2.begin();
  Serial.printf("[OLED] u8g2.begin() returned 0x%02X (0x00 = OK)\n", u8ret);
  u8g2.setBusClock(400000);
  u8g2.setFont(FONT_SMALL);
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_wifi_disconnect();
  pinMode(PIN_BTN_L,  INPUT_PULLUP);
  pinMode(PIN_BTN_R, INPUT_PULLUP);
  pinMode(PIN_BTN_S, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_L),  handleButton,  FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_R), handleButton1, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_S), handleButton2, FALLING);
  initialize_Radios();
  conf();
  update_OLED();
  Serial.println(F("[OLED] setup finished."));
  Serial.println(F("[PWR] S键循环 MAX/HIGH/MIN/OFF (BLE/BT/WiFi); 串口: MIN/HIGH/MAX/OFF/status/help"));
}

void loop() {
  handleSerialCommand(); 
  checkMode();

  if (ChangeRequested2) {
    ChangeRequested2 = false;
    bool stateChanged = handleSKeyPress();
    if (!stateChanged) {
      return;
    }
  }
  static Operation     lastActivity = current;
  static OperationMode lastFocus    = current_Mode;
  if (current_Mode != lastFocus) {
    int fromIdx = menuIndexFromMode(lastFocus);
    int toIdx   = menuIndexFromMode(current_Mode);
    animateToMenu(fromIdx, toIdx);
    lastFocus = current_Mode;

    if (current == ACTIVE_MODE) {
      PowerLevel p = modeSupportsPowerAdjust(current_Mode)
                     ? modePowerLevel[current_Mode] : POWER_MAX;
      if (p == POWER_OFF) {
        RadioA.stopConstCarrier();
        RadioB.stopConstCarrier();
        RadioC.stopConstCarrier();
        const char* ledCol = nrf24InitOk ? "green" : "yellow";
        setNeoPixelColour(ledCol);
        Serial.printf("[NRF24] mode switch -> %s, power=OFF, LED=%s\n",
                      modeName(current_Mode), ledCol);
      } else {
        restartCarriers(powerLevelToPa(p));
        setNeoPixelColour("red");
        Serial.printf("[NRF24] mode switch -> %s, power=%s, LED=red\n",
                      modeName(current_Mode), powerLevelToString(p));
      }
    }
    return;
  }
  if (current != lastActivity) {
    initialize_Radios();
    int  focus     = menuIndexFromMode(current_Mode);
    bool wasActive = (lastActivity == ACTIVE_MODE);
    bool nowActive = (current      == ACTIVE_MODE);
    animateToggleKnobForFocus(focus, wasActive, nowActive);
    lastActivity = current;
    return;
  }

 
  byte currentChannel = 0;
  bool canTransmit = (current == ACTIVE_MODE) &&
                     (!modeSupportsPowerAdjust(current_Mode) ||
                      !isPowerOff(modePowerLevel[current_Mode]));
  if (canTransmit) {
    if (current_Mode == BLE_MODULE) {
      int randomIndex = random(0, sizeof(ble_channels) / sizeof(ble_channels[0]));
      currentChannel = ble_channels[randomIndex];
    } else if (current_Mode == Bluetooth_MODULE) {
      int randomIndex = random(0, sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]));
      currentChannel = bluetooth_channels[randomIndex];
    } else if (current_Mode == WiFi_MODULE) {
      int randomIndex = random(0, sizeof(WiFi_channels) / sizeof(WiFi_channels[0]));
      currentChannel = WiFi_channels[randomIndex];
    } else if (current_Mode == USB_WIRELESS_MODULE) {
      int randomIndex = random(0, sizeof(usbWireless_channels) / sizeof(usbWireless_channels[0]));
      currentChannel = usbWireless_channels[randomIndex];
    } else if (current_Mode == VIDEO_TX_MODULE) {
      int randomIndex = random(0, sizeof(videoTransmitter_channels) / sizeof(videoTransmitter_channels[0]));
      currentChannel = videoTransmitter_channels[randomIndex];
    } else if (current_Mode == RC_MODULE) {
      int randomIndex = random(0, sizeof(rc_channels) / sizeof(rc_channels[0]));
      currentChannel = rc_channels[randomIndex];
    } else if (current_Mode == ZIGBEE_MODULE) {
      int randomIndex = random(0, sizeof(zigbee_channels) / sizeof(zigbee_channels[0]));
      currentChannel = zigbee_channels[randomIndex];
    } else if (current_Mode == NRF24_MODULE) {
      int randomIndex = random(0, sizeof(nrf24_channels) / sizeof(nrf24_channels[0]));
      currentChannel = nrf24_channels[randomIndex];
    }
    RadioA.setChannel(currentChannel);
    RadioB.setChannel(currentChannel);
    RadioC.setChannel(currentChannel);
  }

  static unsigned long lastStatusMs = 0;
  const unsigned long STATUS_INTERVAL_MS = 1000;
  unsigned long nowMs = millis();
  if (current == ACTIVE_MODE && (nowMs - lastStatusMs >= STATUS_INTERVAL_MS)) {
    lastStatusMs = nowMs;
    printNrf24Status(currentChannel);
  }
}
