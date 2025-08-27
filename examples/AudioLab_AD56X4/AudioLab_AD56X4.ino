/*
 * This example showcases usage of AudioLab with AD56X4 SPI DAC. This sketch simply reads a channel and 
 * frequency from Serial monitor in "c f" formant (i.e. "0 100" will set channel 0 to 100 Hz)
 */

#include <AudioLab.h>

#define SAMPLING_DURATION_TIMING_TEST   // Uncomment the following line to perform sampling speed test
#define SYNTHESIS_TIMING_TEST           // uncomment for speed test of synthesis + sampling
#define TIMING_AVERAGE_COUNT 16         // Number of windows to average speed tests over

uint8_t timingAverageCount = 0;
uint32_t samplingDurationSum = 0;
uint32_t synthesisDurationSum = 0;

Wave waves[NUM_OUT_CH];

// variables for storing channel, frequency, and amplitude
int channel = 0; 
float frequency = 25;
float amplitude = 0.5;

// for timing experiments
uint32_t microsTimeSampling = 0;
uint32_t prevMicrosTimeSampling = 0;
uint32_t microsTimeSynth = 0;

void setup() {
  Serial.begin(115200);

  delay(2000);

  Serial.println("AudioLab AD56X4 QUAD DAC test");

  AudioLab.init();

  // Setting waves to some initial frequency and amplitude
  for (int i = 0; i < NUM_OUT_CH; i++) {
    waves[i]= AudioLab.staticWave(i, frequency * (i + 1), amplitude);
  }
}

void loop() {
  if (!AudioLab.ready()) return;

  #ifdef SAMPLING_DURATION_TIMING_TEST
  prevMicrosTimeSampling = microsTimeSampling;
  microsTimeSampling = micros();
  samplingDurationSum += microsTimeSampling - prevMicrosTimeSampling;
  #endif

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

  #ifdef SYNTHESIS_TIMING_TEST
  microsTimeSynth = micros();
  #endif

  AudioLab.synthesize();

  #ifdef SYNTHESIS_TIMING_TEST
  synthesisDurationSum += micros() -  microsTimeSynth;
  #endif

  timingAverageCount += 1;
  if (timingAverageCount == TIMING_AVERAGE_COUNT) {
  #ifdef SAMPLING_DURATION_TIMING_TEST
  Serial.print("Sampling duration: ");
  Serial.print(int(round(1.0 * samplingDurationSum / TIMING_AVERAGE_COUNT)));
  Serial.print("\tSample rate: ");
  Serial.print(int(round(1000000.0 * WINDOW_SIZE / (1.0 * samplingDurationSum / TIMING_AVERAGE_COUNT))));
  #ifdef SYNTHESIS_TIMING_TEST
  Serial.print("\t");
  #else
  Serial.println();
  #endif
  #endif

  #ifdef SYNTHESIS_TIMING_TEST
  Serial.print("Synthesis duration: ");
  Serial.println(int(round(1.0 * synthesisDurationSum / TIMING_AVERAGE_COUNT)));
  #endif
  samplingDurationSum = 0;
  synthesisDurationSum = 0;
  timingAverageCount = 0;
  }
}