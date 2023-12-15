#include "AudioLab.h"

AudioLab_::AudioLab_() {
  _initAudio();
}

AudioLab_ &AudioLab_::getInstance() {
  static AudioLab_ instance;
  return instance;
}

//AudioLab_ &Audio = AudioLab_.getInstance();

AudioLab_ &AudioLab = AudioLab_::getInstance();

void AudioLab_::init() {

  delay(3000);
  pinMode(AUD_OUT_PIN, OUTPUT);
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

  //_initAudio();

  _initISR();
}

void AudioLab_::reset() {
  return;
}

bool AudioLab_::ready() {
  return AUD_IN_BUFFER_FULL();
}

void AudioLab_::flush() {
  SYNC_AUD_IN_OUT_IDX();
}

void AudioLab_::synthesize() {
  //_setupWaves();

  _generateAudio();
}