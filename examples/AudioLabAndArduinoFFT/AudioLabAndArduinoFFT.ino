/*

  This example shows performing FFT on AudioLab input buffer data using arduinoFFT.
  Then using the MajorPeak function to synthesize tone with AudioLab

*/

#include <AudioLab.h>
#include <arduinoFFT.h>

// arduino FFT stuff...
double vReal[WINDOW_SIZE];
double vImag[WINDOW_SIZE];
arduinoFFT FFT = arduinoFFT(vReal, vImag, WINDOW_SIZE, SAMPLE_RATE);

// get pointer to AudioLab input buffer on channel 0
int* AudioLabInputBuffer = AudioLab.getInputBuffer(0);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(1000);

  // init AudioLab
  AudioLab.init();
}

void loop() {
  // AudioLab.ready() returns true when synthesis should occur/input buffer fills (this returns true at (SAMPLE_RATE / WINDOW_SIZE) times per second)
  if (AudioLab.ready()) {

    // copy samples from AudioLab input buffer to vReal array, set vImag to 0
    for (int i = 0; i < WINDOW_SIZE; i++) {
      vReal[i] = AudioLabInputBuffer[i];
      vImag[i] = 0.0;
    }

    // do arduinoFFT stuff...
    FFT.DCRemoval();
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();

    // use arduinoFFT MajorPeak function to get peak frequency in FFT of AudioLab's input buffer...
    double FFTPeakFrequency = FFT.MajorPeak();
    // synthesize a sine wave with the peak frequency, with an amplitude of 100
    AudioLab.dynamicWave(0, FFTPeakFrequency, 100, 0, SINE);

    // call AudioLab.synthesize() after waves are set
    AudioLab.synthesize();
    // optionally call print waves
    AudioLab.printWaves();
  }
}
