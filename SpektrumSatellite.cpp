#include "SpektrumSatellite.h"

SpektrumSatellite::SpektrumSatellite(HardwareSerial& serialInput) :
  _serialInput(serialInput),
  _lastUpdate(millis()),
  _frameIndex(0)
{
}

boolean SpektrumSatellite::readFromSatellite()
{
  if (this->_serialInput.available()) {
    uint8_t b = this->_serialInput.read();

    unsigned long current = millis();
    // detect inter-frame delay
    if (current - this->_lastUpdate > 6)
    {
        // start of frame
        this->_frameIndex = 0;
    }
    // add to frame
    this->_rxInput[this->_frameIndex++] = b;
    if (this->_frameIndex == 16)
    {
        // end of frame
        this->parseFrame();
        this->_frameIndex = 0;
    }
    this->_lastUpdate = current;
    return !this->_frameIndex && this->_frameSeen == 3;     
  }
  return false;
}

void SpektrumSatellite::parseFrame()
{
  this->_header = this->_rxInput[0] << 8 | this->_rxInput[1];

  if (!(this->_rxInput[2] >> 7)) {
    this->_frameSeen = 1;
  } else {
    this->_frameSeen |= 2;
  }

  for (uint8_t i = 2; i < 16; i += 2) {
    uint8_t hi = this->_rxInput[i];
    uint8_t lo = this->_rxInput[i+1];
    // extract channel
    uint8_t ch = (hi >> 3) & 0xf;
    // if channel is valid, extract value
    if (ch < 8) {
      this->_channels[ch] = (hi & 0x7) << 8 | lo;
    }
  }
}

long* SpektrumSatellite::getChannels()
{ 
  /*
  for (int i = 0; i < 8; ++i) {
    Serial.print(_channels[i]); Serial.print(" ");
  }
  Serial.println("");
  */
  return _channels;
}
