#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"


void drawPot(int XPos, byte YPos, int dvalue, const char *dname);
byte getEncodervalue(byte XPos, byte YPos, const char *name, byte oldValue);
void draw_sequencer_option(byte x, const char *nameshort, int value, byte enc, const char *pluginName);
extern int tuning;
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern float *note_frequency;
// TeensyDAW: begin automatically generated code
// Name: 1Osc
// Description: Subtractive Synthesizer
// Voices: 12

// VCO
// Pot 1: Waveform
// Pot 2: Vol
// Pot 3:
// Pot 4:

// Pot 5:
// Pot 6:
// Pot 7:
// Pot 8:

// Filter:
// Pot 9: Frequency
// Pot 10: Resonance
// Pot 11: Sweep
// Pot 12: Type

// Envelope:
// Pot 13: Attack
// Pot 14: Decay
// Pot 15: Sustain
// Pot 16: Release

class Plugin_2 : public FX_Section
{
public:
    //byte myID;
    //byte Potentiometer[NUM_PRESETS][16];
    //byte presetNr;
    AudioSynthWaveformDc dc[12];
    AudioSynthWaveform waveform[12];
    AudioEffectEnvelope Fenv[12];
    AudioFilterStateVariable filter[12];
    AudioMixer4 fMixer[12];
    AudioEffectEnvelope Aenv[12];
    AudioMixer12 mixer;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[98]; // total patchCordCount:98 including array typed ones.

    Plugin_2()
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
        for (int i = 0; i < 12; i++)
        {
            patchCord[pci++] = new AudioConnection(dc[i], 0, Fenv[i], 0);
            patchCord[pci++] = new AudioConnection(waveform[i], 0, filter[i], 0);
            patchCord[pci++] = new AudioConnection(Fenv[i], 0, filter[i], 1);
            patchCord[pci++] = new AudioConnection(filter[i], 0, fMixer[i], 0);
            patchCord[pci++] = new AudioConnection(filter[i], 1, fMixer[i], 1);
            patchCord[pci++] = new AudioConnection(filter[i], 2, fMixer[i], 2);
            patchCord[pci++] = new AudioConnection(fMixer[i], 0, Aenv[i], 0);
            patchCord[pci++] = new AudioConnection(Aenv[i], 0, mixer, i);
        }
    }
    virtual void setup(byte setID) override
    {
        myID = setID;

        for (int i = 0; i < MAX_VOICES; i++)
        {
            dc[i].amplitude(1);

            waveform[i].begin(WAVEFORM_SINE);
            waveform[i].amplitude(1);

            Fenv[i].delay(0);
            Fenv[i].attack(0);
            Fenv[i].hold(0);
            Fenv[i].decay(0);
            Fenv[i].sustain(1);
            Fenv[i].release(200);

            filter[i].frequency(2000);
            filter[i].resonance(1);
            filter[i].octaveControl(4);

            fMixer[i].gain(0, 1);
            fMixer[i].gain(1, 0);
            fMixer[i].gain(2, 0);
            fMixer[i].gain(3, 0);

            Aenv[i].delay(0);
            Aenv[i].attack(0);
            Aenv[i].hold(0);
            Aenv[i].decay(0);
            Aenv[i].sustain(1);
            Aenv[i].release(200);

            mixer.gain(i, 1);
        }
        MixGain.gain(1);
        SongVol.gain(1);
    }
    virtual void noteOn(byte notePlayed, float velocity, byte voice) override
    {
        // plugins own stuff
        float frequency = note_frequency[notePlayed] * tuning;
        waveform[voice].frequency(frequency);
        Fenv[voice].noteOn();
        Aenv[voice].noteOn();
    }
    virtual void noteOff(byte voice) override
    {
        // plugins own stuff
        Fenv[voice].noteOff();
        Aenv[voice].noteOff();
    }
    virtual void draw_plugin() override
    {
        // plugins own stuff
        if (change_plugin_row)
        {
            change_plugin_row = false;
            clearWorkSpace();
            // Serial.println("drawing plugin 2");
            drawPot(0, 0, potentiometer[presetNr][0], "W~Form");
            drawPot(1, 0, potentiometer[presetNr][1], "Volume");

            drawPot(0, 2, potentiometer[presetNr][8], "Filt-Frq");
            drawPot(1, 2, potentiometer[presetNr][9], "Resonance");
            drawPot(2, 2, potentiometer[presetNr][10], "Sweep");
            drawPot(3, 2, potentiometer[presetNr][11], "Type");

            drawPot(0, 3, potentiometer[presetNr][12], "Attack");
            drawPot(1, 3, potentiometer[presetNr][13], "Decay");
            drawPot(2, 3, potentiometer[presetNr][14], "Sustain");
            drawPot(3, 3, potentiometer[presetNr][15], "Release");

            draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
        }
    }
    virtual void set_parameters(byte row) override
    {
        // plugins own stuff
        if (buttonPressed[BUTTON_SHIFT])
        {
            set_presetNr();
        }
    }

    /*
      //AT THE MOMENT:
      //Every plugin has its own function with about 10-15 lines code where only 1-3lines are different
      void set_voice_waveform(byte XPos, byte YPos, const char *name, int min, int max) {
        if (enc_moved[XPos]) {
          int n = XPos + (YPos * NUM_ENCODERS);                                                                  //calculating the desired Pot from x + y
          potentiometer[presetNr][n] = constrain(potentiometer[presetNr][n] + encoded[XPos], 0, MIDI_CC_RANGE);  //adding +-1 from encoderd[Xpos] to the "old Potentiomer value"
          int walveform = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);                           //map the potentiometervalue within the desired range
          drawPot(XPos, YPos, encByte, name);                                                                    //draw the potentiometer to the display
          //last lines are independent for all plugins audio modules
          for (int i = 0; i < MAX_VOICES; i++) {
            waveform[i].begin(walveform);
          }
        }
      }
    */
    // MY VISION:
    void set_voice_waveform(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int walveform = map(get_potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, min, max); // map the potentiometervalue within the desired range
            for (int i = 0; i < MAX_VOICES; i++)
            {
                waveform[i].begin(walveform);
            }
        }
    }
};