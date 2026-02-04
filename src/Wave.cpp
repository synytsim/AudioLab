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

/*
  PARENT CLASS: ClassWave
*/

ClassWave::ClassWave(void) {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;

  this->_phase = 0;
}

ClassWave::ClassWave(float frequency, float amplitude, float phase) {
  this->setFrequency(frequency);
  this->setAmplitude(amplitude);
  this->setPhase(phase);
};

ClassWave::~ClassWave() {}

void ClassWave::setAll(float frequency, float amplitude, float phase) {
  this->setFrequency(frequency);
  this->setAmplitude(amplitude);
  this->setPhase(phase);
}

void ClassWave::resetAll(void) {
  this->frequency = 0;
  this->amplitude = 0;
  this->phase = 0;
}

void ClassWave::setFrequency(float frequency) { 
  if (!(frequency >= 0)) {
    Serial.println("FREQUENCY MUST BE POSITIVE!");
    return;
  }
  this->frequency = frequency; 
}

void ClassWave::setAmplitude(float amplitude) { 
  this->amplitude = amplitude; 
}

void ClassWave::setPhase(float phase) {
  if (!(phase >= 0)) {
    Serial.println("PHASE MUST BE POSITIVE!");
    return;
  }
  this->phase = phase;
  this->_phase = int(round(this->phase * AUD_OUT_SAMPLE_RATE));
}

float ClassWave::getFrequency(void) const { return this->frequency; }
float ClassWave::getAmplitude(void) const { return this->amplitude; }
float ClassWave::getPhase(void) const { return this->phase; }

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
  CHILD CLASSES: DC, Sine, Cosine, Square, Triangle, Sawtooth
*/

DC::DC(void) { }

DC::DC(float amplitude) { this->setAmplitude(amplitude); }

float DC::getWaveValue(void) const { return this->amplitude; }

Sine::Sine(void) { }

Sine::Sine(float frequency, float amplitude, float phase) : ClassWave(frequency, amplitude, phase) {}

float Sine::getWaveValue(void) const {;
  float _timeValue = (GlobalTimeIndex * frequency + _phase) * INVERSE_SAMPLE_RATE;
  int _timeIdx = (_timeValue - floor(_timeValue)) * AUD_OUT_SAMPLE_RATE;
  return amplitude * StaticSineWave[_timeIdx];
}

Cosine::Cosine(void) { }

Cosine::Cosine(float frequency, float amplitude, float phase) : ClassWave(frequency, amplitude, phase) {}

float Cosine::getWaveValue(void) const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + WAVE_OFFSET) * INVERSE_SAMPLE_RATE;
  int _timeIdx = (_timeValue - floor(_timeValue)) * AUD_OUT_SAMPLE_RATE;
  return amplitude * StaticSineWave[_timeIdx];
}

Square::Square(void) { }

Square::Square(float frequency, float amplitude, float phase, float duty_cycle) : ClassWave(frequency, amplitude, phase) {
  this->duty_cycle = duty_cycle;
}

void Square::setDutyCycle(float duty_cycle) {
  this->duty_cycle = duty_cycle;
}

float Square::getDutyCycle(void) const {
  return this->duty_cycle;
}

float Square::getWaveValue(void) const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  if (_timeIdx < duty_cycle) return amplitude;
  return 0.0;
}

Triangle::Triangle(void) { }

Triangle::Triangle(float frequency, float amplitude, float phase) : ClassWave(frequency, amplitude, phase) {}

float Triangle::getWaveValue(void) const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + WAVE_OFFSET) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  if (_timeIdx < 0.5) return 2 * amplitude * _timeIdx;
  return 2 * amplitude - 2 * amplitude * (_timeIdx);
}

Sawtooth::Sawtooth(void) { }

Sawtooth::Sawtooth(float frequency, float amplitude, float phase) : ClassWave(frequency, amplitude, phase) {}

float Sawtooth::getWaveValue(void) const {
  float _timeValue = (GlobalTimeIndex * frequency + _phase + NYQUIST) * INVERSE_SAMPLE_RATE;
  float _timeIdx = _timeValue - floor(_timeValue);
  return amplitude * _timeIdx;
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