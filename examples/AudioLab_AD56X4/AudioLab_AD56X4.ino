/*
 * This example showcases usage of AudioLab with AD56X4 SPI DAC. This sketch simply reads a channel and 
 * frequency from Serial monitor in "c f" formant (i.e. "0 100" will set channel 0 to 100 Hz)
 */

#include <AudioLab.h>

Wave waves[NUM_OUT_CH];

float frequency = 100;
int channel = 0; 

uint8_t waveCount = 0;


uint32_t microsTime = 0;
uint32_t microsTime0 = 0;
uint32_t prevMicrosTime = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  delay(2000);

  Serial.println("AudioLab AD56X4 QUAD DAC test");

  AudioLab.init();

  for (int i = 0; i < NUM_OUT_CH; i++) {
    waves[i]= AudioLab.staticWave(i, (i + 1) * 25, 0.5);
  } 

  microsTime = micros();

}

void loop() {
  if (!AudioLab.ready()) return;

  // Uncomment the three following lines to perform speed test (gives total duration of sampling WINDOW_SIZE samples including sample delay)
  // prevMicrosTime = microsTime;
  // microsTime = micros();
  // Serial.println(microsTime - prevMicrosTime);

  // change frequency on channel by typing "{channel} {frequency}" into serial monitor i.e. "0 100" will set wave on channel 0 to 100 Hz
  if (Serial.available()) {
    int prevChannel = channel;

    channel = Serial.parseInt();
    if (!Serial.available())
      frequency = 0;
    else 
      frequency = Serial.parseFloat();

    Serial.read();

    Serial.println(channel);
    Serial.println(frequency);

    if (channel > -1 && channel < NUM_OUT_CH) {
      if (frequency > 0) waves[channel]->setFrequency(frequency);
    } else channel = prevChannel;
  }

  for (int i = 0; i < NUM_OUT_CH; i++) {
    waves[i]->setDuration(1);
  } 

  // microsTime0 = micros();                  // uncomment for speed test of synthesis + sampling
  AudioLab.synthesize();
  // Serial.println(micros() - microsTime0);  // uncomment for speed test of synthesis + sampling

}