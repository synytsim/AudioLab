/*******************************************************
 * File:        AudioLabSettings.h
 * Description: This file contains settings such as:
 *              - pin declarations, 
 *              - internal DAC settings - With ESP32 Feather footprint A0(34) and A1(39) pins are used for output
 *              - external DAC settings - At this time this library supports the 1 or 2 AD56X4 DAC(s)
 *                                        Copy AD56X4 to Arduino libraries folder
 *              - internal ADC settings - A2 and A3 are used (this can be modified if necassary)
 *              - sample rate           - Try using powers of 2 in order to get most consistent results (8, 16 and 32k are supported)
 *                                        See ADC.h to modify ADC sample rate, use Timing.ino example for fine tuning
 *              - window size           - The window size (and sample rate) determine the frequency at which AudioLab.ready() returns
 *                                        rue (i.e. sample rate of 8192 and window size of 256 = 32 times per second = 8192 / 256)
 *               
 * Author:      Mykyta "Nick" Synytsia
 * Date:        2023/08/25
 * Version:     2.0
 *******************************************************/


#ifndef AUDIOLAB_SETTINGS_H
#define AUDIOLAB_SETTINGS_H

#define DEBUG

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 8192
#endif

#ifndef WINDOW_SIZE
#define WINDOW_SIZE 256
#endif

/******************************** FEATHER ADC PINS ********************************/
#define NUM_IN_CH 2         // Number of input channels to sample (Limit is 2, but adding more is fairly straightforward)

#define IN_PIN_CH1 A2       
#define IN_PIN_CH2 A3       

/******************************** AD56X4 DAC SETTINGS ********************************/
#define USING_AD56X4_DAC    // Uncomment if using AD5644 SPI DAC

#if defined(USING_AD56X4_DAC)
#define DAC_RESOLUTION 16   // The AD56X4 comes in 3 versions: 12, 14 and 16 bit

// #define NUM_OUT_CH 4     // Uncomment if using 1xAD56X4 SPI DAC and comment below
#define NUM_OUT_CH 8        // Uncomment if using 2xAD56X 4 SPI DAC and uncomment below

#define DAC_PIN_SS_1 33     // Slave select pin for first AD56X4 
#define DAC_PIN_SS_2 32     // Slave select pin for second AD56X4 

#define AUD_IN_OUT_SAMPLE_RATIO 2       // Uncomment to write to each AD56X4 channel at 
                                        // (SAMPLE_RATE >> AUD_IN_OUT_SAMPLE_RATIO) Hz
                                        // Note: in powers of 2


/******************************** FEATHER ESP32 AND SAMD51 DAC RESOLUTION ********************************/
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
#else
#error BOARD NOT SUPPORTED/TESTED   // comment out for testing with other boards
#define ADC_RESOLUTION 12
#define DAC_RESOLUTION 12
#endif

/******************************** AUDIO INPUT OUTPUT SAMPLED RATIO ********************************/
#ifndef AUD_IN_OUT_SAMPLE_RATIO
#define AUD_IN_OUT_SAMPLE_RATIO 0 // Modify to achieve required output sample rate (in powers of 2 (0 = 1:1, 1 = 2:1, 2 = 4:1))
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
