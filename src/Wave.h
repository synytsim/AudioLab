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

    int frequency;
    int amplitude;
    int phase;
    uint8_t channel;
    WaveType waveType;

  private:
    // static inline float getTimeValue(int aTimeIdx, int anOffset = 0);
  
  public:

    ClassWave();

    ~ClassWave();  

    void set(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase);
    void reset();

    void setFrequency(int aFrequency);
    void setAmplitude(int anAmplitude);
    void setPhase(int aPhase);
    void setChannel(uint8_t aChannel);

    int getFrequency() const;
    int getAmplitude() const;
    int getPhase() const;
    uint8_t getChannel() const;

    WaveType getWaveType() const;

    // pure virtual function for getting the value associated with the wave
    virtual float getWaveValue(int aTimeIdx) const = 0;

    // calculate values for a 1Hz sine wave sampled at SAMPLE_RATE
    static void calculateSineWave();

};

class Sine: public ClassWave
{
  public:
    Sine();
    float getWaveValue(int aTimeIdx) const;
};

class Cosine: public ClassWave
{
  public:
    Cosine();
    float getWaveValue(int aTimeIdx) const;
};

class Square: public ClassWave
{
  public:
    Square();
    float getWaveValue(int aTimeIdx) const;
};

class Sawtooth: public ClassWave
{
  public:
    Sawtooth();
    float getWaveValue(int aTimeIdx) const;
};

class Triangle: public ClassWave
{
  public:
    Triangle();
    float getWaveValue(int aTimeIdx) const;
};

#endif