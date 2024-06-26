#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Plugin_midi.h>
#include <plugin_List.h>
#include "global_stuff.h"
#include "FX_Section.h"
extern bool change_plugin_row;

extern float *note_frequency;

// Encoder Pins
extern bool enc_moved[4];
extern int encoded[4];
void drawPot(int XPos, byte YPos, int dvalue, const char *dname);
byte getEncodervalue(byte XPos, byte YPos, const char *name, byte oldValue);
void clearWorkSpace();

// TeensyDAW: begin automatically generated code
class Output
{
public:
    byte plugin_channel[NUM_TRACKS]; // this stores the track number that is related to the plugin number f.e plguin_channel[Plugin_0]= Track number 2
    Plugin_Midi Plugin_midi;
    FX_Section fx_section;
    AudioOutputI2S i2s;
    AudioControlSGTL5000 sgtl5000;
    AudioConnection *patchCord[2]; // total patchCordCount:2 including array typed ones.

    // constructor (this is called when class-object is created)
    Output(byte i)
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(fx_section.endmixer, 0, i2s, 0);
        patchCord[pci++] = new AudioConnection(fx_section.endmixer, 0, i2s, 1);
    }
    void setup()
    {

        sgtl5000.enable();
        sgtl5000.volume(1);
        fx_section.setup();

        for (int i = 0; i < NUM_PLUGINS; i++)
        {
            allPlugins[i]->setup();
        }
    }
    void noteOn(byte note, byte velo, byte channel, byte voice)
    {
        // Serial.printf("NoteOn channel: %d, note: %d\n", channel, note);
        if (channel <= 16)
            Plugin_midi.noteOn(note, velo, channel);
        for (byte i = 0; i < NUM_PLUGINS; i++)
        {
            Serial.println("recieve Note");
            if (channel == i + 17)
                allPlugins[i]->noteOn(note, 1, voice);
        }
    }
    void noteOff(byte note, byte velo, byte channel, byte voice)
    {
        if (channel <= 16)
            Plugin_midi.noteOff(note, velo, channel);
        for (byte i = 0; i < NUM_PLUGINS; i++)
        {
            if (channel == i + 17)
                allPlugins[i]->noteOff(note, voice);
        }
    }

    void set_parameters(byte trackID, byte row)
    {
        // Serial.printf("set parameters track: %d, channel: %d\n", trackID, plugin_channel[trackID]);
        if (plugin_channel[trackID] < 17)
            Plugin_midi.set_parameters(row);
        for (byte i = 0; i < NUM_PLUGINS; i++)
        {
            if (plugin_channel[trackID] == i + 17)
                allPlugins[i]->set_parameters(row);
        }
    }
    void draw_plugin(byte trackID, byte channel)
    {
        // Serial.printf("draw plugin track: %d, channel: %d\n", trackID, plugin_channel[trackID]);
        change_plugin_row = true;
        if (plugin_channel[trackID] < 17)
            Plugin_midi.draw_plugin();
        for (byte i = 0; i < NUM_PLUGINS; i++)
        {
            if (plugin_channel[trackID] == i + 17)
                allPlugins[i]->draw_plugin();
        }
    }
    void set_active_plugin_for_track(byte trackID, byte channel)
    {
        plugin_channel[trackID] = channel;
    }

    // TeensyDAW: end automatically generated code
};