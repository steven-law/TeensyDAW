#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

extern float *note_frequency;
extern int tuning;
extern const byte VELOCITY_NOTE_ON;
extern const byte VELOCITY_NOTE_OFF;
extern void drawPot(int XPos, byte YPos, int dvalue, char *dname, int color);
extern void clearWorkSpace();
// Encoder Pins

// TeensyDAW: begin automatically generated code

class Plugin_2
{
public:
    byte myID;
    byte potentiometer[16];
    AudioSynthWaveformDc dc1;
    AudioSynthWaveformDc dc2;
    AudioSynthWaveformDc dc3;
    AudioSynthWaveformDc dc4;
    AudioSynthWaveform waveform1;
    AudioEffectEnvelope envelope_1;
    AudioSynthWaveform waveform2;
    AudioEffectEnvelope envelope_2;
    AudioSynthWaveform waveform3;
    AudioSynthWaveform waveform4;
    AudioEffectEnvelope envelope_3;
    AudioEffectEnvelope envelope_4;
    AudioFilterStateVariable filter1;
    AudioFilterStateVariable filter2;
    AudioFilterStateVariable filter3;
    AudioFilterStateVariable filter4;
    AudioMixer4 mixer4_1;
    AudioMixer4 mixer4_2;
    AudioMixer4 mixer4_3;
    AudioMixer4 mixer4_4;
    AudioEffectEnvelope envelope1;
    AudioEffectEnvelope envelope2;
    AudioEffectEnvelope envelope3;
    AudioEffectEnvelope envelope4;
    AudioMixer4 mixer4;
    AudioConnection *patchCord[32]; // total patchCordCount:32 including array typed ones.

    AudioSynthWaveformDc *dc[4] = {&dc1, &dc2, &dc3, &dc4};
    AudioSynthWaveform *OSC[4] = {&waveform1, &waveform2, &waveform3, &waveform4};
    AudioEffectEnvelope *fEnv[4] = {&envelope_1, &envelope_2, &envelope_3, &envelope_4};
    AudioFilterStateVariable *filter[4] = {&filter1, &filter2, &filter3, &filter4};
    AudioMixer4 *fmixer[4] = {&mixer4_1, &mixer4_2, &mixer4_3, &mixer4_4};
    AudioEffectEnvelope *aEnv[4] = {&envelope1, &envelope2, &envelope3, &envelope4};

