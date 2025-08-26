#ifndef AUDIOLAB_H
#define AUDIOLAB_H

#include "AudioLabSettings.h"
#include "Wave.h"
#include "math.h"

typedef ClassWave* Wave;

class ClassAudioLab
{
  private:    
    // initialize audio input and output
    void initAudio();

    // configure input and output pins
    void configurePins();

    // initializes wave list head pointer
    void initWaveList();

    // calculates values for windowing wave
    void calculateWindowingWave();

    // returns the sum of a channel
    float getSumOfChannel(uint8_t aChannel);

    // reset generate audio, this is only called during certain events such as init() and reset()
    void resetGenerateAudio();

    // reset audio input, output buffers and indexes
    void resetAudInOut();

    // returns pointer to a wave of wave type
    Wave getNewWave(uint8_t aChannel, float aFrequency, float anAmplitude, float aPhase, WaveType aWaveType);

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

    static const uint16_t DAC_MAX = (1 << DAC_RESOLUTION) - 1;
    static const uint16_t DAC_MID = 1 << (DAC_RESOLUTION - 1);

    static const uint16_t ADC_MAX = (1 << ADC_RESOLUTION) - 1;
    static const uint16_t ADC_MID = 1 << (ADC_RESOLUTION - 1);

    // input, output and generate audio buffer sizes
    static const uint16_t AUD_IN_BUFFER_SIZE = WINDOW_SIZE;
    static const uint16_t AUD_OUT_BUFFER_SIZE = AUD_OUT_WINDOW_SIZE * 2;
    static const uint16_t GEN_AUD_BUFFER_SIZE = AUD_OUT_WINDOW_SIZE * 3;

    // generate audio buffers
    static float generateAudioBuffer[NUM_OUT_CH][GEN_AUD_BUFFER_SIZE];
    static float windowingCosWave[AUD_OUT_BUFFER_SIZE];

    // input and output buffers
    static uint16_t inputBuffer[NUM_IN_CH][AUD_IN_BUFFER_SIZE];

    volatile static uint16_t AUD_IN_BUFFER[NUM_IN_CH][AUD_IN_BUFFER_SIZE];
    volatile static uint16_t AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];
  
    // function that gets called when timer is triggered
    static void ARDUINO_ISR_ATTR AUD_IN_OUT(void);
    // returns true when input buffer is full
    static bool ARDUINO_ISR_ATTR AUD_IN_BUFFER_FULL(void);

    // restores input buffer index and synchronizes audio output buffer
    void SYNC_AUD_IN_OUT_IDX(void);

    static void blankFunction(void);

  public:
    /**
     * Initalize AudioLab, configure pins and timer
     */
    void init();

    // reset AudioLab (WIP)
    // void reset();

    /**
     * Returns true when input buffer fills and synthesis should occur
     *
     * @return true if AudioLab is ready for synthesis, otherwise false
     *
     */
    bool ready();

    /**
     * Linearly maps amplitudes of all waves on a channel so their sum will be
     * less than or equal to 1.0.
     * @param aChannel channel of the waves to be mapped
     * @param aMin the minumum value to use for mapping, if amplitude sum surpasses this value then the amplitude sum will be used.
     */
    void mapAmplitudes(uint8_t aChannel, float aMin);

    /**
     * Fills output buffer with synthesized signal composed of assigned waves
     *
     * @note This function MUST be called in the "if (AudioLab.ready())" block
     */
    void synthesize();

    /**
     * Create a 'static' wave object and returns a pointer to the object
     *
     * @param aChannel channel of the wave, must be between [0..NUM_OUT_CH), default 0
     * @param aFrequency frequency of the wave, must be positive, default 0
     * @param anAmplitude amplitude of the wave, default 0
     * @param aPhase phase of the wave, must be positive, default 0
     * @param aWaveType the type of wave, default SINE
     *
     * @return a pointer to Wave object or NULL if error
     *
     * @note static waves exist throughout runtime of program
     */
    Wave staticWave(WaveType aWaveType = SINE);
    Wave staticWave(uint8_t aChannel, WaveType aWaveType = SINE);
    Wave staticWave(uint8_t aChannel, float aFrequency = 0, float anAmplitude = 0, float aPhase = 0,  WaveType aWaveType = SINE);

    /**
     * Create a 'dynamic' wave object and returns a pointer to the object
     *
     * @param aChannel channel of the wave, must be between [0..NUM_OUT_CH), default 0
     * @param aFrequency frequency of the wave, must be positive, default 0
     * @param anAmplitude amplitude of the wave, default 0
     * @param aPhase phase of the wave, must be positive, default 0
     * @param aWaveType the type of wave, default SINE
     *
     * @return a pointer to Wave object or NULL if error
     *
     * @note dynamic waves only exist within the scope of "if (AudioLab.ready())" block
     */
    Wave dynamicWave(WaveType aWaveType = SINE);
    Wave dynamicWave(uint8_t aChannel, WaveType aWaveType = SINE);
    Wave dynamicWave(uint8_t aChannel, float aFrequency = 0, float anAmplitude = 0, float aPhase = 0,  WaveType aWaveType = SINE);
    
    /**
     * Change the wave type of an existing wave
     *
     * @param aWave reference to an existing Wave
     * @param aWaveType the type of wave to change to
     */
    void changeWaveType(Wave& aWave, WaveType aWaveType);

    /**
     * Get pointer to input buffer
     *
     * @param aChannel input buffer channel between [0..NUM_IN_CH), default is 0
     *
     * @return read-only pointer to input buffer or NULL if NUM_IN_CH == 0
     */
    uint16_t *getInputBuffer(uint8_t aChannel = 0);

    /**
     * Prints the waves that will be synthesized to Serial in format (WaveType, Frequency, Amplitude, Phase)
     *
     */
    void printWaves();

    void printAudioOutputBuffer(uint8_t aChannel);

    /**
     * pauses input and output sampling by disabled timer interrupt
     */
    void pauseSampling();

    /**
     * resumes input and output sampling by enabling timer interrupt
     */
    void resumeSampling();

};

extern ClassAudioLab AudioLab;

#endif