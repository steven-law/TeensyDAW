#ifndef PLUGIN_6_H
#define PLUGIN_6_H

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include <pluginClass.h>
#include "filter_ladderlite.h"
// TeensyDAW: begin automatically generated code
// Name: Draw
// Description: Subtractive "Draw-your-own-Waveforms" Synthesizer
// Voices: 12

// VCO
// Pot 1:
// Pot 2: Vol
// Pot 3:
// Pot 4:

// Pot 5:
// Pot 6:
// Pot 7:
// Pot 8:

// Filter:
// Pot 9: Frequency
// Pot 10: Resonance
// Pot 11: Sweep
// Pot 12: Type

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
class Plugin_6 : public PluginControll
{
public:
    bool WaveformAssigned = false;
    int16_t singleCycleWaveform[256]{
        -10993, -10518, -9865, -8878, -6951, -6113, -5206, -2777, -273, 2774, 8324,
        10061, 10983, 12313, 12875, 14671, 15844, 17099, 18247, 19280, 20473, 20687, 20581, 19929, 19847, 19391, 19495, 19289, 19176, 19298, 19243, 19475, 20046, 20349, 20747, 21125, 20561,
        20535, 20694, 19325, 19117, 18274, 17382, 17221, 16641, 14479, 12983, 11605, 9391, 8523, 7499, 6152, 4057, 3409, 2050, -1288, -2348, -3454, -4790, -5372, -5865, -6728, -6500,
        -6597, -9560, -10799, -10911, -10936, -11299, -11865, -13168, -15059, -14272, -14520, -14750, -15351, -15409, -15500, -15723, -15157, -14902, -14650, -14624, -14099, -13345,
        -13068, -12739, -11270, -11033, -10577, -8507, -8394, -8715, -6553, -5084, -4836, -2894, -1719, -596, 783, 3603, 4674, 5114, 5760, 6455, 7021, 7234, 7415, 7904, 7408, 7428,
        6767, 6926, 5740, 5911, 5822, 4501, 4309, 4285, 3389, 3136, 3252, 2055, 1736, 1269, 1126, 1278, 1183, -976, -2556, -3825, -4848, -6878, -8029, -9100, -10135, -10564,
        -10865, -13248, -14075, -15137, -16133, -17270, -17476, -18722, -19834, -20706, -21173, -21372, -21100, -20861, -21007, -20801, -20651, -20308, -19987, -19622, -18808,
        -18582, -18593, -17806, -17443, -16786, -15834, -14955, -14460, -13624, -12590, -12119, -11540, -11650, -10845, -10097, -7801, -6865, -5460, -1722, -31, 1349, 2736, 7506, 10652, 13408,
        16141, 18599, 20778, 22984, 23136, 24147, 28797, 31127, 32474, 32127, 31454, 30954, 30941, 28824, 28700, 28587, 27273, 27786, 27851, 27477, 27054, 27194, 28840, 29043, 28994, 30072,
        30211, 30074, 29399, 26992, 25977, 24240, 21074, 19676, 18864, 15032, 9510, 2258, -7940, -8308, -9310, -12298, -13033, -13232, -13157, -15079, -16023, -16695, -17523, -18465, -19549,
        -20945, -21898, -22213, -25385, -25159, -25832, -25004, -25973, -25690, -24679, -23794, -24252, -24398, -23215, -22498, -21283, -19434, -17713, -15867, -14336, -13670, -12754};
    int xPos_SingleCyclePixel;
    int yPos_SingleCyclePixel;
    int old_xPos_SingleCyclePixel = 32;
    int old_yPos_SingleCyclePixel = 120;
    int xpos_old;
    int ypos_old;
    int16_t old_singleCycleValue = 0;
    AudioSynthWaveformDc dc[12];
    AudioSynthWaveform waveform[12];
    AudioEffectEnvelope Fenv[12];
    AudioFilterStateVariable filter[12];
    AudioFilterLadderLite ladder[12];
    AudioMixer4 fMixer[12];
    AudioEffectEnvelope Aenv[12];
    AudioMixer12 mixer;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[98]; // total patchCordCount:98 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_6(const char *Name, byte ID) : PluginControll(Name, ID)
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(mixer, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
        for (int i = 0; i < 12; i++)
        {
            patchCord[pci++] = new AudioConnection(dc[i], 0, Fenv[i], 0);
            patchCord[pci++] = new AudioConnection(waveform[i], 0, filter[i], 0);
            patchCord[pci++] = new AudioConnection(waveform[i], 0, ladder[i], 0);
            patchCord[pci++] = new AudioConnection(Fenv[i], 0, filter[i], 1);
            patchCord[pci++] = new AudioConnection(Fenv[i], 0, ladder[i], 1);
            patchCord[pci++] = new AudioConnection(filter[i], 0, fMixer[i], 0);
            patchCord[pci++] = new AudioConnection(filter[i], 1, fMixer[i], 1);
            patchCord[pci++] = new AudioConnection(filter[i], 2, fMixer[i], 2);
            patchCord[pci++] = new AudioConnection(ladder[i], 0, fMixer[i], 3);
            patchCord[pci++] = new AudioConnection(fMixer[i], 0, Aenv[i], 0);
            patchCord[pci++] = new AudioConnection(Aenv[i], 0, mixer, i);
        }
    }
    virtual ~Plugin_6() = default;
    virtual void setup() override;
    virtual void noteOn(byte notePlayed, float velocity, byte voice) override;
    virtual void noteOff(byte notePlayed, byte voice) override;
    virtual void set_parameters(byte row) override;
    virtual void draw_plugin() override;

    void set_parameters_page_2(byte row);
    void draw_plugin_page_2();
    void smooth_waveform();
    void clearSingleCycleWaveform();

    void set_voice_waveform(byte XPos, byte YPos, const char *name);
    virtual void assign_voice1_amplitude(byte value) override;

    virtual void assign_filter_frequency(byte value) override;
    virtual void assign_filter_resonance(byte value) override;
    virtual void assign_filter_sweep(byte value) override;
    virtual void assign_filter_type(byte mixerchannel) override;

    virtual void assign_envelope1_attack(byte value, int max) override;
    virtual void assign_envelope1_decay(byte value, int max) override;
    virtual void assign_envelope1_sustain(byte value) override;
    virtual void assign_envelope1_release(byte value, int max) override;
     virtual void set_envelope1_ADSR(byte YPos, int maxA, int maxD, int maxR);
};
#endif // PLUGIN_6_H