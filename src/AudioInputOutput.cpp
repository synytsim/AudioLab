#include "AudioLab.h"
#include "ADC.h"


#ifdef USING_AD56X4_DAC
#include <AD56X4.h>
byte dac_channel[] = { AD56X4_CHANNEL_A, AD56X4_CHANNEL_B, AD56X4_CHANNEL_C, AD56X4_CHANNEL_D };

#endif

const uint16_t SAMPLE_RATIO = 1 << AUD_IN_OUT_SAMPLE_RATIO;

volatile uint16_t ClassAudioLab::AUD_IN_BUFFER[NUM_IN_CH][AUD_IN_BUFFER_SIZE];
volatile uint16_t ClassAudioLab::AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];

volatile uint16_t AUD_IN_BUFFER_IDX = 0;
volatile uint16_t AUD_OUT_BUFFER_IDX = 0;
volatile uint16_t AUD_OUT_BUFFER_POS = 0;
volatile uint8_t AUD_IN_SAMPLE_COUNT = 0;

/*
 * Ensures pin are configured
 */
void ClassAudioLab::configurePins(void) {
  #ifdef USING_AD56X4_DAC

  // setting up AD5644 SPI DAC
  SPI.begin();

  SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE1));
  
  pinMode(DAC_PIN_SS_1, OUTPUT);
  AD56X4.reset(DAC_PIN_SS_1, true);
  AD56X4.useInternalReference(DAC_PIN_SS_1, true);
  AD56X4.setChannel(DAC_PIN_SS_1, AD56X4_SETMODE_INPUT_DAC_ALL, DAC_MID, DAC_MID, DAC_MID, DAC_MID);

  #if (NUM_OUT_CH == 8)
  pinMode(DAC_PIN_SS_2, OUTPUT);
  AD56X4.reset(DAC_PIN_SS_2, true);
  AD56X4.useInternalReference(DAC_PIN_SS_2, true);
  AD56X4.setChannel(DAC_PIN_SS_2, AD56X4_SETMODE_INPUT_DAC_ALL, DAC_MID, DAC_MID, DAC_MID, DAC_MID);
  #endif

  #else

  // otherwise assuming using on-board DAC(s)
  #if NUM_OUT_CH > 0
  #ifdef __SAMD51__
  analogWriteResolution(DAC_RESOLUTION);
  #endif
  pinMode(OUT_PIN_CH1, OUTPUT);
  #endif
  #if NUM_OUT_CH > 1
  pinMode(OUT_PIN_CH2, OUTPUT);
  #endif

  #endif

  // setting up on-board ADC(s)
  #if NUM_IN_CH > 0 
    #if defined(ESP32)
    // analogSetWidth(ADC_RESOLUTION);
    // analogSetAttenuation(ADC_11db);
    analogContinuousSetWidth(12);
    analogContinuousSetAtten(ADC_11db);
    // array of pins, count of the pins, how many conversions per pin in one cycle will happen, sampling frequency, callback function
    if (!analogContinuous(adc_pins, adc_pins_count, CONVERSIONS_PER_PIN, ADC_SAMPLE_RATE, &AUD_IN_OUT)) Serial.println("Error: analogContinous()");

    #elif defined(__SAMD51__)
    analogReadResolution(ADC_RESOLUTION);
    pinMode(IN_PIN_CH1, INPUT);
    analogRead(IN_PIN_CH1);
    #endif
  #endif
  #if NUM_IN_CH > 1
    #if defined(__SAMD51__)
    pinMode(IN_PIN_CH2, INPUT);
    analogRead(IN_PIN_CH2);
    #endif
  #endif
}

void ClassAudioLab::pauseSampling(void) {
  if (!analogContinuousStop()) Serial.println("Error: analogContinousStop()"); 
}

void ClassAudioLab::resumeSampling(void) {
  if (!analogContinuousStart()) Serial.println("Error: analogContinousStart()");
}

/*
 * Resets input and output buffers and buffer positions
 */
void ClassAudioLab::resetAudInOut(void) {
  uint16_t i, c;
  for (i = 0; i < AUD_OUT_BUFFER_SIZE; i++) {
    for (c = 0; c < NUM_OUT_CH; c++) {
      AUD_OUT_BUFFER[c][i] = DAC_MID;
    }
    if (i < AUD_IN_BUFFER_SIZE) {
      for (c = 0; c < NUM_IN_CH; c++) {
        AUD_IN_BUFFER[c][i] = ADC_MID;
      }
    }
  }

  AUD_IN_BUFFER_IDX = 0;
  AUD_OUT_BUFFER_POS = 0;
}

