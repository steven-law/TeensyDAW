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

        fmixer[i].gain(0, 1);
        fmixer[i].gain(1, 0);
        fmixer[i].gain(2, 0);
        fmixer[i].gain(3, 0);

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
            set_voice_waveform(0, 0, "W~Form");
            set_voice_detune(1, 0, "Detune");
            set_voice_amplitude(2, 0, "Volume 1");
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
            set_filter_resonance(1, 2, "Resonance", 0, 5.00);
            set_filter_sweep(2, 2, "Sweep", 0, 7.00);
            set_filter_type(3, 2, "Type");
        }

        if (row == 3)
        {
            set_envelope_attack(0, 3, "Attack", 0, 1000);
            set_envelope_decay(1, 3, "Decay", 0, 500);
            set_envelope_sustain(2, 3, "Sustain");
            set_envelope_release(3, 3, "Release", 0, 2000);
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

        drawPot(0, 3, potentiometer[presetNr][12], "Attack");
        drawPot(1, 3, potentiometer[presetNr][13], "Decay");
        drawPot(2, 3, potentiometer[presetNr][14], "Sustain");
        drawPot(3, 3, potentiometer[presetNr][15], "Release");

        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
    }
}

void Plugin_8::set_voice_waveform(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        int walveform = map(get_Potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, 0, 12);

        for (int i = 0; i < MAX_VOICES; i++)
        {
            waveform[i].begin(walveform);
        }
    }
}
void Plugin_8::set_voice_amplitude(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        float ampl = get_Potentiometer(XPos, YPos, name) / MIDI_CC_RANGE_FLOAT;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            waveform[i].amplitude(ampl);
        }
    }
}
void Plugin_8::set_voice_detune(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        detune1 = (float)((note_frequency[get_Potentiometer(XPos, YPos, name)] * 0.01));
    }
}
void Plugin_8::set_voice1_waveform(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        int walveform = map(get_Potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, 0, 12);
        for (int i = 0; i < MAX_VOICES; i++)
        {
            waveform1[i].begin(walveform);
        }
    }
}
void Plugin_8::set_voice1_amplitude(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        float ampl = get_Potentiometer(XPos, YPos, name) / MIDI_CC_RANGE_FLOAT;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            waveform1[i].amplitude(ampl);
        }
    }
}
void Plugin_8::set_voice1_detune(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        detune2 = (float)((note_frequency[get_Potentiometer(XPos, YPos, name)] * 0.01));
    }
}

void Plugin_8::set_filter_frequency(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        int frequency = note_frequency[get_Potentiometer(XPos, YPos, name)] * tuning;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            filter[i].frequency(frequency);
        }
    }
}
void Plugin_8::set_filter_resonance(byte XPos, byte YPos, const char *name, float min, float max)
{
    if (enc_moved[XPos])
    {
        float reso = get_Potentiometer(XPos, YPos, name) / (MIDI_CC_RANGE_FLOAT / max) + min;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            filter[i].resonance(reso);
        }
    }
}
void Plugin_8::set_filter_sweep(byte XPos, byte YPos, const char *name, float min, float max)
{
    if (enc_moved[XPos])
    {
        float swp = get_Potentiometer(XPos, YPos, name) / (MIDI_CC_RANGE_FLOAT / max) + min;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            filter[i].octaveControl(swp);
        }
    }
}
void Plugin_8::set_filter_type(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        selectFilterType(map(get_Potentiometer(XPos, YPos, name), 0, MIDI_CC_RANGE, 0, 2));
    }
}
void Plugin_8::selectFilterType(byte mixerchannel)
{
    for (int i = 0; i < MAX_VOICES; i++)
    {
        fmixer[i].gain(0, 0);
        fmixer[i].gain(1, 0);
        fmixer[i].gain(2, 0);
        fmixer[i].gain(mixerchannel, 1);
    }
}

void Plugin_8::set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max)
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
void Plugin_8::set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max)
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
void Plugin_8::set_envelope_sustain(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        float sustain = get_Potentiometer(XPos, YPos, name) / MIDI_CC_RANGE_FLOAT;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            Fenv[i].sustain(sustain);
            Aenv[i].sustain(sustain);
        }
    }
}
void Plugin_8::set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max)
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