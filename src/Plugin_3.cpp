
#include <Arduino.h>
//#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include "global_stuff.h"
#include <Plugin_3.h>

extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();

void Plugin_3::setup(byte setID)
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
void Plugin_3::noteOn(byte notePlayed, float velocity, byte voice)
{
    Serial.println("2FM-NoteOn");
    float carrier_frequency = note_frequency[notePlayed] * tuning;
    float modulator_frequency = carrier_frequency * modulator_ratio;
    carrier[voice].frequency(carrier_frequency);
    modulator[voice].frequency(modulator_frequency);
    modEnv[voice].noteOn();
    outEnv[voice].noteOn();
}
void Plugin_3::noteOff(byte notePlayed, byte voice)
{
    modEnv[voice].noteOff();
    outEnv[voice].noteOff();
}
void Plugin_3::set_parameters(byte row)
{
    draw_plugin();
    if (!buttonPressed[BUTTON_SHIFT])
    {
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
    if (buttonPressed[BUTTON_SHIFT])
    {
        set_presetNr();
    }
}
void Plugin_3::draw_plugin()
{
    if (change_plugin_row)
    {
        change_plugin_row = false;
        clearWorkSpace();

        drawPot(0, 0, potentiometer[presetNr][0], "mW~Form");
        drawPot(1, 0, potentiometer[presetNr][1], "mRatio");
        drawPot(2, 0, potentiometer[presetNr][2], "mVolume");
        drawPot(3, 0, potentiometer[presetNr][3], "cW~Form");

        // drawPot(0, 2, potentiometer[8], "Filt-Frq");
        // drawPot(1, 2, potentiometer[9], "Resonance");
        // drawPot(2, 2, potentiometer[10], "Sweep");
        // drawPot(3, 2, potentiometer[10], "Type");
        drawPot(0, 1, potentiometer[presetNr][4], "mAttack");
        drawPot(1, 1, potentiometer[presetNr][5], "mDecay");
        drawPot(2, 1, potentiometer[presetNr][6], "mSustain");
        drawPot(3, 1, potentiometer[presetNr][7], "mRelease");

        drawPot(0, 2, potentiometer[presetNr][8], "Attack");
        drawPot(1, 2, potentiometer[presetNr][9], "Decay");
        drawPot(2, 2, potentiometer[presetNr][10], "Sustain");
        drawPot(3, 2, potentiometer[presetNr][11], "Release");

        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
    }
}


void Plugin_3::set_mod_waveform(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int walveform = map(get_Potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            modulator[i].begin(walveform);
        }
    }
}
void Plugin_3::set_mod_amplitude(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            modulator[i].amplitude((float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT));
        }
    }
}
void Plugin_3::set_mod_ratio(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        int rationem = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, 0, NUM_RATIOS);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            modulator_ratio = ratios[rationem];
        }
    }
}

void Plugin_3::set_car_waveform(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        int walveform = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);

        for (int i = 0; i < MAX_VOICES; i++)
        {
            carrier[i].begin(walveform);
        }
    }
}

void Plugin_3::set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        int attack = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            outEnv[i].attack(attack);
        }
    }
}
void Plugin_3::set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        int decay = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            outEnv[i].decay(decay);
        }
    }
}
void Plugin_3::set_envelope_sustain(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        float sustain = (float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            outEnv[i].sustain(sustain);
        }
    }
}
void Plugin_3::set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        int release = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            outEnv[i].release(release);
        }
    }
}

void Plugin_3::set_envelope_mattack(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        int attack = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            modEnv[i].attack(attack);
        }
    }
}
void Plugin_3::set_envelope_mdecay(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        int decay = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            modEnv[i].decay(decay);
        }
    }
}
void Plugin_3::set_envelope_msustain(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        float sustain = (float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            modEnv[i].sustain(sustain);
        }
    }
}
void Plugin_3::set_envelope_mrelease(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
        int release = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            modEnv[i].release(release);
        }
    }
}



// TeensyDAW: end automatically generated code
