//#include "SpektrumSatellite.h"
#include "SpektrumSatellite2.h"
#include "PPMOutput.h"

SpektrumSatellite2 satellite(Serial1);
PPMOutput ppm(52);

void setup()
{
  //Serial.begin(115200);
  Serial1.begin(115200);
  ppm.initialize();
}

TODO: SpektrumSatellite2 now assumes 10-bit resolution for channel data, can we somehow detect that???

void loop() 
{
  if (satellite.readFromSatellite()) {    
    // Got new data from satellite -> send it over PPM
    long* channels = satellite.getChannels();
    ppm.outputPPM(channels, satellite.getChannelCount());
    /*for (int i = 0; i < 8; ++i) {
      Serial.print(channels[i]); Serial.print(" ");
    }
    Serial.println("");
    */
  }
}