    // constructor (this is called when class-object is created)
    Plugin_2()
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(dc1, 0, envelope_1, 0);
        patchCord[pci++] = new AudioConnection(dc2, 0, envelope_2, 0);
        patchCord[pci++] = new AudioConnection(dc3, 0, envelope_3, 0);
        patchCord[pci++] = new AudioConnection(dc4, 0, envelope_4, 0);
        patchCord[pci++] = new AudioConnection(waveform1, 0, filter1, 0);
        patchCord[pci++] = new AudioConnection(envelope_1, 0, filter1, 1);
        patchCord[pci++] = new AudioConnection(waveform2, 0, filter2, 0);
        patchCord[pci++] = new AudioConnection(envelope_2, 0, filter2, 1);
        patchCord[pci++] = new AudioConnection(waveform3, 0, filter3, 0);
        patchCord[pci++] = new AudioConnection(waveform4, 0, filter4, 0);
        patchCord[pci++] = new AudioConnection(envelope_3, 0, filter3, 1);
        patchCord[pci++] = new AudioConnection(envelope_4, 0, filter4, 1);
        patchCord[pci++] = new AudioConnection(filter1, 0, mixer4_1, 0);
        patchCord[pci++] = new AudioConnection(filter1, 1, mixer4_1, 1);
        patchCord[pci++] = new AudioConnection(filter1, 2, mixer4_1, 2);
        patchCord[pci++] = new AudioConnection(filter2, 0, mixer4_2, 0);
        patchCord[pci++] = new AudioConnection(filter2, 1, mixer4_2, 1);
        patchCord[pci++] = new AudioConnection(filter2, 2, mixer4_2, 2);
        patchCord[pci++] = new AudioConnection(filter3, 0, mixer4_3, 0);
        patchCord[pci++] = new AudioConnection(filter3, 1, mixer4_3, 1);
        patchCord[pci++] = new AudioConnection(filter3, 2, mixer4_3, 2);
        patchCord[pci++] = new AudioConnection(filter4, 0, mixer4_4, 0);
        patchCord[pci++] = new AudioConnection(filter4, 1, mixer4_4, 1);
        patchCord[pci++] = new AudioConnection(filter4, 2, mixer4_4, 2);
        patchCord[pci++] = new AudioConnection(mixer4_1, 0, envelope1, 0);
        patchCord[pci++] = new AudioConnection(mixer4_2, 0, envelope2, 0);
        patchCord[pci++] = new AudioConnection(mixer4_3, 0, envelope3, 0);
        patchCord[pci++] = new AudioConnection(mixer4_4, 0, envelope4, 0);
        patchCord[pci++] = new AudioConnection(envelope1, 0, mixer4, 0);
        patchCord[pci++] = new AudioConnection(envelope2, 0, mixer4, 1);
        patchCord[pci++] = new AudioConnection(envelope3, 0, mixer4, 2);
        patchCord[pci++] = new AudioConnection(envelope4, 0, mixer4, 3);
    }

    void setup(byte setID)
    {
        myID = setID;
        mixer4.gain(0, 1);
        mixer4.gain(1, 1);
        mixer4.gain(2, 1);
        mixer4.gain(3, 1);

        for (int i = 0; i < 4; i++)
        {
            dc[i]->amplitude(1);

            OSC[i]->begin(WAVEFORM_SINE);
            OSC[i]->amplitude(1);

            fEnv[i]->delay(0);
            fEnv[i]->attack(0);
            fEnv[i]->hold(0);
            fEnv[i]->decay(0);
            fEnv[i]->sustain(1);
            fEnv[i]->release(200);

            filter[i]->frequency(2000);
            filter[i]->resonance(1);
            filter[i]->octaveControl(4);

            fmixer[i]->gain(0, 1);
            fmixer[i]->gain(1, 1);
            fmixer[i]->gain(2, 1);
            fmixer[i]->gain(3, 1);

            aEnv[i]->delay(0);
            aEnv[i]->attack(0);
            aEnv[i]->hold(0);
            aEnv[i]->decay(0);
            aEnv[i]->sustain(1);
            aEnv[i]->release(200);
        }
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        float frequency = note_frequency[notePlayed] * tuning;
        OSC[voice]->frequency(frequency);
        fEnv[voice]->noteOn();
        aEnv[voice]->noteOn();
    }
    void noteOff(byte voice)
    {
        fEnv[voice]->noteOff();
        aEnv[voice]->noteOff();
    }

    void set_parameters(byte row)
    {
        if (row == 0)
        {
            set_voice_waveform(0, 0, "W~Form 1", 0, 12);
            set_voice_waveform(1, 0, "W~Form 2", 0, 12);
            set_voice_waveform(2, 0, "W~Form 3", 0, 12);
            set_voice_waveform(3, 0, "W~Form 4", 0, 12);
        }

        if (row == 1)
        {
            set_voice_amplitude(0, 1, "Volume 1", 0, 1);
            set_voice_amplitude(1, 1, "Volume 2", 0, 1);
            set_voice_amplitude(2, 1, "Volume 3", 0, 1);
            set_voice_amplitude(3, 1, "Volume 4", 0, 1);
        }

        if (row == 2)
        {
            set_filter_frequency(0, 2, "Filt-Frq", 60, 10000);
            set_filter_resonance(1, 2, "Resonance", 0, 5.00);
            set_filter_sweep(2, 2, "Sweep", 0, 7.00);
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

        drawPot(0, 0, potentiometer[0], "W~Form 1", ILI9341_BLUE);
        drawPot(1, 0, potentiometer[0], "W~Form 2", ILI9341_BLUE);
        drawPot(2, 0, potentiometer[0], "W~Form 3", ILI9341_BLUE);
        drawPot(3, 0, potentiometer[0], "W~Form 4", ILI9341_BLUE);

        drawPot(0, 1, potentiometer[4], "Volume 1", ILI9341_BLUE);
        drawPot(1, 1, potentiometer[5], "Volume 2", ILI9341_BLUE);
        drawPot(2, 1, potentiometer[6], "Volume 3", ILI9341_BLUE);
        drawPot(3, 1, potentiometer[7], "Volume 4", ILI9341_BLUE);

        drawPot(0, 2, potentiometer[8], "Filt-Frq", ILI9341_BLUE);
        drawPot(1, 2, potentiometer[9], "Resonance", ILI9341_BLUE);
        drawPot(2, 2, potentiometer[10], "Sweep", ILI9341_BLUE);

        drawPot(0, 3, potentiometer[12], "Attack", ILI9341_BLUE);
        drawPot(1, 3, potentiometer[13], "Decay", ILI9341_BLUE);
        drawPot(2, 3, potentiometer[14], "Sustain", ILI9341_BLUE);
        drawPot(3, 3, potentiometer[15], "Release", ILI9341_BLUE);
    }

    void set_voice_waveform(byte XPos, byte YPos, char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int waveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            OSC[XPos]->begin(waveform);
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_voice_amplitude(byte XPos, byte YPos, char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            OSC[XPos]->amplitude((float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT));
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }

    void set_filter_frequency(byte XPos, byte YPos, char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int frequency = note_frequency[potentiometer[n]] * tuning;
            for (int i = 0; i < 4; i++)
            {
                filter[i]->frequency(frequency);
            }
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_filter_resonance(byte XPos, byte YPos, char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            for (int i = 0; i < 4; i++)
            {
                filter[i]->resonance((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            }
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_filter_sweep(byte XPos, byte YPos, char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            for (int i = 0; i < 4; i++)
            {
                filter[i]->octaveControl((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            }
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }

    void set_envelope_attack(byte XPos, byte YPos, char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int attack = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < 4; i++)
            {
                fEnv[i]->attack(attack);
                aEnv[i]->attack(attack);
            }
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_envelope_decay(byte XPos, byte YPos, char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int decay = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < 4; i++)
            {
                fEnv[i]->decay(decay);
                aEnv[i]->decay(decay);
            }

            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_envelope_sustain(byte XPos, byte YPos, char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            float sustain = (float)(potentiometer[n] / MIDI_CC_RANGE_FLOAT);
            for (int i = 0; i < 4; i++)
            {
                fEnv[i]->sustain(sustain);
                aEnv[i]->sustain(sustain);
            }

            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_envelope_release(byte XPos, byte YPos, char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int release = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < 4; i++)
            {
                fEnv[i]->release(release);
                aEnv[i]->release(release);
            }

            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
};
// TeensyDAW: end automatically generated code
