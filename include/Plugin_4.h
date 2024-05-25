#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include <AudioSamples.h>

extern bool buttonPressed[NUM_BUTTONS];
// TeensyDAW: begin automatically generated code
// Name: mDrm
// Description: Plays 12 Samplesounds stored on Flash (PROGMEM)
// Voices: 12

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
class Plugin_4
{
public:
    byte myID;
    byte potentiometer[NUM_PRESETS][16];
    byte presetNr = 0;
    AudioPlayMemory playMem[12];
    AudioMixer12 mixer;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[14]; // total patchCordCount:14 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_4()
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
        for (int i = 0; i < 12; i++)
        {
            patchCord[pci++] = new AudioConnection(playMem[i], 0, mixer, i);
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
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        if (notePlayed == 48)
            playMem[0].play(AudioSampleKick);
        if (notePlayed == 49)
            playMem[1].play(AudioSampleClap);
        if (notePlayed == 50)
            playMem[2].play(AudioSampleHihat);
        if (notePlayed == 51)
            playMem[3].play(AudioSampleSnare);
        if (notePlayed == 52)
            playMem[3].play(AudioSampleP2);
        if (notePlayed == 53)
            playMem[3].play(AudioSamplePongblip);
        if (notePlayed == 54)
            playMem[3].play(AudioSampleTomtom);
        if (notePlayed == 55)
            playMem[3].play(AudioSampleCashregister);
    }
    void noteOff(byte voice)
    {
    }

    void set_parameters(byte row)
    {
        draw_plugin();
        if (!buttonPressed[BUTTON_SHIFT])
        {
            if (row == 0)
            {
                set_mixer_gain(0, 0, "Kick", 0, 1);
                set_mixer_gain(1, 0, "Clap", 0, 1);
                set_mixer_gain(2, 0, "HHat", 0, 1);
                set_mixer_gain(3, 0, "Snare", 0, 1);
            }

            if (row == 1)
            {
                set_mixer_gain(0, 1, "Tick", 0, 1);
                set_mixer_gain(1, 1, "Pong", 0, 1);
                set_mixer_gain(2, 1, "Tom", 0, 1);
                set_mixer_gain(3, 1, "Cash", 0, 1);
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

            drawPot(0, 0, potentiometer[presetNr][0], "Kick");
            drawPot(1, 0, potentiometer[presetNr][1], "Clap");
            drawPot(2, 0, potentiometer[presetNr][2], "HHat");
            drawPot(3, 0, potentiometer[presetNr][3], "Snare");

            drawPot(0, 1, potentiometer[presetNr][4], "Tick");
            drawPot(1, 1, potentiometer[presetNr][5], "Pong");
            drawPot(2, 1, potentiometer[presetNr][6], "Tom");
            drawPot(3, 1, potentiometer[presetNr][7], "Cash");

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
