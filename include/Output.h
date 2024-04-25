#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "Plugin_1.h"
#include <Plugin_midi.h>
// Plugin_1 plugin1(1);
int tuning = 440;
extern float *note_frequency;
extern const byte PLUGIN1_PAGE1;
extern const byte PLUGIN2_PAGE1;
extern const byte PLUGIN3_PAGE1;
extern const byte NUM_PLUGINS;
// Encoder Pins
extern bool enc_moved[4];
extern int encoded[4];


// TeensyDAW: begin automatically generated code
class Output
{
public:
    byte plugin_channel[NUM_TRACKS]; // this stores the track number that is related to the plugin number f.e plguin_channel[Plugin_0]= Track number 2
    Plugin_Midi Plugin_midi;
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
        plugin1.setup(17);
    }
    void noteOn(byte note, byte velo, byte channel)
    {
        if (channel <= 16)
            Plugin_midi.noteOn(note, velo, channel);
        if (channel == 17)
            plugin1.noteOn(note);
    }
    void noteOff(byte note, byte velo, byte channel)
    {
        if (channel <= 16)
            Plugin_midi.noteOff(note, velo, channel);
        if (channel == 17)
            plugin1.noteOff();
    }
    void draw_plugin(byte trackID, byte channel)
    {
        if (plugin_channel[trackID] < 17)
            Plugin_midi.draw_plugin();
        if (plugin_channel[trackID] == 17)
            plugin1.draw_plugin();
    }
    void set_active_plugin_for_track(byte trackID, byte channel)
    {
        plugin_channel[trackID] = channel;
    }
};
// TeensyDAW: end automatically generated code
