#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include "global_stuff.h"
#include <Plugin_8.h>

extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();

void Plugin_8::setup()
{

    for (int i = 0; i < MAX_VOICES; i++)
    {
        dc[i].amplitude(1);

        waveform[i].begin(WAVEFORM_SINE);
        waveform[i].amplitude(1);
        waveform1[i].begin(WAVEFORM_SINE);
        waveform1[i].amplitude(1);

        vcoMixer[i].gain(0, 1);
        vcoMixer[i].gain(1, 1);

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
void Plugin_8::noteOn(byte notePlayed, float velocity, byte voice)
{
    waveform[voice].frequency((note_frequency[notePlayed] + detune1) * tuning);
    waveform1[voice].frequency((note_frequency[notePlayed] + detune2) * tuning);
    Serial.println(detune1);
    Fenv[voice].noteOn();
    Aenv[voice].noteOn();
}
void Plugin_8::noteOff(byte notePlayed, byte voice)
{
    Fenv[voice].noteOff();
    Aenv[voice].noteOff();
}

void Plugin_8::set_parameters(byte row)
{
    draw_plugin();
    if (!buttonPressed[BUTTON_SHIFT])
    {
        if (row == 0)
        {
            set_voice1_waveform(0, 0, "W~Form");
            set_voice1_detune(1, 0, "Detune");
            set_voice1_amplitude(2, 0, "Volume 1");
        }

        if (row == 1)
        {
            set_voice1_waveform(0, 1, "W~Form");
            set_voice1_detune(1, 1, "Detune");
            set_voice1_amplitude(2, 1, "Volume 2");
        }

        if (row == 2)
        {
            set_filter_frequency(0, 2, "Filt-Frq");
            set_filter_resonance(1, 2, "Resonance");
            set_filter_sweep(2, 2, "Sweep");
            set_filter_type(3, 2, "Type");
        }

        if (row == 3)
        {
            set_envelope1_ADSR(3, 1000, 500, 2000);
        }
    }
    if (buttonPressed[BUTTON_SHIFT])
    {
        set_presetNr();
    }
}
void Plugin_8::draw_plugin()
{
    if (change_plugin_row)
    {
        change_plugin_row = false;
        clearWorkSpace();
        // Serial.println("drawing plugin 2");
        drawPot(0, 0, potentiometer[presetNr][0], "W~Form");
        drawPot(1, 0, potentiometer[presetNr][1], "Detune");
        drawPot(2, 0, potentiometer[presetNr][2], "Volume 1");

        drawPot(0, 1, potentiometer[presetNr][4], "W~Form");
        drawPot(1, 1, potentiometer[presetNr][5], "Detune");
        drawPot(2, 1, potentiometer[presetNr][6], "Volume 2");

        drawPot(0, 2, potentiometer[presetNr][8], "Filt-Frq");
        drawPot(1, 2, potentiometer[presetNr][9], "Resonance");
        drawPot(2, 2, potentiometer[presetNr][10], "Sweep");
        drawPot(3, 2, potentiometer[presetNr][11], "Type");

        drawEnvelope(3, potentiometer[presetNr][12], potentiometer[presetNr][13],
                     potentiometer[presetNr][14], potentiometer[presetNr][15]);

        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
    }
}

void Plugin_8::assign_voice1_waveform(byte value)
{
    int walveform = map(value, 0, MIDI_CC_RANGE, 0, 12);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        waveform[i].begin(walveform);
    }
}
void Plugin_8::assign_voice1_amplitude(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        waveform[i].amplitude(ampl);
    }
}
void Plugin_8::set_voice1_detune(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        detune1 = (float)((note_frequency[get_Potentiometer(XPos, YPos, name)] * 0.01));
    }
}
void Plugin_8::assign_voice2_waveform(byte value)
{
    int walveform = map(value, 0, MIDI_CC_RANGE, 0, 12);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        waveform1[i].begin(walveform);
    }
}
void Plugin_8::assign_voice2_amplitude(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        waveform1[i].amplitude(ampl);
    }
}
void Plugin_8::set_voice2_detune(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        detune2 = (float)((note_frequency[get_Potentiometer(XPos, YPos, name)] * 0.01));
    }
}

void Plugin_8::assign_filter_frequency(byte value)
{

    int frequency = note_frequency[value] * tuning;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        filter[i].frequency(frequency);
        ladder[i].frequency(frequency);
    }
}
void Plugin_8::assign_filter_resonance(byte value)
{

    float reso = value / 25.40;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        filter[i].resonance(reso);
        ladder[i].resonance(reso);
    }
}
void Plugin_8::assign_filter_sweep(byte value)
{
    float swp = value / 18.14;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        filter[i].octaveControl(swp);
        ladder[i].octaveControl(swp);
    }
}
void Plugin_8::assign_filter_type(byte mixerchannel)
{
    for (int i = 0; i < MAX_VOICES; i++)
    {
        fMixer[i].gain(0, 0);
        fMixer[i].gain(1, 0);
        fMixer[i].gain(2, 0);
        fMixer[i].gain(3, 0);
        fMixer[i].gain(mixerchannel, 1);
    }
}

void Plugin_8::assign_envelope1_attack(byte value, int max)
{
    int attack = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        Fenv[i].attack(attack);
        Aenv[i].attack(attack);
    }
}
void Plugin_8::assign_envelope1_decay(byte value, int max)
{
    int decay = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        Fenv[i].decay(decay);
        Aenv[i].decay(decay);
    }
}
void Plugin_8::assign_envelope1_sustain(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        Fenv[i].sustain(ampl);
        Aenv[i].sustain(ampl);
    }
}
void Plugin_8::assign_envelope1_release(byte value, int max)
{
    int release = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        Fenv[i].release(release);
        Aenv[i].release(release);
    }
}
void Plugin_8::set_envelope1_ADSR(byte YPos, int maxA, int maxD, int maxR)
{

    if (enc_moved[0])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][0 + rowIx] = constrain(potentiometer[presetNr][0 + rowIx] + encoded[0], 0, MIDI_CC_RANGE);
        assign_envelope1_attack(potentiometer[presetNr][0 + rowIx], maxA);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[1])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][1 + rowIx] = constrain(potentiometer[presetNr][1 + rowIx] + encoded[1], 0, MIDI_CC_RANGE);
        assign_envelope1_decay(potentiometer[presetNr][1 + rowIx], maxD);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[2])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][2 + rowIx] = constrain(potentiometer[presetNr][2 + rowIx] + encoded[2], 0, MIDI_CC_RANGE);
        assign_envelope1_sustain(potentiometer[presetNr][2 + rowIx]);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[3])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][3 + rowIx] = constrain(potentiometer[presetNr][3 + rowIx] + encoded[3], 0, MIDI_CC_RANGE);
        assign_envelope1_release(potentiometer[presetNr][3 + rowIx], maxR);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
}