#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <effect_dynamics.h>
extern bool buttonPressed[NUM_BUTTONS];

// TeensyDAW: begin automatically generated code
// Name: Boom
// Description: Subtractive Synthesizer
// Voices: 1

// Kick
// Pot 1: Sweep
// Pot 2: Noise
// Pot 3: O-Drive
// Pot 4: Decay

//
// Pot 5:
// Pot 6:
// Pot 7:
// Pot 8:

//
// Pot 9:
// Pot 10:
// Pot 11:
// Pot 12:

//
// Pot 13:
// Pot 14:
// Pot 15:
// Pot 16:

// byte getEncodervalue(byte XPos, byte YPos, const char *name, byte oldValue);
class Plugin_7
{
public:
    byte myID;
    byte potentiometer[NUM_PRESETS][16];
    byte presetNr = 0;
    float threshold;
    float attack = 0.1f;
    float release = 0.5f;
    float hysterisis = 1.0f;
    float ratio = 35.0f;
    float kneeWidth = 1.0f;

    AudioSynthFMDrum fm_drum;
    AudioEffectDynamics dynamics;
    AudioAmplifier amp;
    AudioAmplifier MixGain;
    AudioAmplifier SongVol;
    AudioConnection *patchCord[4]; // total patchCordCount:2 including array typed ones.

    // constructor (this is called when class-object is created)
    Plugin_7()
    {
        int pci = 0; // used only for adding new patchcords

        // patchCord[pci++] = new AudioConnection(fm_drum, 0, dynamics, 0);
        // patchCord[pci++] = new AudioConnection(dynamics, 0, MixGain, 0);
        // patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);

        patchCord[pci++] = new AudioConnection(fm_drum, 0, dynamics, 0);
        patchCord[pci++] = new AudioConnection(dynamics, 0, amp, 0);
        patchCord[pci++] = new AudioConnection(amp, 0, MixGain, 0);
        patchCord[pci++] = new AudioConnection(MixGain, 0, SongVol, 0);
    }

    void setup(byte setID)
    {
        myID = setID;
        // dynamics.gate(threshold, attack, release, hysterisis);              // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
        // dynamics.compression(threshold, attack, release, ratio, kneeWidth); // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 1.0f-60.0f ;; 0.0f-32.0f
        dynamics.limit(threshold, attack, release); // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
        dynamics.autoMakeupGain(6.0f);
        dynamics.makeupGain(6.0f);
        MixGain.gain(1);
        SongVol.gain(1);
    }
    void noteOn(byte notePlayed, float velocity, byte voice)
    {
        float frequence = note_frequency[notePlayed] * tuning;
        fm_drum.frequency(frequence);
        fm_drum.noteOn();
    }
    void noteOff(byte voice)
    {
    }
    void set_parameters(byte row)
    {
        draw_plugin();
        if (!buttonPressed[BUTTON_SHIFT])
        {
            if (row == 0)
            {
                set_fmdrum_pitchMod(0, 0, "Sweep", 0, 1);
                set_fmdrum_noise(1, 0, "Noise", 0, 1);
                set_fmdrum_overdrive(2, 0, "O-Drive", 0, 1);
                set_fmdrum_decay(3, 0, "Decay", 0, 2000);
            }

            if (row == 1)
            {
                set_dynamics_threshold(0, 1, "Trshold");
                set_dynamics_attack(1, 1, "Attack");
                set_dynamics_release(2, 1, "Release");
                set_dynamics_hysterisis(3, 1, "Hysteris");
            }

            if (row == 2)
            {
                set_dynamics_ratio(0, 2, "Ratio");
                set_dynamics_kneeWidth(1, 2, "Knewdth");
                set_dynamics_autoMakeupGain(2, 2, "AmakUpG");
                set_dynamics_makeupGain(3, 2, "makUpG");
            }

            if (row == 3)
            {
                set_amp_gain(3, 3, "Gain");
            }
        }
        if (buttonPressed[BUTTON_SHIFT])
        {
            set_presetNr();
        }
    }
    void draw_plugin()
    {

        if (change_plugin_row)
        {
            change_plugin_row = false;
            clearWorkSpace();
            // Serial.println("drawing plugin 5");

            drawPot(0, 0, potentiometer[presetNr][0], "Sweep");
            drawPot(1, 0, potentiometer[presetNr][1], "Noise");
            drawPot(2, 0, potentiometer[presetNr][2], "O-Drive");
            drawPot(3, 0, potentiometer[presetNr][3], "Decay");

            drawPot(0, 1, potentiometer[presetNr][4], "Trshold");
            drawPot(1, 1, potentiometer[presetNr][5], "Attack");
            drawPot(2, 1, potentiometer[presetNr][6], "Release");
            drawPot(3, 1, potentiometer[presetNr][7], "Hysteris");

            drawPot(0, 2, potentiometer[presetNr][8], "Ratio");
            drawPot(1, 2, potentiometer[presetNr][9], "Knewdth");
            drawPot(2, 2, potentiometer[presetNr][10], "AmakUpG");
            drawPot(3, 2, potentiometer[presetNr][11], "makUpG");

            drawPot(3, 3, potentiometer[presetNr][15], "Gain");

            draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
        }
    }

