#ifndef AudioLab_h
#define AudioLab_h

#include <Arduino.h>
#include <driver/dac.h>
#include <driver/adc.h>
#include <math.h>

#define WINDOW_SIZE 256
#define SAMPLE_FREQ 8192

#define NUM_OUT_CH 2 

#define MAX_NUM_WAVES 8

#define AUD_IN_PIN ADC1_CHANNEL_6
#define AUD_OUT_PIN A0

#include "Wave.h"

class AudioLab_
{
  private:
    // constructor for AudioLab
    AudioLab_();

    void _initAudio();
    void _initISR();
    void _initWaveList();

    void _calculateWaves();
    void _calculateWindowingWave();

    float _getWaveVal(Wave* w);
    float _getSumOfChannel(uint8_t ch);

    void _generateAudio();

    void _resetGenerateAudio();
    void _resetAudInOut();

    struct waveNode {
      waveNode() : waveRef(NULL), prev(NULL), next(NULL) {}

      Wave* waveRef;
      waveNode* prev;
      waveNode* next;
    };

    static waveNode* waveListHead[NUM_OUT_CH];
  

    static const int AUD_IN_BUFFER_SIZE = WINDOW_SIZE;
    static const int AUD_OUT_BUFFER_SIZE = WINDOW_SIZE * 2;
    static const int GEN_AUD_BUFFER_SIZE = WINDOW_SIZE * 3;

    static float windowingCosWave[AUD_OUT_BUFFER_SIZE];
    static float sinWave[SAMPLE_FREQ];

    volatile static int AUD_IN_BUFFER[AUD_IN_BUFFER_SIZE];
    volatile static int AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];

    static float generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];

    // function that gets called when timer is triggered
    static void IRAM_ATTR AUD_IN_OUT(void);

    // returns true when input buffer is full
    static bool IRAM_ATTR AUD_IN_BUFFER_FULL(void);

    // restores input buffer index and synchronizes audio output buffer
    void SYNC_AUD_IN_OUT_IDX();

  public:
    // delete assignment operators
    AudioLab_(const AudioLab_ &) = delete;
    AudioLab_ &operator=(const AudioLab_ &) = delete;

    // returns singleton instance of AudioLab
    static AudioLab_ &getInstance(); // public maybe ?

    void init();

    void reset();

    bool ready();

    void flush();

    void pullSamples(int* output);

    void synthesize();

    Wave createWave();

    void _pushWaveNode(Wave* thisWave);
    void _removeWaveNode(Wave* thisWave);    
};

extern AudioLab_ &AudioLab;

#endif