#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "mixers.h"
#include <ILI9341_t3n.h>
#include <ili9341_t3n_font_Arial.h> // from ILI9341_t3
#include "global_stuff.h"
#include <Plugin_6.h>

extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
extern float *note_frequency;
extern int tuning;
void clearWorkSpace();


void drawActiveRect(int xPos, byte yPos, byte xsize, byte ysize, bool state, const char *name, int color);
void myDrawLine(int x0, int y0, int x1, int y1, uint16_t color);
void myDrawRect(int x, int y, int w, int h, uint16_t color);
extern bool display_touched;
extern int touchedX;
extern int touchedY;
extern int pixelTouchX;
extern int gridTouchY;
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

    void Plugin_6::setup(byte setID)
    {
        myID = setID;

        for (int i = 0; i < MAX_VOICES; i++)
        {
            dc[i].amplitude(1);

            waveform[i].begin(WAVEFORM_ARBITRARY);
            waveform[i].amplitude(1);
            waveform[i].arbitraryWaveform(singleCycleWaveform, 172.0);

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
        MixGain.gain(1);
        SongVol.gain(1);
    }
    void Plugin_6::noteOn(byte notePlayed, float velocity, byte voice)
    {
        float frequency = note_frequency[notePlayed] * tuning;
        waveform[voice].frequency(frequency);
        Fenv[voice].noteOn();
        Aenv[voice].noteOn();
    }
    void Plugin_6::noteOff(byte notePlayed,byte voice)
    {
        Fenv[voice].noteOff();
        Aenv[voice].noteOff();
    }

    void Plugin_6::set_parameters(byte row)
    {
        if (buttonPressed[BUTTON_ENTER])
        {
            smooth_waveform();
            change_plugin_row = true;
        }
        draw_plugin();
        if (WaveformAssigned)
            set_parameters_page_2(row);

        if (!WaveformAssigned && display_touched)
        {

            if (touchedX >= 18 * STEP_FRAME_W)
            {
                if (touchedY >= 11 * STEP_FRAME_H && touchedY <= 12 * STEP_FRAME_H) // if we touch the enter button
                {
                    WaveformAssigned = true;
                    change_plugin_row = true;
                    clearWorkSpace();
                    draw_plugin();
                }

                if (touchedY >= 13 * STEP_FRAME_H) // if we touch the clear button
                {
                    WaveformAssigned = true;
                    change_plugin_row = true;
                    clearSingleCycleWaveform();
                }
            }

            // draw singleCycle Waveform BY HAND

            int SCdrawX = touchedX;
            int SCdrawY = touchedY;

            if (!enc_moved[0] && !enc_moved[1])
            {
                xPos_SingleCyclePixel = constrain(SCdrawX, 32, 288);
                yPos_SingleCyclePixel = constrain(SCdrawY, 16, 224);
            }
            if (1 * STEP_FRAME_W < touchedX && touchedX < 18 * STEP_FRAME_W && 16 < touchedY && touchedY < 13 * STEP_FRAME_H)
            {
                int arrayPos = constrain(xPos_SingleCyclePixel - 32, 0, 255);
                int singleCycleValue = map(yPos_SingleCyclePixel, 16, 224, 32768, -32768);
                singleCycleWaveform[arrayPos] = singleCycleValue;
                Serial.printf("ArrayPos=%d, sampleData=%i\n", arrayPos, singleCycleValue);
                // tft.drawPixel(xPos_SingleCyclePixel, yPos_SingleCyclePixel, ILI9341_WHITE);

                myDrawLine(old_xPos_SingleCyclePixel, old_yPos_SingleCyclePixel, xPos_SingleCyclePixel, yPos_SingleCyclePixel, ILI9341_WHITE);
                old_singleCycleValue = singleCycleValue;
                old_xPos_SingleCyclePixel = xPos_SingleCyclePixel;
                old_yPos_SingleCyclePixel = yPos_SingleCyclePixel;
            }
        }
    }
    void Plugin_6::set_parameters_page_2(byte row)
    {
        if (display_touched)
        {
            if (touchedX >= 18 * STEP_FRAME_W && touchedY >= 13 * STEP_FRAME_H)
            {
                // WaveformAssigned = true;
                clearSingleCycleWaveform();
            }
        }
        draw_plugin();
        if (!buttonPressed[BUTTON_SHIFT])
        {

            if (row == 0)
            {
                // set_voice_waveform(0, 0, "W~Form", 0, 12);
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
        if (buttonPressed[BUTTON_SHIFT])
        {
            set_presetNr();
        }
    }
    void Plugin_6::draw_plugin()
    {
        if (WaveformAssigned)
        {
            draw_plugin_page_2();
        }
        if (change_plugin_row)
        {
            smooth_waveform();
            change_plugin_row = false;
            myDrawLine(STEP_FRAME_W * 2, 120, 256 + STEP_FRAME_W * 2, 120, ILI9341_WHITE);
            drawActiveRect(18, 11, 2, 2, WaveformAssigned, "Enter", ILI9341_ORANGE);
            drawActiveRect(18, 13, 2, 2, true, "clear", ILI9341_RED);
            myDrawRect(STEP_FRAME_W * 2 - 1, STEP_FRAME_H * 1 - 1, 256 + 2, 208 + 2, ILI9341_BLACK);
            for (int i = 0; i < 256; i++)
            {
                int xpos = i + 32;
                int ypos = map(singleCycleWaveform[i], 32768, -32768, 16, 224);
                if (i > 0)
                    myDrawLine(xpos_old, ypos_old, xpos, ypos, ILI9341_WHITE);
                xpos_old = xpos;
                ypos_old = ypos;
            }
        }
    }
    void Plugin_6::draw_plugin_page_2()
    {
        if (change_plugin_row)
        {
            change_plugin_row = false;
            clearWorkSpace();
            // Serial.println("drawing plugin 2");

            drawPot(1, 0, potentiometer[presetNr][1], "Volume");

            drawPot(0, 2, potentiometer[presetNr][8], "Filt-Frq");
            drawPot(1, 2, potentiometer[presetNr][9], "Resonance");
            drawPot(2, 2, potentiometer[presetNr][10], "Sweep");
            drawPot(3, 2, potentiometer[presetNr][11], "Type");

            drawPot(0, 3, potentiometer[presetNr][12], "Attack");
            drawPot(1, 3, potentiometer[presetNr][13], "Decay");
            drawPot(2, 3, potentiometer[presetNr][14], "Sustain");
            drawPot(3, 3, potentiometer[presetNr][15], "Release");

            drawActiveRect(18, 13, 2, 2, true, "clear", ILI9341_RED);

            draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "Prset", presetNr, 3, 0);
        }
    }
    void Plugin_6::smooth_waveform()
    {
        for (int i = 0; i < 256; i++)
        {
            if (singleCycleWaveform[i] == 0)
            {
                for (int b = i; b < 256; b++)
                {
                    if (singleCycleWaveform[b] != 0)
                    {
                        singleCycleWaveform[i] = singleCycleWaveform[i - 1] + ((singleCycleWaveform[b] - singleCycleWaveform[i - 1]) / b - (i - 1)) * (i - (i - 1));

                        break;
                    }
                }
                // singleCycleWaveform[i] = singleCycleWaveform[i - 1];
            }
        }
    }
    void Plugin_6::clearSingleCycleWaveform()
    {
        if (display_touched)
        {
            change_plugin_row = false;
            display_touched = false;
            Serial.println("clearing waveform");
            WaveformAssigned = false;
            for (int i = 0; i < 256; i++)
            {
                singleCycleWaveform[i] = 0;
            }
            old_singleCycleValue = 0;
            old_xPos_SingleCyclePixel = 32;
            change_plugin_row = true;
            clearWorkSpace();
            draw_plugin();
        }
    }

    void Plugin_6::set_voice_waveform(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            int walveform = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);

            for (int i = 0; i < MAX_VOICES; i++)
            {
                waveform[i].begin(walveform);
            }
        }
    }
    void Plugin_6::set_voice_amplitude(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                waveform[i].amplitude((float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT));
            }
        }
    }

    void Plugin_6::set_filter_frequency(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            int frequency = note_frequency[potentiometer[presetNr][n]] * tuning;
            for (int i = 0; i < MAX_VOICES; i++)
            {
                filter[i].frequency(frequency);
            }
        }
    }
    void Plugin_6::set_filter_resonance(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                filter[i].resonance((float)(potentiometer[presetNr][n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            }
        }
    }
    void Plugin_6::set_filter_sweep(byte XPos, byte YPos, const char *name, float min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                filter[i].octaveControl((float)(potentiometer[presetNr][n] / (MIDI_CC_RANGE_FLOAT / max)) + min);
            }
        }
    }
    void Plugin_6::set_filter_type(byte XPos, byte YPos, const char *name, int min, float max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            selectFilterType(map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, 0, 3));
        }
    }
    void Plugin_6::selectFilterType(byte mixerchannel)
    {
        for (int i = 0; i < MAX_VOICES; i++)
        {
            fMixer[i].gain(0, 0);
            fMixer[i].gain(1, 0);
            fMixer[i].gain(2, 0);
            fMixer[i].gain(mixerchannel, 1);
        }
    }

    void Plugin_6::set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            int attack = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                Fenv[i].attack(attack);
                Aenv[i].attack(attack);
            }
        }
    }
    void Plugin_6::set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            int decay = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                Fenv[i].decay(decay);
                Aenv[i].decay(decay);
            }
        }
    }
    void Plugin_6::set_envelope_sustain(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            float sustain = (float)(potentiometer[presetNr][n] / MIDI_CC_RANGE_FLOAT);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                Fenv[i].sustain(sustain);
                Aenv[i].sustain(sustain);
            }
        }
    }
    void Plugin_6::set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max)
    {
        if (enc_moved[XPos])
        {
            int n = XPos + (YPos * NUM_ENCODERS);
            potentiometer[presetNr][n] = getEncodervalue(XPos, YPos, name, potentiometer[presetNr][n]);
            int release = map(potentiometer[presetNr][n], 0, MIDI_CC_RANGE, min, max);
            for (int i = 0; i < MAX_VOICES; i++)
            {
                Fenv[i].release(release);
                Aenv[i].release(release);
            }
        }
    }


// TeensyDAW: end automatically generated code