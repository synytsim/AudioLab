/*

  In this example, a basic frequency sweep is done from 10Hz to ~1kHz

*/

#include <AudioLab.h>

// create a static wave(wave that exists throughout runtime of whole program)
// set initial parameters to channel = 0, frequency = 10Hz, amplitude = 1.0, phase = 0
Wave aStaticWave = AudioLab.staticWave(0, 10, 1.0, 0, SINE);

void setup() {
  Serial.begin(115200);
  delay(4000);

  // init AudioLab
  AudioLab.init();
}

void loop() {
  // AudioLab.ready() returns true when synthesis should occur (this returns true at (SAMPLE_RATE / WINDOW_SIZE) times per second)
  if (AudioLab.ready()) {

    // reset the frequency to 10Hz if it is more than or equal to 1000Hz, otherwise multiply the current frequency by 1.01
    if (aStaticWave->getFrequency() >= 1000) aStaticWave->setFrequency(10);
    else aStaticWave->setFrequency(aStaticWave->getFrequency() * 1.01);

    // call AudioLab.synthesize() after waves are set
    AudioLab.synthesize();
    //AudioLab.printWaves();
  }

}
