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

// ESP-specific configurations
#include <esp_bt.h>

// Forward declarations of global objects (defined in config.cpp)
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
extern Adafruit_NeoPixel pixels;

// Fonts (extern pointers to font data)
extern const uint8_t* FONT_SMALL;
extern const uint8_t* FONT_MEDIUM;
extern const uint8_t* FONT_ICON_FALLBACK;

enum OperationMode {WiFi_MODULE, VIDEO_TX_MODULE, RC_MODULE, BLE_MODULE, Bluetooth_MODULE, USB_WIRELESS_MODULE, ZIGBEE_MODULE, NRF24_MODULE};
extern OperationMode current_Mode;

enum Operation {DEACTIVE_MODE, ACTIVE_MODE};
extern volatile Operation current;

// Channel groups (defined in config.cpp)
extern const byte channelGroup_1[4];
extern const byte channelGroup_2[4];
extern const byte channelGroup_3[4];

extern const byte bluetooth_channels[21];
extern const byte ble_channels[3];
extern const byte WiFi_channels[12];
extern const byte usbWireless_channels[3];
extern const byte videoTransmitter_channels[3];
extern const byte rc_channels[4];
extern const byte zigbee_channels[4];
extern const byte nrf24_channels[3];

// Array sizes for use in other translation units
#define CHANNEL_GROUP_1_COUNT 4
#define CHANNEL_GROUP_2_COUNT 4
#define CHANNEL_GROUP_3_COUNT 4

#define BLUETOOTH_CHANNELS_COUNT 21
#define BLE_CHANNELS_COUNT 3
#define WIFI_CHANNELS_COUNT 12
#define USBWIRELESS_CHANNELS_COUNT 3
#define VIDEO_TX_CHANNELS_COUNT 3
#define RC_CHANNELS_COUNT 4
#define ZIGBEE_CHANNELS_COUNT 4
#define NRF24_CHANNELS_COUNT 3

extern volatile bool ChangeRequested;
extern volatile bool ChangeRequested1;
extern volatile bool ChangeRequested2;

extern unsigned long lastPressTime;
extern const unsigned long debounceDelay;

#endif // CONFIG_H
