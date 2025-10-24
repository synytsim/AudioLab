/*
  This example demonstrates usage of Fast4ier library and sampling ADC using a non-blocking approach via
  AudioLab. The approach is almost fully non-blocking since sampling of the ADC is done in the background
  via DMA which allows for processing in-between samples.
  Using DMA is different from using ISR timer (like in previous version of AudioLab) since CPU intervention
  was still required in order to sample ADC. With DMA, the only CPU intervention is copying results from
  DMA buffers to memory.
*/

#include <AudioLab.h>
#include <Fast4ier.h>
#include <signals.h>

complex samples[NUM_IN_CH][WINDOW_SIZE];
float hammingWindow[WINDOW_SIZE];

Wave waves[NUM_OUT_CH];

void setup() {
  Serial.begin(115200);

  // Computing needs to be done only once, hamming window uses a cosine which takes forever to execute.
  ComputeHammingWindow(hammingWindow, WINDOW_SIZE);

  for (int i = 0; i < NUM_OUT_CH; i++) {
    waves[i] = AudioLab.staticWave(i, SINE);
    waves[i]->setAmplitude(0.5);
  }

  // initialize AudioLab things (DMA ADC and DAC output)
  AudioLab.init();
}

void loop() {
  // waiting for AudioLab input buffer to fill. Even when AudioLab.ready() returns true, the sampling 
  // continues in the background. The processor constantly switches context allowing tasks to be performed
  // seemingly simultaneously.
  if (!AudioLab.ready<complex>(*samples)) return;

  // see runFFT() function for details...
  for (int c = 0; c < NUM_IN_CH; c++) {
    runFFT(samples[c], WINDOW_SIZE);
  }

  // do something with magnitude data, in this case find peak frequency
  int maxIdx = 0;
  float max = 0;

  for (int c = 0; c < NUM_IN_CH; c++) {
    int maxIdx = 0;
    float max = 0;
    for (int i = 1; i < int(WINDOW_SIZE) >> 1; i++) {
      if (samples[c][i].re() < max) continue;
      max = samples[c][i].re();
      maxIdx = i;
    }
    // print peak frequency to console
    Serial.print(c);
    Serial.print(" - ");
    Serial.print(maxIdx * SAMPLE_RATE / WINDOW_SIZE);
    Serial.print("Hz\t");

    // set corresponding wave to peak frequency
    waves[c]->setFrequency(maxIdx * SAMPLE_RATE / WINDOW_SIZE);    
    waves[c]->setDuration(1);
  }
  Serial.println();

  // ensure to call synthesize to generate a window of samples for output (Not required if output is irrelevant)
  AudioLab.synthesize();
}

void runFFT(complex *samples, uint16_t windowSize) {
  DCRemoval(samples, windowSize);                 // simply subtracts the average from all samples
  Windowing(samples, hammingWindow, windowSize);  // multiplies samples by some windowing function
  Fast4::FFT(samples, windowSize);                // this is what does the actual FFT
  ComplexToMagnitude(samples, windowSize);        // converting complex values to magnitudes
}
