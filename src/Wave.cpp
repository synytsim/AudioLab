#include "Wave.h"

// sine wave table
float StaticSineWave[SAMPLE_RATE];
bool StaticSineWaveInitialzed = 0;

// various constansts for wave value calculations
const float INVERSE_SAMPLE_RATE = 1.0 / SAMPLE_RATE;
const int NYQUIST = int(SAMPLE_RATE) >> 1;
const int WAVE_OFFSET = NYQUIST >> 1;

// minimum float point frequency 
const float MAX_FLOAT_PRECISION = 0.01;

// global time index (increments at SAMPLE_RATE Hz)
unsigned long GlobalTimeIndex = 0;

const unsigned long MaxGlobalTimeIndex = SAMPLE_RATE / MAX_FLOAT_PRECISION;
const unsigned long MaxGlobalTimeIndexWindow = MaxGlobalTimeIndex - WINDOW_SIZE;

// used for debugging / printing waves
const char* getWaveName(WaveType aWaveType) {
  static const char* const names[] = {
    "SINE", "COSINE", "SQUARE", "SAWTOOTH", "TRIANGLE"
  };
  return names[aWaveType];
}

ClassWave::ClassWave(void) {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
  this->channel = 0;

  this->_amplitude = 0;
  this->_phase = 0;
  //Serial.println("WAVE ADDED");
}

ClassWave::~ClassWave(void) {
  //Serial.println("WAVE REMOVED");
}

void ClassWave::set(uint8_t aChannel, float aFrequency, float anAmplitude, float aPhase) {
  this->channel = aChannel;
  this->frequency = aFrequency;
  this->amplitude = anAmplitude;
  this->phase = aPhase;

  this->_amplitude = int(round(this->amplitude * ((int(DAC_RESOLUTION) << 4) - 1)));
  this->_phase = int(round(this->phase * SAMPLE_RATE));
}

void ClassWave::reset() {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
}

void ClassWave::setFrequency(float aFrequency) { 
  if (!(aFrequency >= 0)) {
    Serial.println("FREQUENCY MUST BE POSITIVE!");
    return;
  }
  this->frequency = aFrequency; 
}

void ClassWave::setAmplitude(float anAmplitude) { 
  this->amplitude = anAmplitude; 
  this->_amplitude = int(round(this->amplitude * ((int(DAC_RESOLUTION) << 4) - 1)));
}

void ClassWave::setPhase(float aPhase) {
  if (!(aPhase >= 0)) {
    Serial.println("PHASE MUST BE POSITIVE!");
    return;
  }
  this->phase = aPhase;
  this->_phase = int(round(this->phase * SAMPLE_RATE));
}

void ClassWave::setChannel(uint8_t aChannel)  {
  if (!(aChannel >= 0 && aChannel < NUM_OUT_CH)) {
    Serial.printf("CANNOT SET CHANNEL %d! USE RANGE BETWEEN [0..NUM_OUT_CH)\r\n", aChannel);
    return;
  }
  this->channel = aChannel;
}

float ClassWave::getFrequency(void) const { return this->frequency; }
float ClassWave::getAmplitude(void) const { return this->amplitude; }
float ClassWave::getPhase(void) const { return this->phase; }
uint8_t ClassWave::getChannel(void) const { return this->channel; }

WaveType ClassWave::getWaveType(void) const { return this->waveType; }

void ClassWave::calculateSineWave(void) {
  if (StaticSineWaveInitialzed) return;
  StaticSineWaveInitialzed = 1;
  float _resolution = 2.0 * PI / SAMPLE_RATE;
  //int _dacMid = int(DAC_RESOLUTION) << 4;
  for (int x = 0; x < SAMPLE_RATE; x++) {
    StaticSineWave[x] = sin(_resolution * x);
  }
}

void ClassWave::iterateTimeIndex(void) { GlobalTimeIndex += 1; }

void ClassWave::synchronizeTimeIndex(void) { 
  if (GlobalTimeIndex == 0) {
    GlobalTimeIndex = MaxGlobalTimeIndexWindow;
    return;
  }
  GlobalTimeIndex -= WINDOW_SIZE; }

Sine::Sine() { this->waveType = SINE; }

float Sine::getWaveValue() const {;
  float _timeValue = (GlobalTimeIndex * frequency + _phase) * INVERSE_SAMPLE_RATE;
  int _timeIdx = (_timeValue - floor(_timeValue)) * SAMPLE_RATE;
  return _amplitude * StaticSineWave[_timeIdx];
}

Cosine::Cosine() { this->waveType = COSINE; }

float Cosine::getWaveValue() const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + WAVE_OFFSET) * INVERSE_SAMPLE_RATE;
  int _timeIdx = (_timeValue - floor(_timeValue)) * SAMPLE_RATE;
  return _amplitude * StaticSineWave[_timeIdx];
}

Square::Square() { this->waveType = SQUARE; }

float Square::getWaveValue() const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  if (_timeIdx < 0.5) return _amplitude;
  return -_amplitude;
}

Triangle::Triangle() { this->waveType = TRIANGLE; }

float Triangle::getWaveValue() const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + WAVE_OFFSET) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  if (_timeIdx < 0.5) return -_amplitude + (_amplitude << 2) * _timeIdx;
  return _amplitude - (_amplitude << 2) * (_timeIdx - 0.5);
}

Sawtooth::Sawtooth() { this->waveType = SAWTOOTH; }

float Sawtooth::getWaveValue() const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + NYQUIST) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  return (_amplitude << 1) * _timeIdx - amplitude;
}