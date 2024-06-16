
#include <Arduino.h>
#include <global_stuff.h>
#include <pluginClass.h>
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern bool buttonPressed[NUM_BUTTONS];
void drawPot(int XPos, byte YPos, int dvalue, const char *dname);

void PluginControll::setup() {}
void PluginControll::noteOn(byte notePlayed, float velocity, byte voice) {}
void PluginControll::noteOff(byte notePlayed, byte voice) {}
void PluginControll::set_parameters(byte row) {}
void PluginControll::draw_plugin() {}

void PluginControll::set_presetNr()
{
    if (enc_moved[PRESET_ENCODER])
    {
        presetNr = constrain(presetNr + encoded[PRESET_ENCODER], 0, NUM_PRESETS - 1);
        change_plugin_row = true;
        draw_plugin();
    }
}
byte PluginControll::get_Potentiometer(byte XPos, byte YPos, const char *name)
{
    int n = XPos + (YPos * NUM_ENCODERS);
    potentiometer[presetNr][n] = constrain(potentiometer[presetNr][n] + encoded[XPos], 0, MIDI_CC_RANGE);
    drawPot(XPos, YPos, potentiometer[presetNr][n], name);
    return potentiometer[presetNr][n];
}

// ASSIGN audio functions
void PluginControll::assign_voice_waveform(byte value) {} // // Normale virtuelle Funktion (kann von Subklassen überschrieben werden)
void PluginControll::assign_voice_amplitude(byte value) {}
void PluginControll::set_voice_waveform(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_voice_waveform(get_Potentiometer(XPos, YPos, name));
    }
}
void PluginControll::set_voice_amplitude(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_voice_amplitude(get_Potentiometer(XPos, YPos, name));
    }
}

// filter
void PluginControll::assign_filter_frequency(byte value) {}
void PluginControll::assign_filter_resonance(byte value) {}
void PluginControll::assign_filter_sweep(byte value) {}
void PluginControll::assign_filter_type(byte mixerchannel) {}
void PluginControll::set_filter_frequency(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_filter_frequency(get_Potentiometer(XPos, YPos, name));
    }
}
void PluginControll::set_filter_resonance(byte XPos, byte YPos, const char *name, float min, float max)
{
    if (enc_moved[XPos])
    {
        assign_filter_resonance(get_Potentiometer(XPos, YPos, name));
    }
}
void PluginControll::set_filter_sweep(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_filter_sweep(get_Potentiometer(XPos, YPos, name));
    }
}
void PluginControll::set_filter_type(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_filter_type(constrain(get_Potentiometer(XPos, YPos, name), 0, 3));
    }
}

// envelope
void PluginControll::assign_envelope_attack(byte value, int max) {}
void PluginControll::assign_envelope_decay(byte value, int max) {}
void PluginControll::assign_envelope_sustain(byte value) {}
void PluginControll::assign_envelope_release(byte value, int max) {}
void PluginControll::set_envelope_attack(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        assign_envelope_attack(get_Potentiometer(XPos, YPos, name), max);
    }
}
void PluginControll::set_envelope_decay(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        assign_envelope_decay(get_Potentiometer(XPos, YPos, name), max);
    }
}
void PluginControll::set_envelope_sustain(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        assign_envelope_sustain(get_Potentiometer(XPos, YPos, name));
    }
}
void PluginControll::set_envelope_release(byte XPos, byte YPos, const char *name, int min, int max)
{
    if (enc_moved[XPos])
    {
        assign_envelope_release(get_Potentiometer(XPos, YPos, name), max);
    }
}
void PluginControll::set_envelope_ADSR(byte YPos, int maxA, int maxD, int maxR) {}

// mixer
void PluginControll::assign_mixer_gain(byte value, byte channel) {}
void PluginControll::set_mixer_gain(byte XPos, byte YPos, const char *name)
{
    if (enc_moved[XPos])
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        assign_mixer_gain(get_Potentiometer(XPos, YPos, name), n);
    }
}
// SET audio function

// filter functions
