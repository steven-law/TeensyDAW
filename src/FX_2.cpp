
#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include "global_stuff.h"
#include <FX_2.h>

void FX_2::setup()
{
    FX_mixer.gain(0, 1);
    FX_mixer.gain(1, 1);
    FX_mixer.gain(2, 1);
    FX_mixer.gain(3, 1);
    FX_mixer.gain(4, 1);
}
void FX_2::noteOn(byte notePlayed, float velocity, byte voice) {}
void FX_2::noteOff(byte notePlayed, byte voice) {}
void FX_2::set_parameters(byte row)
{
    draw_plugin();
    if (row == 0)
    {
        set_BC_bits(0, 0, "BitDepth");
        set_BC_smplRate(1, 0, "SmpRate");
    }
}
void FX_2::draw_plugin()
{
    if (change_plugin_row)
    {
        change_plugin_row = false;
        drawPot(0, 0, potentiometer[presetNr][0], "BitDepth");
        drawPot(1, 0, potentiometer[presetNr][1], "SmpRate");
    }
}

void FX_2::set_BC_bits(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        potentiometer[presetNr][XPos] = constrain(potentiometer[presetNr][XPos] + encoded[XPos], 0, MIDI_CC_RANGE);
        bitcrusher.bits(map(potentiometer[presetNr][XPos], 0, 127, 1, 16));

        drawPot(XPos, YPos, potentiometer[presetNr][XPos], name);
    }
}
void FX_2::set_BC_smplRate(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        potentiometer[presetNr][XPos] = constrain(potentiometer[presetNr][XPos] + encoded[XPos], 0, MIDI_CC_RANGE);
        bitcrusher.sampleRate(map(potentiometer[presetNr][XPos], 0, 127, 1, 44100));

        drawPot(XPos, YPos, potentiometer[presetNr][XPos], name);
    }
}