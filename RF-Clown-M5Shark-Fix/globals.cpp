// RF_CLOWN_GLOBAL_DEFINITIONS_PATCH
// Global definitions moved out of headers to fix Arduino CLI multiple-definition linker errors.

#include "config.h"
#include "setting.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Adafruit_NeoPixel pixels(1, 14, NEO_GRB + NEO_KHZ800);
OperationMode current_Mode = WiFi_MODULE;
volatile Operation current = DEACTIVE_MODE;
byte channelGroup_1[] = {2, 5, 8, 11};
byte channelGroup_2[] = {26, 29, 32, 35};
byte channelGroup_3[] = {80, 83, 86, 89};
volatile bool ChangeRequested  = false;
volatile bool ChangeRequested1 = false;
volatile bool ChangeRequested2 = false;
unsigned long lastPressTime = 0;
bool neoPixelActive = false;
uint8_t oledBrightness = 100;


// Channel group lengths for extern arrays.
extern const size_t channelGroup_1_len = sizeof(channelGroup_1);
extern const size_t channelGroup_2_len = sizeof(channelGroup_2);
extern const size_t channelGroup_3_len = sizeof(channelGroup_3);
