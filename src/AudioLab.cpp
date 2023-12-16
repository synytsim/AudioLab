#include "AudioLab.h"

ClassAudioLab::ClassAudioLab() {
  initAudio();
}

ClassAudioLab &ClassAudioLab::getInstance() {
  static ClassAudioLab instance;
  return instance;
}

ClassAudioLab &AudioLab = ClassAudioLab::getInstance();

// initialize pins and interrupt timer, call once in setup()
void ClassAudioLab::init() {

  delay(3000);
  pinMode(AUD_OUT_PIN, OUTPUT);
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

  initISR();
}

// reset AudioLab, may be useful in certain events
void ClassAudioLab::reset() {
  return;
}

// returns true when new waves can be synthesized
bool ClassAudioLab::ready() {
  return AUD_IN_BUFFER_FULL();
}

// continue audio sampling, this should be called when ready() returns true
void ClassAudioLab::flush() {
  SYNC_AUD_IN_OUT_IDX();
}

// synthesize a window of audio, this should be called after flush() and after any processing is done
void ClassAudioLab::synthesize() {
  generateAudio();
}

// pull samples from audio input buffer into outputBuffer, this should be called after ready() but before flush()
void ClassAudioLab::pullSamples(int* aBuffer) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        aBuffer[i] = AUD_IN_BUFFER[i];
    }
}