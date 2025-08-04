#ifndef AudioLabSettings_h
#define AudioLabSettings_h

#define DEBUG

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 16384
#endif

#ifndef WINDOW_SIZE
#define WINDOW_SIZE 256
#endif

#define NUM_IN_CH 1         // Number of input channels to sample

#define IN_PIN_CH1 A2
#define IN_PIN_CH2 A3

#define USING_AD5644_DAC    // Uncomment if using AD5644 SPI DAC

// AD5644 SPI DAC
#if defined(USING_AD5644_DAC)
#define NUM_OUT_CH 4
#define DAC_RESOLUTION 14
#define DAC_PIN_SS 33
// #define AUD_IN_OUT_SAMPLE_RATIO 2       // Uncomment to write to each channel at (SAMPLE_RATE >> AUD_IN_OUT_SAMPLE_RATIO) Hz

// Default Feather DACs
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
#error BOARD NOT SUPPORTED/TESTED   // comment out for testing with other boards
#define ADC_RESOLUTION 12
#define DAC_RESOLUTION 12
#endif

#ifndef AUD_IN_OUT_SAMPLE_RATIO
#define AUD_IN_OUT_SAMPLE_RATIO 0 // in powers of 2 (0 = 1:1, 1 = 1:2, 2 = 1:4)
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
