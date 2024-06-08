#ifndef Wave_h
#define Wave_h

#include <Arduino.h>
#include <math.h>
#include "AudioLabSettings.h"

enum WaveType
{
  SINE,
  COSINE,
  SQUARE,
  SAWTOOTH,
  TRIANGLE
};

const char* getWaveName(WaveType aWaveType);

// abstract base class for wave
class ClassWave
{
  protected:

    float frequency;
    float amplitude;
    float phase;
    
    int _phase;

    uint8_t channel;
    WaveType waveType;

  private:
    // static inline float getTimeValue(int aTimeIdx, int anOffset = 0);
  
  public:

    ClassWave();

    ~ClassWave();  

    void set(uint8_t aChannel, float aFrequency, float anAmplitude, float aPhase);
    void reset();

    void setFrequency(float aFrequency);
    void setAmplitude(float anAmplitude);
    void setPhase(float aPhase);
    void setChannel(uint8_t aChannel);

    float getFrequency() const;
    float getAmplitude() const;
    float getPhase() const;
    uint8_t getChannel() const;

    WaveType getWaveType() const;

    // pure virtual function for getting the value associated with the wave
    virtual float getWaveValue() const = 0;

    // calculate values for a 1Hz sine wave sampled at SAMPLE_RATE
    static void calculateSineWave();

    static void iterateTimeIndex();

    static void synchronizeTimeIndex();

};

class Sine: public ClassWave
{
  public:
    Sine();
    float getWaveValue() const;
};

class Cosine: public ClassWave
{
  public:
    Cosine();
    float getWaveValue() const;
};

class Square: public ClassWave
{
  public:
    Square();
    float getWaveValue() const;
};

class Sawtooth: public ClassWave
{
  public:
    Sawtooth();
    float getWaveValue() const;
};

class Triangle: public ClassWave
{
  public:
    Triangle();
    float getWaveValue() const;
};

#endif