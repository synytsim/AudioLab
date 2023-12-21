#ifndef Wave_h
#define Wave_h

class Wave
{
  public:

    Wave();
    //Wave(int channel); 
    Wave(int aChannel, int aFrequency, int anAmplitude, int aPhase);

    ~Wave();  

    void setFrequency(int aFrequency);
    void setAmplitude(int anAmplitude);
    void setPhase(int aPhase);
    void setChannel(int aChannel);

    void reset();

    int getFrequency();
    int getAmplitude();
    int getPhase();
    int getChannel();

    float getWaveVal(int waveIdx);

    static void calculateWaves();

  private:

    static float sinWave[SAMPLE_FREQ];

    int frequency;
    int amplitude;
    int phase;
    int channel;
    
    // static void _pushWaveNode(Wave* thisWave);
    // static void _removeWaveNode(Wave* thisWave);
    // static void _getWaveList();

};

#endif