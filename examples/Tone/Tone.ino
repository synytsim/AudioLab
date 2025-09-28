/*

  In this basic example, a 200Hz tone is synthesized on channel 0.

*/

#include <AudioLab.h>

// create a static wave (wave that exists throughout runtime of whole program)
Wave aStaticWave = AudioLab.staticWave(0, SINE);

void setup() {
  Serial.begin(115200);

  delay(2000);

  AudioLab.init();

  // set frequency and amplitude
  aStaticWave->setFrequency(200);
  aStaticWave->setAmplitude(0.5);
}

void loop() {
  // AudioLab.ready() returns true when synthesis should occur (this returns true at (SAMPLE_RATE / WINDOW_SIZE) times per second)
  if (AudioLab.ready()) {

    // can modify channel, frequency, amplitude, phase, and wave type here...
    // aStaticWave->setChannel(1);
    // aStaticWave->setFrequency(100);
    // aStaticWave->setAmplitude(0.25);
    // aStaticWave->setPhase(0.5);
    // AudioLab.changeWaveType(aStaticWave, TRIANGLE);

    aStaticWave->setDuration(1);  // ensure to update duration...

    // call AudioLab.synthesize()
    AudioLab.synthesize();
  }
}
