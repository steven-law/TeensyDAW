#ifndef PLUGIN_CLASS
#define PLUGIN_CLASS

#include <global_stuff.h>
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
void drawPot(int XPos, byte YPos, int dvalue, const char *dname);

class PluginControll
{
public:
    byte myID;
    byte potentiometer[NUM_PRESETS][16];
    byte presetNr = 0;

    const char *name;
    PluginControll(const char *Name, byte ID)
    {
        name = Name;
        myID = ID;
    }
    virtual ~PluginControll() = default;
    virtual const char *get_Name() { return name; }
    virtual byte get_ID() { return myID; }

    virtual void setup() = 0; // Reine virtuelle Funktion (muss von Subklassen implementiert werden)
    virtual void noteOn(byte notePlayed, float velocity, byte voice) = 0;
    virtual void noteOff(byte notePlayed, byte voice) = 0;
    virtual void set_parameters(byte row) = 0;
    virtual void draw_plugin() = 0;

    virtual void set_presetNr();
    virtual byte get_Potentiometer(byte XPos, byte YPos, const char *name);

    // ASSIGN audio functions
    // waveform 1
    virtual void assign_voice1_waveform(byte value); // // Normale virtuelle Funktion (kann von Subklassen überschrieben werden)
    virtual void assign_voice1_amplitude(byte value);
    virtual void set_voice1_waveform(byte XPos, byte YPos, const char *name);
    virtual void set_voice1_amplitude(byte XPos, byte YPos, const char *name);
    // waveform 2
    virtual void assign_voice2_waveform(byte value); // // Normale virtuelle Funktion (kann von Subklassen überschrieben werden)
    virtual void assign_voice2_amplitude(byte value);
    virtual void set_voice2_waveform(byte XPos, byte YPos, const char *name);
    virtual void set_voice2_amplitude(byte XPos, byte YPos, const char *name);

    // filter
    virtual void assign_filter_frequency(byte value);
    virtual void assign_filter_resonance(byte value);
    virtual void assign_filter_sweep(byte value);
    virtual void assign_filter_type(byte mixerchannel);
    virtual void set_filter_frequency(byte XPos, byte YPos, const char *name);
    virtual void set_filter_resonance(byte XPos, byte YPos, const char *name);
    virtual void set_filter_sweep(byte XPos, byte YPos, const char *name);
    virtual void set_filter_type(byte XPos, byte YPos, const char *name);

    // envelope 1
    virtual void assign_envelope1_attack(byte value, int max);
    virtual void assign_envelope1_decay(byte value, int max);
    virtual void assign_envelope1_sustain(byte value);
    virtual void assign_envelope1_release(byte value, int max);
    virtual void set_envelope1_attack(byte XPos, byte YPos, const char *name, int max);
    virtual void set_envelope1_decay(byte XPos, byte YPos, const char *name, int max);
    virtual void set_envelope1_sustain(byte XPos, byte YPos, const char *name);
    virtual void set_envelope1_release(byte XPos, byte YPos, const char *name, int max);
    virtual void set_envelope1_ADSR(byte YPos, int maxA, int maxD, int maxR);
    // envelope 2
    virtual void assign_envelope2_attack(byte value, int max);
    virtual void assign_envelope2_decay(byte value, int max);
    virtual void assign_envelope2_sustain(byte value);
    virtual void assign_envelope2_release(byte value, int max);
    virtual void set_envelope2_attack(byte XPos, byte YPos, const char *name, int min, int max);
    virtual void set_envelope2_decay(byte XPos, byte YPos, const char *name, int min, int max);
    virtual void set_envelope2_sustain(byte XPos, byte YPos, const char *name);
    virtual void set_envelope2_release(byte XPos, byte YPos, const char *name, int min, int max);
    virtual void set_envelope2_ADSR(byte YPos, int maxA, int maxD, int maxR);

    // FMDRUM 1
    virtual void assign_fmdrum1_frequency(byte value);
    virtual void assign_fmdrum1_pitchMod(byte value);
    virtual void assign_fmdrum1_decay(byte value);
    virtual void assign_fmdrum1_noise(byte value);
    virtual void assign_fmdrum1_overdrive(byte value);
    virtual void set_fmdrum1_frequency(byte XPos, byte YPos, const char *name);
    virtual void set_fmdrum1_pitchMod(byte XPos, byte YPos, const char *name);
    virtual void set_fmdrum1_decay(byte XPos, byte YPos, const char *name);
    virtual void set_fmdrum1_noise(byte XPos, byte YPos, const char *name);
    virtual void set_fmdrum1_overdrive(byte XPos, byte YPos, const char *name);
    // FMDRUM 2
    virtual void assign_fmdrum2_frequency(byte value);
    virtual void assign_fmdrum2_pitchMod(byte value);
    virtual void assign_fmdrum2_decay(byte value);
    virtual void assign_fmdrum2_noise(byte value);
    virtual void assign_fmdrum2_overdrive(byte value);
    virtual void set_fmdrum2_frequency(byte XPos, byte YPos, const char *name);
    virtual void set_fmdrum2_pitchMod(byte XPos, byte YPos, const char *name);
    virtual void set_fmdrum2_decay(byte XPos, byte YPos, const char *name);
    virtual void set_fmdrum2_noise(byte XPos, byte YPos, const char *name);
    virtual void set_fmdrum2_overdrive(byte XPos, byte YPos, const char *name);

    // DRUM 1
    virtual void assign_drum1_frequency(byte value);
    virtual void assign_drum1_decay(byte value);
    virtual void set_drum1_frequency(byte XPos, byte YPos, const char *name);
    virtual void set_drum1_decay(byte XPos, byte YPos, const char *name);
    // DRUM 2
    virtual void assign_drum2_frequency(byte value);
    virtual void assign_drum2_decay(byte value);
    virtual void set_drum2_frequency(byte XPos, byte YPos, const char *name);
    virtual void set_drum2_decay(byte XPos, byte YPos, const char *name);
    // DRUM 3
    virtual void assign_drum3_frequency(byte value);
    virtual void assign_drum3_decay(byte value);
    virtual void set_drum3_frequency(byte XPos, byte YPos, const char *name);
    virtual void set_drum3_decay(byte XPos, byte YPos, const char *name);
    // mixer
    virtual void assign_mixer_gain(byte value, byte channel);
    virtual void set_mixer_gain(byte XPos, byte YPos, const char *name);
};
#endif // PLUGIN_CLASS