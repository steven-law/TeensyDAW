#ifndef PLUGIN_1_H
#define PLUGIN_1_H

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include <pluginClass.h>

// TeensyDAW: begin automatically generated code
// Name: Strng
// Description: Synthesize 12 plucked string sounds, such as a guitar sound
// Voices: 12
// Strings
// Pot 1: Vol1
// Pot 2: Vol2
// Pot 3: Vol3
// Pot 4: Vol4

// Pot 5: Vol5
// Pot 6: Vol6
// Pot 7: Vol7
// Pot 8: Vol8

// Pot 9: Vol9
// Pot 10: Vol10
// Pot 11: Vol11
// Pot 12: Vol12

extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();
class Plugin_1 : public PluginControll
{
public:
    AudioSynthKarplusStrong string[12];
    AudioMixer12 mixer;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[14]; // total patchCordCount:14 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_1(const char *Name, byte ID) : PluginControll(Name, ID)
    {
        int pci = 0; // used only for adding new patchcords
        patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
        for (int i = 0; i < 12; i++)
        {
            patchCord[pci++] = new AudioConnection(string[i], 0, mixer, i);
        }
    }
    virtual ~Plugin_1() = default;
    
    virtual void setup() override;
    virtual void noteOn(byte notePlayed, float velocity, byte voice) override;
    virtual void noteOff(byte notePlayed, byte voice) override;
    virtual void set_parameters(byte row) override;
    virtual void draw_plugin() override;

    virtual void assign_mixer_gain(byte value, byte channel) override;
};
#endif // PLUGIN_1_H