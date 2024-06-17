
#include <Arduino.h>
#include <Audio.h>
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

void Plugin_3::setup()
{

    for (int i = 0; i < MAX_VOICES; i++)
    {

        modulator[i].begin(WAVEFORM_SINE);
        modulator[i].frequency(440);
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
    Serial.println("Setup Pl3 Done");
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
            set_voice1_waveform(0, 0, "mW~Form");
            set_mod_ratio(1, 0, "mRatio");
            set_voice1_amplitude(2, 0, "mVolume");
            set_voice2_waveform(3, 0, "cW~Form");
        }

        if (row == 1)
        {
            set_envelope1_attack(0, 1, "mAttack", 1000);
            set_envelope1_decay(1, 1, "mDecay", 500);
            set_envelope1_sustain(2, 1, "mSustain");
            set_envelope1_release(3, 1, "mRelease", 2000);
        }

        if (row == 2)
        {
            set_envelope2_ADSR(2, 1000, 500, 2000);
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

        drawEnvelope(2, potentiometer[presetNr][8], potentiometer[presetNr][9],
                     potentiometer[presetNr][10], potentiometer[presetNr][11]);

        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
    }
}

void Plugin_3::get_peak()
{
    Serial.printf("Pl3: %f  ", peak.read());
}

void Plugin_3::assign_voice1_waveform(byte value)
{
    int walveform = map(value, 0, MIDI_CC_RANGE, 0, 12);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        modulator[i].begin(walveform);
    }
}
void Plugin_3::assign_voice1_amplitude(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        modulator[i].amplitude(ampl);
    }
}
void Plugin_3::set_mod_ratio(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        int rationem = map(get_Potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, 0, NUM_RATIOS);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            modulator_ratio = ratios[rationem];
        }
    }
}
void Plugin_3::assign_voice2_waveform(byte value)
{
    int walveform = map(value, 0, MIDI_CC_RANGE, 0, 12);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        carrier[i].begin(walveform);
    }
}

void Plugin_3::assign_envelope1_attack(byte value, int max)
{
    int attack = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        modEnv[i].attack(attack);
    }
}
void Plugin_3::assign_envelope1_decay(byte value, int max)
{
    int decay = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        modEnv[i].decay(decay);
    }
}
void Plugin_3::assign_envelope1_sustain(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        modEnv[i].sustain(ampl);
    }
}
void Plugin_3::assign_envelope1_release(byte value, int max)
{
    int release = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        modEnv[i].release(release);
    }
}

void Plugin_3::assign_envelope2_attack(byte value, int max)
{
    int attack = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        outEnv[i].attack(attack);
    }
}
void Plugin_3::assign_envelope2_decay(byte value, int max)
{
    int decay = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        outEnv[i].decay(decay);
    }
}
void Plugin_3::assign_envelope2_sustain(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        outEnv[i].sustain(ampl);
    }
}
void Plugin_3::assign_envelope2_release(byte value, int max)
{
    int release = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        outEnv[i].release(release);
    }
}

void Plugin_3::set_envelope2_ADSR(byte YPos, int maxA, int maxD, int maxR)
{

    if (enc_moved[0])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][0 + rowIx] = constrain(potentiometer[presetNr][0 + rowIx] + encoded[0], 0, MIDI_CC_RANGE);
        assign_envelope2_attack(potentiometer[presetNr][0 + rowIx], maxA);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[1])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][1 + rowIx] = constrain(potentiometer[presetNr][1 + rowIx] + encoded[1], 0, MIDI_CC_RANGE);
        assign_envelope2_decay(potentiometer[presetNr][1 + rowIx], maxD);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[2])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][2 + rowIx] = constrain(potentiometer[presetNr][2 + rowIx] + encoded[2], 0, MIDI_CC_RANGE);
        assign_envelope2_sustain(potentiometer[presetNr][2 + rowIx]);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[3])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][3 + rowIx] = constrain(potentiometer[presetNr][3 + rowIx] + encoded[3], 0, MIDI_CC_RANGE);
        assign_envelope2_release(potentiometer[presetNr][3 + rowIx], maxR);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
}
// TeensyDAW: end automatically generated code