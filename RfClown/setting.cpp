/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/rfclown
   ________________________________________ */

#include "setting.h"
#include "config.h"

RF24 RadioA(NRF_CE_PIN_A, NRF_CSN_PIN_A);
RF24 RadioB(NRF_CE_PIN_B, NRF_CSN_PIN_B);
RF24 RadioC(NRF_CE_PIN_C, NRF_CSN_PIN_C);

void setRadiosNeutralState() {
  RadioA.stopListening();
  RadioA.setAutoAck(false);
  RadioA.setRetries(0, 0);
  RadioA.powerDown(); 
  digitalWrite(NRF_CE_PIN_A, LOW); 

  RadioB.stopListening();
  RadioB.setAutoAck(false);
  RadioB.setRetries(0, 0);
  RadioB.powerDown(); 
  digitalWrite(NRF_CE_PIN_B, LOW); 

  RadioC.stopListening();
  RadioC.setAutoAck(false);
  RadioC.setRetries(0, 0);
  RadioC.powerDown(); 
  digitalWrite(NRF_CE_PIN_C, LOW); 
}

void configureNrf(RF24 &radio) {
  radio.begin();
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
}

rf24_pa_dbm_e powerLevelToPa(PowerLevel p) {
  switch (p) {
    case POWER_MIN:  return RF24_PA_MIN;
    case POWER_HIGH: return RF24_PA_HIGH;
    case POWER_OFF:  return RF24_PA_MIN;
    case POWER_MAX:
    default:         return RF24_PA_MAX;
  }
}

const char* powerLevelToString(PowerLevel p) {
  switch (p) {
    case POWER_MIN:  return "MIN";
    case POWER_HIGH: return "HIGH";
    case POWER_OFF:  return "OFF";
    case POWER_MAX:
    default:         return "MAX";
  }
}

void setAllRadiosPALevel(rf24_pa_dbm_e pa) {
  RadioA.setPALevel(pa, true);
  RadioB.setPALevel(pa, true);
  RadioC.setPALevel(pa, true);
}

void applyPowerLevelForMode(OperationMode m) {
  if (!modeSupportsPowerAdjust(m)) {
    setAllRadiosPALevel(RF24_PA_MAX);
    return;
  }
  PowerLevel p = modePowerLevel[m];
  if (p == POWER_OFF) {
    RadioA.powerDown();
    RadioB.powerDown();
    RadioC.powerDown();
  } else {
    RadioA.powerUp();
    RadioB.powerUp();
    RadioC.powerUp();
    setAllRadiosPALevel(powerLevelToPa(p));
  }
}

void setupRadioA() {
  configureNrf(RadioA);
}

void setupRadioB() {
  configureNrf(RadioB);
}

void setupRadioC() {
  configureNrf(RadioC);
}

void initAllRadios() {
  setupRadioA();
  setupRadioB();
  setupRadioC();
}

void Str(uint8_t x, uint8_t y, const uint8_t* asciiArray, size_t len) {
  char buf[64]; 
  for (size_t i = 0; i < len && i < sizeof(buf) - 1; i++) {
    buf[i] = (char)asciiArray[i];
  }
  buf[len] = '\0';

  u8g2.drawStr(x, y, buf);
}

void CenteredStr(uint8_t screenWidth, uint8_t y, const uint8_t* asciiArray, size_t len, const uint8_t* font) {
  char buf[64];
  for (size_t i = 0; i < len && i < sizeof(buf) - 1; i++) {
    buf[i] = (char)asciiArray[i];
  }
  buf[len] = '\0';

  u8g2.setFont((const uint8_t*)font);
  int16_t w = u8g2.getUTF8Width(buf);
  u8g2.setCursor((screenWidth - w) / 2, y);
  u8g2.print(buf);
}

void conf() {
  u8g2.setBitmapMode(1);
  u8g2.clearBuffer();
  CenteredStr(128, 15, txt_n, sizeof(txt_n), u8g2_font_ncenB14_tr);
  CenteredStr(120, 35, txt_c, sizeof(txt_c), u8g2_font_ncenB08_tr);
  CenteredStr(128, 50, txt_v, sizeof(txt_v), u8g2_font_6x10_tf);
  u8g2.sendBuffer();
  delay(3000);
  u8g2.clearBuffer();
  u8g2.drawXBMP(0, 0, 128, 64, cred); 
  u8g2.sendBuffer();
  delay(250);
}

 
