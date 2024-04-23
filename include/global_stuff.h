#define STEP_FRAME_W 16
#define STEP_FRAME_H 16
#define NUM_ENCODERS 4
#define MIDI_CC_RANGE 127
float *note_frequency;

// Encoder Pins
bool enc_moved[4]{0, 0, 0, 0};
int encoded[4];