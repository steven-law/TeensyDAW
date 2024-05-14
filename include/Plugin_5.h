#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <synth_fm_drum.h>
#include "mixers.h"
extern void drawPot(int XPos, byte YPos, int dvalue, const char *dname);
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row; 
extern float *note_frequency;
// TeensyDAW: begin automatically generated code

class Plugin_5
{
public:
    byte myID;
    byte potentiometer[16];
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
    Plugin_5()
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
    void setup(byte setID)
    {
        myID = setID;
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
    void noteOn(byte notePlayed, float velocity, byte voice)
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
    void noteOff(byte notePlayed, byte voice)
    {
        if (notePlayed == 50)
        {
            hhEnv.noteOff();
            hhFilterEnv.noteOff();
        }
    }

    void set_parameters(byte row)
    {
        draw_plugin();
        if (row == 0)
        {
            set_fmdrum_frequency(0, 0, "Freq", 10, 300);
            set_fmdrum_pitchMod(1, 0, "Sweep", 0, 1);
            set_fmdrum_overdrive(2, 0, "O-Drive", 0, 1);
            set_fmdrum_decay(3, 0, "Decay", 0, 2000);
        }

        if (row == 1)
        {
            set_fmsnare_frequency(0, 1, "Freq", 10, 300);
            set_fmsnare_pitchMod(1, 1, "Sweep", 0, 1);
            set_fmsnare_noise(2, 1, "Noise", 0, 1);
            set_fmsnare_decay(3, 1, "Decay", 0, 2000);
        }

        if (row == 2)
        {
            set_hhfilter_frequency(0, 2, "Freq", 1000, 8000);
            set_hhfilter_resonance(1, 2, "Reso", 0, 5.00);
            set_hhEnv_attack(2, 2, "Attack", 0, 50);
            // set_hhEnv_decay(3, 2, "Decay", 0, 50);
            set_hhEnv_release(3, 2, "Decay", 0, 2000);
        }

        if (row == 3)
        {
            set_tomL_frequency(0, 3, "TomL", 0, 0);
            set_tomM_frequency(1, 3, "TomM", 0, 0);
            set_tomH_frequency(2, 3, "TomH", 0, 0);
            set_toms_decay(3, 3, "Decay", 0, 2000);
        }
    }
    void draw_plugin()
    {

        if (change_plugin_row)
        {
            change_plugin_row = false;
            // Serial.println("drawing plugin 5");
            drawPot(0, 0, potentiometer[0], "Freq");
            drawPot(1, 0, potentiometer[1], "Sweep");
            drawPot(2, 0, potentiometer[2], "O-Drive");
            drawPot(3, 0, potentiometer[3], "Decay");

            drawPot(0, 1, potentiometer[4], "Freq");
            drawPot(1, 1, potentiometer[5], "Sweep");
            drawPot(2, 1, potentiometer[6], "Noise");
            drawPot(3, 1, potentiometer[7], "Decay");

            drawPot(0, 2, potentiometer[8], "Freq");
            drawPot(1, 2, potentiometer[9], "Reso");
            drawPot(2, 2, potentiometer[10], "Attack");
            drawPot(3, 2, potentiometer[11], "Decay");

            drawPot(0, 3, potentiometer[12], "TomL");
            drawPot(1, 3, potentiometer[13], "TomM");
            drawPot(2, 3, potentiometer[14], "TomH");
            drawPot(3, 3, potentiometer[15], "Decay");
        }
    }

    void set_fmsnare_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int freq = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            fm_snare.frequency(freq);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmsnare_pitchMod(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_snare.fm(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmsnare_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_snare.decay(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmsnare_noise(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_snare.noise(sustain);
            fm_snare.overdrive(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmsnare_overdrive(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_snare.overdrive(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }

    void set_fmdrum_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int freq = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            fm_drum.frequency(freq);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmdrum_pitchMod(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.fm(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmdrum_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.decay(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmdrum_noise(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.noise(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_fmdrum_overdrive(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.overdrive(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }

    void set_hhfilter_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int frequency = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            filter.frequency(frequency);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_hhfilter_resonance(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                filter.resonance((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_hhfilter_sweep(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                filter.octaveControl((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }

    void set_hhEnv_attack(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int attack = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            hhEnv.attack(attack);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_hhEnv_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int decay = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            hhEnv.decay(decay);
            hhFilterEnv.decay(decay);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_hhEnv_sustain(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            hhEnv.sustain(sustain);
            hhFilterEnv.sustain(sustain);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_hhEnv_release(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int release = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            hhEnv.release(release);
            hhFilterEnv.release(release);
            hhEnv.decay(release / 4);
            hhFilterEnv.decay(release / 4);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }

    void set_tomL_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int freq = note_frequency[potentiometer[n]] * tuning;
            tomL.frequency(freq);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_tomM_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int freq = note_frequency[potentiometer[n]] * tuning;
            tomM.frequency(freq);

            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_tomH_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int freq = note_frequency[potentiometer[n]] * tuning;
            tomH.frequency(freq);

            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_toms_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int decay = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            tomL.length(decay);
            tomM.length(decay);
            tomH.length(decay);
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
};
// TeensyDAW: end automatically generated code