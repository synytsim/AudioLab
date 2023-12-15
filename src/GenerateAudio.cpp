#include "AudioLab.h"

const float _SAMPLE_FREQ = 1.0 / SAMPLE_FREQ;

float AudioLab_::windowingCosWave[AUD_OUT_BUFFER_SIZE];
float AudioLab_::sinWave[SAMPLE_FREQ];

int waveIdx = 0;
int generateAudioBufferIdx = 0;
int generateAudioOutBufferIdx = 0;

void AudioLab_::_resetGenerateAudio() {
  int waveIdx = 0;
  int generateAudioBufferIdx = 0;
  int generateAudioOutBufferIdx = 0;
}

void AudioLab_::_calculateWindowingWave(void) {
  float resolution = float(2.0 * PI / AUD_OUT_BUFFER_SIZE);
  for (int i = 0; i < AUD_OUT_BUFFER_SIZE; i++) {
    windowingCosWave[i] = 0.5 * (1.0 - cos(float(resolution * i)));
  }
}

void AudioLab_::_calculateWaves(void) {
  float resolution = float(2.0 * PI / SAMPLE_FREQ);
  // float tri_wave_step = 4.0 / SAMPLE_FREQ;
  // float saw_wave_step = 1.0 / SAMPLE_FREQ;
  for (int x = 0; x < SAMPLE_FREQ; x++) {
    sinWave[x] = sin(float(resolution * x));
    // cos_wave[x] = cos(float(resolution * x));
    // tri_wave[x] = x <= SAMPLE_FREQ / 2 ? x * tri_wave_step - 1.0 : 3.0 - x * tri_wave_step;
    // sqr_wave[x] = x <= SAMPLE_FREQ / 2 ? 1.0 : 0.0;
    // saw_wave[x] = x * saw_wave_step;
  }
}

float AudioLab_::_getWaveVal(Wave* w) {
  int amp = w->getAmp();
  int freq = w->getFreq();
  int phase = w->getPhase();

  if (amp == 0 || (freq == 0 && phase == 0)) return 0.0;

  float sin_wave_freq_idx = (waveIdx * freq + phase) * _SAMPLE_FREQ;
  int sin_wave_position = (sin_wave_freq_idx - floor(sin_wave_freq_idx)) * SAMPLE_FREQ;
  return amp * sinWave[sin_wave_position];
}

float AudioLab_::_getSumOfChannel(uint8_t ch) {
  float sum = 0.0;
  waveNode* currentNode = waveListHead[ch];
  while (currentNode != NULL) {
    sum += _getWaveVal(currentNode->waveRef);
    currentNode = currentNode->next;
  }
  return sum;
}

void AudioLab_::_generateAudio() {
  // calculate values for waves
  for (int i = 0; i < AUD_OUT_BUFFER_SIZE; i++) {
    // sum together the sine waves for left channel and right channel
    for (int c = 0; c < NUM_OUT_CH; c++) {
      // add windowed value to the existing values in scratch pad audio output buffer at this moment in time
      generateAudioBuffer[c][generateAudioBufferIdx] += _getSumOfChannel(c) * windowingCosWave[i];
    }

    // copy final, synthesized values to volatile audio output buffer
    if (i < AUD_IN_BUFFER_SIZE) {
    // shifting output by 128.0 for ESP32 DAC, min max ensures the value stays between 0 - 255 to ensure clipping won't occur
      for (int c = 0; c < NUM_OUT_CH; c++) {
        AUD_OUT_BUFFER[c][generateAudioOutBufferIdx] = max(0, min(255, int(round(generateAudioBuffer[c][generateAudioBufferIdx] + 128.0))));
      }
      generateAudioOutBufferIdx += 1;
      if (generateAudioOutBufferIdx == AUD_OUT_BUFFER_SIZE) generateAudioOutBufferIdx = 0;
    }

    // increment generate audio index
    generateAudioBufferIdx += 1;
    if (generateAudioBufferIdx == GEN_AUD_BUFFER_SIZE) generateAudioBufferIdx = 0;
    // increment sine wave index
    waveIdx += 1;
    if (waveIdx == SAMPLE_FREQ) waveIdx = 0;
  } 

  // reset the next window to synthesize new signal
  int generateAudioBufferIdxCpy = generateAudioBufferIdx;
  for (int i = 0; i < AUD_IN_BUFFER_SIZE; i++) {
    for (int c = 0; c < NUM_OUT_CH; c++) {
      generateAudioBuffer[c][generateAudioBufferIdxCpy] = 0.0;
    }
    generateAudioBufferIdxCpy += 1;
    if (generateAudioBufferIdxCpy == GEN_AUD_BUFFER_SIZE) generateAudioBufferIdxCpy = 0;
  }
  // determine the next position in the sine wave table, and scratch pad audio output buffer to counter phase cosine wave
  generateAudioBufferIdx = int(generateAudioBufferIdx - AUD_IN_BUFFER_SIZE + GEN_AUD_BUFFER_SIZE) % int(GEN_AUD_BUFFER_SIZE);
  waveIdx = int(waveIdx - AUD_IN_BUFFER_SIZE + SAMPLE_FREQ) % int(SAMPLE_FREQ);
}