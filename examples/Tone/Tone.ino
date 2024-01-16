/*

  In this basic example, a 200Hz tone is synthesized on channel 0.

*/

#include <AudioLab.h>

// create a static wave (wave that exists throughout runtime of whole program)
Wave aStaticWave = AudioLab.staticWave(0, SINE);

void setup() {
  // init AudioLab
  AudioLab.init();

  // set frequency and amplitude (Note: parameter of setAmplitude may change to float between -1.0 and 1.0 in future revision)
  aStaticWave->setFrequency(200);
  aStaticWave->setAmplitude(100);
}

void loop() {
  // AudioLab.ready() returns true when synthesis should occur (this returns true at (SAMPLE_RATE / WINDOW_SIZE) times per second)
  if (AudioLab.ready()) {
    // can also modify channel, frequency, amplitude, phase, and wave type here...
    //aStaticWave->setFrequency(200);

    // call AudioLab.synthesize()
    AudioLab.synthesize();
  }

}
