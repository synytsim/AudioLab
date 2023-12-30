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

const char* getWaveName(WaveType aWaveType);

class ClassWave
{
  protected:

    int frequency;
    int amplitude;
    int phase;
    uint8_t channel;
    WaveType waveType;
  
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

    int getFrequency() const;
    int getAmplitude() const;
    int getPhase() const;
    uint8_t getChannel() const;

    WaveType getWaveType() const;

    //inline float getTimeValue(int aTimeIdx, int anOffset = 0);

    virtual float getWaveValue(int aTimeIdx) const = 0;

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