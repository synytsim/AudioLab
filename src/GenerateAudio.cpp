#include "AudioLab.h"

const float _SAMPLE_FREQ = 1.0 / SAMPLE_FREQ;

volatile int AudioLab::AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];

// float cos_wave_w[AUD_OUT_BUFFER_SIZE];
// float sin_wave[SAMPLE_FREQ];
//int sin_wave_idx = 0;

// struct wave 
// {
//   wave(int amp, int freq, int phase): freq(freq), amp(amp), phase(phase) {}
//   wave(): amp(0), freq(0), phase(0) {}
//   int freq;
//   int amp;
//   int phase;
// };

// wave waves[NUM_OUT_CH]*;
// uint8_t num_waves[NUM_OUT_CH];

void AudioLab::_calculate_windowing_wave(void) {
  float resolution = float(2.0 * PI / AUD_OUT_BUFFER_SIZE);
  for (int i = 0; i < AUD_OUT_BUFFER_SIZE; i++) {
    cos_wave_w[i] = 0.5 * (1.0 - cos(float(resolution * i)));
  }
}

void AudioLab::_calculate_waves(void) {
  float resolution = float(2.0 * PI / SAMPLE_FREQ);
  // float tri_wave_step = 4.0 / SAMPLE_FREQ;
  // float saw_wave_step = 1.0 / SAMPLE_FREQ;
  for (int x = 0; x < SAMPLE_FREQ; x++) {
    sin_wave[x] = sin(float(resolution * x));
    // cos_wave[x] = cos(float(resolution * x));
    // tri_wave[x] = x <= SAMPLE_FREQ / 2 ? x * tri_wave_step - 1.0 : 3.0 - x * tri_wave_step;
    // sqr_wave[x] = x <= SAMPLE_FREQ / 2 ? 1.0 : 0.0;
    // saw_wave[x] = x * saw_wave_step;
  }
}

void AudioLab::_setupWaves() {
  for (int c = 0; c < NUM_OUT_CH; c++) {
    for (int i = 0; i < MAX_NUM_WAVES; i++) {
      _waves[c][i] = wave();
    }
    _num_waves[c] = 0;
  }  

  waveNode* currentNode = _getWaveList();
  while (currentNode != NULL) {
    Wave* wave = currentNode->wave_ref;
    int ch = wave->getChannel();
    int freq = wave->getFreq();
    int amp = wave->getAmp();
    int phase = wave->getPhase();

    _waves[ch][_num_waves[ch]++] = AudioLab::wave(freq, amp, phase);

    currentNode = currentNode->next;
  }
}

float AudioLab::_get_wave_val(wave w) {
  float sin_wave_freq_idx = (sin_wave_idx * w.freq + w.phase) * _SAMPLE_FREQ;
  int sin_wave_position = (sin_wave_freq_idx - floor(sin_wave_freq_idx)) * SAMPLE_FREQ;
  return w.amp * sin_wave[sin_wave_position];
}

float AudioLab::_get_sum_of_channel(uint8_t ch) {
  float sum = 0.0;
  for (int s = 0; s < _num_waves[ch]; s++) {
    if (_waves[ch][s].amp == 0 || (_waves[ch][s].freq == 0 && _waves[ch][s].phase == 0)) continue;
    sum += _get_wave_val(_waves[ch][s]);
  }
  return sum;
}

void AudioLab::_generateAudio() {
  // calculate values for waves
  for (int i = 0; i < AUD_OUT_BUFFER_SIZE; i++) {
    // sum together the sine waves for left channel and right channel
    for (int c = 0; c < NUM_OUT_CH; c++) {
      // add windowed value to the existing values in scratch pad audio output buffer at this moment in time
      generateAudioBuffer[c][generateAudioBufferIdx] += _get_sum_of_channel(c) * cos_wave_w[i];
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
    sin_wave_idx += 1;
    if (sin_wave_idx == SAMPLE_FREQ) sin_wave_idx = 0;
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
  sin_wave_idx = int(sin_wave_idx - AUD_IN_BUFFER_SIZE + SAMPLE_FREQ) % int(SAMPLE_FREQ);
}