#include "AudioLab.h"

float StaticSineWave[SAMPLE_RATE];
bool StaticSineWaveInitialzed = 0;

const float INVERSE_SAMPLE_RATE = 1.0 / SAMPLE_RATE;
const int NYQUIST = int(SAMPLE_RATE) >> 1;
const int WAVE_OFFSET = NYQUIST >> 1;

const char* getWaveName(int id) {
  static const char* const names[] = {
    "SINE", "COSINE", "SQUARE", "SAWTOOTH", "TRIANGLE"
  };
  return (id < (sizeof(names) / sizeof(names[0])) ? names[id] : "");
}

// ClassWave default constructor
ClassWave::ClassWave(void) {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
  this->channel = 0;
  //Serial.println("WAVE ADDED");
}

// ClassWave constructor
ClassWave::ClassWave(int aChannel) {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
  this->channel = aChannel;
  //Serial.println("WAVE ADDED");
}

// ClassWave constructor
ClassWave::ClassWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase) {
  this->frequency = aFrequency;
  this->amplitude = anAmplitude;
  this->phase = aPhase;
  this->channel = aChannel;
  //Serial.println("WAVE ADDED");
}

// ClassWave destructor
ClassWave::~ClassWave(void) {
  //Serial.println("WAVE REMOVED");
}

void ClassWave::set(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase) {
  this->channel = aChannel;
  this->frequency = aFrequency;
  this->amplitude = anAmplitude;
  this->phase = aPhase;
}

void ClassWave::reset() {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
}

void ClassWave::setFrequency(int aFrequency) { this->frequency = aFrequency; }
void ClassWave::setAmplitude(int anAmplitude) { this->amplitude = anAmplitude; }
void ClassWave::setPhase(int aPhase) { this->phase = aPhase; }
void ClassWave::setChannel(uint8_t aChannel)  { this->channel = aChannel; }

int ClassWave::getFrequency(void) { return this->frequency; }
int ClassWave::getAmplitude(void) { return this->amplitude; }
int ClassWave::getPhase(void) { return this->phase; }
uint8_t ClassWave::getChannel(void) { return this->channel; }

WaveType ClassWave::getWaveType(void) { return this->waveType; }

// calculate values for a 1Hz sine wave sampled at SAMPLE_RATE
void ClassWave::calculateSineWave(void) {
  if (StaticSineWaveInitialzed) return;
  StaticSineWaveInitialzed = 1;
  float _resolution = float(2.0 * PI / SAMPLE_RATE);
  for (int x = 0; x < SAMPLE_RATE; x++) {
    StaticSineWave[x] = sin(float(_resolution * x));
  }
}
// float ClassWave::getTimeValue(int aTimeIdx, int anOffset) {
//   float _waveTimeValue= (aTimeIdx * frequency + phase + anOffset) * INVERSE_SAMPLE_RATE;
//   return _waveTimeValue - floor(_waveTimeValue);
// }

Sine::Sine() { this->waveType = SINE; }

float Sine::getWaveValue(int aTimeIdx) {;
  float _timeValue = (aTimeIdx * frequency + phase) * INVERSE_SAMPLE_RATE;
  int _timeIdx = (_timeValue - floor(_timeValue)) * SAMPLE_RATE;
  return amplitude * StaticSineWave[_timeIdx];
}

Cosine::Cosine() { this->waveType = COSINE; }

float Cosine::getWaveValue(int aTimeIdx) {
  float _timeValue = (aTimeIdx * frequency + phase + WAVE_OFFSET) * INVERSE_SAMPLE_RATE;
  int _timeIdx = (_timeValue - floor(_timeValue)) * SAMPLE_RATE;
  return amplitude * StaticSineWave[_timeIdx];
}

Square::Square() { this->waveType = SQUARE; }

float Square::getWaveValue(int aTimeIdx) {
  float _timeValue = (aTimeIdx * frequency + phase) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  if (_timeIdx < 0.5) return amplitude;
  return -amplitude;
}

Triangle::Triangle() { this->waveType = TRIANGLE; }

float Triangle::getWaveValue(int aTimeIdx) {
  float _timeValue = (aTimeIdx * frequency + phase + WAVE_OFFSET) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  if (_timeIdx < 0.5) return -amplitude + (amplitude << 2) * _timeIdx;
  return amplitude - (amplitude << 2) * (_timeIdx - 0.5);
}

Sawtooth::Sawtooth() { this->waveType = SAWTOOTH; }

float Sawtooth::getWaveValue(int aTimeIdx) {
  float _timeValue = (aTimeIdx * frequency + phase + NYQUIST) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  return (amplitude << 1) * _timeIdx - amplitude;
}