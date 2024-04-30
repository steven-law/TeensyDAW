#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Plugin_midi.h>
#include "mixers.h"
#include "Plugin_1.h"
#include "Plugin_2.h"
#include "Plugin_3.h"
#include "Plugin_4.h"

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
    Plugin_1 plugin_1;
    Plugin_2 plugin_2;
    Plugin_3 plugin_3;
    Plugin_4 plugin_4;
    AudioMixer4 mixer;
    AudioOutputI2S i2s;
    AudioControlSGTL5000 sgtl5000;
    AudioConnection *patchCord[6]; // total patchCordCount:4 including array typed ones.

    // constructor (this is called when class-object is created)
    Output(byte i)
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(plugin_1.mixer, 0, mixer, 0);
        patchCord[pci++] = new AudioConnection(plugin_2.mixer, 0, mixer, 1);
        patchCord[pci++] = new AudioConnection(plugin_3.mixer, 0, mixer, 2);
        patchCord[pci++] = new AudioConnection(plugin_4.mixer, 0, mixer, 3);
        patchCord[pci++] = new AudioConnection(mixer, 0, i2s, 0);
        patchCord[pci++] = new AudioConnection(mixer, 0, i2s, 1);
    }
    void setup()
    {
        mixer.gain(0, 1);
        mixer.gain(1, 1);
        // mixer.gain(2, 1);
        // mixer.gain(3, 1);
        // mixer.gain(4, 1);
        sgtl5000.enable();
        sgtl5000.volume(1);
        plugin_1.setup(17);
        plugin_2.setup(18);
        plugin_3.setup(19);
        plugin_4.setup(20);
        // plugin_5.setup(21);
    }
    void noteOn(byte note, byte velo, byte channel, byte voice)
    {
        if (channel <= 16)
            Plugin_midi.noteOn(note, velo, channel);
        if (channel == 17)
            plugin_1.noteOn(note, 1, voice);
        if (channel == 18)
            plugin_2.noteOn(note, 1, voice);
        if (channel == 19)
            plugin_3.noteOn(note, 1, voice);
        if (channel == 20)
            plugin_4.noteOn(note, 1, voice);
    }
    void set_parameters(byte trackID, byte row)
    {
        if (plugin_channel[trackID] == 17)
            plugin_1.set_parameters(row);
        if (plugin_channel[trackID] == 18)
            plugin_2.set_parameters(row);
        if (plugin_channel[trackID] == 19)
            plugin_3.set_parameters(row);
        if (plugin_channel[trackID] == 20)
            plugin_4.set_parameters(row);
    }
    void noteOff(byte note, byte velo, byte channel, byte voice)
    {
        if (channel <= 16)
            Plugin_midi.noteOff(note, velo, channel);
        if (channel == 17)
            plugin_1.noteOff(voice);
        if (channel == 18)
            plugin_2.noteOff(voice);
        if (channel == 19)
            plugin_3.noteOff(voice);
        if (channel == 20)
            plugin_4.noteOff(voice);
    }
    void draw_plugin(byte trackID, byte channel)
    {
        if (plugin_channel[trackID] < 17)
            Plugin_midi.draw_plugin();
        if (plugin_channel[trackID] == 17)
            plugin_1.draw_plugin();
        if (plugin_channel[trackID] == 18)
            plugin_2.draw_plugin();
        if (plugin_channel[trackID] == 19)
            plugin_3.draw_plugin();
        if (plugin_channel[trackID] == 20)
            plugin_4.draw_plugin();
    }
    void set_active_plugin_for_track(byte trackID, byte channel)
    {
        plugin_channel[trackID] = channel;
    }
};
// TeensyDAW: end automatically generated code
