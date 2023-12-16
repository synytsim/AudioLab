#ifndef Wave_h
#define Wave_h

class Wave
{
  public:

    Wave();
    //Wave(int channel); 
    //Wave(int channel, int freq, int amp, int phase = 0);

    ~Wave();  

    void setFrequency(int aFrequency);
    void setAmplitude(int anAmplitude);
    void setPhase(int aPhase);
    void setChannel(int aChannel);

    int getFrequency();
    int getAmplitude();
    int getPhase();
    int getChannel();

  private:

    int frequency;
    int amplitude;
    int phase;
    int channel;
    
    // static void _pushWaveNode(Wave* thisWave);
    // static void _removeWaveNode(Wave* thisWave);
    // static void _getWaveList();

};

#endif