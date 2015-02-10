#ifndef __SPEKTRUM_SATELLITE2_H__
#define __SPEKTRUM_SATELLITE2_H__

#include <arduino.h>

class HardwareSerial;

class SpektrumSatellite2
{
public:
  SpektrumSatellite2(HardwareSerial& serialInput);
  boolean readFromSatellite();
  int getChannelCount();
  long* getChannels();
  
private:
  void parseFrame();
  
private:
  HardwareSerial& _serialInput;
  uint8_t _rxInput[24];
  int _numChannels;
  long _channels[12];
  int _frameIndex;
  boolean _syncSeen;
};

#endif // __SPEKTRUM_SATELLITE_H__
