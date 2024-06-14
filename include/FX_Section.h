#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "global_stuff.h"
#include <plugin_List.h>
//#include "Plugin_1.h"
//#include "Plugin_2.h"
//#include "Plugin_3.h"
//#include "Plugin_4.h"
//#include "Plugin_5.h"
//#include "Plugin_6.h"
//#include "Plugin_7.h"
//#include "Plugin_8.h"
#include "mixers.h"

// TeensyDAW: begin automatically generated code
//void drawPot(int XPos, byte YPos, int dvalue, const char *dname);
//extern Plugin_3 plugin_3;

class FX_Section
{
public:
    byte FX1_Potentiomer[2];
    byte FX2_Potentiomer[2];
    byte FX3_Potentiomer[2];
    //Plugin_1 plugin_1;
    //Plugin_2 plugin_2;
   AudioAnalyzePeak peak3;
   AudioAnalyzePeak peak2;
    //Plugin_4 plugin_4;
    //Plugin_5 plugin_5;
    //Plugin_6 plugin_6;
    //Plugin_7 plugin_7;
    //Plugin_8 plugin_8;
    AudioAmplifier dry_1;
    AudioAmplifier FX1_1;
    AudioAmplifier FX2_1;
    AudioAmplifier FX3_1;
    AudioAmplifier dry_2;
    AudioAmplifier FX1_2;
    AudioAmplifier FX2_2;
    AudioAmplifier FX3_2;
    AudioAmplifier dry_3;
    AudioAmplifier FX1_3;
    AudioAmplifier FX2_3;
    AudioAmplifier FX3_3;
    AudioAmplifier dry_4;
    AudioAmplifier FX1_4;
    AudioAmplifier FX2_4;
    AudioAmplifier FX3_4;
    AudioAmplifier dry_5;
    AudioAmplifier FX1_5;
    AudioAmplifier FX2_5;
    AudioAmplifier FX3_5;
    AudioAmplifier dry_6;
    AudioAmplifier FX1_6;
    AudioAmplifier FX2_6;
    AudioAmplifier FX3_6;
    AudioAmplifier dry_7;
    AudioAmplifier FX1_7;
    AudioAmplifier FX2_7;
    AudioAmplifier FX3_7;
    AudioAmplifier dry_8;
    AudioAmplifier FX1_8;
    AudioAmplifier FX2_8;
    AudioAmplifier FX3_8;
    AudioMixer12 dry_mixer;
    AudioMixer12 FX1_mixer;
    AudioMixer12 FX2_mixer;
    AudioMixer12 FX3_mixer;
    AudioEffectFreeverb freeverb;
    AudioEffectBitcrusher bitcrusher;
    AudioMixer4 endmixer;
    AudioConnection *patchCord[72]; // total patchCordCount:46 including array typed ones.

