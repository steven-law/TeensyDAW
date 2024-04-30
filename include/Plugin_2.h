#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
extern float *note_frequency;
extern int tuning;
extern const byte VELOCITY_NOTE_ON;
extern const byte VELOCITY_NOTE_OFF;
extern void drawPot(int XPos, byte YPos, int dvalue, const char *dname, int color);
extern void clearWorkSpace();
// TeensyDAW: begin automatically generated code

class Plugin_2
{
public:
    byte myID;
    byte potentiometer[16];
    AudioSynthWaveformDc dc[MAX_VOICES];
    AudioSynthWaveform waveform[MAX_VOICES];
    AudioEffectEnvelope Fenv[MAX_VOICES];
    AudioFilterStateVariable filter[MAX_VOICES];
    AudioMixer4 fMixer[MAX_VOICES];
    AudioEffectEnvelope Aenv[MAX_VOICES];
    AudioMixer12 mixer;
    AudioConnection *patchCord[96]; // total patchCordCount:96 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_2()
    {
        int pci = 0; // used only for adding new patchcords

        for (int i = 0; i < MAX_VOICES; i++)
        {
            patchCord[pci++] = new AudioConnection(dc[i], 0, Fenv[i], 0);
            patchCord[pci++] = new AudioConnection(waveform[i], 0, filter[i], 0);
            patchCord[pci++] = new AudioConnection(Fenv[i], 0, filter[i], 1);
            patchCord[pci++] = new AudioConnection(filter[i], 0, fMixer[i], 0);
            patchCord[pci++] = new AudioConnection(filter[i], 1, fMixer[i], 1);
            patchCord[pci++] = new AudioConnection(filter[i], 2, fMixer[i], 2);
            patchCord[pci++] = new AudioConnection(fMixer[i], 0, Aenv[i], 0);
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

            Fenv[i].delay(0);
            Fenv[i].attack(0);
            Fenv[i].hold(0);
            Fenv[i].decay(0);
            Fenv[i].sustain(1);
            Fenv[i].release(200);

            filter[i].frequency(2000);
            filter[i].resonance(1);
            filter[i].octaveControl(4);

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
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        float frequency = note_frequency[notePlayed] * tuning;
        waveform[voice].frequency(frequency);
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
        if (row == 0)
        {
            set_voice_waveform(0, 0, "W~Form", 0, 12);
            set_voice_amplitude(1, 0, "Volume", 0, 1);
        }

        if (row == 1)
        {
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
        clearWorkSpace();

        drawPot(0, 0, potentiometer[0], "W~Form", ILI9341_BLUE);
        drawPot(1, 0, potentiometer[5], "Volume", ILI9341_BLUE);

        drawPot(0, 2, potentiometer[8], "Filt-Frq", ILI9341_BLUE);
        drawPot(1, 2, potentiometer[9], "Resonance", ILI9341_BLUE);
        drawPot(2, 2, potentiometer[10], "Sweep", ILI9341_BLUE);
        drawPot(3, 2, potentiometer[10], "Type", ILI9341_BLUE);

        drawPot(0, 3, potentiometer[12], "Attack", ILI9341_BLUE);
        drawPot(1, 3, potentiometer[13], "Decay", ILI9341_BLUE);
        drawPot(2, 3, potentiometer[14], "Sustain", ILI9341_BLUE);
        drawPot(3, 3, potentiometer[15], "Release", ILI9341_BLUE);
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
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
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
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
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
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
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
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
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
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_filter_type(byte XPos, byte YPos, const char *name, int min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            selectFilterType(map(potentiometer[n], 0,MIDI_CC_RANGE,0,3));
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void selectFilterType(byte mixerchannel)
    {
        for (int i = 0; i < MAX_VOICES; i++)
        {
            fMixer[i].gain(0, 0);
            fMixer[i].gain(1, 0);
            fMixer[i].gain(2, 0);
            fMixer[i].gain(mixerchannel, 1);
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
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
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

            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
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

            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
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

            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
};
// TeensyDAW: end automatically generated code
