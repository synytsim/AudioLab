#include "AudioLab.h"

#ifdef ESP32
#include <driver/dac.h>
#include <driver/adc.h>
#endif

#ifdef USING_ADAFRUIT_MCP4728_DAC
#include <Adafruit_MCP4728.h>

Adafruit_MCP4728 dac = Adafruit_MCP4728();

#endif

const uint16_t SAMPLE_RATIO = 1 << AUD_IN_OUT_SAMPLE_RATIO;

volatile uint16_t ClassAudioLab::AUD_IN_BUFFER[NUM_IN_CH][AUD_IN_BUFFER_SIZE];
volatile uint16_t ClassAudioLab::AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];

volatile uint16_t AUD_IN_BUFFER_IDX = 0;
volatile uint16_t AUD_OUT_BUFFER_IDX = 0;
volatile uint16_t AUD_OUT_BUFFER_POS = 0;
volatile uint8_t AUD_IN_SAMPLE_COUNT = 0;

void ClassAudioLab::configurePins(void) {
  #if defined(USING_ADAFRUIT_MCP4728_DAC)

  if (!dac.begin(0x60)) {
    Serial.println("Failed to find MCP4728 chip");
    while (1);
  } else {
    delay(1000);
    if (!dac.fastWrite(DAC_MID, DAC_MID, DAC_MID, DAC_MID)) {
      Serial.println("Error writing to MCP4728");
      while (1);
    }
  }

  #else

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
  if (AUD_IN_BUFFER_FULL()) return;

  if (AUD_IN_SAMPLE_COUNT == 0) {
    AUD_OUT_BUFFER_IDX = AUD_OUT_BUFFER_POS + (AUD_IN_BUFFER_IDX >> AUD_IN_OUT_SAMPLE_RATIO);
  #if defined(USING_ADAFRUIT_MCP4728_DAC)
    dac.fastWrite(AUD_OUT_BUFFER[0][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[1][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[2][AUD_OUT_BUFFER_IDX], AUD_OUT_BUFFER[3][AUD_OUT_BUFFER_IDX]); 
  // #elif defined(USING_ADAFRUIT_ADXL5644_DAC)

  #else 
  
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
  #endif
  }

  #if NUM_IN_CH > 0
  AUD_IN_BUFFER[0][AUD_IN_BUFFER_IDX] = analogRead(IN_PIN_CH1);
  #endif
  #if NUM_IN_CH > 1
  AUD_IN_BUFFER[1][AUD_IN_BUFFER_IDX] = analogRead(IN_PIN_CH2);
  #endif
  
  AUD_IN_BUFFER_IDX += 1;

  AUD_IN_SAMPLE_COUNT += 1;
  if (AUD_IN_SAMPLE_COUNT >= SAMPLE_RATIO) AUD_IN_SAMPLE_COUNT = 0;

}

bool ClassAudioLab::AUD_IN_BUFFER_FULL(void) {
  return !(AUD_IN_BUFFER_IDX < WINDOW_SIZE);
}

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