#include "AudioLab.h"

const int sampleDelayTime = 1000000 / SAMPLE_RATE;

#if defined(ESP32)

hw_timer_t *SAMPLING_TIMER = NULL;

void ClassAudioLab::initISR(void) {
  // setup timer interrupt for sampling
  SAMPLING_TIMER = timerBegin(1000000);               // setting clock prescaler 1MHz (80MHz / 80)
  timerAlarm(SAMPLING_TIMER, sampleDelayTime, true, 0);        // trigger interrupt every sampleDelayTime microseconds
  timerAttachInterrupt(SAMPLING_TIMER, &AUD_IN_OUT);  // attach interrupt function
  //timerStart(SAMPLING_TIMER);
}

// void ClassAudioLab::pauseSampling(void) {
//   //timerStart(SAMPLING_TIMER);  // disable interrupt
//   timerDetachInterrupt(SAMPLING_TIMER);
// }

// void ClassAudioLab::resumeSampling(void) {
//   //timerStop(SAMPLING_TIMER);   // enable interrupt
//   //timerRestart(SAMPLING_TIMER);
//   timerAlarm(SAMPLING_TIMER, sampleDelayTime, true, 0);        // trigger interrupt every sampleDelayTime microseconds
//   timerAttachInterrupt(SAMPLING_TIMER, &AUD_IN_OUT);
// }

#elif defined(__SAMD51__)

#include "SAMDTimerInterrupt.h"

SAMDTimer SAMPLING_TIMER(TIMER_TC3);

void ClassAudioLab::initISR(void) {
  // setup timer interrupt for sampling
  SAMPLING_TIMER.attachInterruptInterval(sampleDelayTime, ClassAudioLab::blankFunction);   // enable interrupt
  SAMPLING_TIMER.detachInterrupt();
}

void ClassAudioLab::pauseSampling(void) {
  SAMPLING_TIMER.detachInterrupt();
  SAMPLING_TIMER.restartTimer();
}

void ClassAudioLab::resumeSampling(void) {
  SAMPLING_TIMER.attachInterruptInterval(sampleDelayTime, ClassAudioLab::AUD_IN_OUT);   // enable interrupt
  SAMPLING_TIMER.restartTimer();
}

#endif

void ClassAudioLab::blankFunction(void) {
  return;
}