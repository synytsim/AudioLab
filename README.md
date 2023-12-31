# AudioLab

This library is used for sampling and synthesizing signals

## Installation

Clone this repository into your Arduino libraries folder, or download .zip of the repository, extract it, and place in your Arduino libraries folder

## Using in Arduino

To use this library in Arduino add this line to your sketch 

`include <AudioLab.h>`

## API

* **void AudioLab.init**(void) - initialize AudioLab, call once in Arduino IDE `void setup()`

* **bool AudioLab.ready**(void) - returns true when synthesis function should be called. Call this function continuously in Arduino IDE `void loop()` with `if (AudioLab.ready())`

* **void AudioLab.synthesize**(void) - synthesize signal for audio output, this function should be called once in `if (AudioLab.ready())` block (after waves are assigned)

* **Wave AudioLab.staticWave**(uint8_t aChannel, WaveType aWaveType = SINE) - returns a pointer to wave object that will exist throughout runtime of program

* **Wave AudioLab.dynamicWave**(uint8_t aChannel, WaveType aWaveType = SINE) - returns a pointer to wave object that will only exist in scope of `if (AudioLab.ready())` block

* **void AudioLab.changeWaveType**(Wave& aWave, WaveType aWaveType) - change the wave type of an existing wave

* **const int* getInputBuffer**(uint8_t aChannel = 0) - returns pointer to input buffer

* **void AudioLab.printWaves**(void) - prints information about waves that will be synthesized

* **void AudioLab.pauseSampling**(void) - disables alarm of timer used for sampling until `resumeSampling()` is called

* **void AudioLab.resumeSampling**(void) - enables alarm of timer used for sampling
