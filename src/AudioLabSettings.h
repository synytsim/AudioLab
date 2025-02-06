#ifndef AudioLabSettings_h
#define AudioLabSettings_h

#define DEBUG

#define SAMPLE_RATE 8192

#define WINDOW_SIZE 128

#define AUD_IN_OUT_SAMPLE_RATIO 0 // in powers of 2! (0 = 1:1, 1 = 1:2, 2 = 1:4)

#define NUM_IN_CH 1

#define IN_PIN_CH1 A2
#define IN_PIN_CH2 A3

//#define USING_ADAFRUIT_MCP4728_DAC

#if defined(USING_ADAFRUIT_MCP4728_DAC)
#define NUM_OUT_CH 4
#define DAC_RESOLUTION 12
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
