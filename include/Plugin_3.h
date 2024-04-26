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
// TeensyDAW: begin automatically generated code

class Plugin_3
{
public:
    byte myID;
    byte potentiometer[16];
    AudioSynthKarplusStrong string;
    AudioSynthKarplusStrong string1;
    AudioSynthKarplusStrong string2;
    AudioSynthKarplusStrong string3;
    AudioMixer4 mixer4;
    AudioFilterStateVariable filter;
    AudioConnection *patchCord[5]; // total patchCordCount:5 including array typed ones.

    AudioSynthKarplusStrong *strings[4] = {&string, &string1, &string2, &string3};

    // constructor (this is called when class-object is created)
    Plugin_3()
    {
        int pci = 0; // used only for adding new patchcords
        patchCord[pci++] = new AudioConnection(string, 0, mixer4, 0);
        patchCord[pci++] = new AudioConnection(string1, 0, mixer4, 1);
        patchCord[pci++] = new AudioConnection(string2, 0, mixer4, 2);
        patchCord[pci++] = new AudioConnection(string3, 0, mixer4, 3);
        patchCord[pci++] = new AudioConnection(mixer4, 0, filter, 0);
    }
    void setup(byte setID)
    {
        myID = setID;
        /*
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
*/
        filter.frequency(2000);
        filter.octaveControl(7);

        mixer4.gain(0, 1);
        mixer4.gain(1, 1);
        mixer4.gain(2, 1);
        mixer4.gain(3, 1);
        /*
                envelope1.delay(0);
                envelope1.attack(0);
                envelope1.hold(0);
                envelope1.decay(0);
                envelope1.sustain(1);
                envelope1.release(200);
                */
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        float frequency = note_frequency[notePlayed] * tuning;
        strings[voice]->noteOn(frequency, velocity);
        Serial.printf("ON voice: %d, freq: %f \n", voice, frequency);
    }
    void noteOff(byte voice)
    {
        strings[voice]->noteOff(0);
        Serial.printf("OFF voice: %d,  \n", voice);
    }
    void set_parameters(byte row)
    { /*
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
         */
        if (row == 2)
        {
            set_filter_frequency(0, 2, "Filt-Frq", 60, 10000);
            set_filter_resonance(1, 2, "Resonance", 0, 5.00);
            set_filter_sweep(2, 2, "Sweep", 0, 7.00);
        }
        /*
        if (row == 3)
        {
            set_envelope_attack(0, 3, "Attack", 0, 1000);
            set_envelope_decay(1, 3, "Decay", 0, 500);
            set_envelope_sustain(2, 3, "Sustain", 0, 1);
            set_envelope_release(3, 3, "Release", 0, 2000);
        }*/
    }
    void draw_plugin()
    {
        clearWorkSpace();
        /*
        drawPot(0, 0, potentiometer[0], "W~Form 1", ILI9341_BLUE);
        drawPot(1, 0, potentiometer[1], "W~Form 2", ILI9341_BLUE);
        drawPot(2, 0, potentiometer[2], "W~Form 3", ILI9341_BLUE);
        drawPot(3, 0, potentiometer[3], "W~Form 4", ILI9341_BLUE);

        drawPot(0, 1, potentiometer[4], "Volume 1", ILI9341_BLUE);
        drawPot(1, 1, potentiometer[5], "Volume 2", ILI9341_BLUE);
        drawPot(2, 1, potentiometer[6], "Volume 3", ILI9341_BLUE);
        drawPot(3, 1, potentiometer[7], "Volume 4", ILI9341_BLUE);
*/
        drawPot(0, 2, potentiometer[8], "Filt-Frq", ILI9341_BLUE);
        drawPot(1, 2, potentiometer[9], "Resonance", ILI9341_BLUE);
        drawPot(2, 2, potentiometer[10], "Sweep", ILI9341_BLUE);
        /*
                drawPot(0, 3, potentiometer[12], "Attack", ILI9341_BLUE);
                drawPot(1, 3, potentiometer[13], "Decay", ILI9341_BLUE);
                drawPot(2, 3, potentiometer[14], "Sustain", ILI9341_BLUE);
                drawPot(3, 3, potentiometer[15], "Release", ILI9341_BLUE);
                */
    }
    void set_filter_frequency(byte XPos, byte YPos, char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            int frequency = note_frequency[potentiometer[n]] * tuning;
            filter.frequency(frequency);
            Serial.println(frequency);
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_filter_resonance(byte XPos, byte YPos, char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            filter.resonance((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }
    void set_filter_sweep(byte XPos, byte YPos, char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            filter.octaveControl((float)(potentiometer[n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            drawPot(XPos, YPos, potentiometer[n], name, ILI9341_BLUE);
        }
    }

    void encoder_to_value(byte XPos, byte n)
    {
        if (enc_moved[XPos])
        {
            potentiometer[n] = constrain(potentiometer[n] + encoded[XPos], 0, MIDI_CC_RANGE);
            enc_moved[XPos] = false;
        }
    }
};
// TeensyDAW: end automatically generated code
