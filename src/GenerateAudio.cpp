#include "AudioLab.h"

float ClassAudioLab::windowingCosWave[AUD_OUT_BUFFER_SIZE];
//float ClassAudioLab::sinWave[SAMPLE_FREQ];
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
  
  initWaveList();

  resetGenerateAudio();
  resetAudInOut();

  Wave* wavesInitializer = (Wave*)malloc(sizeof(Wave));
  wavesInitializer->calculateWaves();
  free(wavesInitializer);
}

// calculates values for windowing wave
void ClassAudioLab::calculateWindowingWave(void) {
  float _resolution = float(2.0 * PI / AUD_OUT_BUFFER_SIZE);
  for (int x = 0; x < AUD_OUT_BUFFER_SIZE; x++) {
    windowingCosWave[x] = 0.5 * (1.0 - cos(float(_resolution * x)));
  }
}

// returns the sum of waves of a channel
float ClassAudioLab::getSumOfChannel(uint8_t aChannel) {
  float _sum = 0.0;
  // iterate through waves in a channel and get the total sum of the waves
  waveNode* _currentNode = waveListHead[aChannel];
  if (_currentNode == NULL) return 0.0;
  while (_currentNode != NULL) {
    _sum += _currentNode->waveRef->getWaveVal(globalWaveIdx);
    //_sum += getWaveVal(_currentNode->waveRef);
    _currentNode = _currentNode->next;
  }
  return _sum;
}

// synthesizes one window of values for audio output
void ClassAudioLab::generateAudio() {

  // temporarily remove waves that don't need to be synthesized (to improve performance)
  Wave* _tempWavePtr[numWaveNodes];
  int _tempWavePtrCount = 0;

  for (int c = 0; c < NUM_OUT_CH; c++) {
    waveNode* _currentNode = waveListHead[c];
    while (_currentNode != NULL) {
      Wave* _wavePtr = _currentNode->waveRef;

      float _amplitude = _wavePtr->getAmplitude() * 0.5;
      int _frequency = _wavePtr->getFrequency();
      int _phase = _wavePtr->getPhase();

      _currentNode = _currentNode->next;

      if (_amplitude == 0 || (_frequency == 0 && _phase == 0)) {
        _tempWavePtr[_tempWavePtrCount++] = _wavePtr;
        removeWaveNode(_wavePtr);
      }
    }
  }

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

  // push temporarily removed waves back onto wave list
  for (int i = 0; i < _tempWavePtrCount; i++) {
    pushWaveNode(_tempWavePtr[i]);
  }
}