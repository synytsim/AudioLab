#include "AudioLab.h"

AudioLab::waveNode* AudioLab::waveListHead = NULL;
uint8_t AudioLab::numWaveNodes = 0;

AudioLab::waveNode* AudioLab::_getWaveList() { return waveListHead; }

Wave AudioLab::createWave() {
  if (numWaveNodes == MAX_NUM_WAVES) {
    Serial.print("CANNOT ADD WAVE");
  } else {
    return Wave();
  }
}

void AudioLab::_pushWaveNode(Wave* thisWave) {
  if (thisWave == NULL) return;

  numWaveNodes += 1;

  waveNode* newNode = (waveNode*)malloc(sizeof(waveNode));
  newNode->wave_ref = thisWave;
  newNode->prev = NULL;
  newNode->next = NULL;

  if (waveListHead == NULL)
  {
    waveListHead = newNode;
  } 
  else 
  {
    waveNode* currentNode = waveListHead;
    waveNode* prevNode = NULL;
    
    while( currentNode != NULL ) {
      prevNode = currentNode;
      currentNode = currentNode->next;
    }

    currentNode = newNode;

    if (prevNode != NULL) prevNode->next = currentNode;
    currentNode->prev = prevNode;
  }
}

void AudioLab::_removeWaveNode(Wave* thisWave) {
  if (thisWave == NULL) return;

  numWaveNodes -= 1;

  waveNode* currentNode = waveListHead;

  if (numWaveNodes == 0) 
  {
    free(currentNode);
    waveListHead = NULL;
  }
  else 
  {
    while( currentNode->wave_ref != thisWave ) {
      currentNode = currentNode->next;
    }

    waveNode* prevNode = currentNode->prev;
    waveNode* nextNode = currentNode->next;

    if (currentNode == waveListHead) {
      waveListHead = nextNode;
      nextNode->prev = NULL;
    } else {
      if (prevNode != NULL) prevNode->next = nextNode;
      if (nextNode != NULL) nextNode->prev = prevNode;
    }
    free(currentNode);
  }
}