#include "AudioLab.h"

float ClassAudioLab::windowingCosWave[AUD_OUT_BUFFER_SIZE];

float ClassAudioLab::generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];

ClassAudioLab::WaveNode* ClassAudioLab::generateAudioWaveList[NUM_OUT_CH]; 

uint16_t generateAudioBufferIdx = 0;
uint16_t generateAudioOutBufferIdx = 0;

void ClassAudioLab::resetGenerateAudio() {
  // restore scratch pad buffer values
  uint16_t c, i;
  for (c = 0; c < NUM_OUT_CH; c++) {
    freeWaveList(generateAudioWaveList[c]);
    for (i = 0; i < GEN_AUD_BUFFER_SIZE; i++) {
      generateAudioBuffer[c][i] = 0.0;
    }
  }
  // restore indexes
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

/*
  Compute cosine wave with amplitude 0 to 1.0 with one cycle equivalent to WINDOW_SIZE * 2 samples.
  This waveform is used for windowing the waveform in generateAudioBuffer over 2 windows
*/
void ClassAudioLab::calculateWindowingWave(void) {
  float _resolution = float(2.0 * PI / AUD_OUT_BUFFER_SIZE);
  for (int x = 0; x < AUD_OUT_BUFFER_SIZE; x++) {
    windowingCosWave[x] = 0.5 * (1.0 - cos(float(_resolution * x)));
  }
}

/*
  Sums up amplitudes of the waveforms in a channel at current moment in time
*/
float ClassAudioLab::getSumOfChannel(uint8_t aChannel) {
  float _sum = 0.0;
  // iterate through waves in a channel and get the total sum of the waves
  WaveNode* _currentNode = generateAudioWaveList[aChannel];
  if (_currentNode == NULL) return _sum;
  while (_currentNode != NULL) {
    _sum += _currentNode->waveRef->getWaveValue();
    _currentNode = _currentNode->next;
  }
  return _sum;
}

/*

*/
void ClassAudioLab::synthesize() {
  uint16_t c, i, _generateAudioBufferIdxCpy;

  // copy wave pointers for waves which should be synthesized from StaticWaveList to generateAudioWaveList
  WaveNode* _currentNode = globalWaveList;
  while (_currentNode != NULL) {
    Wave _wavePtr = _currentNode->waveRef;
    _currentNode = _currentNode->next;
    if (_wavePtr->getDuration() == 0 || _wavePtr->getAmplitude() == 0 || (_wavePtr->getFrequency() == 0 && _wavePtr->getPhase() == 0)) continue;
    pushWaveNode(_wavePtr, generateAudioWaveList[_wavePtr->getChannel()]);
  }

  // calculate values for audio output buffer
  for (i = 0; i < AUD_OUT_WINDOW_SIZE; i++) {
    // sum together the sine waves for all channels
    for (c = 0; c < NUM_OUT_CH; c++) {
      // add windowed value to the existing values in scratch pad audio output buffer at this moment in time
      generateAudioBuffer[c][generateAudioBufferIdx] += getSumOfChannel(c) * windowingCosWave[i];

      // copy final, synthesized values to volatile audio output buffer
      AUD_OUT_BUFFER[c][generateAudioOutBufferIdx] = uint16_t(round(generateAudioBuffer[c][generateAudioBufferIdx] * (DAC_MID - 1) + DAC_MID));
    }
    generateAudioOutBufferIdx += 1;
    if (generateAudioOutBufferIdx == AUD_OUT_BUFFER_SIZE) generateAudioOutBufferIdx = 0;  

    // increment generate audio index
    generateAudioBufferIdx += 1;
    if (generateAudioBufferIdx == GEN_AUD_BUFFER_SIZE) generateAudioBufferIdx = 0;
    // increment time index
    ClassWave::iterateTimeIndex();
  }
  
  for (i = 0; i < AUD_OUT_WINDOW_SIZE; i++) {
    // sum together the sine waves for all channels
    for (c = 0; c < NUM_OUT_CH; c++) {
      generateAudioBuffer[c][generateAudioBufferIdx] += getSumOfChannel(c) * windowingCosWave[i + AUD_OUT_WINDOW_SIZE];
    }
    // increment generate audio index
    generateAudioBufferIdx += 1;
    if (generateAudioBufferIdx == GEN_AUD_BUFFER_SIZE) generateAudioBufferIdx = 0;
    // increment time index
    ClassWave::iterateTimeIndex();
  }

  // reset the next window to synthesize new signal
  _generateAudioBufferIdxCpy = generateAudioBufferIdx;
  for (i = 0; i < AUD_OUT_WINDOW_SIZE; i++) {
    for (c = 0; c < NUM_OUT_CH; c++) {
      generateAudioBuffer[c][_generateAudioBufferIdxCpy] = 0.0;
    }
    _generateAudioBufferIdxCpy += 1;
    if (_generateAudioBufferIdxCpy == GEN_AUD_BUFFER_SIZE) _generateAudioBufferIdxCpy = 0;
  }
  // determine the next position in the sine wave table, and scratch pad audio output buffer to counter phase cosine wave
  generateAudioBufferIdx = int(generateAudioBufferIdx + AUD_OUT_BUFFER_SIZE) % int(GEN_AUD_BUFFER_SIZE);
  ClassWave::synchronizeTimeIndex();
}