#include "AudioLab.h"

const float _SAMPLE_FREQ = 1.0 / SAMPLE_FREQ;

float ClassAudioLab::windowingCosWave[AUD_OUT_BUFFER_SIZE];
float ClassAudioLab::sinWave[SAMPLE_FREQ];
int globalWaveIdx = 0;

float ClassAudioLab::generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];
int generateAudioBufferIdx = 0;
int generateAudioOutBufferIdx = 0;

// reset generate audio, this is only called during certain events such as init() and reset()
void ClassAudioLab::resetGenerateAudio() {
  // restore scratch pad buffer values
  for (int i = 0; i < GEN_AUD_BUFFER_SIZE; i++) {
    for (int c = 0; c < NUM_OUT_CH; c++) {
      generateAudioBuffer[c][i] = 0.0;
    }
  }

  // restore indexes
  int globalWaveIdx = 0;
  int generateAudioBufferIdx = 0;
  int generateAudioOutBufferIdx = 0;
}

// initialize audio
void ClassAudioLab::initAudio() {
  calculateWindowingWave();
  calculateWaves();
  initWaveList();

  resetGenerateAudio();
  resetAudInOut();
}

// calculates values for windowing wave
void ClassAudioLab::calculateWindowingWave(void) {
  float _resolution = float(2.0 * PI / AUD_OUT_BUFFER_SIZE);
  for (int x = 0; x < AUD_OUT_BUFFER_SIZE; x++) {
    windowingCosWave[x] = 0.5 * (1.0 - cos(float(_resolution * x)));
  }
}

// calculate values for various waves
void ClassAudioLab::calculateWaves(void) {
  float _resolution = float(2.0 * PI / SAMPLE_FREQ);
  // float tri_wave_step = 4.0 / SAMPLE_FREQ;
  // float saw_wave_step = 1.0 / SAMPLE_FREQ;
  for (int x = 0; x < SAMPLE_FREQ; x++) {
    sinWave[x] = sin(float(_resolution * x));
    // cos_wave[x] = cos(float(resolution * x));
    // tri_wave[x] = x <= SAMPLE_FREQ / 2 ? x * tri_wave_step - 1.0 : 3.0 - x * tri_wave_step;
    // sqr_wave[x] = x <= SAMPLE_FREQ / 2 ? 1.0 : 0.0;
    // saw_wave[x] = x * saw_wave_step;
  }
}

// this will eventually be moved to Wave class, when functionality for synthesizing other types of waves is implemented
float ClassAudioLab::getWaveVal(Wave* aWave) {
  int _amplitude = aWave->getAmplitude();
  int _frequency = aWave->getFrequency();
  int _phase = aWave->getPhase();

  // no need to calculate values if amplitude is 0 or frequency and phase are 0
  if (_amplitude == 0 || (_frequency == 0 && _phase == 0)) return 0.0;

  // calculate position of wave at _frequency and _phase offset
  // slightly faster way of doing: (globalWaveIdx * _frequency + _phase) % SAMPLE_FREQ
  float _waveFrequencyIdx = (globalWaveIdx * _frequency + _phase) * _SAMPLE_FREQ;
  int _localWaveIdx = (_waveFrequencyIdx - floor(_waveFrequencyIdx)) * SAMPLE_FREQ;
  // return (_amplitude * wave value) at wave position
  return _amplitude * sinWave[_localWaveIdx];
}

// returns the sum of waves of a channel
float ClassAudioLab::getSumOfChannel(uint8_t aChannel) {
  float sum = 0.0;
  // iterate through waves in a channel and get the total sum of the waves
  waveNode* currentNode = waveListHead[aChannel];
  while (currentNode != NULL) {
    sum += getWaveVal(currentNode->waveRef);
    currentNode = currentNode->next;
  }
  return sum;
}

// synthesizes one window of values for audio output
void ClassAudioLab::generateAudio() {
  // calculate values for waves
  for (int i = 0; i < AUD_OUT_BUFFER_SIZE; i++) {
    // sum together the sine waves for left channel and right channel
    for (int c = 0; c < NUM_OUT_CH; c++) {
      // add windowed value to the existing values in scratch pad audio output buffer at this moment in time
      generateAudioBuffer[c][generateAudioBufferIdx] += getSumOfChannel(c) * windowingCosWave[i];
    }

    // copy final, synthesized values to volatile audio output buffer
    if (i < AUD_IN_BUFFER_SIZE) {
      // shifting output by 128.0 for ESP32 DAC, min max ensures the value stays between 0 - 255
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
    globalWaveIdx += 1;
    if (globalWaveIdx == SAMPLE_FREQ) globalWaveIdx = 0;
  } 

  // reset the next window to synthesize new signal
  int _generateAudioBufferIdxCpy = generateAudioBufferIdx;
  for (int i = 0; i < AUD_IN_BUFFER_SIZE; i++) {
    for (int c = 0; c < NUM_OUT_CH; c++) {
      generateAudioBuffer[c][_generateAudioBufferIdxCpy] = 0.0;
    }
    _generateAudioBufferIdxCpy += 1;
    if (_generateAudioBufferIdxCpy == GEN_AUD_BUFFER_SIZE) _generateAudioBufferIdxCpy = 0;
  }
  // determine the next position in the sine wave table, and scratch pad audio output buffer to counter phase cosine wave
  generateAudioBufferIdx = int(generateAudioBufferIdx - AUD_IN_BUFFER_SIZE + GEN_AUD_BUFFER_SIZE) % int(GEN_AUD_BUFFER_SIZE);
  globalWaveIdx = int(globalWaveIdx - AUD_IN_BUFFER_SIZE + SAMPLE_FREQ) % int(SAMPLE_FREQ);
}