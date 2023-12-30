#include "AudioLab.h"

volatile int ClassAudioLab::AUD_IN_BUFFER[NUM_IN_CH][AUD_IN_BUFFER_SIZE];
volatile int ClassAudioLab::AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];

volatile int AUD_IN_BUFFER_IDX = 0;
volatile int AUD_OUT_BUFFER_POS = 0;

// reset audio input, output buffers and indexes
void ClassAudioLab::resetAudInOut() {
  for (int i = 0; i < AUD_OUT_BUFFER_SIZE; i++) {
    for (int c = 0; c < NUM_OUT_CH; c++) {
      AUD_OUT_BUFFER[c][i] = 128;
    }
    if (i < AUD_IN_BUFFER_SIZE) {
      for (int c = 0; c < NUM_IN_CH; c++) {
        AUD_IN_BUFFER[c][i] = 2048;
      }
    }
  }

  AUD_IN_BUFFER_IDX = 0;
  AUD_OUT_BUFFER_POS = 0;
}

// audio output and input
void IRAM_ATTR ClassAudioLab::AUD_IN_OUT() {
  if (AUD_IN_BUFFER_FULL()) return;

  int AUD_OUT_BUFFER_IDX = AUD_OUT_BUFFER_POS + AUD_IN_BUFFER_IDX;
  #if NUM_OUT_CH > 0
  dacWrite(OUT_PIN_CH1, AUD_OUT_BUFFER[0][AUD_OUT_BUFFER_IDX]);
  #endif
  #if NUM_OUT_CH > 1
  dacWrite(OUT_PIN_CH2, AUD_OUT_BUFFER[1][AUD_OUT_BUFFER_IDX]);
  #endif

  #if NUM_IN_CH > 0
  AUD_IN_BUFFER[0][AUD_IN_BUFFER_IDX] = adc1_get_raw(IN_PIN_CH1);
  #endif
  #if NUM_IN_CH > 1
  AUD_IN_BUFFER[1][AUD_IN_BUFFER_IDX] = adc1_get_raw(IN_PIN_CH2);
  #endif
  
  AUD_IN_BUFFER_IDX += 1;
}

// returns true when audio input buffer is full
bool IRAM_ATTR ClassAudioLab::AUD_IN_BUFFER_FULL() {
  return !(AUD_IN_BUFFER_IDX < WINDOW_SIZE);
}

// synchronizes audio output buffer position and reset audio input buffer index
void ClassAudioLab::SYNC_AUD_IN_OUT_IDX() {
  AUD_OUT_BUFFER_POS += AUD_IN_BUFFER_SIZE;
  if (AUD_OUT_BUFFER_POS >= AUD_OUT_BUFFER_SIZE) AUD_OUT_BUFFER_POS = 0;
  AUD_IN_BUFFER_IDX = 0;
}