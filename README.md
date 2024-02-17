# AudioLab

This library is used for sampling and synthesis of signals in realtime. Supports stereo input and output at a modifiable sample rate and window size of up to 10kHz

## Installation

Clone this repository into your Arduino libraries folder, or download .zip of the repository, extract it, and place in your Arduino libraries folder

## Using in Arduino

To use this library in Arduino add this line to your sketch 

`include <AudioLab.h>`

The pins AudioLab uses on the Adafruit ESP32 Feather:
* **A0** and **A1** for output
* **A2** and **A3** for input

The sample rate, window size and number of input and output channels can be modified in the **AudioLabSettings.h** file located in the **src** folder

## API

### AudioLab

AudioLab is a singleton instance of ClassAudioLab

* **AudioLab.init**(void) - initialize AudioLab, call once in Arduino IDE `void setup()`

* **bool AudioLab.ready**(void) - returns true when synthesis function should be called. Call this function continuously in Arduino IDE `void loop()` with `if (AudioLab.ready())`

* **AudioLab.synthesize**(void) - synthesize signal for audio output, this function should be called once in `if (AudioLab.ready())` block (after waves are assigned)

* **Wave AudioLab.staticWave**(uint8_t aChannel, WaveType aWaveType = SINE) - returns a pointer to wave object that will exist throughout runtime of program

* **Wave AudioLab.dynamicWave**(uint8_t aChannel, WaveType aWaveType = SINE) - returns a pointer to wave object that will only exist in scope of `if (AudioLab.ready())` block

* **AudioLab.changeWaveType**(Wave& aWave, WaveType aWaveType) - change the wave type of an existing wave

* **int*** **AudioLab.getInputBuffer**(uint8_t aChannel = 0) - returns pointer to input buffer on a channel

* **AudioLab.printWaves**(void) - prints information about waves that will be synthesized

* **AudioLab.pauseSampling**(void) - disables alarm of timer used for sampling until `resumeSampling()` is called

* **AudioLab.resumeSampling**(void) - enables alarm of timer used for sampling

### Wave

Wave is a pointer to a derived object of base ClassWave, to create a valid pointer use `Wave aWaveName = AudioLab.staticWave()` or `Wave aWaveName = AudioLab.dynamicWave()`

**aWaveName->set**(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase) - set wave's parameters

**aWaveName->reset**() - reset wave's frequency, amplitude, and phase

**aWaveName->setFrequency**(int aFrequency) - set wave's frequency

**aWaveName->setAmplitude**(int anAmplitude) - set wave's amplitude

**aWaveName->setPhase**(int aPhase) - set wave's phase

**aWaveName->setChannel**(uint8_t aChannel) - set wave's channel

**int aWaveName->getFrequency**(void) - get wave's frequency

**int aWaveName->getAmplitude**(void) - get wave's amplitude

**int aWaveName->getPhase**(void) - get wave's phase

**int aWaveName->getChannel**(void) - get wave's channel

## Compatibility

At the time the library is only compatible with the Adafruit HUZZAH ESP32 Feather (other boards have not been tested), support for other boards will be added in future update