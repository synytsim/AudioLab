#ifndef Wave_h
#define Wave_h

enum WaveType
{
  SINE,
  COSINE,
  SQUARE,
  SAWTOOTH,
  TRIANGLE
};

class ClassWave
{
  protected:

    int frequency;
    int amplitude;
    int phase;
    uint8_t channel;
  
  public:

    ClassWave();
    ClassWave(int channel); 
    ClassWave(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase = 0);

    ~ClassWave();  

    void set(uint8_t aChannel, int aFrequency, int anAmplitude, int aPhase = 0);
    void reset();

    void setFrequency(int aFrequency);
    void setAmplitude(int anAmplitude);
    void setPhase(int aPhase);
    void setChannel(uint8_t aChannel);

    int getFrequency();
    int getAmplitude();
    int getPhase();
    uint8_t getChannel();

    //inline float getTimeValue(int aTimeIdx, int anOffset = 0);

    virtual float getWaveValue(int aTimeIdx) = 0;

    static void calculateSineWave();

};

class Sine: public ClassWave
{
  public:
    float getWaveValue(int aTimeIdx);
};

class Cosine: public ClassWave
{
  public:
    float getWaveValue(int aTimeIdx);
};

class Square: public ClassWave
{
  public:
    float getWaveValue(int aTimeIdx);
};

class Sawtooth: public ClassWave
{
  public:
    float getWaveValue(int aTimeIdx);
};

class Triangle: public ClassWave
{
  public:
    float getWaveValue(int aTimeIdx);
};

#endif