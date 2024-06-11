#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"

extern int tuning;
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
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
class Plugin_1
{
public:
    byte myID;
    byte potentiometer[NUM_PRESETS][16];
    byte presetNr = 0;
    AudioSynthKarplusStrong string[12];
    AudioMixer12 mixer;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[14]; // total patchCordCount:14 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_1()
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
        for (int i = 0; i < 12; i++)
        {
            patchCord[pci++] = new AudioConnection(string[i], 0, mixer, i);
        }
    }
    void setup(byte setID)
    {
        myID = setID;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            mixer.gain(i, 1);
        }
        MixGain.gain(1);
        SongVol.gain(1);
        Serial.println("Setup Pl1 Done");
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        float frequency = note_frequency[notePlayed] * tuning;
        string[voice].noteOn(frequency, velocity);
        // Serial.printf("ON voice: %d, freq: %f \n", voice, frequency);
    }
    void noteOff(byte voice)
    {
        string[voice].noteOff(0);
        // Serial.printf("OFF voice: %d,  \n", voice);
    }
    void set_parameters(byte row)
    {
        draw_plugin();
        if (!buttonPressed[BUTTON_SHIFT])
        {
            if (row == 0)
            {

                set_mixer_gain(0, 0, "Vol", 0, 1);
                set_mixer_gain(1, 0, "Vol", 0, 1);
                set_mixer_gain(2, 0, "Vol", 0, 1);
                set_mixer_gain(3, 0, "Vol", 0, 1);
            }

            if (row == 1)
            {

                set_mixer_gain(0, 1, "Vol", 0, 1);
                set_mixer_gain(1, 1, "Vol", 0, 1);
                set_mixer_gain(2, 1, "Vol", 0, 1);
                set_mixer_gain(3, 1, "Vol", 0, 1);
            }

            if (row == 2)
            {

                set_mixer_gain(0, 2, "Vol", 0, 1);
                set_mixer_gain(1, 2, "Vol", 0, 1);
                set_mixer_gain(2, 2, "Vol", 0, 1);
                set_mixer_gain(3, 2, "Vol", 0, 1);
            }

            if (row == 3)
            {
            }
        }
        if (buttonPressed[BUTTON_SHIFT])
        {
            set_presetNr();
        }
    }
    void draw_plugin()
    {
        if (change_plugin_row)
        {
            change_plugin_row = false;
            clearWorkSpace();
            // Serial.println("drawing plugin 1");
            drawPot(0, 0, potentiometer[presetNr][0], "Vol");
            drawPot(1, 0, potentiometer[presetNr][1], "Vol");
            drawPot(2, 0, potentiometer[presetNr][2], "Vol");
            drawPot(3, 0, potentiometer[presetNr][3], "Vol");

            drawPot(0, 1, potentiometer[presetNr][4], "Vol");
            drawPot(1, 1, potentiometer[presetNr][5], "Vol");
            drawPot(2, 1, potentiometer[presetNr][6], "Vol");
            drawPot(3, 1, potentiometer[presetNr][7], "Vol");

            drawPot(0, 2, potentiometer[presetNr][8], "Vol");
            drawPot(1, 2, potentiometer[presetNr][9], "Vol");
            drawPot(2, 2, potentiometer[presetNr][10], "Vol");
            drawPot(3, 2, potentiometer[presetNr][11], "Vol");

            draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
        }
    }

    void set_mixer_gain(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT);
            mixer.gain(n, sustain);
        }
    }

    void set_presetNr()
    {
        if (enc_moved[PRESET_ENCODER])
        {
            presetNr = constrain(presetNr + encoded[PRESET_ENCODER], 0, NUM_PRESETS - 1);
            change_plugin_row = true;
            draw_plugin();
        }
    }
};
// TeensyDAW: end automatically generated code
