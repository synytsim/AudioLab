#include "AudioLab.h"

float ClassAudioLab::windowingCosWave[AUD_OUT_BUFFER_SIZE];

float ClassAudioLab::generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];

uint16_t generateAudioBufferIdx = 0;
uint16_t generateAudioOutBufferIdx = 0;

void ClassAudioLab::resetGenerateAudio() {
  // restore scratch pad buffer values
  uint16_t c, i;
  for (c = 0; c < NUM_OUT_CH; c++) {
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

void ClassAudioLab::synthesize() {
  uint16_t c, i, _generateAudioBufferIdxCpy;

  // calculate values for audio output buffer
  for (i = 0; i < AUD_OUT_WINDOW_SIZE; i++) {
    // sum together the sine waves for all channels
    for (c = 0; c < NUM_OUT_CH; c++) {
      // add windowed value to the existing values in scratch pad audio output buffer at this moment in time
      generateAudioBuffer[c][generateAudioBufferIdx] += this->channel[c].getValue() * windowingCosWave[i];

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
      // add windowed value to the existing values in scratch pad audio output buffer at this moment in time
      generateAudioBuffer[c][generateAudioBufferIdx] += this->channel[c].getValue() * windowingCosWave[i + AUD_OUT_WINDOW_SIZE];
    }
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