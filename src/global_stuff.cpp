#include <Arduino.h>
#include "global_stuff.h"

// individual trackcolors
int trackColor[9]{6150246, 8256638, 1095334, 12643941, 2583100, 9365295, 12943157, 5678954, ILI9341_WHITE};

int tuning = 440;
float *note_frequency;

// Encoder Pins
bool enc_moved[4]{0, 0, 0, 0};
int encoded[4];

bool buttonPressed[NUM_BUTTONS];



int pixelTouchX = 0;
int gridTouchY = 0;
byte lastPotRow = 0;
byte encoder_function = 0;
bool change_plugin_row;


byte active_track = ACTIVE_TRACK_1;

byte arrangerpage;



