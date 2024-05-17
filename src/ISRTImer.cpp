#include "AudioLab.h"

const int sampleDelayTime = 1000000 / SAMPLE_RATE;

hw_timer_t* SAMPLING_TIMER = NULL;

void ClassAudioLab::initISR(void) {
  // setup timer interrupt for sampling
  SAMPLING_TIMER = timerBegin(0, 80, true);                       // setting clock prescaler 1MHz (80MHz / 80)
  timerAttachInterrupt(SAMPLING_TIMER, &AUD_IN_OUT, true); // attach interrupt function
  timerAlarmWrite(SAMPLING_TIMER, sampleDelayTime, true);         // trigger interrupt every sampleDelayTime microseconds
}

void ClassAudioLab::pauseSampling(void) {
  timerAlarmDisable(SAMPLING_TIMER);  // enable interrupt
}

void ClassAudioLab::resumeSampling(void) {
  timerAlarmEnable(SAMPLING_TIMER);   // disable interrupt
}