/*

  In this example, a basic frequency sweep is done from 10Hz to AUD_OUT_SAMPLE_RATE

*/

#include <AudioLab.h>

// create a static wave(wave that exists throughout runtime of whole program)
// set initial parameters to channel = 0, frequency = 100Hz, amplitude = 1.0, phase = 0
Wave aStaticWave = AudioLab.staticWave(0, 100, 1.0, 0, SINE);

void setup() {
  Serial.begin(115200);

  delay(2000);

  AudioLab.init();
}

void loop() {
  // AudioLab.ready() returns true when synthesis should occur (this returns true at (SAMPLE_RATE / WINDOW_SIZE) times per second)
  if (AudioLab.ready()) {

    // reset the frequency to 10Hz if it is more than or equal to AUD_OUT_SAMPLE_RATE, otherwise multiply the current frequency by 1.01
    if (aStaticWave->getFrequency() >= AUD_OUT_SAMPLE_RATE) aStaticWave->setFrequency(100);
    else aStaticWave->setFrequency(aStaticWave->getFrequency() * 1.001);
    aStaticWave->setDuration(1);

    Serial.println(aStaticWave->getFrequency());

    // call AudioLab.synthesize() after waves are set
    AudioLab.synthesize();
    //AudioLab.printWaves();
  }

}