    void set_fmdrum_pitchMod(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.fm(sustain); // float 0-1
        }
    }
    void set_fmdrum_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.decay(sustain); // float 0-1
        }
    }
    void set_fmdrum_noise(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.noise(sustain); // float 0-1
        }
    }
    void set_fmdrum_overdrive(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT);
            fm_drum.overdrive(sustain); // float 0-1
        }
    }

    void set_dynamics_threshold(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            threshold = (float)map(potentiometer[presetNr][n], 0, 127, MIN_DB, MAX_DB);
            dynamics.gate(threshold, attack, release, hysterisis);              // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
            dynamics.compression(threshold, attack, release, ratio, kneeWidth); // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 1.0f-60.0f ;; 0.0f-32.0f
            dynamics.limit(threshold, attack, release);                         // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
        }
    }
    void set_dynamics_attack(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            attack = (float)map(potentiometer[presetNr][n], 0, 127, MIN_T, MAX_T);
            dynamics.gate(threshold, attack, release, hysterisis);              // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
            dynamics.compression(threshold, attack, release, ratio, kneeWidth); // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 1.0f-60.0f ;; 0.0f-32.0f
            dynamics.limit(threshold, attack, release);                         // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
        }
    }
    void set_dynamics_release(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            release = (float)map(potentiometer[presetNr][n], 0, 127, MIN_T, MAX_T);
            dynamics.gate(threshold, attack, release, hysterisis);              // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
            dynamics.compression(threshold, attack, release, ratio, kneeWidth); // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 1.0f-60.0f ;; 0.0f-32.0f
            dynamics.limit(threshold, attack, release);                         // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
        }
    }
    void set_dynamics_hysterisis(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            hysterisis = (float)map(potentiometer[presetNr][n], 0, 127, 0.0f, 6.0f);
            dynamics.gate(threshold, attack, release, hysterisis); // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 0.0f-6.0f
        }
    }

    void set_dynamics_ratio(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            ratio = (float)map(potentiometer[presetNr][n], 0, 127, RATIO_OFF, RATIO_INFINITY);
            dynamics.compression(threshold, attack, release, ratio, kneeWidth); // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 1.0f-60.0f ;; 0.0f-32.0f
        }
    }
    void set_dynamics_kneeWidth(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            kneeWidth = (float)map(potentiometer[presetNr][n], 0, 127, 0.0f, 32.0f);
            dynamics.compression(threshold, attack, release, ratio, kneeWidth); // float -110.0f-0.0f ;; 0.03f-4.00f ;; 0.03f-4.00f ;; 1.0f-60.0f ;; 0.0f-32.0f
        }
    }
    void set_dynamics_makeupGain(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)map(potentiometer[presetNr][n], 0, 127, -12.0f, 24.0f);
            dynamics.makeupGain(sustain); // float -12.0f, 24.0f
        }
    }
    void set_dynamics_autoMakeupGain(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)map(potentiometer[presetNr][n], 0, 127, 0.0f, 60.0f);
            dynamics.autoMakeupGain(sustain); // float 0.0f, 60.0f
        }
    }
    void set_amp_gain(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)map(potentiometer[presetNr][n], 0, 127, 0.0f, 6.0f);
            amp.gain(sustain); // float 0.0f, 60.0f
        }
    }

    void set_presetNr()
    {
        if (enc_moved[PRESET_ENCODER])
        {
            presetNr = constrain(presetNr + encoded[PRESET_ENCODER], 0, NUM_PRESETS - 1);
            change_plugin_row = true;
            draw_plugin();
        }
    }
};
// TeensyDAW: end automatically generated code
