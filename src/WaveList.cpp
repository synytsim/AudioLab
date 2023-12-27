#include "AudioLab.h"

ClassAudioLab::WaveNode* ClassAudioLab::globalWaveList = NULL;

// push a wave node onto the waveList
void ClassAudioLab::pushWaveNode(Wave aWave, WaveNode*& aWaveList, bool isDynamic) {

  WaveNode* _newNode = new WaveNode(aWave, NULL, isDynamic);

  // if list is empty, assign node to aWaveList
  if (aWaveList == NULL) {
    aWaveList = _newNode;
    return;
  }

  // otherwise, look through list for next empty location
  WaveNode* _currentNode = aWaveList;

  while(_currentNode->next != NULL) {
    _currentNode = _currentNode->next;
  }

  _currentNode->next = _newNode;
}

// remove a node from wave list
void ClassAudioLab::removeWaveNode(Wave aWave, WaveNode*& aWaveList) {
  WaveNode* _currentNode = aWaveList;

  if (_currentNode->waveRef == aWave) {
    aWaveList = _currentNode->next;
    delete _currentNode;
    return;
  }

  WaveNode* _previousNode = _currentNode;
  _currentNode = _currentNode->next;

  while(_currentNode->waveRef != aWave) {
    _previousNode = _currentNode;
    _currentNode = _currentNode->next;
  }

  _previousNode->next = _currentNode->next;

  delete _currentNode;
}

// returns pointer to wave of specified type
Wave ClassAudioLab::getNewWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase, WaveType aWaveType) {
  Wave _newWave = NULL;
  if (aWaveType == SINE) _newWave = new Sine;
  else if (aWaveType == COSINE) _newWave = new Cosine;
  else if (aWaveType == SQUARE) _newWave = new Square;
  else if (aWaveType == SAWTOOTH) _newWave = new Sawtooth;
  else if (aWaveType == TRIANGLE) _newWave = new Triangle;

  _newWave->setChannel(aChannel);
  _newWave->setFrequency(aFrequency);
  _newWave->setAmplitude(anAmplitude);
  _newWave->setPhase(aPhase);

  return _newWave;
}

// returns a static pointer to a wave of specified type (this wave exists throughout runtime of whole program)
Wave ClassAudioLab::staticWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase, WaveType aWaveType) {
  Wave _newWave = getNewWave(aChannel, aFrequency, anAmplitude, aPhase, aWaveType);
  pushWaveNode(_newWave, globalWaveList, 0);
  return _newWave;
}

// returns a dynamic pointer to a wave of specified type (this wave only exists within the scope of ready())
Wave ClassAudioLab::dynamicWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase, WaveType aWaveType) {
  Wave _newWave = getNewWave(aChannel, aFrequency, anAmplitude, aPhase, aWaveType);
  pushWaveNode(_newWave, globalWaveList, 1);
  return _newWave;
}

void ClassAudioLab::freeWaveListHelper(WaveNode* waveNode) {
  if (waveNode == NULL) return;
  freeWaveListHelper(waveNode->next);
  delete waveNode;
}

void ClassAudioLab::freeWaveList(WaveNode*& aWaveList) {
  freeWaveListHelper(aWaveList);
  aWaveList = NULL;
}

// removes all dynamic waves from wave list
void ClassAudioLab::removeDynamicWaves() {
  WaveNode* _currentNode = globalWaveList;
  WaveNode* _thisNode = NULL;

  while (_currentNode != NULL) {
    _thisNode = _currentNode;
    _currentNode = _currentNode->next;

    if (_thisNode->isDynamic == 0) continue;
    delete _thisNode->waveRef;
    removeWaveNode(_thisNode->waveRef, globalWaveList);
  }
}

// void ClassAudioLab::printWaveList() {
//   waveNode* _currentNode = waveListHead[_channel];
// }