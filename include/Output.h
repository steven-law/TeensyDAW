#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "Plugin_1.h"
//Plugin_1 plugin1(1);
 
// TeensyDAW: begin automatically generated code
class Output
{
public:
#define tuning 440
    float *note_frequency;
    Plugin_1 plugin1;
    AudioMixer4 mixer4;
    AudioOutputI2S i2s;
    AudioControlSGTL5000 sgtl5000;
    AudioConnection *patchCord[2]; // total patchCordCount:2 including array typed ones.

    // constructor (this is called when class-object is created)
    Output(byte i)
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(plugin1.envelope1, 0, mixer4, 0);
        patchCord[pci++] = new AudioConnection(mixer4, 0, i2s, 0);
    }
    void setup()
    {
        sgtl5000.enable();
        sgtl5000.volume(1);
        plugin1.setup();
    }
};
// TeensyDAW: end automatically generated code
