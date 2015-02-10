#include "SpektrumSatellite2.h"

SpektrumSatellite2::SpektrumSatellite2(HardwareSerial& serialInput) :
  _serialInput(serialInput),
  _frameIndex(0),
  _syncSeen(false),
  _numChannels(0)
{
}

boolean SpektrumSatellite2::readFromSatellite()
{
  if (this->_serialInput.available()) {
    // Read from serial input until sync frame is seen. Once sync frame is seen, we know that last frame
    // was complete, so we can parse it and make it available. This way, we don't have to know how many
    // channels are coming through serial line, but we can just always read until we see sync frame and 
    // then consider full sequence complete.
    this->_rxInput[this->_frameIndex++] = this->_serialInput.read();

    // NOTE: we have already incremented this->_frameIndex above, so we need to look at frames -1 and -2 here
    if (this->_frameIndex > 1 && this->_rxInput[this->_frameIndex - 2] == 0x03 && this->_rxInput[this->_frameIndex - 1] == 0x01) {
      // We just got the sync frame, so we'll parse the frame and reset
      this->parseFrame();
      this->_numChannels = this->_frameIndex / 2;
      this->_frameIndex = 0;
      return true;
    }    
  }
  return false;
}

void SpektrumSatellite2::parseFrame()
{
  for (int i = 0; i < this->_numChannels; ++i) {
    // 10-bit input to long
    this->_channels[i] = ((this->_rxInput[2*i] & 0x03) << 8) | this->_rxInput[2*i+1];
  }
}

int SpektrumSatellite2::getChannelCount()
{
  return this->_numChannels;
}

long* SpektrumSatellite2::getChannels()
{ 
  return _channels;
}
