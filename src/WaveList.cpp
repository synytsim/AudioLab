#include "AudioLab.h"

ClassAudioLab::waveNode* ClassAudioLab::waveListHead[NUM_OUT_CH];
int ClassAudioLab::numWaveNodes = 0;

// initializes wave list pointers
void ClassAudioLab::initWaveList() {
  for (int i = 0; i < NUM_OUT_CH; i++) {
    waveListHead[i] = NULL;
  }
  numWaveNodes = 0;
}

bool ClassAudioLab::waveNodeExists(Wave* aWave) {
  for (int _channel = 0; _channel < NUM_OUT_CH; _channel++) {
    waveNode* _currentNode = waveListHead[_channel];
    while (_currentNode != NULL) {
      if (_currentNode->waveRef == aWave) return 1;
      _currentNode = _currentNode->next;
    }
  }
  return 0;
}

// push a wave node onto the waveList, this function is called by constructor of Wave
void ClassAudioLab::pushWaveNode(Wave* aWave, bool quickWave) {
  if (aWave == NULL) return;
  numWaveNodes += 1;

  int _channel = aWave->getChannel();

  waveNode* _newNode = (waveNode*)malloc(sizeof(waveNode));
  _newNode->waveRef = aWave;
  _newNode->prev = NULL;
  _newNode->next = NULL;
  _newNode->isDynamic = quickWave;

  if (waveListHead[_channel] == NULL) {
    waveListHead[_channel] = _newNode;
  } else {
    waveNode* _currentNode = waveListHead[_channel];
    waveNode* _prevNode = NULL;
    
    while( _currentNode != NULL ) {
      _prevNode = _currentNode;
      _currentNode = _currentNode->next;
    }

    _currentNode = _newNode;

    if (_prevNode != NULL) _prevNode->next = _currentNode;
    _currentNode->prev = _prevNode;
  }
}

// remove a wave node from waveList, this function is called by destructor of Wave
void ClassAudioLab::removeWaveNode(Wave* aWave) {
  if (aWave == NULL) return;
  numWaveNodes -= 1;

  int _channel = aWave->getChannel();

  waveNode* _currentNode = waveListHead[_channel];
  
  while(_currentNode->waveRef != aWave) {
    _currentNode = _currentNode->next;
  }

  waveNode* _prevNode = _currentNode->prev;
  waveNode* _nextNode = _currentNode->next;

  if (_currentNode == waveListHead[_channel]) {
    if (_currentNode->next == NULL) waveListHead[_channel] = NULL;
    else {
      waveListHead[_channel] = _nextNode;
      waveListHead[_channel]->prev = NULL;
    }
  } else {
    _prevNode->next = _nextNode;
    if (_nextNode != NULL) _nextNode->prev = _prevNode;
  }
  free(_currentNode);
}

void ClassAudioLab::quickWave(int aChannel, int aFrequency, int anAmplitude, int aPhase) {
  Wave* _newWave = (Wave*)malloc(sizeof(Wave));
  _newWave->setChannel(aChannel);
  _newWave->setFrequency(aFrequency);
  _newWave->setAmplitude(anAmplitude);
  _newWave->setPhase(aPhase);

  pushWaveNode(_newWave, 1);
  // Wave(aChannel, aFrequency, anAmplitude, aPhase);
}

void ClassAudioLab::removeOutOfScopeWaves() {
  for (int channel = 0; channel < NUM_OUT_CH; channel++) {
    waveNode* _currentNode = waveListHead[channel];
    waveNode* _thisNode = NULL;

    while (_currentNode != NULL) {
      _thisNode = _currentNode;
      _currentNode = _currentNode->next;

      if (_thisNode->isDynamic == 0) continue;
      Wave* _thisWave = _thisNode->waveRef;
      removeWaveNode(_thisWave);
      free(_thisWave);
    }
  }
}

// void ClassAudioLab::printWaveList() {
//   waveNode* _currentNode = waveListHead[_channel];
// }