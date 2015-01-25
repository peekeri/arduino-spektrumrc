#include "SpektrumSatellite.h"
#include "PPMOutput.h"

SpektrumSatellite satellite(Serial1);
PPMOutput ppm(52);

void setup()
{
  //Serial.begin(115200);
  Serial1.begin(115200);
  ppm.initialize();
}

void loop() 
{
  if (satellite.readFromSatellite()) {    
    // Got new data from satellite -> send it over PPM
    long* channels = satellite.getChannels();
    ppm.outputPPM(channels, 8);
    /*for (int i = 0; i < 8; ++i) {
      Serial.print(channels[i]); Serial.print(" ");
    }
    Serial.println("");
    */
  }
}