    // constructor (this is called when class-object is created)
    FX_Section()
    {
        int pci = 0; // used only for adding new patchcords


patchCord[pci++] = new AudioConnection(plugin_3.modulator[0], 0, peak3, 0);
patchCord[pci++] = new AudioConnection(plugin_2.waveform[0], 0, peak2, 0);
        patchCord[pci++] = new AudioConnection(plugin_1.SongVol, 0, dry_1, 0);
        patchCord[pci++] = new AudioConnection(plugin_1.SongVol, 0, FX1_1, 0);
        patchCord[pci++] = new AudioConnection(plugin_1.SongVol, 0, FX2_1, 0);
        patchCord[pci++] = new AudioConnection(plugin_1.SongVol, 0, FX3_1, 0);
        patchCord[pci++] = new AudioConnection(plugin_2.SongVol, 0, dry_2, 0);
        patchCord[pci++] = new AudioConnection(plugin_2.SongVol, 0, FX1_2, 0);
        patchCord[pci++] = new AudioConnection(plugin_2.SongVol, 0, FX2_2, 0);
        patchCord[pci++] = new AudioConnection(plugin_2.SongVol, 0, FX3_2, 0);
        patchCord[pci++] = new AudioConnection(plugin_3.SongVol, 0, dry_3, 0);
        patchCord[pci++] = new AudioConnection(plugin_3.SongVol, 0, FX1_3, 0);
        patchCord[pci++] = new AudioConnection(plugin_3.SongVol, 0, FX2_3, 0);
        patchCord[pci++] = new AudioConnection(plugin_3.SongVol, 0, FX3_3, 0);
        patchCord[pci++] = new AudioConnection(plugin_4.SongVol, 0, dry_4, 0);
        patchCord[pci++] = new AudioConnection(plugin_4.SongVol, 0, FX1_4, 0);
        patchCord[pci++] = new AudioConnection(plugin_4.SongVol, 0, FX2_4, 0);
        patchCord[pci++] = new AudioConnection(plugin_4.SongVol, 0, FX3_4, 0);
        patchCord[pci++] = new AudioConnection(plugin_5.SongVol, 0, dry_5, 0);
        patchCord[pci++] = new AudioConnection(plugin_5.SongVol, 0, FX1_5, 0);
        patchCord[pci++] = new AudioConnection(plugin_5.SongVol, 0, FX2_5, 0);
        patchCord[pci++] = new AudioConnection(plugin_5.SongVol, 0, FX3_5, 0);
        patchCord[pci++] = new AudioConnection(plugin_6.SongVol, 0, dry_6, 0);
        patchCord[pci++] = new AudioConnection(plugin_6.SongVol, 0, FX1_6, 0);
        patchCord[pci++] = new AudioConnection(plugin_6.SongVol, 0, FX2_6, 0);
        patchCord[pci++] = new AudioConnection(plugin_6.SongVol, 0, FX3_6, 0);
        patchCord[pci++] = new AudioConnection(plugin_7.SongVol, 0, dry_7, 0);
        patchCord[pci++] = new AudioConnection(plugin_7.SongVol, 0, FX1_7, 0);
        patchCord[pci++] = new AudioConnection(plugin_7.SongVol, 0, FX2_7, 0);
        patchCord[pci++] = new AudioConnection(plugin_7.SongVol, 0, FX3_7, 0);
        patchCord[pci++] = new AudioConnection(plugin_8.SongVol, 0, dry_8, 0);
        patchCord[pci++] = new AudioConnection(plugin_8.SongVol, 0, FX1_8, 0);
        patchCord[pci++] = new AudioConnection(plugin_8.SongVol, 0, FX2_8, 0);
        patchCord[pci++] = new AudioConnection(plugin_8.SongVol, 0, FX3_8, 0);
        patchCord[pci++] = new AudioConnection(dry_1, 0, dry_mixer, 0);
        patchCord[pci++] = new AudioConnection(FX1_1, 0, FX1_mixer, 0);
        patchCord[pci++] = new AudioConnection(FX2_1, 0, FX2_mixer, 0);
        patchCord[pci++] = new AudioConnection(FX3_1, 0, FX3_mixer, 0);
        patchCord[pci++] = new AudioConnection(dry_2, 0, dry_mixer, 1);
        patchCord[pci++] = new AudioConnection(FX1_2, 0, FX1_mixer, 1);
        patchCord[pci++] = new AudioConnection(FX2_2, 0, FX2_mixer, 1);
        patchCord[pci++] = new AudioConnection(FX3_2, 0, FX3_mixer, 1);
        patchCord[pci++] = new AudioConnection(dry_3, 0, dry_mixer, 2);
        patchCord[pci++] = new AudioConnection(FX1_3, 0, FX1_mixer, 2);
        patchCord[pci++] = new AudioConnection(FX2_3, 0, FX2_mixer, 2);
        patchCord[pci++] = new AudioConnection(FX3_3, 0, FX3_mixer, 2);
        patchCord[pci++] = new AudioConnection(dry_4, 0, dry_mixer, 3);
        patchCord[pci++] = new AudioConnection(FX1_4, 0, FX1_mixer, 3);
        patchCord[pci++] = new AudioConnection(FX2_4, 0, FX2_mixer, 3);
        patchCord[pci++] = new AudioConnection(FX3_4, 0, FX3_mixer, 3);
        patchCord[pci++] = new AudioConnection(dry_5, 0, dry_mixer, 4);
        patchCord[pci++] = new AudioConnection(FX1_5, 0, FX1_mixer, 4);
        patchCord[pci++] = new AudioConnection(FX2_5, 0, FX2_mixer, 4);
        patchCord[pci++] = new AudioConnection(FX3_5, 0, FX3_mixer, 4);
        patchCord[pci++] = new AudioConnection(dry_6, 0, dry_mixer, 5);
        patchCord[pci++] = new AudioConnection(FX1_6, 0, FX1_mixer, 5);
        patchCord[pci++] = new AudioConnection(FX2_6, 0, FX2_mixer, 5);
        patchCord[pci++] = new AudioConnection(FX3_6, 0, FX3_mixer, 5);
        patchCord[pci++] = new AudioConnection(dry_7, 0, dry_mixer, 6);
        patchCord[pci++] = new AudioConnection(FX1_7, 0, FX1_mixer, 6);
        patchCord[pci++] = new AudioConnection(FX2_7, 0, FX2_mixer, 6);
        patchCord[pci++] = new AudioConnection(FX3_7, 0, FX3_mixer, 6);
        patchCord[pci++] = new AudioConnection(dry_8, 0, dry_mixer, 7);
        patchCord[pci++] = new AudioConnection(FX1_8, 0, FX1_mixer, 7);
        patchCord[pci++] = new AudioConnection(FX2_8, 0, FX2_mixer, 7);
        patchCord[pci++] = new AudioConnection(FX3_8, 0, FX3_mixer, 7);
        patchCord[pci++] = new AudioConnection(dry_mixer, 0, endmixer, 0);
        patchCord[pci++] = new AudioConnection(FX1_mixer, 0, freeverb, 0);
        patchCord[pci++] = new AudioConnection(FX2_mixer, 0, bitcrusher, 0);
        patchCord[pci++] = new AudioConnection(FX3_mixer, 0, endmixer, 3);
        patchCord[pci++] = new AudioConnection(freeverb, 0, endmixer, 1);
        patchCord[pci++] = new AudioConnection(bitcrusher, 0, endmixer, 2);
        
    
    }
    void setup()
    {
        dry_mixer.gain(0, 1);
        dry_mixer.gain(1, 1);
        dry_mixer.gain(2, 1);
        dry_mixer.gain(3, 1);
        dry_mixer.gain(4, 1);

        FX1_mixer.gain(0, 1);
        FX1_mixer.gain(1, 1);
        FX1_mixer.gain(2, 1);
        FX1_mixer.gain(3, 1);
        FX1_mixer.gain(4, 1);

        FX2_mixer.gain(0, 1);
        FX2_mixer.gain(1, 1);
        FX2_mixer.gain(2, 1);
        FX2_mixer.gain(3, 1);
        FX2_mixer.gain(4, 1);

        FX3_mixer.gain(0, 1);
        FX3_mixer.gain(1, 1);
        FX3_mixer.gain(2, 1);
        FX3_mixer.gain(3, 1);
        FX3_mixer.gain(4, 1);

        endmixer.gain(0, 1);
        endmixer.gain(1, 1);
        endmixer.gain(2, 1);
        endmixer.gain(3, 1);
    }
    void draw_Bitcrusher()
    {
        if (change_plugin_row)
        {
            change_plugin_row = false;
            drawPot(0, 0, FX2_Potentiomer[0], "BitDepth");
            drawPot(1, 0, FX2_Potentiomer[1], "SmpRate");
        }
    }
    void set_Bitcrusher(byte row)
    {
        draw_Bitcrusher();
        if (row == 0)
        {
            set_BC_bits(0, 0, "BitDepth");
            set_BC_smplRate(1, 0, "SmpRate");
        }
    }
    void set_BC_bits(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            FX2_Potentiomer[XPos] = constrain(FX2_Potentiomer[XPos] + encoded[XPos], 0, MIDI_CC_RANGE);
            bitcrusher.bits(map(FX2_Potentiomer[XPos], 0, 127, 1, 16));

            drawPot(XPos, YPos, FX2_Potentiomer[XPos], name);
        }
    }
    void set_BC_smplRate(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            FX2_Potentiomer[XPos] = constrain(FX2_Potentiomer[XPos] + encoded[XPos], 0, MIDI_CC_RANGE);
            bitcrusher.sampleRate(map(FX2_Potentiomer[XPos], 0, 127, 1, 44100));

            drawPot(XPos, YPos, FX2_Potentiomer[XPos], name);
        }
    }

    void draw_freeverb()
    {
        if (change_plugin_row)
        {
            change_plugin_row = false;
            drawPot(0, 0, FX1_Potentiomer[0], "Roomsize");
            drawPot(1, 0, FX1_Potentiomer[1], "Damping");
        }
    }
    void set_freeverb(byte row)
    {
        draw_freeverb();
        if (row == 0)
        {
            set_RV_roomsize(0, 0, "Roomsize");
            set_RC_damping(1, 0, "Damping");
        }
    }
    void set_RV_roomsize(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            FX1_Potentiomer[XPos] = constrain(FX1_Potentiomer[XPos] + encoded[XPos], 0, MIDI_CC_RANGE);
            float size = (float)(FX1_Potentiomer[XPos] / MIDI_CC_RANGE_FLOAT);
            freeverb.roomsize(size);
            drawPot(XPos, YPos, FX1_Potentiomer[XPos], name);
        }
    }
    void set_RC_damping(byte XPos, byte YPos, const char *name)
    {
        if (enc_moved[XPos])
        {
            FX1_Potentiomer[XPos] = constrain(FX1_Potentiomer[XPos] + encoded[XPos], 0, MIDI_CC_RANGE);
            float damp = (float)(FX1_Potentiomer[XPos] / MIDI_CC_RANGE_FLOAT);
            freeverb.damping(damp);
            drawPot(XPos, YPos, FX1_Potentiomer[XPos], name);
        }
    }

void get_peak(){
    Serial.printf("FX2: %f , FX3: %f \n", peak2.read(), peak3.read());
}
};
// TeensyDAW: end automatically generated code
