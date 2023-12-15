#include "AudioLab.h"

Wave::Wave() {
  Audio._pushWaveNode(this);
  _freq = 0;
  _amp = 0;
  _phase = 0;
  _channel = 0;
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

Wave::~Wave() {
  Audio._removeWaveNode(this);
  Serial.println("WAVE REMOVED");
}