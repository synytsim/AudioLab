#include "AudioLab.h"

ClassAudioLab::ClassAudioLab() {
  initAudio();
}

ClassAudioLab &ClassAudioLab::getInstance() {
  static ClassAudioLab instance;
  return instance;
}

ClassAudioLab &AudioLab = ClassAudioLab::getInstance();

// initialize pins and interrupt timer, call once in setup()
void ClassAudioLab::init() {

  delay(3000);
  pinMode(OUT_PIN_CH1, OUTPUT);
  pinMode(OUT_PIN_CH2, OUTPUT);

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(IN_PIN_CH1, ADC_ATTEN_11db);
  adc1_config_channel_atten(IN_PIN_CH2, ADC_ATTEN_11db);

  float _sampleDelay = 1000000 / SAMPLE_RATE;
  Serial.printf("SAMPLE RATE: %d Hz    WINDOW SIZE: %d    SPEED : %.1f Hz    TIME PER WINDOW: %.1f ms", SAMPLE_RATE, WINDOW_SIZE, float(SAMPLE_RATE) / WINDOW_SIZE,  _sampleDelay * WINDOW_SIZE * 0.001);
  Serial.println();

  delay(1000);
  initISR();

  Serial.println("AudioLab setup complete");
  
  resumeSampling();
}

// reset AudioLab, may be useful in certain events
void ClassAudioLab::reset() {
  return;
}

// returns true when input buffer fills and signal synthesis should occur. If a buffer is passed, then values from mono input buffer are stored to passed buffer.
bool ClassAudioLab::ready(int* aBuffer) {
  if (!AUD_IN_BUFFER_FULL()) return false;

  // if aBuffer was passed, store samples from volatile input buffer to passed buffer
  if (aBuffer != NULL) {
    for (int c = 0; c < NUM_IN_CH; c++) {
      int j = c * WINDOW_SIZE;
      for (int i = 0; i < WINDOW_SIZE; i++) {
        aBuffer[j + i] = AUD_IN_BUFFER[c][i];
      }
    }
  }

  // resume audio sampling
  SYNC_AUD_IN_OUT_IDX();
  
  // remove dynamic waves
  removeDynamicWaves();

  return true;
}

// synthesize a window of audio, this should be called after flush() and after any processing is done
void ClassAudioLab::synthesize() {
  generateAudio();
}