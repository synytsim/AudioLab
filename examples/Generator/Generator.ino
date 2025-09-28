/*
 * This example showcases usage of AudioLab as waveform generator. This sketch simply reads channel, 
 * frequency and amplitude from Serial monitor in "c f a" formant (i.e. "0 100 0.5" will set channel 0 to 100 Hz with amplitude 0.5)
 */

#include <AudioLab.h>

Wave waves[NUM_OUT_CH];

// variables for storing channel, frequency, and amplitude
int channel = 0; 
float frequency = 25;
float amplitude = 0.5;

void setup() {
  Serial.begin(115200);

  delay(2000);

  AudioLab.init();

  // Setting waves to some initial frequency and amplitude
  for (int i = 0; i < NUM_OUT_CH; i++) {
    waves[i]= AudioLab.staticWave(i, frequency * (i + 1), amplitude);
  }
}

void loop() {
  if (!AudioLab.ready()) return;

  // change frequency on channel by typing "{channel} {frequency} {amplitude" into serial monitor 
  // (i.e. "0 100 1.0" will set wave on channel 0 to 100 Hz and max amplitude)
  if (Serial.available()) {
    
    channel = Serial.parseInt();
    frequency = Serial.parseFloat();
    amplitude = Serial.parseFloat();
    Serial.read();

    Serial.print("channel: ");
    Serial.print(channel);
    Serial.print("\tfrequency: ");
    Serial.print(frequency);
    Serial.print("\tamplitude: ");
    Serial.println(amplitude);

    waves[channel]->setFrequency(frequency);
    waves[channel]->setAmplitude(amplitude);
  }

  // incrementing duration for each wave (by window basis)
  for (int i = 0; i < NUM_OUT_CH; i++) {
    waves[i]->setDuration(1);
  }

  AudioLab.synthesize();
}