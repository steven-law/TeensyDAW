#include <Arduino.h>
#include <global_stuff.h>
#include <Plugin_3.h>
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
    virtual const char *get_Name() { return name; }
    virtual byte get_ID() { return myID; }
    virtual void noteOn(byte notePlayed, float velocity, byte voice) {}
    virtual void noteOff(byte voice) {}
    virtual void set_parameters(byte row) {}
    virtual void draw_plugin() {}
    virtual void set_presetNr()
    {
        if (enc_moved[PRESET_ENCODER])
        {
            presetNr = constrain(presetNr + encoded[PRESET_ENCODER], 0, NUM_PRESETS - 1);
            change_plugin_row = true;
            draw_plugin();
        }
    }

    virtual byte get_Potentiometer(byte XPos, byte YPos, const char *name)
    {
        int n = XPos + (YPos * NUM_ENCODERS);
        potentiometer[presetNr][n] = constrain(potentiometer[presetNr][n] + encoded[XPos], 0, MIDI_CC_RANGE);
        drawPot(XPos, YPos, potentiometer[presetNr][n], name);
        return potentiometer[presetNr][n];
    }
};
