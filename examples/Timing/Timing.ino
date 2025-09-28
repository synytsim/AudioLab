/*
 * This sketch can be used to perform timing experiments to ensure accurate sample rate.
 * Resulting sampling duration and sample rate are printed to Serial.
 * 
 * See ADC_SAMPLE_RATE in AudioInputOutput.cpp and modify as needed for accuracy. 
 * Typically ADC_SAMPLE_RATE will only need to be modified if using sample rates other
 * than 8, 16 or 32 kHz.
 * 
 * In addition, timing of synthesis can be done to get an idea of how much time is spent
 * per window on waveform synthesis.
 */

#include <AudioLab.h>

#define SAMPLING_DURATION_TIMING_TEST   // Uncomment the following line to perform sampling speed test
#define SYNTHESIS_TIMING_TEST           // uncomment for speed test of synthesis + sampling
#define TIMING_AVERAGE_COUNT 16         // Number of windows to average speed tests over

uint8_t timingAverageCount = 0;
uint32_t samplingDurationSum = 0;
uint32_t synthesisDurationSum = 0;

// for timing experiments
uint32_t microsTimeSampling = 0;
uint32_t prevMicrosTimeSampling = 0;
uint32_t microsTimeSynth = 0;

void setup() {
  Serial.begin(115200);

  delay(2000);

  AudioLab.init();
}

void loop() {
  if (!AudioLab.ready()) return;

  #ifdef SAMPLING_DURATION_TIMING_TEST
  prevMicrosTimeSampling = microsTimeSampling;
  microsTimeSampling = micros();
  samplingDurationSum += microsTimeSampling - prevMicrosTimeSampling;
  #endif 

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