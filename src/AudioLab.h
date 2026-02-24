#ifndef AUDIOLAB_H
#define AUDIOLAB_H

#include "AudioLabSettings.h"
#include "Wave.h"

class Channel {
  private:
    Node *node;
    Operand op;
  public:
    Channel();
    Channel& operator=(const ClassWave& right);
    Channel& operator=(const Node& right);
    Channel& operator=(const Composite& right);

    void clear(void);

    float getValue(void) const;
};

class ClassAudioLab
{
  private:    
    // initialize audio input and output
    void initAudio();

    // configure input and output pins
    void configurePins();

    #if defined(USING_AD56X4_DAC)
    void resetAD56X4(void);
    #endif

    // calculates values for windowing wave
    void calculateWindowingWave();

    // reset generate audio, this is only called during certain events such as init() and reset()
    void resetGenerateAudio();

    // reset audio input, output buffers and indexes
    void resetAudInOut();

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
    volatile static uint16_t AUD_IN_BUFFER[NUM_IN_CH][AUD_IN_BUFFER_SIZE];
    volatile static uint16_t AUD_OUT_BUFFER[NUM_OUT_CH][AUD_OUT_BUFFER_SIZE];
  
    // function that gets called when timer is triggered
    static void ARDUINO_ISR_ATTR AUD_IN_OUT(void);
    
    // returns true when input buffer is full
    static bool AUD_IN_BUFFER_FULL(void);

    // restores input buffer index and synchronizes audio output buffer
    void SYNC_AUD_IN_OUT_IDX(void);

  public:
    
    Channel channel[NUM_OUT_CH];

    /**
     * Initalize AudioLab, configure pins and timer
     */
    void init();

    /**
     * Returns true when synthesis should occur
     *
     * @return true if AudioLab is ready for synthesis, otherwise false
     * @note this definition should be used if ADC samples are not needed
     */
    bool ready(void);


    /**
     * Returns true when input buffer fills with ADC samples and synthesis should occur
     *
     * @param buffer buffer to copy samples to, must be of size [NUM_IN_CH][WINDOW_SIZE]
     * @return true if AudioLab is ready for synthesis, otherwise false
     * @note use this definition if aquiring samples from ADC
     *
     */
    template <typename T>
    bool ready(T *buffer, uint16_t bufferSize = WINDOW_SIZE) {
      if (!AUD_IN_BUFFER_FULL()) return false;

      uint16_t c, i;
      // store samples from volatile input buffer to non-volatile buffer
      for (c = 0; c < NUM_IN_CH; c++) {
        for (i = 0; i < WINDOW_SIZE; i++) {
          buffer[c * bufferSize + i] = AUD_IN_BUFFER[c][i];
        }
      }

      // reset/synchronize input and output indexes to continue sampling
      SYNC_AUD_IN_OUT_IDX();

      return true;
    };

    /**
     * Fills output buffer with synthesized signal composed of assigned waves
     *
     * @note This function MUST be called in the "if (AudioLab.ready())" block
     */
    void synthesize();

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