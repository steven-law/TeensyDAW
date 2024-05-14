#include <Arduino.h>

void clearWorkSpace();
extern bool change_plugin_row; 
class Plugin_Midi
{
public:
    void noteOn(byte note, byte velocity, byte channel)
    {
        usbMIDI.sendNoteOn(note, VELOCITY_NOTE_ON, channel); // Send a Note (pitch 42, velo 127 on channel 1)
    }
    void noteOff(byte note, byte velocity, byte channel)
    {
        usbMIDI.sendNoteOff(note, VELOCITY_NOTE_OFF, channel); // Send a Note (pitch 42, velo 127 on channel 1)
    }
    void set_parameters(byte row)
    {
        draw_plugin();
        if (row == 0)
        {
        }

        if (row == 1)
        {
        }

        if (row == 2)
        {
        }

        if (row == 3)
        {
        }
    }
    void draw_plugin()
    {
        if (change_plugin_row)
        {
            change_plugin_row = false;
            // Serial.println("drawing MIDI plugin");
        }
    }
};