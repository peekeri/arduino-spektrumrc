#ifndef __PPMOUTPUT_H__
#define __PPMOUTPUT_H__

#include <arduino.h>

// Signal duration when it is on its lowest value, in microseconds
const long SIGNAL_SHORTEST = 550;
// Signal duration when it is on its highest value, in microseconds
const long SIGNAL_LONGEST = 1880;
// Signal spacer inserted between each channel
const long SIGNAL_SPACER = 400;
// Total PPM frame duration in microseconds
const long PPM_FRAME_LENGTH = 22000;
// Max value transmitter may send 
const long MAX_RX_THROW = 2048;

struct Signal {
  uint8_t state; // HIGH or LOW
  long duration;  // duration of signal in microseconds
  
  Signal(uint8_t s, int d): state(s), duration(d) {}
  Signal() : state(LOW), duration(SIGNAL_SHORTEST) {}
};


// Outputs continuous stream of PPM frames to given output pin. 
class PPMOutput
{
public:
  PPMOutput(int ppmOutputPin);
  
  void initialize();
  void outputPPM(long* channels, int numChannels);
  void interrupt(boolean restart);
  
private:
  Signal nextSignal();
  void applyInputSignals();
  
private:
  int _ppmOutputPin;
  bool _signalUp;
  Signal _signals[16];
  Signal _inputSignals[16];
  int _currentSignal;
  byte _rxInput[16];
};

#endif // __PPMOUTPUT_H__
