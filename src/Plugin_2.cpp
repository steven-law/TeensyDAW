#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include "global_stuff.h"
#include <Plugin_2.h>

extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();
extern const char *filterName[4];

void Plugin_2::setup()
{

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
        ladder[i].frequency(2000);
        ladder[i].resonance(1);
        ladder[i].octaveControl(4);

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
void Plugin_2::noteOn(byte notePlayed, float velocity, byte voice)
{
    float frequency = note_frequency[notePlayed] * tuning;
    waveform[voice].frequency(frequency);
    Fenv[voice].noteOn();
    Aenv[voice].noteOn();
}
void Plugin_2::noteOff(byte notePlayed, byte voice)
{
    Fenv[voice].noteOff();
    Aenv[voice].noteOff();
}
void Plugin_2::set_parameters(byte row)
{
    draw_plugin();
    if (!buttonPressed[BUTTON_SHIFT])
    {
        if (row == 0)
        {
            set_voice_waveform(0, 0, "W~Form");
            set_voice_amplitude(1, 0, "Volume");
        }

        if (row == 1)
        {
        }

        if (row == 2)
        {
            set_filter_frequency(0, 2, "Filt-Frq");
            set_filter_resonance(1, 2, "Resonance", 0, 5.00);
            set_filter_sweep(2, 2, "Sweep");
            set_filter_type(3, 2, filterName[potentiometer[presetNr][11]]);
        }

        if (row == 3)
        {
            set_envelope_ADSR(3, 1000, 500, 2000);
            // set_envelope_attack(0, 3, "Attack", 0, 1000);
            // set_envelope_sustain(2, 3, "Sustain");
            // set_envelope_decay(1, 3, "Decay", 0, 500);
            // set_envelope_release(3, 3, "Release", 0, 2000);
        }
    }
    if (buttonPressed[BUTTON_SHIFT])
    {
        set_presetNr();
    }
}
void Plugin_2::draw_plugin()
{
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
        drawPot(3, 2, potentiometer[presetNr][11], filterName[potentiometer[presetNr][11]]);

        drawEnvelope(3, potentiometer[presetNr][12], potentiometer[presetNr][13],
                     potentiometer[presetNr][14], potentiometer[presetNr][15]);
        // drawPot(0, 3, potentiometer[presetNr][12], "Attack");
        // drawPot(1, 3, potentiometer[presetNr][13], "Decay");
        // drawPot(2, 3, potentiometer[presetNr][14], "Sustain");
        // drawPot(3, 3, potentiometer[presetNr][15], "Release");

        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
    }
}

void Plugin_2::set_voice_waveform(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_voice_waveform(get_Potentiometer(XPos, YPos, name));
    }
}
void Plugin_2::assign_voice_waveform(byte value)
{
    int walveform = map(value, 0, MIDI_CC_RANGE, 0, 12);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        waveform[i].begin(walveform);
    }
}

void Plugin_2::set_voice_amplitude(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_voice_amplitude(get_Potentiometer(XPos, YPos, name));
    }
}
void Plugin_2::assign_voice_amplitude(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        waveform[i].amplitude(ampl);
    }
}

void Plugin_2::set_filter_frequency(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_filter_frequency(get_Potentiometer(XPos, YPos, name));
    }
}
void Plugin_2::assign_filter_frequency(byte value)
{

    int frequency = note_frequency[value] * tuning;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        filter[i].frequency(frequency);
        ladder[i].frequency(frequency);
    }
}

void Plugin_2::set_filter_resonance(byte XPos, byte YPos, const char *name, float min, float max)
{
    if (enc_moved[XPos])
    {
        assign_filter_resonance(get_Potentiometer(XPos, YPos, name));
    }
}
void Plugin_2::assign_filter_resonance(byte value)
{

    float reso = value / 25.40;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        filter[i].resonance(reso);
        ladder[i].resonance(reso);
    }
}
void Plugin_2::set_filter_sweep(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_filter_sweep(get_Potentiometer(XPos, YPos, name));
    }
}
void Plugin_2::assign_filter_sweep(byte value)
{
    float swp = value / 18.14;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        filter[i].octaveControl(swp);
        ladder[i].octaveControl(swp);
    }
}
void Plugin_2::set_filter_type(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        selectFilterType(constrain(get_Potentiometer(XPos, YPos, name), 0, 3));
    }
}
void Plugin_2::selectFilterType(byte mixerchannel)
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

void Plugin_2::assign_envelope_attack(byte value, int max)
{
    int attack = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        Fenv[i].attack(attack);
        Aenv[i].attack(attack);
    }
}
void Plugin_2::assign_envelope_decay(byte value, int max)
{
    int decay = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        Fenv[i].decay(decay);
        Aenv[i].decay(decay);
    }
}
void Plugin_2::assign_envelope_sustain(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    for (int i = 0; i < MAX_VOICES; i++)
    {
        Fenv[i].sustain(ampl);
        Aenv[i].sustain(ampl);
    }
}
void Plugin_2::assign_envelope_release(byte value, int max)
{
    int release = map(value, 0, MIDI_CC_RANGE, 0, max);
    for (int i = 0; i < MAX_VOICES; i++)
    {
        Fenv[i].release(release);
        Aenv[i].release(release);
    }
}
void Plugin_2::set_envelope_ADSR(byte YPos, int maxA, int maxD, int maxR)
{

    if (enc_moved[0])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][0 + rowIx] = constrain(potentiometer[presetNr][0 + rowIx] + encoded[0], 0, MIDI_CC_RANGE);
        assign_envelope_attack(potentiometer[presetNr][0 + rowIx], maxA);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[1])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][1 + rowIx] = constrain(potentiometer[presetNr][1 + rowIx] + encoded[1], 0, MIDI_CC_RANGE);
        assign_envelope_decay(potentiometer[presetNr][1 + rowIx], maxD);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[2])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][2 + rowIx] = constrain(potentiometer[presetNr][2 + rowIx] + encoded[2], 0, MIDI_CC_RANGE);
        assign_envelope_sustain(potentiometer[presetNr][2 + rowIx]);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
    if (enc_moved[3])
    {
        byte rowIx = YPos * 4;
        potentiometer[presetNr][3 + rowIx] = constrain(potentiometer[presetNr][3 + rowIx] + encoded[3], 0, MIDI_CC_RANGE);
        assign_envelope_release(potentiometer[presetNr][3 + rowIx], maxR);
        drawEnvelope(YPos, potentiometer[presetNr][0 + rowIx], potentiometer[presetNr][1 + rowIx],
                     potentiometer[presetNr][2 + rowIx], potentiometer[presetNr][3 + rowIx]);
    }
}
void Plugin_2::set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int attack = map(get_Potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            Fenv[i].attack(attack);
            Aenv[i].attack(attack);
        }
    }
}
void Plugin_2::set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {

        int decay = map(get_Potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            Fenv[i].decay(decay);
            Aenv[i].decay(decay);
        }
    }
}
void Plugin_2::set_envelope_sustain(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {

        float sustain = (float)(get_Potentiometer(XPos, YPos, name) / MIDI_CC_RANGE_FLOAT);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            Fenv[i].sustain(sustain);
            Aenv[i].sustain(sustain);
        }
    }
}
void Plugin_2::set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        int release = map(get_Potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, min, max);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            Fenv[i].release(release);
            Aenv[i].release(release);
        }
    }
}