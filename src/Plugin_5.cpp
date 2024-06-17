#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include "global_stuff.h"
#include <Plugin_5.h>

extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();

void Plugin_5::setup()
{

    for (int i = 0; i < MAX_VOICES; i++)
    {
        mixer.gain(i, 1);
    }
    pink.amplitude(1);
    hhEnv.delay(0);
    hhEnv.attack(0);
    hhEnv.hold(0);
    hhEnv.decay(0);
    hhEnv.sustain(0.4);
    hhFilterEnv.sustain(0.4);
    hhEnv.release(200);

    MixGain.gain(1);
    SongVol.gain(1);
    tomL.secondMix(0);
    tomL.pitchMod(0.7);
    tomM.secondMix(0);
    tomM.pitchMod(0.7);
    tomH.secondMix(0);
    tomH.pitchMod(0.7);
}
void Plugin_5::noteOn(byte notePlayed, float velocity, byte voice)
{
    if (notePlayed == 48)
        fm_drum.noteOn();
    if (notePlayed == 49)
        fm_snare.noteOn();
    if (notePlayed == 50)
    {
        hhEnv.noteOn();
        hhFilterEnv.noteOn();
    }
    if (notePlayed == 51)
        tomL.noteOn();
    if (notePlayed == 52)
        tomM.noteOn();
    if (notePlayed == 53)
        tomH.noteOn();
}
void Plugin_5::noteOff(byte notePlayed, byte voice)
{
    if (notePlayed == 50)
    {
        hhEnv.noteOff();
        hhFilterEnv.noteOff();
    }
}

void Plugin_5::set_parameters(byte row)
{
    draw_plugin();
    if (!buttonPressed[BUTTON_SHIFT])
    {
        if (row == 0)
        {
            set_fmdrum1_frequency(0, 0, "Freq");
            set_fmdrum1_pitchMod(1, 0, "Sweep");
            set_fmdrum1_overdrive(2, 0, "O-Drive");
            set_fmdrum1_decay(3, 0, "Decay");
        }

        if (row == 1)
        {
            set_fmdrum2_frequency(0, 1, "Freq");
            set_fmdrum2_pitchMod(1, 1, "Sweep");
            set_fmdrum2_noise(2, 1, "Noise");
            set_fmdrum2_decay(3, 1, "Decay");
        }

        if (row == 2)
        {
            set_envelope1_attack(2, 2, "Attack", 50);
            set_envelope1_release(3, 2, "Decay", 2000);
            set_filter_frequency(0, 2, "Freq");
            set_filter_resonance(1, 2, "Reso");
        }

        if (row == 3)
        {
            set_drum1_frequency(0, 3, "TomL");
            set_drum2_frequency(1, 3, "TomM");
            set_drum3_frequency(2, 3, "TomH");
            set_drum1_decay(3, 3, "Decay");
        }
    }
    if (buttonPressed[BUTTON_SHIFT])
    {
        set_presetNr();
    }
}
void Plugin_5::draw_plugin()
{

    if (change_plugin_row)
    {
        change_plugin_row = false;
        clearWorkSpace();
        // Serial.println("drawing plugin 5");
        drawPot(0, 0, potentiometer[presetNr][0], "Freq");
        drawPot(1, 0, potentiometer[presetNr][1], "Sweep");
        drawPot(2, 0, potentiometer[presetNr][2], "O-Drive");
        drawPot(3, 0, potentiometer[presetNr][3], "Decay");

        drawPot(0, 1, potentiometer[presetNr][4], "Freq");
        drawPot(1, 1, potentiometer[presetNr][5], "Sweep");
        drawPot(2, 1, potentiometer[presetNr][6], "Noise");
        drawPot(3, 1, potentiometer[presetNr][7], "Decay");

        drawPot(0, 2, potentiometer[presetNr][8], "Freq");
        drawPot(1, 2, potentiometer[presetNr][9], "Reso");
        drawPot(2, 2, potentiometer[presetNr][10], "Attack");
        drawPot(3, 2, potentiometer[presetNr][11], "Decay");

        drawPot(0, 3, potentiometer[presetNr][12], "TomL");
        drawPot(1, 3, potentiometer[presetNr][13], "TomM");
        drawPot(2, 3, potentiometer[presetNr][14], "TomH");
        drawPot(3, 3, potentiometer[presetNr][15], "Decay");

        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
    }
}

