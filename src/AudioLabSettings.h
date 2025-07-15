#ifndef AudioLabSettings_h
#define AudioLabSettings_h

#define DEBUG

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 8192
#endif

#ifndef WINDOW_SIZE
#define WINDOW_SIZE 64
#endif

#define AUD_IN_OUT_SAMPLE_RATIO 0 // in powers of 2 (0 = 1:1, 1 = 1:2, 2 = 1:4)

#define NUM_IN_CH 1

#define IN_PIN_CH1 A2
#define IN_PIN_CH2 A3

//#define USING_ADAFRUIT_MCP4728_DAC

//#define USING_ADAFRUIT_AD5644_DAC

// UNCOMMENT IF USING MCP4728 EXTERNAL DAC
#if defined(USING_MCP4728_DAC)
#define NUM_OUT_CH 4
#define DAC_RESOLUTION 12
// UNCOMMENT IF USING AD5644 EXTERNAL DAC
#elif defined(USING_AD5644_DAC)
#define NUM_OUT_CH 4
#define DAC_RESOLUTION 14
// DEFAULT FEATHER DACS
#else
#define NUM_OUT_CH 2
#define OUT_PIN_CH1 A0
#define OUT_PIN_CH2 A1
#endif

#if defined(ESP32)
#define ADC_RESOLUTION 12
    #ifndef DAC_RESOLUTION
    #define DAC_RESOLUTION 8
    #endif
#elif defined(__SAMD51__)
#define ADC_RESOLUTION 12
    #ifndef DAC_RESOLUTION
    #define DAC_RESOLUTION 12
    #endif
#else
#error BOARD NOT SUPPORTED
#endif

const int AUD_OUT_SAMPLE_RATE = int(SAMPLE_RATE) >> AUD_IN_OUT_SAMPLE_RATIO;
const int AUD_OUT_WINDOW_SIZE = int(WINDOW_SIZE) >> AUD_IN_OUT_SAMPLE_RATIO;

#if defined(DEBUG)
#define DBG_print(...) Serial.print(__VA_ARGS__)
#define DBG_println(...) Serial.println(__VA_ARGS__)
#define DBG_printf(...) Serial.printf(__VA_ARGS__)
#else
#define DBG_print(...) NULL
#define DBG_println(...) NULL
#define DBG_printf(...) NULL
#endif

#endif
