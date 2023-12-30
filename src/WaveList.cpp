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

  // check if head contains aWave
  if (_currentNode->waveRef == aWave) {
    // make head the next node
    aWaveList = _currentNode->next;
    // remove node
    delete _currentNode;
    return;
  }

  WaveNode* _previousNode = _currentNode;
  _currentNode = _currentNode->next;

  // search for aWave in waveList
  while(_currentNode->waveRef != aWave) {
    _previousNode = _currentNode;
    _currentNode = _currentNode->next;
  }

  // re-link list
  _previousNode->next = _currentNode->next;

  // delete node
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

void ClassAudioLab::changeWaveType(Wave& aWave, WaveType aWaveType) {
  WaveNode* _currentNode = globalWaveList;

  bool _waveExists = 0;
  while (_currentNode != NULL) {
    if (_currentNode->waveRef == aWave) {
      _waveExists = 1;
      break;
    }
    _currentNode = _currentNode->next;
  }
  if (_waveExists == 0) return;
  if (aWave->getWaveType() == aWaveType) return;

  Wave _newWave = getNewWave(aWave->getChannel(), aWave->getFrequency(), aWave->getAmplitude(), aWave->getPhase(), aWaveType);
  delete _currentNode->waveRef;
  _currentNode->waveRef = _newWave;
  aWave = _newWave;
}

// helper for freewaveList()
void ClassAudioLab::freeWaveListHelper(WaveNode* waveNode) {
  if (waveNode == NULL) return;
  freeWaveListHelper(waveNode->next);
  delete waveNode;
}

// recusively frees nodes in a waveList
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

// void ClassAudioLab::printWaves(void) {
//   WaveNode* _currentNode = globalWaveList;

//   char _output[NUM_OUT_CH][320];

//   for (int i = 0; i < NUM_OUT_CH; i++) {
//     sprintf(_output[i], "CH %d: ", i);
//   }

//   while (_currentNode != NULL) {
//     Wave _wavePtr = _currentNode->waveRef;
//     _currentNode = _currentNode->next;

//     if (_wavePtr->getAmplitude() == 0 || (_wavePtr->getFrequency() == 0 && _wavePtr->getPhase() == 0)) continue;
//     char _buffer[20];
//     sprintf(_buffer, "(%d, %d, %d)  ", _wavePtr->getFrequency(), _wavePtr->getAmplitude(), _wavePtr->getPhase());
//     strcat(_output[_wavePtr->getChannel()], _buffer);
//   }

//   for (int i = 0; i < NUM_OUT_CH; i++) {
//     Serial.println(_output[i]);
//   }
// }

const char* getWaveName(size_t id) {
    static const char* const names[] = {
        "SINE", "COSINE", "SQUARE", "SAWTOOTH", "TRIANGLE"
    };
    return (id < (sizeof(names) / sizeof(names[0])) ? names[id] : "");
}

void ClassAudioLab::printWaves(void) {
  Serial.println("Printing waves (WaveType, Frequency, Amplitude, Phase)");
  for (int c = 0; c < NUM_OUT_CH; c++) {
    WaveNode* _currentNode = globalWaveList;
    Serial.printf("Channel %d: ", c);
    while (_currentNode != NULL) {
      Wave _wavePtr = _currentNode->waveRef;
      _currentNode = _currentNode->next;

      if (_wavePtr->getChannel() != c) continue;

      Serial.printf("(%s, %d, %d, %d)  ", getWaveName(_wavePtr->getWaveType()), _wavePtr->getFrequency(), _wavePtr->getAmplitude(), _wavePtr->getPhase());
    }
    Serial.println();
  }
}