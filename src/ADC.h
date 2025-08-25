#include <AudioLab.h>

#ifdef ESP32

#if (NUM_IN_CH == 1)
uint8_t adc_pins[] = {IN_PIN_CH1};  //some of ADC1 pins for ESP32
    #if (SAMPLE_RATE == 8192)
    #define ADC_SAMPLE_RATE 80255 // ~8kHz at 4 conversions per pin and 2 4-channel SPI DACs
    #elif (SAMPLE_RATE == 16384)
    #define ADC_SAMPLE_RATE 160825 // ~16kHz at 4 conversions per pin and 2 4-channel SPI DACs
    #elif (SAMPLE_RATE == 32768)
    #define ADC_SAMPLE_RATE 323500 // ~32kHz at 4 conversions per pin and 2 4-channel SPI DACs

    #else
    #warning ADC_SAMPLE_RATE MAY BE INACCURATE
    #define ADC_SAMPLE_RATE SAMPLE_RATE * adc_pins_count * (CONVERSION_PER_PIN + 1)
    #endif
#else
uint8_t adc_pins[] = {IN_PIN_CH1, IN_PIN_CH2};  //some of ADC1 pins for ESP32
#if (SAMPLE_RATE == 8192)
    #define ADC_SAMPLE_RATE 80255 // ~8kHz at 4 conversions per pin and 2 4-channel SPI DACs
    #elif (SAMPLE_RATE == 16384)
    #define ADC_SAMPLE_RATE 160825 // ~16kHz at 4 conversions per pin and 2 4-channel SPI DACs
    #elif (SAMPLE_RATE == 32768)
    #define ADC_SAMPLE_RATE 323500 // ~32kHz at 4 conversions per pin and 2 4-channel SPI DACs
    #else
    #warning ADC_SAMPLE_RATE MAY BE INACCURATE
    #define ADC_SAMPLE_RATE SAMPLE_RATE * adc_pins_count * (CONVERSION_PER_PIN + 1)
    #endif
#endif

#define CONVERSIONS_PER_PIN 4

// Calculate how many pins are declared in the array - needed as input for the setup function of ADC Continuous
const uint8_t adc_pins_count = sizeof(adc_pins) / sizeof(uint8_t);

// Result structure for ADC Continuous reading
adc_continuous_data_t *result = NULL;
#endif