#include "AudioLab.h"

const float _SAMPLE_RATE = 1.0 / SAMPLE_RATE;
const int NYQUIST = int(SAMPLE_RATE) >> 1;
const int WAVE_OFFSET = NYQUIST >> 1;

float StaticSineWave[SAMPLE_RATE];
bool StaticSineWaveInitialzed = 0;

// ClassWave constructor
ClassWave::ClassWave(void) {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
  this->channel = 0;
  //Serial.println("WAVE ADDED");
}

ClassWave::ClassWave(int aChannel) {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
  this->channel = aChannel;
  //Serial.println("WAVE ADDED");
}

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
  this->setChannel(aChannel);
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

void ClassWave::setChannel(uint8_t aChannel)  {  
  if (this->channel == aChannel) return;

  if (AudioLab.waveNodeExists(this)) {
    bool _isDynamic = AudioLab.removeWaveNode(this);
    this->channel = aChannel; 
    AudioLab.pushWaveNode(this, _isDynamic);
  } else {
    this->channel = aChannel;
  }
}

int ClassWave::getFrequency(void) { return this->frequency; }
int ClassWave::getAmplitude(void) { return this->amplitude; }
int ClassWave::getPhase(void) { return this->phase; }
uint8_t ClassWave::getChannel(void) { return this->channel; }

// calculate values for a 1Hz sine wave sampled at SAMPLE_RATE
void ClassWave::calculateSineWave(void) {
  if (StaticSineWaveInitialzed) return;
  StaticSineWaveInitialzed = 1;
  float _resolution = float(2.0 * PI / SAMPLE_RATE);
  for (int x = 0; x < SAMPLE_RATE; x++) {
    StaticSineWave[x] = sin(float(_resolution * x));
  }
}

float ClassWave::getTimeValue(int aTimeIdx, int anOffset) {
  float _waveTimeValue= (aTimeIdx * frequency + phase + anOffset) * _SAMPLE_RATE;
  return _waveTimeValue - floor(_waveTimeValue);
}

float Sine::getWaveValue(int aTimeIdx) {;
  return amplitude * StaticSineWave[int(getTimeValue(aTimeIdx) * SAMPLE_RATE)];
}

float Cosine::getWaveValue(int aTimeIdx) {
  return amplitude * StaticSineWave[int(getTimeValue(aTimeIdx, WAVE_OFFSET) * SAMPLE_RATE)];
}

float Square::getWaveValue(int aTimeIdx) {
  if (getTimeValue(aTimeIdx) < 0.5) return amplitude;
  return -amplitude;
}

float Triangle::getWaveValue(int aTimeIdx) {
  float _localTimeValue = getTimeValue(aTimeIdx, WAVE_OFFSET);
  if (_localTimeValue < 0.5) return -amplitude + 4 * amplitude * _localTimeValue;
  return amplitude - 4 * amplitude * (_localTimeValue - 0.5);
}

float Sawtooth::getWaveValue(int aTimeIdx) {
  return 2 * amplitude * getTimeValue(aTimeIdx, NYQUIST) - amplitude;
}