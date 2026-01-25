#include "Wave.h"

// sine wave table
float StaticSineWave[AUD_OUT_SAMPLE_RATE];
bool StaticSineWaveInitialzed = 0;

// various constansts for wave value calculations
const float INVERSE_SAMPLE_RATE = 1.0 / AUD_OUT_SAMPLE_RATE;
const int NYQUIST = int(AUD_OUT_SAMPLE_RATE) >> 1;
const int WAVE_OFFSET = NYQUIST >> 1;

// minimum float point frequency 
const float MAX_FLOAT_PRECISION = 0.01;

// global time index (increments at SAMPLE_RATE Hz)
unsigned long GlobalTimeIndex = 0;

const unsigned long MaxGlobalTimeIndex = AUD_OUT_SAMPLE_RATE / MAX_FLOAT_PRECISION;
const unsigned long MaxGlobalTimeIndexWindow = MaxGlobalTimeIndex - AUD_OUT_WINDOW_SIZE;

// used for debugging / printing waves
const char* getWaveName(WaveType aWaveType) {
  static const char* const waveNames[] = {
    "ZERO", "SINE", "COSINE", "SQUARE", "SAWTOOTH", "TRIANGLE"
  };
  return waveNames[aWaveType];
}

/*
  PARENT CLASS: ClassWave
*/

ClassWave::ClassWave(void) {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;

  this->_phase = 0;
}

ClassWave::~ClassWave(void) {}

void ClassWave::set(float aFrequency, float anAmplitude, float aPhase) {
  this->frequency = aFrequency;
  this->amplitude = anAmplitude;
  this->phase = aPhase;
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
}

void ClassWave::setPhase(float aPhase) {
  if (!(aPhase >= 0)) {
    Serial.println("PHASE MUST BE POSITIVE!");
    return;
  }
  this->phase = aPhase;
  this->_phase = int(round(this->phase * AUD_OUT_SAMPLE_RATE));
}

float ClassWave::getFrequency(void) const { return this->frequency; }
float ClassWave::getAmplitude(void) const { return this->amplitude; }
float ClassWave::getPhase(void) const { return this->phase; }

WaveType ClassWave::getWaveType(void) const { return this->waveType; }

void ClassWave::calculateSineWave(void) {
  if (StaticSineWaveInitialzed) return;
  StaticSineWaveInitialzed = 1;
  float _resolution = 2.0 * PI / AUD_OUT_SAMPLE_RATE;
  for (int x = 0; x < AUD_OUT_SAMPLE_RATE; x++) {
    StaticSineWave[x] = sin(_resolution * x);
  }
}

void ClassWave::iterateTimeIndex(void) { GlobalTimeIndex += 1; }

void ClassWave::synchronizeTimeIndex(void) { 
  if (GlobalTimeIndex == 0) {
    GlobalTimeIndex = MaxGlobalTimeIndexWindow;
    return;
  }
  GlobalTimeIndex -= AUD_OUT_WINDOW_SIZE; 
}

/*
  CHILD CLASSES: Zero, Sine, Cosine, Square, Triangle, Sawtooth
*/

Zero::Zero() { this->waveType = ZERO; }

float Zero::getWaveValue() const { return 0.0; }

Sine::Sine() { this->waveType = SINE; }

float Sine::getWaveValue() const {;
  float _timeValue = (GlobalTimeIndex * frequency + _phase) * INVERSE_SAMPLE_RATE;
  int _timeIdx = (_timeValue - floor(_timeValue)) * AUD_OUT_SAMPLE_RATE;
  return amplitude * StaticSineWave[_timeIdx];
}

Cosine::Cosine() { this->waveType = COSINE; }

float Cosine::getWaveValue() const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + WAVE_OFFSET) * INVERSE_SAMPLE_RATE;
  int _timeIdx = (_timeValue - floor(_timeValue)) * AUD_OUT_SAMPLE_RATE;
  return amplitude * StaticSineWave[_timeIdx];
}

Square::Square() { this->waveType = SQUARE; }

float Square::getWaveValue() const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  if (_timeIdx < 0.5) return amplitude;
  return -amplitude;
}

Triangle::Triangle() { this->waveType = TRIANGLE; }

float Triangle::getWaveValue() const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + WAVE_OFFSET) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  if (_timeIdx < 0.5) return -amplitude + amplitude * 4 * _timeIdx;
  return amplitude - amplitude * 4 * (_timeIdx - 0.5);
}

Sawtooth::Sawtooth() { this->waveType = SAWTOOTH; }

float Sawtooth::getWaveValue() const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + NYQUIST) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  return amplitude * 2 * _timeIdx - amplitude;
}

Add& ClassWave::operator+(const ClassWave& right) {
    Operand* operandLeft = new Operand(this);
    Operand* operandRight = new Operand((ClassWave *)&right);
    return *(new Add(operandLeft, operandRight));
};

Mul& ClassWave::operator*(const ClassWave& right) {
    Operand* operandLeft = new Operand(this);
    Operand* operandRight = new Operand((ClassWave *)&right);
    return *(new Mul(operandLeft, operandRight));
};

Add& ClassWave::operator+(const Node& right) {
    Operand* operand = new Operand(this);
    return *(new Add(operand, (Node *)&right));
};

Mul& ClassWave::operator*(const Node& right) {
    Operand* operand = new Operand(this);
    return *(new Mul(operand, (Node *)&right));
};

Add& ClassWave::operator+(const Composite& right) {
    Operand* operand = new Operand(this);
    return *(new Add(operand, &right.op1->copy()));
};

Mul& ClassWave::operator*(const Composite& right) {
    Operand* operand = new Operand(this);
    return *(new Mul(operand, &right.op1->copy()));
};