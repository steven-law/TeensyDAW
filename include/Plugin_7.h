#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// TeensyDAW: begin automatically generated code
//Name: Boom
//Description: Subtractive Synthesizer
//Voices: 1

//Kick
//Pot 1: Sweep
//Pot 2: Noise
//Pot 3: O-Drive
//Pot 4: Decay

//
//Pot 5: 
//Pot 6: 
//Pot 7: 
//Pot 8: 

//
//Pot 9: 
//Pot 10: 
//Pot 11: 
//Pot 12: 

//
//Pot 13: 
//Pot 14: 
//Pot 15: 
//Pot 16: 
class Plugin_7
{
public:
    byte myID;
    byte potentiometer[8];
    AudioSynthFMDrum fm_drum;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[2]; // total patchCordCount:2 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_7()
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(fm_drum, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
    }

    void setup(byte setID)
    {
        myID = setID;

        MixGain.gain(1);
        SongVol.gain(1);
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        float frequence = note_frequency[notePlayed] * tuning;
        fm_drum.frequency(frequence);
        fm_drum.noteOn();
    }
    void noteOff(byte voice)
    {
    }
    void set_parameters(byte row)
    {
        draw_plugin();
        if (row == 0)
        {
            set_fmdrum_pitchMod(0, 0, "Sweep", 0, 1);
            set_fmdrum_noise(1, 0, "Noise", 0, 1);
            set_fmdrum_overdrive(2, 0, "O-Drive", 0, 1);
            set_fmdrum_decay(3, 0, "Decay", 0, 2000);
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
            // Serial.println("drawing plugin 5");

            drawPot(0, 0, potentiometer[0], "Sweep");
            drawPot(1, 0, potentiometer[1], "Noise");
            drawPot(2, 0, potentiometer[2], "O-Drive");
            drawPot(3, 0, potentiometer[3], "Decay");
        }
    }
  
    void set_fmdrum_pitchMod(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.fm(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmdrum_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.decay(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmdrum_noise(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.noise(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmdrum_overdrive(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.overdrive(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
};
// TeensyDAW: end automatically generated code
