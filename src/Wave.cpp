#include "AudioLab.h"

// Wave constructor
Wave::Wave() {
  AudioLab.pushWaveNode(this);
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

// Wave::Wave(int channel, int freq, int amp, int phase) {
//   AudioLab::_pushWaveNode(this);

//   _freq = freq;
//   _amp = amp;
//   _phase = phase;
//   _channel = channel;
//   Serial.println("WAVE ADDED");
// }

// Wave destructor
Wave::~Wave() {
  AudioLab.removeWaveNode(this);
  Serial.println("WAVE REMOVED");
}

void Wave::setFrequency(int aFrequency) { this->frequency = aFrequency; }
void Wave::setAmplitude(int anAmplitude) { this->amplitude = anAmplitude; }
void Wave::setPhase(int aPhase) { this->phase = aPhase; }
void Wave::setChannel(int aChannel)  { this->channel = aChannel; }

int Wave::getFrequency() { return this->frequency; }
int Wave::getAmplitude() { return this->amplitude; }
int Wave::getPhase() { return this->phase; }
int Wave::getChannel() { return this->channel; }