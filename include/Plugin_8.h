#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "mixers.h"

// TeensyDAW: begin automatically generated code
//Name: dTune
//Description: 2VCO Detuned Subtractive Synthesizer
//Voices: 12

//VCO 1
//Pot 1: Waveform
//Pot 2: Detune
//Pot 3: Vol
//Pot 4: 

//VCO 2
//Pot 5: Waveform
//Pot 6: Detune
//Pot 7: Vol
//Pot 8: 

//Filter:
//Pot 9: Frequency
//Pot 10: Resonance
//Pot 11: Sweep
//Pot 12: Type

//Envelope:
//Pot 13: Attack
//Pot 14: Decay
//Pot 15: Sustain
//Pot 16: Release 
class Plugin_8
{
public:
    byte myID;
    byte potentiometer[16];
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
    Plugin_8()
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
    void setup(byte setID)
    {
        myID = setID;

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
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        waveform[voice].frequency((note_frequency[notePlayed] + detune1) * tuning);
        waveform1[voice].frequency((note_frequency[notePlayed] + detune2) * tuning);
        Serial.println(detune1);
        Fenv[voice].noteOn();
        Aenv[voice].noteOn();
    }
    void noteOff(byte voice)
    {
        Fenv[voice].noteOff();
        Aenv[voice].noteOff();
    }

    void set_parameters(byte row)
    {
        draw_plugin();
        if (row == 0)
        {
            set_voice_waveform(0, 0, "W~Form", 0, 12);
            set_voice_detune(1, 0, "Detune", 0, 1);
            set_voice_amplitude(2, 0, "Volume 1", 0, 1);
        }

        if (row == 1)
        {
            set_voice1_waveform(0, 1, "W~Form", 0, 12);
            set_voice1_detune(1, 1, "Detune", 0, 1);
            set_voice1_amplitude(2, 1, "Volume 2", 0, 1);
        }

        if (row == 2)
        {
            set_filter_frequency(0, 2, "Filt-Frq", 60, 10000);
            set_filter_resonance(1, 2, "Resonance", 0, 5.00);
            set_filter_sweep(2, 2, "Sweep", 0, 7.00);
            set_filter_type(3, 2, "Type", 0, 3);
        }

        if (row == 3)
        {
            set_envelope_attack(0, 3, "Attack", 0, 1000);
            set_envelope_decay(1, 3, "Decay", 0, 500);
            set_envelope_sustain(2, 3, "Sustain", 0, 1);
            set_envelope_release(3, 3, "Release", 0, 2000);
        }
    }
    void draw_plugin()
    {
        if (change_plugin_row)
        {
            change_plugin_row = false;
            // Serial.println("drawing plugin 2");
            drawPot(0, 0, potentiometer[0], "W~Form");
            drawPot(1, 0, potentiometer[1], "Detune");
            drawPot(2, 0, potentiometer[2], "Volume 1");

            drawPot(0, 1, potentiometer[4], "W~Form");
            drawPot(1, 1, potentiometer[5], "Detune");
            drawPot(2, 1, potentiometer[6], "Volume 2");

            drawPot(0, 2, potentiometer[8], "Filt-Frq");
            drawPot(1, 2, potentiometer[9], "Resonance");
            drawPot(2, 2, potentiometer[10], "Sweep");
            drawPot(3, 2, potentiometer[11], "Type");

            drawPot(0, 3, potentiometer[12], "Attack");
            drawPot(1, 3, potentiometer[13], "Decay");
            drawPot(2, 3, potentiometer[14], "Sustain");
            drawPot(3, 3, potentiometer[15], "Release");
        }
    }

    void set_voice_waveform(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int walveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);

            for (int i = 0; i < MAX_VOICES; i++)
            {
                waveform[i].begin(walveform);
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_voice_amplitude(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                waveform[i].amplitude((float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT));
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_voice_detune(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            detune1 = (float)((note_frequency[potentiometer[n]] * 0.01));

            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_voice1_waveform(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int walveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);

            for (int i = 0; i < MAX_VOICES; i++)
            {
                waveform1[i].begin(walveform);
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_voice1_amplitude(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                waveform1[i].amplitude((float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT));
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_voice1_detune(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            detune2 = (float)((note_frequency[potentiometer[n]] * 0.01));
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }

    void set_filter_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int frequency = note_frequency[potentiometer[n]] * tuning;
            for (int i = 0; i < MAX_VOICES; i++)
            {
                filter[i].frequency(frequency);
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_filter_resonance(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                filter[i].resonance((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_filter_sweep(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                filter[i].octaveControl((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_filter_type(byte XPos, byte YPos, const char *name, int min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            selectFilterType(map(potentiometer[n], 0, MIDI_CC_RANGE, 0, 3));
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void selectFilterType(byte mixerchannel)
    {
        for (int i = 0; i < MAX_VOICES; i++)
        {
            fmixer[i].gain(0, 0);
            fmixer[i].gain(1, 0);
            fmixer[i].gain(2, 0);
            fmixer[i].gain(mixerchannel, 1);
        }
    }

    void set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int attack = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                Fenv[i].attack(attack);
                Aenv[i].attack(attack);
            }
            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int decay = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                Fenv[i].decay(decay);
                Aenv[i].decay(decay);
            }

            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_envelope_sustain(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                Fenv[i].sustain(sustain);
                Aenv[i].sustain(sustain);
            }

            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
    void set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int release = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                Fenv[i].release(release);
                Aenv[i].release(release);
            }

            drawPot(XPos, YPos, potentiometer[n], name);
        }
    }
};
// TeensyDAW: end automatically generated code
