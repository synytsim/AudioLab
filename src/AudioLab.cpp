#include "AudioLab.h"


AudioLab::AudioLab() {}

AudioLab &AudioLab::getInstance() {
  static AudioLab instance;
  return instance;
}

//AudioLab &Audio = AudioLab.getInstance();

AudioLab &Audio = AudioLab::getInstance();

void AudioLab::init() {

  delay(3000);
  pinMode(AUD_OUT_PIN, OUTPUT);
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

  _initAudio();

  _initISR();
}

void AudioLab::reset() {
  return;
}

bool AudioLab::ready() {
  return AUD_IN_BUFFER_FULL();
}

void AudioLab::flush() {
  RESET_AUD_IN_OUT_IDX();
}

void AudioLab::synthesize() {
  _setupWaves();

  _generateAudio();
}