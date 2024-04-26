#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Plugin_midi.h>
#include "Plugin_1.h"
#include "Plugin_2.h"
#include "Plugin_3.h"
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
extern void encoder_to_value(byte XPos, byte n);
// TeensyDAW: begin automatically generated code
class Output
{
public:
    byte plugin_channel[NUM_TRACKS]; // this stores the track number that is related to the plugin number f.e plguin_channel[Plugin_0]= Track number 2
    Plugin_Midi Plugin_midi;
    Plugin_1 plugin1;
    Plugin_2 plugin_2;
    Plugin_3 plugin_3;
    AudioMixer4 mixer4;
    AudioOutputI2S i2s;
    AudioControlSGTL5000 sgtl5000;
    AudioConnection *patchCord[5]; // total patchCordCount:5 including array typed ones.

    // constructor (this is called when class-object is created)
    Output(byte i)
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(plugin1.envelope1, 0, mixer4, 0);
        patchCord[pci++] = new AudioConnection(plugin_2.mixer4, 0, mixer4, 1);
        patchCord[pci++] = new AudioConnection(plugin_3.filter, 0, mixer4, 2);
        patchCord[pci++] = new AudioConnection(mixer4, 0, i2s, 0);
        patchCord[pci++] = new AudioConnection(mixer4, 0, i2s, 1);
    }
    void setup()
    {
        mixer4.gain(0, 1);
        mixer4.gain(1, 1);
        mixer4.gain(2, 1);
        mixer4.gain(3, 1);
        sgtl5000.enable();
        sgtl5000.volume(1);
        plugin1.setup(17);
        plugin_2.setup(18);
        plugin_3.setup(19);
    }
    void noteOn(byte note, byte velo, byte channel, byte voice)
    {
        if (channel <= 16)
            Plugin_midi.noteOn(note, velo, channel);
        if (channel == 17)
            plugin1.noteOn(note, voice);
        if (channel == 18)
            plugin_2.noteOn(note, 1, voice);
        if (channel == 19)
            plugin_3.noteOn(note, 1, voice);
    }
    void set_parameters(byte trackID, byte row)
    {
        if (plugin_channel[trackID] == 17)
            plugin1.set_parameters(row);
        if (plugin_channel[trackID] == 18)
            plugin_2.set_parameters(row);
        if (plugin_channel[trackID] == 19)
            plugin_3.set_parameters(row);
    }
    void noteOff(byte note, byte velo, byte channel, byte voice)
    {
        if (channel <= 16)
            Plugin_midi.noteOff(note, velo, channel);
        if (channel == 17)
            plugin1.noteOff();
        if (channel == 18)
            plugin_2.noteOff(voice);
        if (channel == 19)
            plugin_3.noteOff(voice);
    }
    void draw_plugin(byte trackID, byte channel)
    {
        if (plugin_channel[trackID] < 17)
            Plugin_midi.draw_plugin();
        if (plugin_channel[trackID] == 17)
            plugin1.draw_plugin();
        if (plugin_channel[trackID] == 18)
            plugin_2.draw_plugin();
        if (plugin_channel[trackID] == 19)
            plugin_3.draw_plugin();
    }
    void set_active_plugin_for_track(byte trackID, byte channel)
    {
        plugin_channel[trackID] = channel;
    }
};
// TeensyDAW: end automatically generated code
