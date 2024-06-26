#include <Arduino.h>






//  sizes and positions
#define GRID_LENGTH_HOR 256
#define GRID_LENGTH_VERT 192
#define OCTAVE_CHANGE_LEFTMOST 18
#define OCTAVE_CHANGE_RIGHTMOST 20
#define OCTAVE_CHANGE_UP_TOPMOST 2
#define OCTAVE_CHANGE_UP_BOTTOMMOST 3
#define OCTAVE_CHANGE_DOWN_TOPMOST 4
#define OCTAVE_CHANGE_DOWN_BOTTOMMOST 5
#define SEQUENCER_OPTIONS_VERY_RIGHT 18
#define STEP_FRAME_W 16
#define STEP_FRAME_H 16
#define NUM_ENCODERS 4
#define MIDI_CC_RANGE 127
#define MIDI_CC_RANGE_FLOAT 127.00
#define MAX_VOICES 12
#define NUM_PRESETS 8
#define PRESET_ENCODER 3
#define phraseSegmentLength 16 // a variable for the zoomfactor in songmode

// pages
#define TRACK_1_PAGE 0
#define TRACK_2_PAGE 1
#define TRACK_3_PAGE 2
#define TRACK_4_PAGE 3
#define TRACK_5_PAGE 4
#define TRACK_6_PAGE 5
#define TRACK_7_PAGE 6
#define TRACK_8_PAGE 7
#define STARTUPSCREEN_PAGE 8
// #define EMPTY 9
#define SONGMODE_PAGE_1 0
#define SONGMODE_PAGE_2 1
#define SONGMODE_PAGE_3 2
#define SONGMODE_PAGE_4 3
#define SONGMODE_PAGE_5 4
#define SONGMODE_PAGE_6 5
#define SONGMODE_PAGE_7 6
#define SONGMODE_PAGE_8 7
#define SONGMODE_PAGE_9 8
#define SONGMODE_PAGE_10 9
#define SONGMODE_PAGE_11 10
#define SONGMODE_PAGE_12 11
#define SONGMODE_PAGE_13 12
#define SONGMODE_PAGE_14 13
#define SONGMODE_PAGE_15 14
#define SONGMODE_PAGE_16 15

// encoder functions
#define INPUT_FUNCTIONS_FOR_CURSOR 0
#define INPUT_FUNCTIONS_FOR_SEQUENCER 1
#define INPUT_FUNCTIONS_FOR_SEQUENCER_ALT 2
#define INPUT_FUNCTIONS_FOR_ARRANGER 3
#define INPUT_FUNCTIONS_FOR_ARRANGER_ALT 4
#define INPUT_FUNCTIONS_FOR_PLUGIN 5
#define INPUT_FUNCTIONS_FOR_SEQUENCER_MODES 6
#define INPUT_FUNCTIONS_FOR_MIXER1 7
#define INPUT_FUNCTIONS_FOR_MIXER2 8
#define INPUT_FUNCTIONS_FOR_MIXER3 9
#define INPUT_FUNCTIONS_FOR_FX1 10
#define INPUT_FUNCTIONS_FOR_FX2 11
#define INPUT_FUNCTIONS_FOR_FX3 12



#define NUM_STEPS 16
#define NUM_NOTES 12

#define STEP_QUANT 16

//BUTTONS
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1
#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_ROW 4
#define BUTTON_REC 5
#define BUTTON_PLAY 6
#define BUTTON_STOP 7
#define BUTTON_TRACK 8
#define BUTTON_PLUGIN 9
#define BUTTON_SONG 10
#define BUTTON_MIXER 11
#define BUTTON_FX 12
#define BUTTON_SMP 13
#define BUTTON_SHIFT 14
#define BUTTON_ENTER 15
#define BUTTONS_PER_ROW 8
#define NUM_BUTTONS 16
// active Tracks
//  pages
#define ACTIVE_TRACK_1 0
#define ACTIVE_TRACK_2 1
#define ACTIVE_TRACK_3 2
#define ACTIVE_TRACK_4 3
#define ACTIVE_TRACK_5 4
#define ACTIVE_TRACK_6 5
#define ACTIVE_TRACK_7 6
#define ACTIVE_TRACK_8 7

#define ARRANGER_FRAME_H 24
#define TRACK_FRAME_H 24
#define SEQ_GRID_LEFT 30
#define SEQ_GRID_RIGHT (18 * STEP_FRAME_W) - 2
#define SEQ_GRID_TOP 1
#define SEQ_GRID_BOTTOM 12

#define VELOCITY_NOTE_ON 127
#define VELOCITY_NOTE_OFF 0
#define NUM_TRACKS 8

#define NUM_PLUGINS 8

void drawPot(int XPos, byte YPos, int dvalue, const char *dname);
byte getEncodervalue(byte XPos, byte YPos, const char *name, byte oldValue);
void draw_sequencer_option(byte x, const char *nameshort, int value, byte enc, const char *pluginName);
void draw_Value(byte index, byte XPos, byte YPos, byte offest_X, int offset_Y, int value, int color, bool drawRect);
void drawEnvelope(byte YPos, byte attack, byte decay, byte sustain, byte release);




