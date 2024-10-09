#include "AudioLab.h"

float ClassAudioLab::windowingCosWave[AUD_OUT_BUFFER_SIZE];
float ClassAudioLab::generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];

ClassAudioLab::WaveNode* ClassAudioLab::generateAudioWaveList[NUM_OUT_CH]; 

int generateAudioBufferIdx = 0;
int generateAudioOutBufferIdx = 0;
int staticTimeIdx = 0; 

void ClassAudioLab::resetGenerateAudio() {
  //Serial.printf("%d, %d, %d, %d\n", DAC_MAX, DAC_MID, ADC_MAX, ADC_MID);
  // restore scratch pad buffer values
  for (int c = 0; c < NUM_OUT_CH; c++) {
    freeWaveList(generateAudioWaveList[c]);
    for (int i = 0; i < GEN_AUD_BUFFER_SIZE; i++) {
      generateAudioBuffer[c][i] = 0.0;
    }
  }
  // restore indexes
  staticTimeIdx = 0;
  generateAudioBufferIdx = 0;
  generateAudioOutBufferIdx = 0;
}

void ClassAudioLab::initAudio() {
  // calculate cosine wave for windowing
  calculateWindowingWave();

  // reset buffers
  resetGenerateAudio();
  resetAudInOut();

  // calculate sine wave values
  ClassWave::calculateSineWave();
}

void ClassAudioLab::calculateWindowingWave(void) {
  float _resolution = float(2.0 * PI / AUD_OUT_BUFFER_SIZE);
  for (int x = 0; x < AUD_OUT_BUFFER_SIZE; x++) {
    windowingCosWave[x] = 0.5 * (1.0 - cos(float(_resolution * x)));
  }
}

float ClassAudioLab::getSumOfChannel(uint8_t aChannel) {
  float _sum = 0.0;
  // iterate through waves in a channel and get the total sum of the waves
  WaveNode* _currentNode = generateAudioWaveList[aChannel];
  if (_currentNode == NULL) return _sum;
  while (_currentNode != NULL) {
    _sum += _currentNode->waveRef->getWaveValue();
    //_sum += getWaveVal(_currentNode->waveRef);
    _currentNode = _currentNode->next;
  }
  return _sum * (DAC_MID - 1);
}

void ClassAudioLab::generateAudio() {
  // copy wave pointers for waves that need to be synthesized from StaticWaveList to generateAudioWaveList
  WaveNode* _currentNode = globalWaveList;
  while (_currentNode != NULL) {
    Wave _wavePtr = _currentNode->waveRef;
    _currentNode = _currentNode->next;
    if (_wavePtr->getAmplitude() == 0 || (_wavePtr->getFrequency() == 0 && _wavePtr->getPhase() == 0) || _wavePtr->getDuration() == 0) continue;
    pushWaveNode(_wavePtr, generateAudioWaveList[_wavePtr->getChannel()]);
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
        AUD_OUT_BUFFER[c][generateAudioOutBufferIdx] = max(0, min(DAC_MAX, int(round(generateAudioBuffer[c][generateAudioBufferIdx] + DAC_MID))));
      }
      generateAudioOutBufferIdx += 1;
      if (generateAudioOutBufferIdx == AUD_OUT_BUFFER_SIZE) generateAudioOutBufferIdx = 0;
    }

    // increment generate audio index
    generateAudioBufferIdx += 1;
    if (generateAudioBufferIdx == GEN_AUD_BUFFER_SIZE) generateAudioBufferIdx = 0;
    // increment time index
    ClassWave::iterateTimeIndex();
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
  // generateAudioBufferIdx = int(generateAudioBufferIdx - AUD_IN_BUFFER_SIZE + GEN_AUD_BUFFER_SIZE) % int(GEN_AUD_BUFFER_SIZE);
  generateAudioBufferIdx = int(generateAudioBufferIdx + AUD_OUT_BUFFER_SIZE) % int(GEN_AUD_BUFFER_SIZE);
  ClassWave::synchronizeTimeIndex();

  // free generateAudioWaveList
  for (int c = 0; c < NUM_OUT_CH; c++) {
    freeWaveList(generateAudioWaveList[c]);
  }
}