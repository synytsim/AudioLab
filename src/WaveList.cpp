#include "AudioLab.h"

ClassAudioLab::WaveNode* ClassAudioLab::globalWaveList = NULL;

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

void ClassAudioLab::freeWaveListHelper(WaveNode* waveNode) {
  if (waveNode == NULL) return;
  freeWaveListHelper(waveNode->next);
  delete waveNode;
}

void ClassAudioLab::freeWaveList(WaveNode*& aWaveList) {
  freeWaveListHelper(aWaveList);
  aWaveList = NULL;
}

void ClassAudioLab::removeDynamicWaves(void) {
  WaveNode* _currentNode = globalWaveList;
  WaveNode* _thisNode = NULL;

  while (_currentNode != NULL) {
    _thisNode = _currentNode;
    _currentNode = _currentNode->next;

    uint16_t _thisNodeWaveDuration = _thisNode->waveRef->getDuration();
    if (_thisNodeWaveDuration > 0) {
      _thisNode->waveRef->setDuration(_thisNodeWaveDuration - 1);
      continue;
    }

    if (_thisNode->isDynamic == 0) continue;

    delete _thisNode->waveRef;
    removeWaveNode(_thisNode->waveRef, globalWaveList);
  }
}