// kick
void Plugin_5::assign_fmdrum1_frequency(byte value)
{
    int freq = map(value, 0, MIDI_CC_RANGE, 10, 300);
    fm_drum.frequency(freq);
}
void Plugin_5::assign_fmdrum1_pitchMod(byte value)
{
    float sustain = value / MIDI_CC_RANGE_FLOAT;
    fm_drum.fm(sustain);
}
void Plugin_5::assign_fmdrum1_decay(byte value)
{
    float sustain = value / MIDI_CC_RANGE_FLOAT;
    fm_drum.decay(sustain);
}
void Plugin_5::assign_fmdrum1_noise(byte value)
{
    float sustain = value / MIDI_CC_RANGE_FLOAT;
    fm_drum.noise(sustain);
}
void Plugin_5::assign_fmdrum1_overdrive(byte value)
{
    float sustain = value / MIDI_CC_RANGE_FLOAT;
    fm_drum.overdrive(sustain);
}
// snare
void Plugin_5::assign_fmdrum2_frequency(byte value)
{
    int freq = map(value, 0, MIDI_CC_RANGE, 10, 300);
    fm_snare.frequency(freq);
}
void Plugin_5::assign_fmdrum2_pitchMod(byte value)
{
    float sustain = value / MIDI_CC_RANGE_FLOAT;
    fm_snare.fm(sustain);
}
void Plugin_5::assign_fmdrum2_decay(byte value)
{
    float sustain = value / MIDI_CC_RANGE_FLOAT;
    fm_snare.decay(sustain);
}
void Plugin_5::assign_fmdrum2_noise(byte value)
{
    float sustain = value / MIDI_CC_RANGE_FLOAT;
    fm_snare.noise(sustain);
}
void Plugin_5::assign_fmdrum2_overdrive(byte value)
{
    float sustain = value / MIDI_CC_RANGE_FLOAT;
    fm_snare.overdrive(sustain);
}
// hihat
void Plugin_5::assign_envelope1_attack(byte value, int max)
{
    int attack = map(value, 0, MIDI_CC_RANGE, 0, max);
    hhFilterEnv.attack(attack);
    hhEnv.attack(attack);
}
void Plugin_5::assign_envelope1_decay(byte value, int max)
{
    int decay = map(value, 0, MIDI_CC_RANGE, 0, max);
    hhFilterEnv.decay(decay);
    hhEnv.decay(decay);
}
void Plugin_5::assign_envelope1_sustain(byte value)
{
    float ampl = value / MIDI_CC_RANGE_FLOAT;
    hhFilterEnv.sustain(ampl);
    hhEnv.sustain(ampl);
}
void Plugin_5::assign_envelope1_release(byte value, int max)
{
    int release = map(value, 0, MIDI_CC_RANGE, 0, max);
    hhFilterEnv.release(release);
    hhEnv.release(release);
    hhFilterEnv.decay(release / 4);
    hhEnv.decay(release / 4);
}
void Plugin_5::assign_filter_frequency(byte value)
{
    int frequency = note_frequency[value] * tuning;
    filter.frequency(frequency);
}
void Plugin_5::assign_filter_resonance(byte value)
{
    float reso = value / 25.40;
    filter.resonance(reso);
}
void Plugin_5::assign_filter_sweep(byte value)
{
    float swp = value / 18.14;
    filter.octaveControl(swp);
}

// toms
void Plugin_5::assign_drum1_frequency(byte value)
{
    int freq = note_frequency[value] * tuning;
    tomL.frequency(freq);
}
void Plugin_5::assign_drum2_frequency(byte value)
{
    int freq = note_frequency[value] * tuning;
    tomM.frequency(freq);
}
void Plugin_5::assign_drum3_frequency(byte value)
{
    int freq = note_frequency[value] * tuning;
    tomH.frequency(freq);
}
void Plugin_5::assign_drum1_decay(byte value)
{
    int decay = map(value, 0, MIDI_CC_RANGE, 0, 2000);
    tomL.length(decay);
    tomM.length(decay);
    tomH.length(decay);
}
