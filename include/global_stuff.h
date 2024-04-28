#define STEP_FRAME_W 16
#define STEP_FRAME_H 16
#define NUM_ENCODERS 4
#define MIDI_CC_RANGE 127
#define MIDI_CC_RANGE_FLOAT 127.00
#define MAX_VOICES 12
const byte PLUGIN1_PAGE1 = 100;
const byte PLUGIN2_PAGE1 = 105;
const byte PLUGIN3_PAGE1 = 110;
const byte NUM_PLUGINS = 3;
const byte VELOCITY_NOTE_ON = 127;
const byte VELOCITY_NOTE_OFF = 0;
const byte NUM_TRACKS =8;

float *note_frequency;

// Encoder Pins
bool enc_moved[4]{0, 0, 0, 0};
int encoded[4];

