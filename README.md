# AudioLab

This library is used for sampling and synthesizing signals

## Installation

Clone this repository into your Arduino libraries folder, or download .zip of the repository, extract it, and place in your Arduino libraries folder

## Using in Arduino

To use this library in Arduino add this line to your sketch 

`include <AudioLab.h>`

## API

* **AudioLab.init**(void) - initialize AudioLab, call once in Arduino IDE `void setup()`
* **bool AudioLab.ready**(int* aBuffer = NULL) - returns true when synthesis function should be called, optionally samples from mono input buffer can be stored to a buffer in the sketch. Call this function continuously in Arduino IDE `void loop()` with `if (AudioLab.ready())`
* **bool AudioLab.ready**(int* aBufferChannel1, int* aBufferChannel2) - returns true when synthesis function should be called, optionally samples from stereo input buffer can be stored to a buffers in the sketch. Call this function continuously in Arduino IDE `void loop()` with `if (AudioLab.ready())`
* **AudioLab.synthesize**(void) - synthesize signal for audio output, this function should be called once in `if (AudioLab.ready())` block (after waves are assigned)
* **Wave AudioLab.staticWave**(uint8_t aChannel, WaveType aWaveType = SINE) - returns a pointer to wave object that will exist throughout runtime of program
* **Wave AudioLab.dynamicWave**(uint8_t aChannel, WaveType aWaveType = SINE) - returns a pointer to wave object that will only exist in scope of **if AudioLab.ready()** block