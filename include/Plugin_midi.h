#include <Arduino.h>
extern const byte VELOCITY_NOTE_ON;
extern const byte VELOCITY_NOTE_OFF;

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
void draw_plugin()
    {
        clearWorkSpace();
    }


};