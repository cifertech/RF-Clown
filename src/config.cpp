#include "config.h"

// Instantiate display and neopixel objects
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Adafruit_NeoPixel pixels(1, 14, NEO_GRB + NEO_KHZ800);

// Font pointers
const uint8_t* FONT_SMALL  = u8g2_font_5x8_tf;
const uint8_t* FONT_MEDIUM = u8g2_font_6x12_tf;
const uint8_t* FONT_ICON_FALLBACK = u8g2_font_open_iconic_thing_2x_t;

// Operation state
OperationMode current_Mode = WiFi_MODULE;
volatile Operation current = DEACTIVE_MODE;

// Channel groups
const byte channelGroup_1[] = {2, 5, 8, 11};
const byte channelGroup_2[] = {26, 29, 32, 35};
const byte channelGroup_3[] = {80, 83, 86, 89};

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
