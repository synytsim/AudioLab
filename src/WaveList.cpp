#include "AudioLab.h"

AudioLab::waveNode* AudioLab::waveListHead[NUM_OUT_CH];
uint8_t AudioLab::numWaveNodes = 0;

void AudioLab::_initWaveList() {
  for (int i = 0; i < NUM_OUT_CH; i++) {
    waveListHead[i] = NULL;
  }
}

Wave AudioLab::createWave() {
  if (numWaveNodes == MAX_NUM_WAVES) {
    Serial.print("CANNOT ADD WAVE");
  } else {
    return Wave();
  }
}

void AudioLab::_pushWaveNode(Wave* thisWave) {
  if (thisWave == NULL) return;

  int channel = thisWave->getChannel();

  numWaveNodes += 1;

  waveNode* newNode = (waveNode*)malloc(sizeof(waveNode));
  newNode->wave_ref = thisWave;
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

void AudioLab::_removeWaveNode(Wave* thisWave) {
  if (thisWave == NULL) return;

  int channel = thisWave->getChannel();

  numWaveNodes -= 1;

  waveNode* currentNode = waveListHead[channel];

  if (numWaveNodes == 0) 
  {
    free(currentNode);
    waveListHead[channel] = NULL;
  }
  else 
  {
    while( currentNode->wave_ref != thisWave ) {
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