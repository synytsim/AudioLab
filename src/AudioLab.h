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

class AudioLab
{
  private:

    AudioLab();

    void _initAudio();
    void _initISR();

    void _setupWaves();

    void _calculate_waves();
    void _calculate_windowing_wave();

    struct waveNode {
      waveNode() : wave_ref(NULL), prev(NULL), next(NULL) {}

      Wave* wave_ref;
      waveNode* prev;
      waveNode* next;
    };

    static waveNode* waveListHead[NUM_OUT_CH];
    static uint8_t numWaveNodes;
    
    void _initWaveList();

    static const int AUD_IN_BUFFER_SIZE = WINDOW_SIZE;
    static const int AUD_OUT_BUFFER_SIZE = WINDOW_SIZE * 2;
    static const int GEN_AUD_BUFFER_SIZE = WINDOW_SIZE * 3;

    static float cos_wave_w[AUD_OUT_BUFFER_SIZE];
    static float sin_wave[SAMPLE_FREQ];

    int sin_wave_idx = 0;

    float _get_wave_val(Wave* w);
    float _get_sum_of_channel(uint8_t ch);

    void _generateAudio();

    volatile static int AUD_IN_BUFFER[AUD_IN_BUFFER_SIZE];
    volatile static int AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];

    // audio input and output buffer index
    volatile static int AUD_IN_BUFFER_IDX;
    volatile static int AUD_OUT_BUFFER_POS;

    static float generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];

    // timer object
    static hw_timer_t *SAMPLING_TIMER;

    // function that gets called when timer is triggered
    static void IRAM_ATTR AUD_IN_OUT(void);

    static bool IRAM_ATTR AUD_IN_BUFFER_FULL(void);

    void RESET_AUD_IN_OUT_IDX();

  public:
  
    AudioLab(const AudioLab &) = delete;
    AudioLab &operator=(const AudioLab &) = delete;

    static AudioLab &getInstance(); // public maybe ?

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

extern AudioLab &Audio;

#endif