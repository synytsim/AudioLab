#include "AudioLab.h"

int ClassAudioLab::inputBuffer[NUM_IN_CH][AUD_IN_BUFFER_SIZE];

ClassAudioLab::ClassAudioLab() {
  initAudio();
}

ClassAudioLab &ClassAudioLab::getInstance(void) {
  static ClassAudioLab instance;
  return instance;
}

ClassAudioLab &AudioLab = ClassAudioLab::getInstance();

void ClassAudioLab::init(void) {
  
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

  delay(1000);
  initISR();

  Serial.println("AudioLab setup complete");
  
  resumeSampling();
}

// void ClassAudioLab::reset(void) {
//   return;
// }

bool ClassAudioLab::ready(void) {
  if (!AUD_IN_BUFFER_FULL()) return false;

  // store samples from volatile input buffer to non-volatile buffer
  for (int c = 0; c < NUM_IN_CH; c++) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
      inputBuffer[c][i] = AUD_IN_BUFFER[c][i];
    }
  }

  // reset/synchronize input and output indexes to continue sampling
  SYNC_AUD_IN_OUT_IDX();

  // free generateAudioWaveList
  for (int c = 0; c < NUM_OUT_CH; c++) {
    freeWaveList(generateAudioWaveList[c]);
  }
  
  // remove dynamic waves
  removeDynamicWaves();

  return true;
}

void ClassAudioLab::mapAmplitudes(uint8_t aChannel, float aMin) {
  if (aChannel < 0 || aChannel >= NUM_OUT_CH) return;
  if (aMin == 0.0) return;

  float _amplitudeSum = 0.0;

  //Serial.println("0");

  WaveNode* current = globalWaveList;
  // if (current == NULL) return;
  while (current != NULL) {
    if (current->waveRef->getChannel() == aChannel && current->waveRef->checkMappingEnabled() && current->waveRef->getDuration() > 0) {
      _amplitudeSum += current->waveRef->getAmplitude();
    }
    current = current->next;
  }

  //Serial.println("1");

  if (_amplitudeSum == 0.0) return;
  float _divideBy = 1.0 / (_amplitudeSum > aMin ? _amplitudeSum : aMin);

  current = globalWaveList;
  while (current != NULL) {
    if (current->waveRef->getChannel() == aChannel && current->waveRef->checkMappingEnabled() && current->waveRef->getDuration() > 0) {
      current->waveRef->setAmplitude(current->waveRef->getAmplitude() * current->waveRef->getMappingWeight() * _divideBy);
    }
    current = current->next;
  }

  // Serial.println("2");
}

void ClassAudioLab::synthesize(void) {
  generateAudio();
}

Wave ClassAudioLab::getNewWave(uint8_t aChannel, float aFrequency, float anAmplitude, float aPhase, WaveType aWaveType) {
  Wave _newWave = NULL;
  if (aWaveType == SINE) _newWave = new Sine;
  else if (aWaveType == COSINE) _newWave = new Cosine;
  else if (aWaveType == SQUARE) _newWave = new Square;
  else if (aWaveType == SAWTOOTH) _newWave = new Sawtooth;
  else if (aWaveType == TRIANGLE) _newWave = new Triangle;

  bool _error = 0;
  if (!((aChannel >= 0 && aChannel < NUM_OUT_CH) && aFrequency >= 0 && aPhase >= 0)) {
    _error = 1;
  }

  _newWave->setChannel(aChannel);
  _newWave->setFrequency(aFrequency);
  _newWave->setAmplitude(anAmplitude);
  _newWave->setDuration(1);
  _newWave->setPhase(aPhase);

  if (_error) {
    delete _newWave;
    _newWave = NULL;
  }

  return _newWave;
}

Wave ClassAudioLab::staticWave(WaveType aWaveType) { return staticWave(0, 0, 0, 0, aWaveType); }

Wave ClassAudioLab::staticWave(uint8_t aChannel, WaveType aWaveType) { return staticWave(aChannel, 0, 0, 0, aWaveType); }

Wave ClassAudioLab::staticWave(uint8_t aChannel, float aFrequency, float anAmplitude, float aPhase, WaveType aWaveType) {
  Wave _newWave = getNewWave(aChannel, aFrequency, anAmplitude, aPhase, aWaveType);
  if (_newWave != NULL) pushWaveNode(_newWave, globalWaveList, 0);
  else Serial.println("CREATE STATIC WAVE FAILED DUE TO INVALID PARAMETERS!");
  return _newWave;
}

