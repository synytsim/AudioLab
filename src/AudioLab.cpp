#include "AudioLab.h"
#include <typeinfo>

ClassAudioLab AudioLab;

Zero zero = Zero();

Channel::Channel() {
  this->op = Operand();
  this->op.set(zero);
  this->node = &this->op;
}

Channel& Channel::operator=(const ClassWave& right) {
  if (typeid(*this->node) == typeid(Add) || typeid(*this->node) == typeid(Mul))
    delete this->node;
  this->op.set(right);
  this->node = &this->op;
  return *this;
}

Channel& Channel::operator=(const Node& right) {
  if (typeid(*this->node) == typeid(Add) || typeid(*this->node) == typeid(Mul))
    delete this->node;
  this->node = (Node *)&right;
  return *this;
}

Channel& Channel::operator=(const Composite& right) {
  if (typeid(*this->node) == typeid(Add) || typeid(*this->node) == typeid(Mul))
    delete this->node;
  this->node = (Node *)&right;
  return *this;
}

void Channel::clear() {
  if (typeid(*this->node) == typeid(Add) || typeid(*this->node) == typeid(Mul))
    delete this->node;
  this->op.set(zero);
  this->node = &this->op;
}

float Channel::getValue() {
  return this->node->getValue();
}

void ClassAudioLab::init(void) {
  initAudio();
  
  configurePins();

  float _sampleDelay = 1000000 / SAMPLE_RATE;

  Serial.print("SAMPLE RATE: ");
  Serial.print(SAMPLE_RATE);
  Serial.print(" Hz    AUD OUT SAMPLE RATE: ");
  Serial.print(AUD_OUT_SAMPLE_RATE);
  Serial.print(" Hz    WINDOW SIZE: ");
  Serial.print(WINDOW_SIZE);
  Serial.print("    CONTROL RATE: ");
  Serial.print(float(SAMPLE_RATE) / WINDOW_SIZE, 2);
  Serial.print(" Hz    TIME PER WINDOW: ");
  Serial.print(_sampleDelay * WINDOW_SIZE * 0.001, 2);
  Serial.println(" ms");

  Serial.println("AudioLab setup complete");

  delay(1000);
  
  resumeSampling();
}

bool ClassAudioLab::ready(void) {
  if (!AUD_IN_BUFFER_FULL()) return false;
    // reset/synchronize input and output indexes to continue sampling
    SYNC_AUD_IN_OUT_IDX();
    return true;
}