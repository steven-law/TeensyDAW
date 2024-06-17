#ifndef PLUGIN_5_H
#define PLUGIN_5_H

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include <pluginClass.h>
#include <synth_fm_drum.h>

// TeensyDAW: begin automatically generated code
// Name: Drum
// Description: Synthesize 12 Drum sounds
// Voices: 12

// Kick
// Pot 1: Freq
// Pot 2: Sweep
// Pot 3: O-Drive
// Pot 4: Decay

// Snare
// Pot 5: Freq
// Pot 6: Sweep
// Pot 7: Noise
// Pot 8: Decay

// Hihat
// Pot 9: Freq
// Pot 10: Reso
// Pot 11: Attack
// Pot 12: Decay

// Toms
// Pot 13: TomL
// Pot 14: TomM
// Pot 15: TomH
// Pot 16: Decay
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();
class Plugin_5 : public PluginControll
{
public:
    AudioSynthWaveformDc dc;
    AudioSynthNoisePink pink;
    AudioEffectEnvelope hhFilterEnv;
    AudioFilterStateVariable filter;
    AudioSynthFMDrum fm_drum;
    AudioSynthFMDrum fm_snare;
    AudioEffectEnvelope hhEnv;
    AudioSynthSimpleDrum tomL;
    AudioSynthSimpleDrum tomM;
    AudioSynthSimpleDrum tomH;
    AudioMixer12 mixer;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[12]; // total patchCordCount:12 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_5(const char *Name, byte ID) : PluginControll(Name, ID)
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(dc, 0, hhFilterEnv, 0);
        patchCord[pci++] = new AudioConnection(pink, 0, filter, 0);
        patchCord[pci++] = new AudioConnection(hhFilterEnv, 0, filter, 1);
        patchCord[pci++] = new AudioConnection(filter, 2, hhEnv, 0);
        patchCord[pci++] = new AudioConnection(fm_drum, 0, mixer, 0);
        patchCord[pci++] = new AudioConnection(fm_snare, 0, mixer, 1);
        patchCord[pci++] = new AudioConnection(hhEnv, 0, mixer, 2);
        patchCord[pci++] = new AudioConnection(tomL, 0, mixer, 3);
        patchCord[pci++] = new AudioConnection(tomM, 0, mixer, 4);
        patchCord[pci++] = new AudioConnection(tomH, 0, mixer, 5);
        patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
    }
    virtual ~Plugin_5() = default;

    virtual void setup() override;
    virtual void noteOn(byte notePlayed, float velocity, byte voice) override;
    virtual void noteOff(byte notePlayed, byte voice) override;
    virtual void set_parameters(byte row) override;
    virtual void draw_plugin() override;

    virtual void assign_fmdrum1_frequency(byte value) override;
    virtual void assign_fmdrum1_pitchMod(byte value) override;
    virtual void assign_fmdrum1_decay(byte value) override;
    virtual void assign_fmdrum1_noise(byte value) override;
    virtual void assign_fmdrum1_overdrive(byte value) override;

    virtual void assign_fmdrum2_frequency(byte value) override;
    virtual void assign_fmdrum2_pitchMod(byte value) override;
    virtual void assign_fmdrum2_decay(byte value) override;
    virtual void assign_fmdrum2_noise(byte value) override;
    virtual void assign_fmdrum2_overdrive(byte value) override;

    virtual void assign_filter_frequency(byte value) override;
    virtual void assign_filter_resonance(byte value) override;
    virtual void assign_filter_sweep(byte value) override;

    virtual void assign_envelope1_attack(byte value, int max) override; //envelope for modulator
    virtual void assign_envelope1_decay(byte value, int max) override;
    virtual void assign_envelope1_sustain(byte value) override;
    virtual void assign_envelope1_release(byte value, int max) override;

    virtual void assign_drum1_frequency(byte value) override;
    virtual void assign_drum2_frequency(byte value) override;
    virtual void assign_drum3_frequency(byte value) override;
    virtual void assign_drum1_decay(byte value) override;


};
#endif // PLUGIN_5_H