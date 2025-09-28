/*
  This example demonstrates usage of Fast4ier library with AudioLab.
*/

#include <AudioLab.h>
#include <Fast4ier.h>

uint16_t *AudioLabInputBuffer = AudioLab.getInputBuffer(0);
complex samples[WINDOW_SIZE];
float hammingWindow[WINDOW_SIZE];

void setup() {
  Serial.begin(115200);

  // Computing needs to be done only once, hamming window uses a cosine which takes forever to execute.
  ComputeHammingWindow(hammingWindow, WINDOW_SIZE);

  // initialize AudioLab things (DMA ADC and DAC output)
  AudioLab.init();
}

void loop() {
  // waiting for AudioLab input buffer to fill. Even when AudioLab.ready() returns true, the sampling 
  // continues in the background. The processor constantly switches context allowing tasks to be performed
  // seemingly simultaneously.
  if (!AudioLab.ready()) return;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    samples[i] = AudioLabInputBuffer[i];
  }

  // see runFFT() function for details...
  runFFT(samples, WINDOW_SIZE);

  // do something with magnitude data, in this case find peak frequency
  int maxIdx = 0;
  float max = 0;

  for (int i = 1; i < int(WINDOW_SIZE) >> 1; i++) {
    if (samples[i].re() < max) continue;
    max = samples[i].re();
    maxIdx = i;
  }

  // convert bin to frequency...
  maxIdx = maxIdx * SAMPLE_RATE / WINDOW_SIZE;
  Serial.println(maxIdx);

  // optionally, create a wave on channel 0 with frequency equal to peak frequency
  // AudioLab.dynamicWave(0, maxIdx, 0.5);

  // ensure to call synthesize to generate a window of samples for output (Not required if output is irrelevant)
  AudioLab.synthesize();
}

void runFFT(complex *samples, uint16_t windowSize) {
  DCRemoval(samples, windowSize);                 // simply subtracts the average from all samples
  Windowing(samples, hammingWindow, windowSize);  // multiplies samples by some windowing function
  Fast4::FFT(samples, windowSize);                // this is what does the actual FFT
  ComplexToMagnitude(samples, windowSize);        // converting complex values to magnitudes
}

void DCRemoval(complex *input, uint16_t windowSize) {
    int i;
    complex average = 0;
    for (i = 0; i < windowSize; i++) {
        average += input[i];
    }

    average /= windowSize;

    for (i = 0; i < windowSize; i++) {
        input[i] -= average;
    }
}

void Windowing(complex *input, float *window, uint16_t windowSize) {
    for (int i = 0; i < windowSize; i++) {
        input[i] *= window[i];
    }
}

void ComputeHammingWindow(float *window, uint16_t windowSize) {
    float step = 2 * PI / (windowSize - 1);
    for (int i = 0; i < windowSize; i++) {
        window[i] = 0.54 - 0.46 * cos(step * i);
    }
}

void ComplexToMagnitude(complex *input, uint16_t windowSize) {
    for (int i = 0; i < windowSize; i++) {
        input[i] = sqrt(sq(input[i].re()) + sq(input[i].im()));
    }
}
