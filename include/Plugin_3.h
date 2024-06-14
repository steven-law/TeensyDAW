#ifndef PLUGIN_3_H
#define PLUGIN_3_H

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include <pluginClass.h>

/*
M WF    M Ratio   M Vol    C WF
M Atk   M Dec     M St     M Rl
C At    C Dc      C St     C Rl
*/
// TeensyDAW: begin automatically generated code
// Name: 2FM
// Description: Simple FM Snthesis Modulator -> Carrier
// Voices: 12

// Operator
// Pot 1: mWaveform
// Pot 2: mRatio
// Pot 3: mVolume
// Pot 4: cWaveform

// Mod Envelope
// Pot 5: mAttack
// Pot 6: mDecay
// Pot 7: mSustain
// Pot 8: mRelease

// Envelope:
// Pot 9: Attack
// Pot 10: Decay
// Pot 11: Sustain
// Pot 12: Release
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();
class Plugin_3 : public PluginControll
{
public:
#define NUM_RATIOS 10
    const float ratios[NUM_RATIOS] = {0.125, 0.25, 0.5, 0.75, 1, 2, 3, 4, 6, 8};
    float modulator_ratio = 1;
    AudioSynthWaveformModulated modulator[12];
    AudioEffectEnvelope modEnv[12];
    AudioSynthWaveformModulated carrier[12];
    AudioEffectEnvelope outEnv[12];
    AudioMixer12 mixer;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[51]; // total patchCordCount:50 including array typed ones.
AudioAnalyzePeak peak;
    // constructor (this is called when class-object is created)
    Plugin_3(const char *Name, byte ID) : PluginControll(Name, ID)
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
        patchCord[pci++] = new AudioConnection(modulator[0], 0, peak, 0);
        for (int i = 0; i < 12; i++)
        {
            patchCord[pci++] = new AudioConnection(modulator[i], 0, modEnv[i], 0);
            patchCord[pci++] = new AudioConnection(modEnv[i], 0, carrier[i], 0);
            patchCord[pci++] = new AudioConnection(carrier[i], 0, outEnv[i], 0);
            patchCord[pci++] = new AudioConnection(outEnv[i], 0, mixer, i);
        }
    }
    virtual ~Plugin_3() = default;

    virtual void setup(byte setID) override;
    virtual void noteOn(byte notePlayed, float velocity, byte voice) override;
    virtual void noteOff(byte notePlayed, byte voice) override;
    virtual void set_parameters(byte row) override;
    virtual void draw_plugin() override;

void get_peak();
    void set_mod_waveform(byte XPos, byte YPos, const char *name, int min, int max);
    void set_mod_amplitude(byte XPos, byte YPos, const char *name, int min, int max);
    void set_mod_ratio(byte XPos, byte YPos, const char *name, int min, int max);
    void set_car_waveform(byte XPos, byte YPos, const char *name, int min, int max);

    void set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_sustain(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max);

    void set_envelope_mattack(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_mdecay(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_msustain(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_mrelease(byte XPos, byte YPos, const char *name, int min, int max);
};
#endif // PLUGIN_3_H