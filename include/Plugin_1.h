#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
extern float *note_frequency;
extern int tuning;
extern const byte VELOCITY_NOTE_ON;
extern const byte VELOCITY_NOTE_OFF;
extern void drawPot(int XPos, byte YPos, int dvalue, const char *dname, int color);
extern void clearWorkSpace();
// TeensyDAW: begin automatically generated code

class Plugin_1
{
public:
    byte myID;
    byte potentiometer[16];
    AudioSynthKarplusStrong string[MAX_VOICES];
    AudioMixer12 mixer;
    AudioConnection *patchCord[12]; // total patchCordCount:12 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_1()
    {
        int pci = 0; // used only for adding new patchcords
        for (int i = 0; i < MAX_VOICES; i++)
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
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        float frequency = note_frequency[notePlayed] * tuning;
        string[voice].noteOn(frequency, velocity);
        Serial.printf("ON voice: %d, freq: %f \n", voice, frequency);
    }
    void noteOff(byte voice)
    {
        string[voice].noteOff(0);
        Serial.printf("OFF voice: %d,  \n", voice);
    }
    void set_parameters(byte row)
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
    void draw_plugin()
    {
        clearWorkSpace();

        drawPot(0, 0, potentiometer[0], "Vol", ILI9341_BLUE);
        drawPot(1, 0, potentiometer[5], "Vol", ILI9341_BLUE);
        drawPot(2, 0, potentiometer[0], "Vol", ILI9341_BLUE);
        drawPot(3, 0, potentiometer[5], "Vol", ILI9341_BLUE);

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
  
    void set_filter_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int frequency = note_frequency[potentiometer[n]] * tuning;

            // filter.frequency(frequency);
            Serial.println(frequency);
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_filter_resonance(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            // filter.resonance((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_filter_sweep(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            // filter.octaveControl((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
};
// TeensyDAW: end automatically generated code
