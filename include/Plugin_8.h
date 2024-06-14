#ifndef PLUGIN_8_H
#define PLUGIN_8_H

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include <pluginClass.h>
// TeensyDAW: begin automatically generated code
// Name: dTune
// Description: 2VCO Detuned Subtractive Synthesizer
// Voices: 12

// VCO 1
// Pot 1: Waveform
// Pot 2: Detune
// Pot 3: Vol
// Pot 4:

// VCO 2
// Pot 5: Waveform
// Pot 6: Detune
// Pot 7: Vol
// Pot 8:

// Filter:
// Pot 9: Frequency
// Pot 10: Resonance
// Pot 11: Sweep
// Pot 12: Type

// Envelope:
// Pot 13: Attack
// Pot 14: Decay
// Pot 15: Sustain
// Pot 16: Release
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();
class Plugin_8 : public PluginControll
{
public:
    float detune1;
    float detune2;
    AudioSynthWaveform waveform[12];
    AudioSynthWaveform waveform1[12];
    AudioSynthWaveformDc dc[12];
    AudioMixer4 vcoMixer[12];
    AudioEffectEnvelope Fenv[12];
    AudioFilterStateVariable filter[12];
    AudioMixer4 fmixer[12];
    AudioEffectEnvelope Aenv[12];
    AudioMixer12 mixer;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[122]; // total patchCordCount:122 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_8(const char *Name, byte ID) : PluginControll(Name, ID)
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
        for (int i = 0; i < 12; i++)
        {
            patchCord[pci++] = new AudioConnection(waveform[i], 0, vcoMixer[i], 0);
            patchCord[pci++] = new AudioConnection(waveform1[i], 0, vcoMixer[i], 1);
            patchCord[pci++] = new AudioConnection(dc[i], 0, Fenv[i], 0);
            patchCord[pci++] = new AudioConnection(vcoMixer[i], 0, filter[i], 0);
            patchCord[pci++] = new AudioConnection(Fenv[i], 0, filter[i], 1);
            patchCord[pci++] = new AudioConnection(filter[i], 0, fmixer[i], 0);
            patchCord[pci++] = new AudioConnection(filter[i], 1, fmixer[i], 1);
            patchCord[pci++] = new AudioConnection(filter[i], 2, fmixer[i], 2);
            patchCord[pci++] = new AudioConnection(fmixer[i], 0, Aenv[i], 0);
            patchCord[pci++] = new AudioConnection(Aenv[i], 0, mixer, i);
        }
    }
   virtual ~Plugin_8() = default;
    virtual void setup(byte setID) override;
    virtual void noteOn(byte notePlayed, float velocity, byte voice) override;
    virtual void noteOff(byte notePlayed, byte voice) override;
    virtual void set_parameters(byte row) override;
    virtual void draw_plugin() override;

    void set_voice_waveform(byte XPos, byte YPos, const char *name, int min, int max);
    void set_voice_amplitude(byte XPos, byte YPos, const char *name, int min, int max);
    void set_voice_detune(byte XPos, byte YPos, const char *name, int min, int max);
    void set_voice1_waveform(byte XPos, byte YPos, const char *name, int min, int max);
    void set_voice1_amplitude(byte XPos, byte YPos, const char *name, int min, int max);
    void set_voice1_detune(byte XPos, byte YPos, const char *name, int min, int max);

    void set_filter_frequency(byte XPos, byte YPos, const char *name, int min, int max);
    void set_filter_resonance(byte XPos, byte YPos, const char *name, float min, float max);
    void set_filter_sweep(byte XPos, byte YPos, const char *name, float min, float max);
    void set_filter_type(byte XPos, byte YPos, const char *name, int min, float max);
    void selectFilterType(byte mixerchannel);

    void set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_sustain(byte XPos, byte YPos, const char *name, int min, int max);
    void set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max);

};
#endif // PLUGIN_8_H