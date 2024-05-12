#include <Arduino.h>
#include "Track.h"
extern byte VELOCITY_NOTE_ON;
extern byte VELOCITY_NOTE_OFF;


 void Track::play_SeqMode0(byte cloock)
    {

        for (int v = 0; v < MAX_VOICES; v++)
        {
            if (array[clip_to_play[internal_clock_bar]][cloock][v] < NO_NOTE)
            {
                if (!note_is_on[v])
                {
                    noteToPlay[v] = array[clip_to_play[internal_clock_bar]][cloock][v] + noteOffset[internal_clock_bar];
                    note_is_on[v] = true;
                    Masterout->noteOn(noteToPlay[v], VELOCITY_NOTE_ON, MIDI_channel_out, v);
                }
            }
            if (array[clip_to_play[internal_clock_bar]][cloock][v] == NO_NOTE)
            {
                if (note_is_on[v])
                {
                    note_is_on[v] = false;
                    Masterout->noteOff(noteToPlay[v], VELOCITY_NOTE_OFF, MIDI_channel_out, v);

                    // Serial.printf("OFF   tick: %d, voice: %d, note: %d\n", cloock, v, noteToPlay[v]);
                }
            }
        }
    }
