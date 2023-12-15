#include "AudioLab.h"

AudioLab_::waveNode* AudioLab_::waveListHead[NUM_OUT_CH];

void AudioLab_::_initWaveList() {
  for (int i = 0; i < NUM_OUT_CH; i++) {
    waveListHead[i] = NULL;
  }
}

Wave AudioLab_::createWave() {
  return Wave();
}

void AudioLab_::_pushWaveNode(Wave* thisWave) {
  if (thisWave == NULL) return;

  int channel = thisWave->getChannel();

  // numWaveNodes += 1;

  waveNode* newNode = (waveNode*)malloc(sizeof(waveNode));
  newNode->waveRef = thisWave;
  newNode->prev = NULL;
  newNode->next = NULL;

  if (waveListHead[channel] == NULL)
  {
    waveListHead[channel] = newNode;
  } 
  else 
  {
    waveNode* currentNode = waveListHead[channel];
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

void AudioLab_::_removeWaveNode(Wave* thisWave) {
  if (thisWave == NULL) return;

  int channel = thisWave->getChannel();

  // numWaveNodes -= 1;

  waveNode* currentNode = waveListHead[channel];

  if (currentNode->next == NULL) 
  {
    free(currentNode);
    waveListHead[channel] = NULL;
  }
  else 
  {
    while( currentNode->waveRef != thisWave ) {
      currentNode = currentNode->next;
    }

    waveNode* prevNode = currentNode->prev;
    waveNode* nextNode = currentNode->next;

    if (currentNode == waveListHead[channel]) {
      waveListHead[channel] = nextNode;
      nextNode->prev = NULL;
    } else {
      if (prevNode != NULL) prevNode->next = nextNode;
      if (nextNode != NULL) nextNode->prev = prevNode;
    }
    free(currentNode);
  }
}