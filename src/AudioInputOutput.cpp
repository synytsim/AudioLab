#include "AudioLab.h"

#ifdef ESP32
#include <driver/dac.h>
#include <driver/adc.h>
#endif

#ifdef USING_AD5644_DAC
#include "AD5644\AD56X4_vibrosonics.h"
byte dac_channel[] = { AD56X4_CHANNEL_A, AD56X4_CHANNEL_B, AD56X4_CHANNEL_C, AD56X4_CHANNEL_D };

#endif

const uint16_t SAMPLE_RATIO = 1 << AUD_IN_OUT_SAMPLE_RATIO;

volatile uint16_t ClassAudioLab::AUD_IN_BUFFER[NUM_IN_CH][AUD_IN_BUFFER_SIZE];
volatile uint16_t ClassAudioLab::AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];

volatile uint16_t AUD_IN_BUFFER_IDX = 0;
volatile uint16_t AUD_OUT_BUFFER_IDX = 0;
volatile uint16_t AUD_OUT_BUFFER_POS = 0;
volatile uint8_t AUD_IN_SAMPLE_COUNT = 0;

// TEST TEST TEST TEST TEST TEST TEST TEST

#ifdef AUDIOLAB_TEST
const uint16_t NUM_SAMPLES = 10000;
uint16_t samples[NUM_SAMPLES];
uint16_t samples_2[NUM_SAMPLES];

unsigned long time_samples;

void ClassAudioLab::testSetup() {
  analogReadResolution(12);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  randomSeed(analogRead(A2));

  // pinMode(DAC_PIN_SS, OUTPUT);
  SPI.begin();
  AD56X4.reset(DAC_PIN_SS, true);
  AD56X4.useInternalReference(DAC_PIN_SS, true);
  AD56X4.setChannel(DAC_PIN_SS, AD56X4_SETMODE_INPUT_DAC_ALL, DAC_MID, DAC_MID, DAC_MID, DAC_MID);
  for (int i = 0; i < NUM_SAMPLES; i++) {
      samples[i] = random(DAC_MAX);
  }
}

void ClassAudioLab::testLoop() {

  // testing writing to each channel
  time_samples = micros();
  for (int i = 0; i < NUM_SAMPLES; i++) {
    AD56X4.setChannel(DAC_PIN_SS, AD56X4_SETMODE_INPUT_DAC, samples[i], samples[i], samples[i], samples[i]);
  }
  time_samples = micros() - time_samples;
  Serial.printf("All channel time:\n\tTotal: %d\n\tPerSample%.2f\n", time_samples, 1.0 * time_samples / NUM_SAMPLES);

  // writing to single channel
  time_samples = micros();
  for (int i = 0; i < NUM_SAMPLES; i++) {
    AD56X4.setChannel(DAC_PIN_SS, AD56X4_SETMODE_INPUT_DAC, AD56X4_CHANNEL_A, samples[i]);
  }
  time_samples = micros() - time_samples;
  Serial.printf("single channel time:\n\tTotal: %d\n\tPerSample: %.2f\n", time_samples, 1.0 * time_samples / NUM_SAMPLES);

  // testing mono sampling speed with analogRead
  time_samples = micros();
  for (int i = 0; i < NUM_SAMPLES; i++) {
    samples[i] = analogRead(A2);
  }
  time_samples = micros() - time_samples;
  Serial.printf("mono analogRead:\n\tTotal: %d\n\tPerSample: %.2f\n", time_samples, 1.0 * time_samples / NUM_SAMPLES);
  
  // testing stereo sampling speed with analogRead
  time_samples = micros();
  for (int i = 0; i < NUM_SAMPLES; i++) {
    samples[i] = analogRead(A2);
    samples_2[i] = analogRead(A3);
  }
  time_samples = micros() - time_samples;
  Serial.printf("stereo analogRead:\n\tTotal: %d\n\tPerSample: %.2f\n", time_samples, 1.0 * time_samples / NUM_SAMPLES);
}

#endif

/*
 * Ensures pin are configured
 */
void ClassAudioLab::configurePins(void) {
  #ifdef USING_AD5644_DAC

  // setting up AD5644 SPI DAC
  pinMode(DAC_PIN_SS, OUTPUT);
  SPI.begin();
  AD56X4.reset(DAC_PIN_SS, true);
  AD56X4.useInternalReference(DAC_PIN_SS, true);
  AD56X4.setChannel(DAC_PIN_SS, AD56X4_SETMODE_INPUT_DAC_ALL, DAC_MID, DAC_MID, DAC_MID, DAC_MID);

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
    analogSetWidth(ADC_RESOLUTION);
    analogSetAttenuation(ADC_11db);
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

  #if defined(USING_AD5644_DAC)
  }                               // end if statement when AD5644 is in use
  
  // if input:output ratio is 1:1, each channel is written to at SAMPLE_RATE Hz
  #if (AUD_IN_OUT_SAMPLE_RATIO == 0)
  AD56X4.setChannel(DAC_PIN_SS, AD56X4_SETMODE_INPUT_DAC_ALL, AUD_OUT_BUFFER[0][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[1][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[2][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[3][AUD_OUT_BUFFER_IDX]);
  #else
  AD56X4.setChannel(DAC_PIN_SS, AD56X4_SETMODE_INPUT_DAC, dac_channel[AUD_IN_SAMPLE_COUNT], AUD_OUT_BUFFER[AUD_IN_SAMPLE_COUNT][AUD_OUT_BUFFER_IDX]);
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

  // For sampling on-board DACs
  #if NUM_IN_CH > 0
  AUD_IN_BUFFER[0][AUD_IN_BUFFER_IDX] = analogRead(IN_PIN_CH1);
  #endif
  #if NUM_IN_CH > 1
  AUD_IN_BUFFER[1][AUD_IN_BUFFER_IDX] = analogRead(IN_PIN_CH2);
  #endif
  
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