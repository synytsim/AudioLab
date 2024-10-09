#ifndef AudioLabSettings_h
#define AudioLabSettings_h

#define SAMPLE_RATE 8192

#define WINDOW_SIZE 256

#define NUM_IN_CH 1

#define IN_PIN_CH1 A2
#define IN_PIN_CH2 A3

#define NUM_OUT_CH 2

#define OUT_PIN_CH1 A0
#define OUT_PIN_CH2 A1

#if defined(ARDUINO_FEATHER_ESP32)
#define ADC_RESOLUTION 12
#define DAC_RESOLUTION 8
#elif defined(__SAMD51__)
#define ADC_RESOLUTION 12
#define DAC_RESOLUTION 12
#else
#error BOARD NOT SUPPORTED
#endif

#endif