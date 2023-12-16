#include "AudioLab.h"

ClassAudioLab::waveNode* ClassAudioLab::waveListHead[NUM_OUT_CH];

// initializes wave list pointers
void ClassAudioLab::initWaveList() {
  for (int i = 0; i < NUM_OUT_CH; i++) {
    waveListHead[i] = NULL;
  }
}

// create a Wave
Wave ClassAudioLab::createWave() {
  return Wave();
}

// push a wave node onto the waveList, this function is called by constructor of Wave
void ClassAudioLab::pushWaveNode(Wave* aWave) {
  if (aWave == NULL) return;

  int _channel = aWave->getChannel();

  waveNode* _newNode = (waveNode*)malloc(sizeof(waveNode));
  _newNode->waveRef = aWave;
  _newNode->prev = NULL;
  _newNode->next = NULL;

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

  int _channel = aWave->getChannel();

  waveNode* _currentNode = waveListHead[_channel];

  if (_currentNode->next == NULL) {
    free(_currentNode);
    waveListHead[_channel] = NULL;
  } else {
    while( _currentNode->waveRef != aWave ) {
      _currentNode = _currentNode->next;
    }

    waveNode* _prevNode = _currentNode->prev;
    waveNode* _nextNode = _currentNode->next;

    if (_currentNode == waveListHead[_channel]) {
      waveListHead[_channel] = _nextNode;
      _nextNode->prev = NULL;
    } else {
      if (_prevNode != NULL) _prevNode->next = _nextNode;
      if (_nextNode != NULL) _nextNode->prev = _prevNode;
    }
    free(_currentNode);
  }
}