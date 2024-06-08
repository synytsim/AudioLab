/*

  This example shows the difference between static and dynamic waves. Of course, everything in this example
  can be done with just static waves but dynamic waves allow you to not worry about creating global Wave variables
  and having to reset them when they're not used

  In general, static waves should be used when stored in global variable. Meanwhile, dynamic waves must only be used
  inside the "if (AudioLab.ready())" block.

*/

#include <AudioLab.h>

// create a static wave(wave that exists throughout runtime of whole program)
// set initial parameters to channel = 0, frequency = 10Hz, amplitude = 0.3, phase = 0
Wave aStaticWave = AudioLab.staticWave(0, 10, 0.3, 0, SINE);

// THIS WILL NOT WORK! The object that Wave is pointing to will be deleted once inside "if (AudioLab.ready())" block
// Wave aDynamicWave = AudioLab.dynamicWave(0, 10, 20, 0, SINE);

void setup() {
  // init AudioLab
  AudioLab.init();
}

int someCounter = 0;

void loop() {
  // AudioLab.ready() returns true when synthesis should occur (this returns true at (SAMPLE_RATE / WINDOW_SIZE) times per second)
  if (AudioLab.ready()) {

    // When someCounter is less than 32, create a dynamic wave on channel = 0, frequency = 45, amplitude = 50, phase = 0
    if (someCounter < 32) {
      // dynamic wave will be created, but then deleted once it is out of scope of the "if (AudioLab.ready())" block
      Wave aDynamicWave = AudioLab.dynamicWave(0, 45, 0.5, 0, SINE);

      // THIS WILL NOT WORK CORRECTLY! The program will continue running until chip runs out of memory
      //Wave aStaticWave2 = AudioLab.staticWave(0, 45, 50, 0, SINE);
    }

    // just a counter that resets when it reaches 100
    someCounter += 1;
    if (someCounter >= 100) someCounter = 0;

    // call AudioLab.synthesize() after waves are set
    AudioLab.synthesize();
  }
}
