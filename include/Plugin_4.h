#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "mixers.h"

// TeensyDAW: begin automatically generated code

class Plugin_4
{
public:
    byte myID;
    byte potentiometer[16];
    AudioPlayMemory playMem[12];
    AudioMixer12 mixer;
    AudioConnection *patchCord[12]; // total patchCordCount:12 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_4()
    {
        int pci = 0; // used only for adding new patchcords

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
        if (notePlayed == 53)
            playMem[3].play(AudioSampleTomtom);
        if (notePlayed == 54)
            playMem[3].play(AudioSampleCashregister);
    }
    void noteOff(byte voice)
    {
    }

    void set_parameters(byte row)
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
        clearWorkSpace();

        drawPot(0, 0, potentiometer[0], "Kick", ILI9341_BLUE);
        drawPot(1, 0, potentiometer[5], "Clap", ILI9341_BLUE);
        drawPot(2, 0, potentiometer[0], "HHat", ILI9341_BLUE);
        drawPot(3, 0, potentiometer[5], "Snare", ILI9341_BLUE);

        // drawPot(0, 2, potentiometer[8], "Filt-Frq", ILI9341_BLUE);
        // drawPot(1, 2, potentiometer[9], "Resonance", ILI9341_BLUE);
        // drawPot(2, 2, potentiometer[10], "Sweep", ILI9341_BLUE);
        // drawPot(3, 2, potentiometer[10], "Type", ILI9341_BLUE);
        drawPot(0, 1, potentiometer[12], "Vol", ILI9341_BLUE);
        drawPot(1, 1, potentiometer[13], "Vol", ILI9341_BLUE);
        drawPot(2, 1, potentiometer[14], "Vol", ILI9341_BLUE);
        drawPot(3, 1, potentiometer[15], "Vol", ILI9341_BLUE);

        drawPot(0, 2, potentiometer[12], "Vol", ILI9341_BLUE);
        drawPot(1, 2, potentiometer[13], "Vol", ILI9341_BLUE);
        drawPot(2, 2, potentiometer[14], "Vol", ILI9341_BLUE);
        drawPot(3, 2, potentiometer[15], "Vol", ILI9341_BLUE);
    }

    void set_mixer_gain(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            mixer.gain(n, sustain);
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
};
// TeensyDAW: end automatically generated code
