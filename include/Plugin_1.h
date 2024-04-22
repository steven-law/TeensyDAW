#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// TeensyDAW: begin automatically generated code
extern float *note_frequency;
extern int tuning;
class Plugin_1
{
public:

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
};
// TeensyDAW: end automatically generated code
