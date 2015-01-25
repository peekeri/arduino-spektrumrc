#ifndef __SPEKTRUM_SATELLITE_H__
#define __SPEKTRUM_SATELLITE_H__

#include <arduino.h>

class HardwareSerial;

class SpektrumSatellite 
{
public:
  SpektrumSatellite(HardwareSerial& serialInput);
  boolean readFromSatellite();
  long* getChannels();
  
private:
  void parseFrame();
  
private:
  HardwareSerial& _serialInput;
  char _rxInput[16];
  long _channels[8];
  unsigned long _lastUpdate;
  int _frameIndex;
  char _frameSeen;
  char _header;
};

#endif // __SPEKTRUM_SATELLITE_H__
