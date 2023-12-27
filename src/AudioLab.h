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

    // initialize audio input and output
    void initAudio();

    // initialize timer interrupt
    void initISR();

    // initializes wave list head pointer
    void initWaveList();

    // calculates values for windowing wave
    void calculateWindowingWave();

    // returns the sum of a channel
    float getSumOfChannel(uint8_t aChannel);

    // generate one window of signal
    void generateAudio();

    // reset generate audio, this is only called during certain events such as init() and reset()
    void resetGenerateAudio();

    // reset audio input, output buffers and indexes
    void resetAudInOut();

    // returns pointer to a wave of wave type
    Wave getNewWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase, WaveType aWaveType);

    // removes all dynamic waves from wave list
    void removeDynamicWaves();

    // wave node
    struct WaveNode {
      WaveNode() : waveRef(NULL), next(NULL), isDynamic(0) {}
      WaveNode(Wave aReference, WaveNode* nextNode, int dynamic) : waveRef(aReference), next(nextNode), isDynamic(dynamic) {}

      Wave waveRef;
      WaveNode* next;
      bool isDynamic;
    };

    // push a wave node onto the waveList
    static void pushWaveNode(Wave aWave, WaveNode*& aWaveList, bool isDynamic = 0);

    // remove a node from wave list, returns whether or not wave is dynamic
    static void removeWaveNode(Wave aWave, WaveNode*& aWaveList);    

    // recursively deletes all nodes from wavelist (wave reference associated with a node remains untouched!)
    void freeWaveList(WaveNode*& aWaveList);
    void freeWaveListHelper(WaveNode* waveNode);

    // linked list storing references to all waves
    static WaveNode* globalWaveList;

    // linked list storing references to waves the need to be synthesizes (ie amplitude != 0 or frequency and phase != 0)
    static WaveNode* generateAudioWaveList[NUM_OUT_CH]; 

    // input, output and generate audio buffer sizes
    static const int AUD_IN_BUFFER_SIZE = WINDOW_SIZE;
    static const int AUD_OUT_BUFFER_SIZE = WINDOW_SIZE * 2;
    static const int GEN_AUD_BUFFER_SIZE = WINDOW_SIZE * 3;

    // generate audio buffers
    static float generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];
    static float windowingCosWave[AUD_OUT_BUFFER_SIZE];

    // input and output buffers
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

    // initialize AudioLab
    void init();

    // reset AudioLab
    void reset();

    // returns true when signal synthesis should occur and/or input buffer fills
    bool ready();

    // restores/synchronizes input/output indexes, and clears dynamic waves
    void flush();

    // pulls samples from input buffer into a buffer
    void pullSamples(int* aBuffer);

    // synthesizes one window of signal
    void synthesize();

    // returns a "static" pointer to a wave of specified type (this wave exists throughout runtime of whole program)
    Wave staticWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase = 0,  WaveType aWaveType = SINE);

    // returns a "dynamic" pointer to a wave of specified type (this wave only exists within the scope of ready())
    Wave dynamicWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase = 0,  WaveType aWaveType = SINE);

};

extern ClassAudioLab &AudioLab;

#endif