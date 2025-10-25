/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/rfclown
   ________________________________________ */

#ifndef CONFIG_H
#define CONFIG_H

#define SCREEN_W 128
#define SCREEN_H 64

// ---------------- Pins ----------------
#define PIN_BTN_L  27
#define PIN_BTN_R  25
#define PIN_BTN_S  26

// nRF24-specific Pins
#define NRF_CE_PIN_A    5   
#define NRF_CSN_PIN_A   17 
#define NRF_CE_PIN_B    16  
#define NRF_CSN_PIN_B   4  
#define NRF_CE_PIN_C    15  
#define NRF_CSN_PIN_C   2  

// Common dependencies
#include "setting.h"
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <Preferences.h>
#include <vector>
#include <string>
#include <math.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Adafruit_NeoPixel pixels(1, 14, NEO_GRB + NEO_KHZ800);

// BLE-specific dependencies
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// nRF24-specific dependencies
#include <nRF24L01.h>
#include <RF24.h>

// WiFi-specific dependencies
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <esp_system.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <string>

// ESP-specific configurations
#include <esp_bt.h>
#include <esp_wifi.h>

// External declarations
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
extern Adafruit_NeoPixel pixels;

static const uint8_t* FONT_SMALL  = u8g2_font_5x8_tf;
static const uint8_t* FONT_MEDIUM = u8g2_font_6x12_tf;
static const uint8_t* FONT_ICON_FALLBACK = u8g2_font_open_iconic_thing_2x_t;

enum OperationMode {WiFi_MODULE, VIDEO_TX_MODULE, RC_MODULE, BLE_MODULE, Bluetooth_MODULE, USB_WIRELESS_MODULE, ZIGBEE_MODULE, NRF24_MODULE};
OperationMode current_Mode = WiFi_MODULE;

enum Operation {DEACTIVE_MODE, ACTIVE_MODE};
volatile Operation current = DEACTIVE_MODE;

byte channelGroup_1[] = {2, 5, 8, 11};
byte channelGroup_2[] = {26, 29, 32, 35};
byte channelGroup_3[] = {80, 83, 86, 89};

const byte bluetooth_channels[] =        {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
const byte ble_channels[]       =        {2, 26, 80};
const byte WiFi_channels[]      =        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
const byte usbWireless_channels[] =      {40, 50, 60};
const byte videoTransmitter_channels[] = {70, 75, 80};
const byte rc_channels[]        =        {1, 3, 5, 7};
const byte zigbee_channels[]    =        {11, 15, 20, 25};
const byte nrf24_channels[]     =        {76, 78, 79};

volatile bool ChangeRequested  = false;
volatile bool ChangeRequested1 = false;
volatile bool ChangeRequested2 = false;

unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 200;

#endif // CONFIG_H