Wave ClassAudioLab::dynamicWave(WaveType aWaveType) { return dynamicWave(0, 0, 0, 0, aWaveType); }

Wave ClassAudioLab::dynamicWave(uint8_t aChannel, WaveType aWaveType) { return dynamicWave(aChannel, 0, 0, 0, aWaveType); }

Wave ClassAudioLab::dynamicWave(uint8_t aChannel, float aFrequency, float anAmplitude, float aPhase, WaveType aWaveType) {
  Wave _newWave = getNewWave(aChannel, aFrequency, anAmplitude, aPhase, aWaveType);
  if (_newWave != NULL) pushWaveNode(_newWave, globalWaveList, 1);
  else Serial.println("CREATE DYNAMIC WAVE FAILED DUE TO INVALID PARAMETERS!");
  return _newWave;
}

void ClassAudioLab::changeWaveType(Wave& aWave, WaveType aWaveType) {
  WaveNode* _currentNode = globalWaveList;

  // looks through globalWaveList to find reference
  bool _waveExists = 0;
  while (_currentNode != NULL) {
    if (_currentNode->waveRef == aWave) {
      _waveExists = 1;
      break;
    }
    _currentNode = _currentNode->next;
  }

  // if wave was not found, print message and return
  if (_waveExists == 0) {
    Serial.printf("CANNOT CHANGE WAVE TYPE! WAVE AT ADDRESS 0x%02x DOES NOT EXIST IN GLOBAL WAVE LIST\r\n", aWave);
    return;
  }
  // if aWave type is the same as aWaveType return
  if (aWave->getWaveType() == aWaveType) return;

  // replace this wave with a new wave of a different type but same parameters (channel, frequency, amplitude, phase)
  Wave _newWave = getNewWave(aWave->getChannel(), aWave->getFrequency(), aWave->getAmplitude(), aWave->getPhase(), aWaveType);
  delete _currentNode->waveRef;
  _currentNode->waveRef = _newWave;
  aWave = _newWave;
}

int *ClassAudioLab::getInputBuffer(uint8_t aChannel) {
  if (!(aChannel >= 0 && aChannel < NUM_IN_CH)) {
    // DBG_printf("INVALID INPUT CHANNEL %d, USE RANGE BETWEEN [0..NUM_IN_CH)\r\n", aChannel);
    return NULL;
  }
  return inputBuffer[aChannel];
}

void ClassAudioLab::printWaves(void) {
  bool _return = 1;
  WaveNode* _currentNode = globalWaveList;
  while (_currentNode != NULL) {
    Wave _wavePtr = _currentNode->waveRef;
    _currentNode = _currentNode->next;
    if (_wavePtr->getAmplitude() > 0.0  && _wavePtr->getFrequency() > 0.0 && _wavePtr->getDuration() > 0) _return = 0;
  }

  if (_return) return;

  Serial.println("Printing waves (WaveType, Frequency, Amplitude, Phase)");
  for (int c = 0; c < NUM_OUT_CH; c++) {
    _currentNode = globalWaveList;
    Serial.printf("Channel %d: ", c);
    while (_currentNode != NULL) {
      Wave _wavePtr = _currentNode->waveRef;
      _currentNode = _currentNode->next;

      if (_wavePtr->getChannel() != c) continue;
      if (_wavePtr->getAmplitude() == 0.0 || _wavePtr->getFrequency() == 0.0 || _wavePtr->getDuration() == 0) continue;

      // Serial.printf("(%s, %.2f, %.2f, %.2f)  ", getWaveName(_wavePtr->getWaveType()), double(_wavePtr->getFrequency()), double(_wavePtr->getAmplitude()), double(_wavePtr->getPhase()));
      Serial.print("(");
      Serial.print(getWaveName(_wavePtr->getWaveType()));
      Serial.print(", ");
      Serial.print(_wavePtr->getFrequency(), 2);
      Serial.print(", ");
      Serial.print(_wavePtr->getAmplitude(), 2);
      Serial.print(", ");
      Serial.print(_wavePtr->getPhase(), 2);
      Serial.print(")  ");
    }
    Serial.println();
  }
}