void ClassAudioLab::AUD_IN_OUT(void) {
  if (AUD_IN_BUFFER_FULL()) return; // do nothing if input buffer is full (shouldn't really happen)

  // this condition ensures that AUD_OUT_BUFFER_IDX is properly updated regardless of input:output sample rate ratio
  if (AUD_IN_SAMPLE_COUNT == 0) { // start if
    AUD_OUT_BUFFER_IDX = AUD_OUT_BUFFER_POS + (AUD_IN_BUFFER_IDX >> AUD_IN_OUT_SAMPLE_RATIO);

  #if defined(USING_AD56X4_DAC)
  }                               // end if statement when AD5644 is in use
  
  // if input:output ratio is 1:1, each channel is written to at SAMPLE_RATE Hz
  #if (AUD_IN_OUT_SAMPLE_RATIO == 0)
  AD56X4.setChannel(DAC_PIN_SS_1, AD56X4_SETMODE_INPUT_DAC_ALL, AUD_OUT_BUFFER[0][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[1][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[2][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[3][AUD_OUT_BUFFER_IDX]);
  #if (NUM_OUT_CH == 8)
  AD56X4.setChannel(DAC_PIN_SS_2, AD56X4_SETMODE_INPUT_DAC_ALL, AUD_OUT_BUFFER[4][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[5][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[6][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[7][AUD_OUT_BUFFER_IDX]);
  #endif
  // if input:output ratio is not 1:1, each channel is written to at SAMPLE_RATE >> AUD_IN_OUT_SAMPLE_RATIO
  #else
  AD56X4.setChannel(DAC_PIN_SS_1, AD56X4_SETMODE_INPUT_DAC, dac_channel[AUD_IN_SAMPLE_COUNT], AUD_OUT_BUFFER[AUD_IN_SAMPLE_COUNT][AUD_OUT_BUFFER_IDX]);
  #if (NUM_OUT_CH == 8)
  AD56X4.setChannel(DAC_PIN_SS_2, AD56X4_SETMODE_INPUT_DAC, dac_channel[AUD_IN_SAMPLE_COUNT], AUD_OUT_BUFFER[4 + AUD_IN_SAMPLE_COUNT][AUD_OUT_BUFFER_IDX]);
  #endif
  #endif
  
  #else   // otherwise, assuming on-board DACs are used, continue if statement
  
  #if NUM_OUT_CH > 0
    #if defined(ESP32)
    dacWrite(OUT_PIN_CH1, AUD_OUT_BUFFER[0][AUD_OUT_BUFFER_IDX]);
    #elif defined(__SAMD51__)
    analogWrite(OUT_PIN_CH1, AUD_OUT_BUFFER[0][AUD_OUT_BUFFER_IDX]);
    #endif
  #endif
  #if NUM_OUT_CH > 1
    #if defined(ESP32)
    dacWrite(OUT_PIN_CH2, AUD_OUT_BUFFER[1][AUD_OUT_BUFFER_IDX]);
    #elif defined(__SAMD51__)
    analogWrite(OUT_PIN_CH2, AUD_OUT_BUFFER[1][AUD_OUT_BUFFER_IDX]);
    #endif
  #endif
  }                               // end if statement for on-board DACs
  #endif

  // For sampling on-board ADCs
  #if NUM_IN_CH > 0
  analogContinuousRead(&result, 0);
  AUD_IN_BUFFER[0][AUD_IN_BUFFER_IDX] = result[0].avg_read_raw;
  #endif
  #if NUM_IN_CH > 1
  AUD_IN_BUFFER[1][AUD_IN_BUFFER_IDX] = result[1].avg_read_raw;
  #endif

  // #if NUM_IN_CH > 0
  // AUD_IN_BUFFER[0][AUD_IN_BUFFER_IDX] = analogRead(IN_PIN_CH1);
  // #endif
  // #if NUM_IN_CH > 1
  // AUD_IN_BUFFER[1][AUD_IN_BUFFER_IDX] = analogRead(IN_PIN_CH2);
  // #endif
  
  AUD_IN_BUFFER_IDX += 1;

  AUD_IN_SAMPLE_COUNT += 1;
  if (AUD_IN_SAMPLE_COUNT >= SAMPLE_RATIO) AUD_IN_SAMPLE_COUNT = 0; // this is primarily for input:output sample rate ratio

}

/*
 * Returns true when input buffer is filled
 */
bool ClassAudioLab::AUD_IN_BUFFER_FULL(void) {
  return !(AUD_IN_BUFFER_IDX < WINDOW_SIZE);
}

/*
 * This function ensures that input and output buffer positions are synchronized
 */
void ClassAudioLab::SYNC_AUD_IN_OUT_IDX(void) {
  AUD_OUT_BUFFER_POS += AUD_OUT_WINDOW_SIZE;
  if (AUD_OUT_BUFFER_POS >= AUD_OUT_BUFFER_SIZE) AUD_OUT_BUFFER_POS = 0;
  AUD_IN_BUFFER_IDX = 0;
}

void ClassAudioLab::printAudioOutputBuffer(uint8_t aChannel) {
  for (uint16_t s = AUD_OUT_BUFFER_POS; s < AUD_OUT_BUFFER_POS + AUD_OUT_WINDOW_SIZE; s++) {
    Serial.println(AUD_OUT_BUFFER[aChannel][s]);
  }
}