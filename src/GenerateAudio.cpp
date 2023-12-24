#include "AudioLab.h"

float ClassAudioLab::windowingCosWave[AUD_OUT_BUFFER_SIZE];
float ClassAudioLab::generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];

int generateAudioBufferIdx = 0;
int generateAudioOutBufferIdx = 0;
int staticTimeIdx = 0;

// reset generate audio, this is only called during certain events such as init() and reset()
void ClassAudioLab::resetGenerateAudio() {
  // restore scratch pad buffer values
  for (int i = 0; i < GEN_AUD_BUFFER_SIZE; i++) {
    for (int c = 0; c < NUM_OUT_CH; c++) {
      generateAudioBuffer[c][i] = 0.0;
    }
  }
  // restore indexes
  staticTimeIdx = 0;
  generateAudioBufferIdx = 0;
  generateAudioOutBufferIdx = 0;
}

// initialize audio
void ClassAudioLab::initAudio() {
  // calculate cosine wave for windowing
  calculateWindowingWave();
  
  // initialize wave list
  initWaveList();

  // reset buffers
  resetGenerateAudio();
  resetAudInOut();

  // calculate sine wave values
  ClassWave::calculateSineWave();
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
    _sum += _currentNode->waveRef->getWaveValue(staticTimeIdx);
    //_sum += getWaveVal(_currentNode->waveRef);
    _currentNode = _currentNode->next;
  }
  return _sum;
}

// synthesizes one window of values for audio output
void ClassAudioLab::generateAudio() {

  // temporarily remove waves that don't need to be synthesized (to improve performance) and store in temporary buffers
  Wave _tempWavePtr[numWaveNodes];
  bool _tempWaveIsDynamic[numWaveNodes];
  int _tempWavePtrCount = 0;

  // look through each channel to find waves that don't need to be synthesized
  for (int c = 0; c < NUM_OUT_CH; c++) {
    waveNode* _currentNode = waveListHead[c];
    while (_currentNode != NULL) {
      Wave _wavePtr = _currentNode->waveRef;

      _currentNode = _currentNode->next;

      // remove this node and store necassary information in temporary buffers
      if (_wavePtr->getAmplitude() == 0 || (_wavePtr->getFrequency() == 0 && _wavePtr->getPhase() == 0)) {
        _tempWavePtr[_tempWavePtrCount] = _wavePtr;
        _tempWaveIsDynamic[_tempWavePtrCount++] = removeWaveNode(_wavePtr);
      }
    }
  }

  // calculate values for audio output buffer
  for (int i = 0; i < AUD_OUT_BUFFER_SIZE; i++) {
    // sum together the sine waves for all channels
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
    // increment time index
    staticTimeIdx += 1;
    if (staticTimeIdx == SAMPLE_RATE) staticTimeIdx = 0;
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
  staticTimeIdx = int(staticTimeIdx - AUD_IN_BUFFER_SIZE + SAMPLE_RATE) % int(SAMPLE_RATE);

  // push temporarily removed waves back onto wave list
  for (int i = 0; i < _tempWavePtrCount; i++) {
    pushWaveNode(_tempWavePtr[i], _tempWaveIsDynamic[i]);
  }
}