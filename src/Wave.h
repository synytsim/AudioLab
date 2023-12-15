#ifndef Wave_h
#define Wave_h

#include "AudioLab.h"

class Wave
{
  public:

    Wave();
    //Wave(int channel); 
    //Wave(int channel, int freq, int amp, int phase = 0);

    ~Wave();  

    void setFreq(int freq) { _freq = freq; }
    void setAmp(int amp) { _amp = amp; }
    void setPhase(int phase) { _phase = phase; }
    void setChannel(int channel)  { _channel = channel; }

    int getFreq() { return _freq; }
    int getAmp() { return _amp; }
    int getPhase() { return _phase; }
    int getChannel() { return _channel; }

  private:

    int _freq;
    int _amp;
    int _phase;
    int _channel;
    
    // static void _pushWaveNode(Wave* thisWave);
    // static void _removeWaveNode(Wave* thisWave);
    // static void _getWaveList();

};

#endif