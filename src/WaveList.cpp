#include "AudioLab.h"

ClassAudioLab::waveNode* ClassAudioLab::waveListHead[NUM_OUT_CH];
int ClassAudioLab::numWaveNodes = 0;

// initializes wave list head pointer
void ClassAudioLab::initWaveList() {
  for (uint8_t _channel = 0; _channel < NUM_OUT_CH; _channel++) {
    waveListHead[_channel] = NULL;
  }
  numWaveNodes = 0;
}

// returns true if waveNode with pointer to aWave exists
bool ClassAudioLab::waveNodeExists(Wave aWave) {
  for (uint8_t _channel = 0; _channel < NUM_OUT_CH; _channel++) {
    waveNode* _currentNode = waveListHead[_channel];
    while (_currentNode != NULL) {
      if (_currentNode->waveRef == aWave) return 1;  
      _currentNode = _currentNode->next;
    }
  }
  return 0;
}

// push a wave node onto the waveList, this function is called by constructor of Wave
void ClassAudioLab::pushWaveNode(Wave aWave, bool dynamicWave) {
  if (aWave == NULL) return;
  // increment number of wave nodes
  numWaveNodes += 1;


  // allocate memory for wave node
  waveNode* _newNode = (waveNode*)malloc(sizeof(waveNode));
  _newNode->waveRef = aWave;
  _newNode->prev = NULL;
  _newNode->next = NULL;
  _newNode->isDynamic = dynamicWave;

  // get the channel of aWave
  uint8_t _channel = aWave->getChannel();

  // if wave list is empty assign new node to wave list head
  if (waveListHead[_channel] == NULL) {
    waveListHead[_channel] = _newNode;
  } else {
    // otherwise find the next free pointer in wave list
    waveNode* _prevNode = waveListHead[_channel];
    waveNode* _currentNode = _prevNode->next;
    
    while( _currentNode != NULL ) {
      _prevNode = _currentNode;
      _currentNode = _currentNode->next;
    }

    // assign new node to free pointer
    _currentNode = _newNode;

    // link next and previous nodes
    _prevNode->next = _currentNode;
    _currentNode->prev = _prevNode;
  }
}

// remove a wave node from waveList, this function is called by destructor of Wave
bool ClassAudioLab::removeWaveNode(Wave aWave) {
  if (aWave == NULL) return 0;
  // decrement number of wave nodes
  numWaveNodes -= 1;

  // get the channel of aWave
  uint8_t _channel = aWave->getChannel();

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
  bool _isDynamic = _currentNode->isDynamic;
  free(_currentNode);
  return _isDynamic;
}

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

Wave ClassAudioLab::staticWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase, WaveType aWaveType) {
  Wave _newWave = getNewWave(aChannel, aFrequency, anAmplitude, aPhase, aWaveType);
  pushWaveNode(_newWave, 0);
  return _newWave;
}

Wave ClassAudioLab::dynamicWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase, WaveType aWaveType) {
  Wave _newWave = getNewWave(aChannel, aFrequency, anAmplitude, aPhase, aWaveType);
  pushWaveNode(_newWave, 1);
  return _newWave;
}

void ClassAudioLab::removeDynamicWaves() {
  for (uint8_t _channel = 0; _channel < NUM_OUT_CH; _channel++) {
    waveNode* _currentNode = waveListHead[_channel];
    waveNode* _thisNode = NULL;

    while (_currentNode != NULL) {
      _thisNode = _currentNode;
      _currentNode = _currentNode->next;

      if (_thisNode->isDynamic == 0) continue;
      Wave _thisWave = _thisNode->waveRef;
      removeWaveNode(_thisWave);
      delete _thisWave;
    }
  }
}

// void ClassAudioLab::printWaveList() {
//   waveNode* _currentNode = waveListHead[_channel];
// }