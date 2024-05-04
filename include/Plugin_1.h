#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
extern void drawPot(int XPos, byte YPos, int dvalue, const char *dname);
extern int tuning;
// TeensyDAW: begin automatically generated code

class Plugin_1
{
public:
    byte myID;
    byte potentiometer[16];
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
    void draw_plugin()
    {

        if (change_plugin_row)
        {
            change_plugin_row = false;
            // Serial.println("drawing plugin 1");
            drawPot(0, 0, potentiometer[0], "Vol");
            drawPot(1, 0, potentiometer[1], "Vol");
            drawPot(2, 0, potentiometer[2], "Vol");
            drawPot(3, 0, potentiometer[3], "Vol");

            drawPot(0, 1, potentiometer[4], "Vol");
            drawPot(1, 1, potentiometer[5], "Vol");
            drawPot(2, 1, potentiometer[6], "Vol");
            drawPot(3, 1, potentiometer[7], "Vol");

            drawPot(0, 2, potentiometer[8], "Vol");
            drawPot(1, 2, potentiometer[9], "Vol");
            drawPot(2, 2, potentiometer[10], "Vol");
            drawPot(3, 2, potentiometer[11], "Vol");
        }
    }

    void set_mixer_gain(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            mixer.gain(n, sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }

    void set_filter_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int frequency = note_frequency[potentiometer[n]] * tuning;

            // filter.frequency(frequency);
            Serial.println(frequency);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_filter_resonance(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            // filter.resonance((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_filter_sweep(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            // filter.octaveControl((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
};
// TeensyDAW: end automatically generated code
