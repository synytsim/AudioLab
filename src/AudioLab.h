#ifndef AudioLab_h
#define AudioLab_h

#include <Arduino.h>
#include <driver/dac.h>
#include <driver/adc.h>
#include <math.h>

#define WINDOW_SIZE 256

#define SAMPLE_RATE 8192

#define NUM_OUT_CH 2 

#define AUD_IN_PIN ADC1_CHANNEL_6
#define AUD_OUT_PIN1 A0
#define AUD_OUT_PIN2 A1

#include "Wave.h"

typedef ClassWave* Wave;

class ClassAudioLab
{
  private:
    // constructor for AudioLab
    ClassAudioLab();

    void initAudio();
    void initISR();
    void initWaveList();

    void calculateWindowingWave();
    float getSumOfChannel(uint8_t aChannel);
    void generateAudio();

    void resetGenerateAudio();
    void resetAudInOut();

    inline Wave getNewWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase, WaveType aWaveType);
    static void removeDynamicWaves();

    struct waveNode {
      waveNode() : waveRef(NULL), isDynamic(0), prev(NULL), next(NULL) {}

      Wave waveRef;
      bool isDynamic;
      waveNode* prev;
      waveNode* next;
    };

    static waveNode* waveListHead[NUM_OUT_CH];
    static int numWaveNodes;

    static const int AUD_IN_BUFFER_SIZE = WINDOW_SIZE;
    static const int AUD_OUT_BUFFER_SIZE = WINDOW_SIZE * 2;
    static const int GEN_AUD_BUFFER_SIZE = WINDOW_SIZE * 3;

    static float generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];
    static float windowingCosWave[AUD_OUT_BUFFER_SIZE];

    volatile static int AUD_IN_BUFFER[AUD_IN_BUFFER_SIZE];
    volatile static int AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];

    // function that gets called when timer is triggered
    static void IRAM_ATTR AUD_IN_OUT(void);

    // returns true when input buffer is full
    static bool IRAM_ATTR AUD_IN_BUFFER_FULL(void);

    // restores input buffer index and synchronizes audio output buffer
    void SYNC_AUD_IN_OUT_IDX();

  public:
    // delete assignment operators
    ClassAudioLab(const ClassAudioLab &) = delete;
    ClassAudioLab &operator=(const ClassAudioLab &) = delete;

    // returns instance of AudioLab
    static ClassAudioLab &getInstance();

    void init();

    void reset();

    bool ready();

    void flush();

    void pullSamples(int* aBuffer);

    void synthesize();

    Wave staticWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase = 0,  WaveType aWaveType = SINE);
    Wave dynamicWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase = 0,  WaveType aWaveType = SINE);

    static void pushWaveNode(Wave aWave, bool dynamicWave = 0);
    static bool removeWaveNode(Wave aWave);    
    static bool waveNodeExists(Wave aWave);
};

extern ClassAudioLab &AudioLab;

#endif