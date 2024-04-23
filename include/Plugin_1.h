#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// TeensyDAW: begin automatically generated code
extern float *note_frequency;
extern int tuning;
extern void drawPot(int XPos, byte YPos, int dvalue, char *dname, int color);

class Plugin_1
{
public:
    byte potentiometer[16];
    AudioSynthWaveform LFO1;
    AudioSynthWaveformModulated Voice1;
    AudioSynthWaveformModulated Voice2;
    AudioSynthWaveformModulated Voice3;
    AudioSynthWaveformModulated Voice4;
    AudioSynthWaveformDc dc;
    AudioMixer4 mixer4_1;
    AudioEffectEnvelope envelope;
    AudioFilterStateVariable filter;
    AudioMixer4 mixer4;
    AudioEffectEnvelope envelope1;
    AudioConnection *patchCord[15]; // total patchCordCount:15 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_1()
    {
        int pci = 0; // used only for adding new patchcords

        patchCord[pci++] = new AudioConnection(LFO1, 0, Voice1, 0);
        patchCord[pci++] = new AudioConnection(LFO1, 0, Voice2, 0);
        patchCord[pci++] = new AudioConnection(LFO1, 0, Voice3, 0);
        patchCord[pci++] = new AudioConnection(LFO1, 0, Voice4, 0);
        patchCord[pci++] = new AudioConnection(Voice1, 0, mixer4_1, 0);
        patchCord[pci++] = new AudioConnection(Voice2, 0, mixer4_1, 1);
        patchCord[pci++] = new AudioConnection(Voice3, 0, mixer4_1, 2);
        patchCord[pci++] = new AudioConnection(Voice4, 0, mixer4_1, 3);
        patchCord[pci++] = new AudioConnection(dc, 0, envelope, 0);
        patchCord[pci++] = new AudioConnection(mixer4_1, 0, filter, 0);
        patchCord[pci++] = new AudioConnection(envelope, 0, filter, 1);
        patchCord[pci++] = new AudioConnection(filter, 0, mixer4, 0);
        patchCord[pci++] = new AudioConnection(filter, 1, mixer4, 1);
        patchCord[pci++] = new AudioConnection(filter, 2, mixer4, 2);
        patchCord[pci++] = new AudioConnection(mixer4, 0, envelope1, 0);
    }
    void setup()
    {
        Voice1.begin(WAVEFORM_SINE);
        Voice1.frequency(440);
        Voice1.amplitude(1);
        mixer4_1.gain(0, 1);
        envelope.delay(0);
        envelope.attack(0);
        envelope.hold(0);
        envelope.decay(0);
        envelope.sustain(1);
        envelope.release(200);
        dc.amplitude(1);

        filter.frequency(2000);
        mixer4.gain(0, 1);
        envelope1.delay(0);
        envelope1.attack(0);
        envelope1.hold(0);
        envelope1.decay(0);
        envelope1.sustain(1);
        envelope1.release(200);
    }
    void noteOn(byte notePlayed)
    {
        float frequency = note_frequency[notePlayed] * tuning;
        Voice1.frequency(frequency);
        envelope1.noteOn();
        envelope.noteOn();
    }
    void noteOff()
    {
        // float frequency = note_frequency[notePlayed] * tuning;
        envelope1.noteOff();
        envelope.noteOff();
    }
    void set_parameters(byte row)
    {
        if (row == 0)
        {
            set_voice1_waveform(0, 0, "W~Form 1", 0, 12);
            set_voice2_waveform(1, 0, "W~Form 2", 0, 12);
            set_voice3_waveform(2, 0, "W~Form 3", 0, 12);
            set_voice4_waveform(3, 0, "W~Form 4", 0, 12);
        }

        if (row == 1)
        {
            set_voice1_amplitude(0, 1, "Volume 1", 0, 1);
            set_voice2_amplitude(1, 1, "Volume 2", 0, 1);
            set_voice3_amplitude(2, 1, "Volume 3", 0, 1);
            set_voice4_amplitude(3, 1, "Volume 4", 0, 1);
        }
        if (row == 2)
        {
            set_filter_frequency(0, 2, "Filt-Frq", 60, 10000);
            set_filter_resonance(1, 2, "Resonance", 0, 5);
        }
        if (row == 3)
        {
            set_envelope_attack(0, 3, "Attack", 0, 2000);
            set_envelope_decay(1, 3, "Decay", 0, 2000);
            set_envelope_sustain(2, 3, "Sustain", 0, 1);
            set_envelope_release(3, 3, "Release", 0, 2000);
        }
    }

    void set_voice1_waveform(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        int waveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        Voice1.begin(waveform);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_voice2_waveform(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        int waveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        Voice2.begin(waveform);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_voice3_waveform(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        int waveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        Voice3.begin(waveform);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_voice4_waveform(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        int waveform = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        Voice4.begin(waveform);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }

    void set_voice1_amplitude(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        float amplitude = (float)map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        Voice1.amplitude(amplitude);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_voice2_amplitude(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        float amplitude = (float)map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        Voice2.amplitude(amplitude);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_voice3_amplitude(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        float amplitude = (float)map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        Voice3.amplitude(amplitude);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_voice4_amplitude(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        float amplitude = (float)map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        Voice4.amplitude(amplitude);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }

    void set_filter_frequency(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        int frequency = note_frequency[potentiometer[n]];
        filter.frequency(frequency);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_filter_resonance(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        float resonance = (float)map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        filter.resonance(resonance);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }

    void set_envelope_attack(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        int attack = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        envelope1.attack(attack);
        envelope.attack(attack);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_envelope_decay(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        int decay = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        envelope1.decay(decay);
        envelope.decay(decay);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_envelope_sustain(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        float sustain = (float)map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        envelope1.sustain(sustain);
        envelope.sustain(sustain);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }
    void set_envelope_release(byte XPos, byte YPos, char *name, int min, int max)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        encoder_to_value(XPos, n);
        int release = map(potentiometer[n], 0, MIDI_CC_RANGE, min, max);
        envelope1.release(release);
        envelope.release(release);
        drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
    }

    void encoder_to_value(byte XPos, byte n)
    {
        if (enc_moved[XPos])
        {
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, 127);
            enc_moved[XPos] = false;
        }
    }
};
// TeensyDAW: end automatically generated code
