#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

static const uint8_t TX = 1;
static const uint8_t RX = 2;

static const uint8_t SDA = 47;
static const uint8_t SCL = 48;

// Modified elsewhere
static const uint8_t SS = 3;
static const uint8_t MOSI = 18;
static const uint8_t MISO = 8;
static const uint8_t SCK = 17;

#define SERIAL_RX 2
#define SERIAL_TX 1
#define BAD_RX SERIAL_RX
#define BAD_TX SERIAL_TX
#define USB_as_HID 1

#define BTN_ALIAS "\"OK\""
#define HAS_5_BUTTONS
#define SEL_BTN 1
#define UP_BTN 41
#define DW_BTN 40
#define R_BTN 38
#define L_BTN 39
#define BTN_ACT LOW

// --- Пины энкодера (как на T-Embed) ---
// #define HAS_ENCODER
// #define ENCODER_INA 38
// #define ENCODER_INB 40
// #define ENCODER_KEY 0
// #define HAS_BTN 1
// #define BTN_ALIAS "\"Mid\""
// #define SEL_BTN ENCODER_KEY
// #define BTN_ACT LOW

#define RXLED 19
#define LED -1
#define LED_ON HIGH
#define LED_OFF LOW

#define USE_CC1101_VIA_SPI
#define CC1101_GDO0_PIN 9
#define CC1101_GDO2_PIN 10
#define CC1101_SS_PIN 46
#define CC1101_MOSI_PIN SPI_MOSI_PIN
#define CC1101_SCK_PIN SPI_SCK_PIN
#define CC1101_MISO_PIN SPI_MISO_PIN

#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN 21
#define NRF24_SS_PIN 14
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#define NRF24_SCK0_PIN SPI_SCK_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN

#define FP 1
#define FM 2
#define FG 3

#define HAS_SCREEN 1
#define ROTATION 1
#define MINBRIGHT (uint8_t)1

#define USER_SETUP_LOADED 1
#define ST7735_DRIVER 1
#define ST7735_GREENTAB3 1
// #define TFT_X_OFFSET -3 // Смещение по ширине (влево на 3 пикселя)
// #define TFT_Y_OFFSET -2 // Смещение по высоте (вверх на 2 пикселя)
// #define TFT_RGB_ORDER 1
#define TFT_WIDTH 128
#define TFT_HEIGHT 160
#define TFT_BACKLIGHT_ON 1
#define TFT_BL 6
#define TFT_RST 16
#define TFT_DC 15
#define TFT_MISO -8
#define TFT_MOSI 17
#define TFT_SCLK 18
#define TFT_CS 7
#define TOUCH_CS -1 // SDCARD_CS to make sure SDCard works
#define SMOOTH_FONT 1
#define SPI_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 4000000
#define SPI_TOUCH_FREQUENCY 2500000

#define SDCARD_CS -3
#define SDCARD_SCK -18
#define SDCARD_MISO -8
#define SDCARD_MOSI -17

#define GROVE_SDA 47
#define GROVE_SCL 48

#define SPI_SCK_PIN 13//18
#define SPI_MOSI_PIN 12//17
#define SPI_MISO_PIN 11//10
#define SPI_SS_PIN 43

// RGB LED

#define HAS_RGB_LED 1
#define RGB_LED 45
#define LED_TYPE WS2812B
#define LED_ORDER GRB
#define LED_TYPE_IS_RGBW 0
#define LED_COUNT 16
#define LED_COLOR_STEP 15
//#define BAT_PIN 8
//#define USE_BQ25896

// Mic#
#define PIN_CLK 1
#define PIN_DATA -10
#define PIN_WS 2

// IO EXPANDER
#define USE_IO_EXPANDER
#define IO_EXPANDER_AW9523
#define IO_EXP_GPS 13
#define IO_EXP_MIC 4
#define IO_EXP_VIBRO 2
#define IO_EXP_CC_RX 7
#define IO_EXP_CC_TX 12
#endif /* Pins_Arduino_h */
