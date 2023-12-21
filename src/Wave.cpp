#include "AudioLab.h"

float Wave::sinWave[SAMPLE_FREQ];
bool wavesInitialzed = 0;

const float _SAMPLE_FREQ = 1.0 / SAMPLE_FREQ;

// Wave constructor
Wave::Wave(void) {
  AudioLab.pushWaveNode(this, 0);
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
  this->channel = 0;
  Serial.println("WAVE ADDED");
}

// Wave::Wave(int channel) {
//   AudioLab::_pushWaveNode(this);

//   _freq = 0;
//   _amp = 0;
//   _phase = 0;
//   _channel = channel;
//   Serial.println("WAVE ADDED");
// }

Wave::Wave(int aChannel, int aFrequency, int anAmplitude, int aPhase) {
  AudioLab.pushWaveNode(this, 0);

  frequency = aFrequency;
  amplitude = anAmplitude;
  phase = aPhase;
  channel = aChannel;
  Serial.println("WAVE ADDED");
}

// Wave destructor
Wave::~Wave(void) {
  AudioLab.removeWaveNode(this);
  Serial.println("WAVE REMOVED");
}

void Wave::setFrequency(int aFrequency) { this->frequency = aFrequency; }
void Wave::setAmplitude(int anAmplitude) { this->amplitude = anAmplitude; }
void Wave::setPhase(int aPhase) { this->phase = aPhase; }
void Wave::setChannel(int aChannel)  { 
  if (AudioLab.waveNodeExists(this)) {
    AudioLab.removeWaveNode(this);
    this->channel = aChannel; 
    AudioLab.pushWaveNode(this);
    return;
  }
  this->channel = aChannel; 
}

void Wave::reset() {
  frequency = 0;
  amplitude = 0;
  phase = 0;
}

int Wave::getFrequency(void) { return this->frequency; }
int Wave::getAmplitude(void) { return this->amplitude; }
int Wave::getPhase(void) { return this->phase; }
int Wave::getChannel(void) { return this->channel; }

float Wave::getWaveVal(int waveIdx) {
  float _waveFrequencyIdx = (waveIdx * frequency + phase) * _SAMPLE_FREQ;
  int _localWaveIdx = (_waveFrequencyIdx - floor(_waveFrequencyIdx)) * SAMPLE_FREQ;
  // return (_amplitude * wave value) at wave position
  return amplitude * sinWave[_localWaveIdx];
}

// calculate values for various waves
void Wave::calculateWaves(void) {
  if (wavesInitialzed) return;
  wavesInitialzed = 1;
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