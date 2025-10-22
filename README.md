# AudioLab

This library is used for sampling and synthesis of signals in realtime with the ESP32. It supports stereo input and multi-channel output at a modifiable sample rate (up to 32kHz) and window size. Higher sample rates can be achieved by modifying ADC_SAMPLE_RATE in AudioInputOutput.cpp. Audio input and output sample rates can be varied via AUD_IN_OUT_SAMPLE_RATIO to reduce memory usage and impact of writing to DAC (which is done via a blocking technique, however writing to DAC is quite fast).

On the other hand, to sample ADC, a non-blocking approach is used. Allowing performing complex analysis and synthesize/output signals in realtime via Arduino loop(). See examples folder for some applications including tone generator, FFT example, and more. Also, see AudioLabSettings.h file to fine to settings for application, this file contains settings for modifying sample rates, window size and number of input and output channels.

Additionally, usage of a single or dual AD56X4 SPI DAC is supported for 4-8 channel output.

## Installation

Clone this repository into Arduino libraries folder

If using the AD56X4 SPI DAC, clone the following repository into Arduino libraries folder: https://github.com/synytsim/AD56X4_ESP32

## Using in Arduino

To use this library in Arduino add this line to your sketch and see examples to get started

`include <AudioLab.h>`

The pins AudioLab uses on the Adafruit ESP32 Feather:
* **A0** and **A1** for output (if using on-board DACs)
* **A2** and **A3** for input
* For external DAC (AD56X4) pin **33** and/or **32** are used as slave select, modifying these is fairly straightforward, but may need some fiddling.

The sample rate, window size and number of input and output channels can be modified in the **AudioLabSettings.h** file located in the **src** folder.

## API

### AudioLab

AudioLab is a singleton instance of ClassAudioLab

* **AudioLab.init**(void) - initialize AudioLab, call once in Arduino IDE `void setup()`

* **bool AudioLab.ready<typename>**(typename *buffer) - returns true when input buffer fills. Call this function continuously in Arduino IDE `void loop()` with `if (AudioLab.ready(...)) {...}` or `if (!AudioLab.ready(...)) return;` The buffer parameter is used to copy results from volatile buffer to non-volatile buffer. Buffer must be at least of size [NUM_IN_CH][WINDOW_SIZE].

* **bool AudioLab.ready**(void) - use this definition of `AudioLab.ready()` if aquired signal isn't needed

* **AudioLab.synthesize**(void) - synthesize signal for audio output, this function should be called once in `if (AudioLab.ready())` block (after waves are assigned)

* **Wave AudioLab.staticWave**(uint8_t aChannel, WaveType aWaveType = SINE) - returns a pointer to wave object that will exist throughout runtime of program

* **Wave AudioLab.dynamicWave**(uint8_t aChannel, WaveType aWaveType = SINE) - returns a pointer to wave object that will only exist in scope of `if (AudioLab.ready())` block

* **AudioLab.changeWaveType**(Wave& aWave, WaveType aWaveType) - change the wave type of an existing wave

* **AudioLab.printWaves**(void) - prints information about waves that will be synthesized

* **AudioLab.pauseSampling**(void) - disables alarm of timer used for sampling until `resumeSampling()` is called

* **AudioLab.resumeSampling**(void) - enables alarm of timer used for sampling

### Wave

Wave is a pointer to a derived object of base ClassWave, to create a valid pointer use `Wave aWaveName = AudioLab.staticWave()` or `Wave aWaveName = AudioLab.dynamicWave()`

* **aWaveName->set**(uint8_t aChannel, float aFrequency, float anAmplitude, float aPhase) - set wave's parameters

* **aWaveName->reset**(void) - reset wave's frequency, amplitude, and phase

* **aWaveName->setFrequency**(float aFrequency) - set wave's frequency

* **aWaveName->setAmplitude**(float anAmplitude) - set wave's amplitude

* **aWaveName->setPhase**(float aPhase) - set wave's phase

* **aWaveName->setChannel**(uint8_t aChannel) - set wave's channel

* **float aWaveName->getFrequency**(void) - get wave's frequency

* **float aWaveName->getAmplitude**(void) - get wave's amplitude

* **float aWaveName->getPhase**(void) - get wave's phase

* **uint8_t aWaveName->getChannel**(void) - get wave's channel

## Compatibility

At the time the library is compatible with Adafruit HUZZAH ESP32 Feather