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
/*
M WF    M Ratio   M Vol    C WF
M Atk   M Dec     M St     M Rl
C At    C Dc      C St     C Rl
*/
// TeensyDAW: begin automatically generated code
//Name: 2FM
//Description: Simple FM Snthesis Modulator -> Carrier
//Voices: 12

//Operator
//Pot 1: mWaveform
//Pot 2: mRatio
//Pot 3: mVolume
//Pot 4: cWaveform

//Mod Envelope
//Pot 5: mAttack
//Pot 6: mDecay
//Pot 7: mSustain
//Pot 8: mRelease

//Envelope:
//Pot 9: Attack
//Pot 10: Decay
//Pot 11: Sustain
//Pot 12: Release
class Plugin_3
{
public:
#define NUM_RATIOS 10
    const float ratios[NUM_RATIOS] = {0.125, 0.25, 0.5, 0.75, 1, 2, 3, 4, 6, 8};
    float modulator_ratio = 1;
    byte myID;
    byte potentiometer[16];
    byte presetNr = 0;
    AudioSynthWaveformModulated     modulator[12];
    AudioEffectEnvelope             modEnv[12];
    AudioSynthWaveformModulated     carrier[12];
    AudioEffectEnvelope             outEnv[12];
    AudioMixer12                    mixer;
    AudioAmplifier                  MixGain;
    AudioAmplifier                  SongVol;
    AudioConnection                  *patchCord[50]; // total patchCordCount:50 including array typed ones.


    // constructor (this is called when class-object is created)
    Plugin_3()
    {
        int pci = 0; // used only for adding new patchcords

         patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
        for (int i = 0; i < 12; i++) {
            patchCord[pci++] = new AudioConnection(modulator[i], 0, modEnv[i], 0);
            patchCord[pci++] = new AudioConnection(modEnv[i], 0, carrier[i], 0);
            patchCord[pci++] = new AudioConnection(carrier[i], 0, outEnv[i], 0);
            patchCord[pci++] = new AudioConnection(outEnv[i], 0, mixer, i);
        }
    }
    void setup(byte setID)
    {
        myID = setID;

        for (int i = 0; i < MAX_VOICES; i++)
        {

            modulator[i].begin(WAVEFORM_SINE);
            modulator[i].amplitude(1);

            modEnv[i].delay(0);
            modEnv[i].attack(0);
            modEnv[i].hold(0);
            modEnv[i].decay(0);
            modEnv[i].sustain(1);
            modEnv[i].release(200);

            carrier[i].begin(WAVEFORM_SINE);
            carrier[i].amplitude(1);

            outEnv[i].delay(0);
            outEnv[i].attack(0);
            outEnv[i].hold(0);
            outEnv[i].decay(0);
            outEnv[i].sustain(1);
            outEnv[i].release(200);

            mixer.gain(i, 1);
        }
        MixGain.gain(1);
        SongVol.gain(1);
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        float carrier_frequency = note_frequency[notePlayed] * tuning;
        float modulator_frequency = carrier_frequency * modulator_ratio;
        carrier[voice].frequency(carrier_frequency);
        modulator[voice].frequency(modulator_frequency);
        modEnv[voice].noteOn();
        outEnv[voice].noteOn();
    }
    void noteOff(byte voice)
    {
        modEnv[voice].noteOff();
        outEnv[voice].noteOff();
    }

    void set_parameters(byte row)
    {
        draw_plugin();
        if (row == 0)
        {
            set_mod_waveform(0, 0, "mW~Form", 0, 12);
            set_mod_ratio(1, 0, "mRatio", 0, 1);
            set_mod_amplitude(2, 0, "mVolume", 0, 1);
            set_car_waveform(3, 0, "cW~Form", 0, 12);
        }

        if (row == 1)
        {
            set_envelope_mattack(0, 1, "mAttack", 0, 1000);
            set_envelope_mdecay(1, 1, "mDecay", 0, 500);
            set_envelope_msustain(2, 1, "mSustain", 0, 1);
            set_envelope_mrelease(3, 1, "mRelease", 0, 2000);
        }

        if (row == 2)
        {
            set_envelope_attack(0, 2, "Attack", 0, 1000);
            set_envelope_decay(1, 2, "Decay", 0, 500);
            set_envelope_sustain(2, 2, "Sustain", 0, 1);
            set_envelope_release(3, 2, "Release", 0, 2000);
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

            drawPot(0, 0, potentiometer[0], "mW~Form");
            drawPot(1, 0, potentiometer[1], "mRatio");
            drawPot(2, 0, potentiometer[2], "mVolume");
            drawPot(3, 0, potentiometer[3], "cW~Form");

            // drawPot(0, 2, potentiometer[8], "Filt-Frq");
            // drawPot(1, 2, potentiometer[9], "Resonance");
            // drawPot(2, 2, potentiometer[10], "Sweep");
            // drawPot(3, 2, potentiometer[10], "Type");
            drawPot(0, 1, potentiometer[4], "mAttack");
            drawPot(1, 1, potentiometer[5], "mDecay");
            drawPot(2, 1, potentiometer[6], "mSustain");
            drawPot(3, 1, potentiometer[7], "mRelease");

            drawPot(0, 2, potentiometer[8], "Attack");
            drawPot(1, 2, potentiometer[9], "Decay");
            drawPot(2, 2, potentiometer[10], "Sustain");
            drawPot(3, 2, potentiometer[11], "Release");

            draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3,0);
        }
    }

    void set_mod_waveform(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int walveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);

            for (int i = 0; i < MAX_VOICES; i++)
            {
                modulator[i].begin(walveform);
            }
        }
    }
    void set_mod_amplitude(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                modulator[i].amplitude((float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT));
            }
        }
    }
    void set_mod_ratio(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int rationem = map(potentiometer[n], 0, MIDI_CC_RANGE, 0, NUM_RATIOS);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                modulator_ratio = ratios[rationem];
            }
        }
    }

    void set_car_waveform(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int walveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);

            for (int i = 0; i < MAX_VOICES; i++)
            {
                carrier[i].begin(walveform);
            }
        }
    }

    void set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int attack = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                outEnv[i].attack(attack);
            }
        }
    }
    void set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int decay = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                outEnv[i].decay(decay);
            }
        }
    }
    void set_envelope_sustain(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                outEnv[i].sustain(sustain);
            }
        }
    }
    void set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int release = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                outEnv[i].release(release);
            }
        }
    }

    void set_envelope_mattack(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int attack = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                modEnv[i].attack(attack);
            }
        }
    }
    void set_envelope_mdecay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
          potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int decay = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                modEnv[i].decay(decay);
            }
        }
    }
    void set_envelope_msustain(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                modEnv[i].sustain(sustain);
            }
        }
    }
    void set_envelope_mrelease(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = getEncodervalue(XPos, YPos, name, potentiometer[n]);
            int release = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                modEnv[i].release(release);
            }
        }
    }
};
// TeensyDAW: end automatically generated code
