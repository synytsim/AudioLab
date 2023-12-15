#include "AudioLab.h"

const int sampleDelayTime = 1000000 / SAMPLE_FREQ;

hw_timer_t* SAMPLING_TIMER = NULL;

volatile int AudioLab_::AUD_IN_BUFFER[AUD_IN_BUFFER_SIZE];
volatile int AudioLab_::AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];
float AudioLab_::generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];

volatile int AUD_IN_BUFFER_IDX = 0;
volatile int AUD_OUT_BUFFER_POS = 0;

void AudioLab_::_resetAudInOut() {
  AUD_IN_BUFFER_IDX = 0;
  AUD_OUT_BUFFER_POS = 0;
}

void IRAM_ATTR AudioLab_::AUD_IN_OUT() {
  if (AUD_IN_BUFFER_FULL()) return;

  int AUD_OUT_BUFFER_IDX = AUD_OUT_BUFFER_POS + AUD_IN_BUFFER_IDX;
  dacWrite(AUD_OUT_PIN, AUD_OUT_BUFFER[0][AUD_OUT_BUFFER_IDX]);

  AUD_IN_BUFFER[AUD_IN_BUFFER_IDX] = adc1_get_raw(AUD_IN_PIN);

  AUD_IN_BUFFER_IDX += 1;
}

bool IRAM_ATTR AudioLab_::AUD_IN_BUFFER_FULL() {
  return !(AUD_IN_BUFFER_IDX < WINDOW_SIZE);
}

void AudioLab_::SYNC_AUD_IN_OUT_IDX() {
  AUD_OUT_BUFFER_POS += AUD_IN_BUFFER_SIZE;
  if (AUD_OUT_BUFFER_POS >= AUD_OUT_BUFFER_SIZE) AUD_OUT_BUFFER_POS = 0;
  AUD_IN_BUFFER_IDX = 0;
}

// #define DAC_MID 2048
// #define ADC_MID 128

void AudioLab_::_initAudio() {
  _calculateWindowingWave();
  _calculateWaves();
  _initWaveList();
  
  for (int i = 0; i < GEN_AUD_BUFFER_SIZE; i++) {
    for (int c = 0; c < NUM_OUT_CH; c++) {
    generateAudioBuffer[c][i] = 0.0;
      if (i < AUD_OUT_BUFFER_SIZE) {
        AUD_OUT_BUFFER[c][i] = 128;
        if (i < AUD_IN_BUFFER_SIZE) {
          AUD_IN_BUFFER[i] = 2048;
        }
      }
    }
  }
  _resetGenerateAudio();
  _resetAudInOut();
}

void AudioLab_::_initISR() {
    // setup timer interrupt for audio sampling
  SAMPLING_TIMER = timerBegin(0, 80, true);                       // setting clock prescaler 1MHz (80MHz / 80)
  timerAttachInterrupt(SAMPLING_TIMER, &AUD_IN_OUT, true); // attach interrupt function
  timerAlarmWrite(SAMPLING_TIMER, sampleDelayTime, true);         // trigger interrupt every 1000 microseconds
  timerAlarmEnable(SAMPLING_TIMER);                               // enabled interrupt
}

void AudioLab_::pullSamples(int* output) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        output[i] = AUD_IN_BUFFER[i];
    }
}