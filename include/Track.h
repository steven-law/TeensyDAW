#include <Arduino.h>
#include <ILI9341_t3n.h>
#include <ili9341_t3n_font_Arial.h> // from ILI9341_t3

// #include "global_stuff.h"
extern int trackColor[9];
// Encoder Pins
extern bool enc_moved[4];
extern int encoded[4];
extern bool change_plugin_row;
extern byte encoder_function;
extern int pixelTouchX;
extern int gridTouchY;
extern byte active_track;
extern byte arrangerpage;
class Track
{
public:
    void play_SeqMode0(byte cloock);
    void play_SeqMode1(byte cloock);
    void set_SeqMode1_parameters(byte row);
    void set_SeqMode1_value(byte XPos, byte YPos, const char *name, int min, int max);
    void draw_SeqMode1();

    void play_SeqMode2(byte cloock);
    void set_SeqMode2_parameters(byte row);
    void set_SeqMode2_value(byte XPos, byte YPos, const char *name, int min, int max);
    void draw_SeqMode2();
#define OCTAVE_CHANGE_TEXT 3
#define NOTES_PER_OCTAVE 12
#define SEQUENCER_OPTIONS_VERY_RIGHT 18
#define BARS_PER_PAGE 16
#define NO_NOTE 128
#define MAX_TICKS 96
#define MAX_CLIPS 9
#define NUM_USER_CLIPS 7
    ILI9341_t3n *tft; // Pointer to the display object

    byte MIDI_channel_in;
    byte MIDI_channel_out;
    byte octave = 4;
    byte my_Arranger_Y_axis;
    byte sequencer_mode = 0;
    byte note;
    byte noteToPlay[MAX_VOICES];
    int pixelOn_X;
    int pixelOn_Y;
    byte clip_to_edit = 0;
    byte clip_to_play[255];
    int noteOffset[255];
    byte tick;
    Output *Masterout;
    byte internal_clock = 0;
    bool internal_clock_is_on = false;
    byte internal_clock_bar = 0;
    byte step_division = 1;
    byte sequence_length = MAX_TICKS;
    byte array[MAX_CLIPS][MAX_TICKS + 1][MAX_VOICES];
    byte active_voice = 0;
    byte search_free_voice = 0;
    const char *pluginNames[32] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16",
                                   "Strng", "1OSC", "2FM", "MDrm", "Drum", "Draw", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    bool note_is_on[MAX_VOICES] = {true, true, true, true, true, true, true, true, true, true, true, true};
    bool ready_for_NoteOff[MAX_VOICES] = {false, false, false, false, false, false, false, false, false, false, false, false};
    int encoder_colour[NUM_ENCODERS] = {ILI9341_BLUE, ILI9341_RED, ILI9341_GREEN, ILI9341_WHITE};
    const char *noteNames[12]{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    byte SeqMod_1_Poti[2];
    byte SeqMod_2_Poti[16];
    byte SeqMod_3_Poti[12];
    byte maxVal;
    byte mixGainPot;
    float mixGain = 1;
    byte mixDryPot;
    float mixDry = 1;
    byte mixFX1Pot;
    float mixFX1 = 0;
    byte mixFX2Pot;
    float mixFX2 = 0;
    byte mixFX3Pot;
    float mixFX3 = 0;
    byte mute_norm_solo_pot = 1;
    bool muted;
    bool soloed;

    Track(ILI9341_t3n *display, Output *OutPut, byte Y, byte cI, byte cO)
    {

        tft = display;
        Masterout = OutPut;
        my_Arranger_Y_axis = Y;
        MIDI_channel_in = cI;
        MIDI_channel_out = cO;
        for (int c = 0; c < MAX_CLIPS; c++)
        {
            for (int t = 0; t < MAX_TICKS + 1; t++)
            {
                for (int v = 0; v < MAX_VOICES; v++)
                {
                    array[c][t][v] = NO_NOTE;
                }
            }
        }
        for (int i = 0; i < 255; i++)
        {
            clip_to_play[i] = 8;
            noteOffset[i] = 0;
        }
    }
    // sequencer Modes
    void play_sequencer_mode(byte cloock, byte start, byte end)
    {

        if (cloock % step_division == 0)
        {
            internal_clock++;
            internal_clock_is_on = true;
        }
        else
            internal_clock_is_on = false;
        if (internal_clock == sequence_length)
        {
            internal_clock = 0;
            internal_clock_bar++;
        }
        if (internal_clock_bar == end)
            internal_clock_bar = start;
        // Serial.printf("bar: %d, tick: %d\n", internal_clock_bar, internal_clock);
        //  Serial.println(internal_clock_bar);
        if (internal_clock_is_on)
        {
            if (!muted || soloed)
            {
                if (sequencer_mode == 0)
                {
                    play_SeqMode0(internal_clock);
                }
                if (sequencer_mode == 1)
                {
                    play_SeqMode1(internal_clock);
                }
                if (sequencer_mode == 2)
                {
                    play_SeqMode2(internal_clock);
                }
            }
        }
    }
    void set_SeqMode_parameters(byte row)
    {
        if (sequencer_mode == 1)
            set_SeqMode1_parameters(row);
        if (sequencer_mode == 2)
            set_SeqMode2_parameters(row);
    }
    void draw_sequencer_modes(byte mode)
    {
        clearWorkSpace();
        change_plugin_row = true;
        if (mode == 1)
            draw_SeqMode1();
        if (mode == 2)
            draw_SeqMode2();
    }
    /*void play_SeqMode0(byte cloock)
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
    }*/

    // sequencer options:
    // octave
    void set_octave(byte n)
    {
        if (enc_moved[n])
        {
            octave = constrain(octave + encoded[n], 0, 9);
            Serial.println(octave);
            drawOctaveNumber();
            clear_notes_in_grid();
            draw_notes_in_grid();
            enc_moved[n] = false;
        }
    }
    void drawOctaveNumber()
    {
        // draw the octave number
        tft->fillRect(STEP_FRAME_W * 18 + 1, STEP_FRAME_H * OCTAVE_CHANGE_TEXT, STEP_FRAME_W * 2, STEP_FRAME_H * 1 + 1, ILI9341_DARKGREY);
        tft->setCursor(STEP_FRAME_W * 18 + 11, STEP_FRAME_H * OCTAVE_CHANGE_TEXT);
        tft->setFont(Arial_16);
        tft->setTextColor(ILI9341_GREEN);
        tft->setTextSize(1);
        tft->print(octave);
    }
    byte get_octave()
    {
        return octave;
    }
    // sequencer Mode
    void set_sequencer_mode(byte n)
    {
        if (enc_moved[n])
        {
            sequencer_mode = constrain(sequencer_mode + encoded[n], 0, 16);
            draw_sequencer_mode(n);
            enc_moved[n] = false;
        }
    }
    void draw_sequencer_mode(byte n)
    {
        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "sMod", sequencer_mode, n);
    }
    byte get_sequencer_mode()
    {
        return sequencer_mode;
    }
    // step division
    void set_step_division(byte n)
    {
        if (enc_moved[n])
        {
            step_division = constrain(step_division + encoded[n], 1, 16);
            draw_step_division(n);
            enc_moved[n] = false;
        }
    }
    void draw_step_division(byte n)
    {
        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "sDiv", step_division, n);
    }
    byte get_step_division()
    {
        return step_division;
    }
    // midi channel out
    void set_MIDI_channel_out(byte n)
    {
        if (enc_moved[n])
        {
            MIDI_channel_out = constrain(MIDI_channel_out + encoded[n], 1, 32);
            Masterout->set_active_plugin_for_track(my_Arranger_Y_axis - 1, MIDI_channel_out);
            draw_MIDI_channel_out(n);
            Serial.println(MIDI_channel_out);
            enc_moved[n] = false;
        }
    }
    void draw_MIDI_channel_out(byte n)
    {
        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "MdCh", MIDI_channel_out, n, pluginNames[MIDI_channel_out]);
    }
    byte get_MIDI_channel_out()
    {
        return MIDI_channel_out;
    }
    // sequence length
    void set_sequence_length(byte n)
    {

        if (enc_moved[n])
        {
            sequence_length = constrain(sequence_length + encoded[n], 2, MAX_TICKS);
            draw_sequence_length(n);
            enc_moved[n] = false;
        }
    }
    void draw_sequence_length(byte n)
    {
        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "tiks", sequence_length, n);
    }
    byte get_sequence_length()
    {
        return sequence_length;
    }
    // clip to edit
    void set_clip_to_edit(byte n)
    {
        if (enc_moved[n])
        {
            clip_to_edit = constrain(clip_to_edit + encoded[n], 0, NUM_USER_CLIPS);
            draw_clip_to_edit(n);
            clear_notes_in_grid();
            draw_notes_in_grid();
            enc_moved[n] = false;
        }
    }
    void draw_clip_to_edit(byte n)
    {
        for (int ClipNr = 0; ClipNr < 8; ClipNr++)
        {
            tft->drawRect(STEP_FRAME_W * 2 * ClipNr + STEP_FRAME_W * 2, STEP_FRAME_H * 13 + 1, STEP_FRAME_W * 2, STEP_FRAME_H - 1, ILI9341_DARKGREY);
        }
        tft->drawRect(STEP_FRAME_W * 2 * clip_to_edit + STEP_FRAME_W * 2, STEP_FRAME_H * 13 + 1, STEP_FRAME_W * 2, STEP_FRAME_H - 1, encoder_colour[n]);
    }
    byte get_clip_to_edit()
    {
        return clip_to_edit;
    }

    // helpers
    void draw_sequencer_option(byte x, const char *nameshort, int value, byte enc, const char *pluginName = 0)
    {

        int color;
        if (encoder_function == INPUT_FUNCTIONS_FOR_ARRANGER)
            color = my_Arranger_Y_axis - 1;
        else
            color = active_track;
        byte y = 6 + (enc * 2);
        // show function
        tft->setCursor(STEP_FRAME_W * x + 2, STEP_FRAME_H * (y - 1) + 6);
        tft->setFont(Arial_8);
        tft->setTextColor(trackColor[color]);
        tft->setTextSize(1);
        tft->print(nameshort);
        // show value
        tft->drawRect(STEP_FRAME_W * x, STEP_FRAME_H * y, STEP_FRAME_W * 2, STEP_FRAME_H, encoder_colour[enc]);
        tft->fillRect(STEP_FRAME_W * x + 1, STEP_FRAME_H * y + 1, STEP_FRAME_W * 2 - 2, STEP_FRAME_H - 2, ILI9341_DARKGREY);
        tft->setCursor(STEP_FRAME_W * x + 8, STEP_FRAME_H * y + 3);
        tft->setFont(Arial_10);
        tft->setTextColor(ILI9341_BLACK);
        tft->setTextSize(1);
        if (pluginName != 0)
        {
            tft->setCursor(STEP_FRAME_W * x + 2, STEP_FRAME_H * y + 4);
            tft->setFont(Arial_8);
            tft->print(pluginName);
        }
        else
            tft->print(value);
    }
    void draw_sequencer_screen()
    {
        drawOctaveNumber();
        draw_sequence_length(0);
        draw_step_division(1);
        draw_sequencer_mode(2);
        draw_MIDI_channel_out(3);
    }
    // sequencer note input stuff
    void set_note_on_tick(int x, byte y)
    {
        note = (y - SEQ_GRID_TOP) + (octave * NOTES_PER_OCTAVE);
        tick = (x - SEQ_GRID_LEFT) / 2;
        pixelOn_X = x;
        pixelOn_Y = y * STEP_FRAME_H;
        check_for_free_voices(tick, note);
        array[clip_to_edit][tick][active_voice] = note;

        Serial.printf("Track: %d , set Note: %d  to tick: %d with Voice : %d \n", active_track, array[clip_to_edit][tick][active_voice], tick, active_voice);
        // Serial.printf("Voice 1: %d \nVoice 2: %d \nVoice 3: %d \nVoice 4: %d \n", array[tick][0], array[tick][1], array[tick][2], array[tick][3]);
        Serial.println(x);
        Serial.println(tick);
        draw_note_on_tick(pixelOn_X);
    }
    void check_for_free_voices(byte onTick, byte cnote)
    {
        for (int i = 0; i < MAX_VOICES; i++)
        {
            if (array[clip_to_edit][onTick][i] == cnote)
            {
                array[clip_to_edit][onTick][i] = NO_NOTE;
                note = NO_NOTE;
                search_free_voice = i;
            }
        }
        if (array[clip_to_edit][onTick][search_free_voice] < NO_NOTE)
            search_free_voice++;
        if (search_free_voice == MAX_VOICES)
            search_free_voice = 0;
        active_voice = search_free_voice;
    }
    void clear_notes_on_tick(byte cl_X)
    {
        for (int i = 0; i < NOTES_PER_OCTAVE; i++)
        {
            for (int w = -4; w < 5; w++)
            {
                tft->drawPixel(cl_X + 2, (STEP_FRAME_H * (i + 1)) + w + 8, ILI9341_DARKGREY);
                tft->drawPixel(cl_X + 3, (STEP_FRAME_H * (i + 1)) + w + 8, ILI9341_DARKGREY);
            }
        }
    }
    void draw_note_on_tick(byte dr_X)
    {
        clear_notes_on_tick(dr_X);
        int stepcolor = trackColor[active_track] + (clip_to_edit * 20);
        int thisTick = (dr_X - SEQ_GRID_LEFT) / 2;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            if (array[clip_to_edit][thisTick][i] >= octave * NOTES_PER_OCTAVE && array[clip_to_edit][thisTick][i] < (octave + 1) * NOTES_PER_OCTAVE)
            {
                if (array[clip_to_edit][thisTick][i] < NO_NOTE)
                {
                    byte PixelOn_Y = ((array[clip_to_edit][thisTick][i] - (octave * NOTES_PER_OCTAVE)) + 1) * STEP_FRAME_H;
                    int dr_pixel_X = dr_X;
                    for (int w = -4; w < 5; w++)
                    {
                        tft->drawPixel(dr_pixel_X + 2, PixelOn_Y + w + 8, stepcolor);
                        tft->drawPixel(dr_pixel_X + 3, PixelOn_Y + w + 8, stepcolor);
                    }
                }
            }
        }
    }
    void clear_notes_in_grid()
    {
        for (int i = 0; i < MAX_TICKS; i++)
        {
            clear_notes_on_tick((i * 2) + SEQ_GRID_LEFT);
        }
    }
    void draw_notes_in_grid()
    {
        for (int i = 0; i < MAX_TICKS; i++)
        {
            draw_note_on_tick((i * 2) + SEQ_GRID_LEFT);
        }
    }

    // stepsequencer
    void drawStepSequencerStatic()
    {
        clearWorkSpace();
        draw_Notenames();
        drawOctaveTriangle();
        draw_Clipselector();
        // draw the Main Grid
        for (int i = 0; i < 17; i++)
        { // vert Lines
            int step_Frame_X = i * 12;
            tft->drawFastVLine(step_Frame_X + STEP_FRAME_W * 2, STEP_FRAME_H, GRID_LENGTH_VERT, ILI9341_WHITE); //(x, y-start, length, color)
            if (i % 4 == 0)
            {
                tft->drawFastVLine((i * 12) + 32, STEP_FRAME_H, STEP_FRAME_H * 12, ILI9341_LIGHTGREY); //(x, y-start, y-length, color)
            }
        }
        for (int i = 0; i < 13; i++)
        { // hor lines
            int step_Frame_Y = i * 16;
            tft->drawFastHLine(STEP_FRAME_W * 2, step_Frame_Y + STEP_FRAME_H, NUM_STEPS * 12, ILI9341_WHITE); //(x-start, y, length, color)
        }
        tft->asyncUpdateActive();
    }
    void draw_Notenames()
    {
        for (int n = 0; n < NUM_NOTES; n++)
        { // hor notes
            tft->fillRect(STEP_FRAME_W, STEP_FRAME_H * n + STEP_FRAME_H, STEP_FRAME_W, STEP_FRAME_H, trackColor[my_Arranger_Y_axis - 1]);
            tft->setCursor(20, STEP_FRAME_H * n + 20);
            tft->setFont(Arial_8);
            tft->setTextColor(ILI9341_BLACK);
            tft->setTextSize(1);
            tft->print(noteNames[n]);
        }
    }
    void drawOctaveTriangle()
    {
        // draw Octavebuttons
        int leftmost = STEP_FRAME_W * OCTAVE_CHANGE_LEFTMOST;
        int rightmost = STEP_FRAME_W * OCTAVE_CHANGE_RIGHTMOST;
        int UP_topmost = STEP_FRAME_H * OCTAVE_CHANGE_UP_TOPMOST;
        int UP_bottommost = STEP_FRAME_H * OCTAVE_CHANGE_UP_BOTTOMMOST;
        int DOWN_topmost = STEP_FRAME_H * OCTAVE_CHANGE_DOWN_TOPMOST;
        int DOWN_bottommost = STEP_FRAME_H * OCTAVE_CHANGE_DOWN_BOTTOMMOST;
        tft->fillRect(leftmost + 1, STEP_FRAME_H * 2, STEP_FRAME_W * 2, STEP_FRAME_H * 3, ILI9341_DARKGREY);
        tft->fillTriangle(leftmost + 1, UP_bottommost, rightmost, UP_bottommost, leftmost + STEP_FRAME_W, UP_topmost, ILI9341_LIGHTGREY);        // octave arrow up
        tft->fillTriangle(leftmost + 1, DOWN_topmost, rightmost - 2, DOWN_topmost, leftmost + STEP_FRAME_W, DOWN_bottommost, ILI9341_LIGHTGREY); // x1, y1, x2, y2, x3, y3
    }
    void draw_Clipselector()
    {
        for (int ClipNr = 0; ClipNr < 8; ClipNr++)
        {
            tft->fillRect(STEP_FRAME_W * 2 * ClipNr + STEP_FRAME_W * 2 + 1, STEP_FRAME_H * 13 + 2, STEP_FRAME_W * 2 - 2, STEP_FRAME_H - 3, trackColor[active_track] + (ClipNr * 20));
            tft->setCursor(STEP_FRAME_W * 2 * ClipNr + STEP_FRAME_W * 2 + 4, STEP_FRAME_H * 13 + 4);
            tft->setFont(Arial_8);
            tft->setTextColor(ILI9341_BLACK);
            tft->setTextSize(1);
            tft->print("Clip ");
            tft->print(ClipNr);
        }
    }

    //----------------------------------------------------------------
    // arranger stuff
    // clip to play
    void set_clip_to_play(byte n, byte b)
    {
        if (gridTouchY == my_Arranger_Y_axis)
        {
            byte when = ((b - SEQ_GRID_LEFT) / STEP_FRAME_W) + (BARS_PER_PAGE * (arrangerpage));
            if (enc_moved[n])
            {
                clip_to_play[when] = constrain(clip_to_play[when] + encoded[n], 0, NUM_USER_CLIPS + 1);
                draw_clip_to_play(n, when);
                enc_moved[n] = false;
            }
        }
    }
    void draw_clip_to_play(byte n, byte b)
    {

        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "clNr", clip_to_play[b], n);
        draw_arrangment_line(n, b);
    }
    void drawsongmodepageselector()
    {
        // draw 16 rects of 16x16px in the 13th row
        for (int pages = 2; pages < 18; pages++)
        {
            // drawActiveRect(pages, 13, 1, 1, selectPage == pages + 8, "", ILI9341_LIGHTGREY);
            tft->drawRect(STEP_FRAME_W * pages, STEP_FRAME_H * 13 + 4, STEP_FRAME_W, STEP_FRAME_H, ILI9341_WHITE);
            tft->setFont(Arial_8);
            tft->setTextColor(ILI9341_WHITE);
            tft->setCursor(STEP_FRAME_W * pages + 3, STEP_FRAME_H * 13 + 8);
            tft->print((pages - 1));
        }
    }
    void gridSongMode(int songpageNumber)
    { // static Display rendering
        // int page_phrase_start = songpageNumber * 16;
        // int page_phrase_end = (songpageNumber + 1) * 16;
        drawsongmodepageselector();
        // drawActiveRect(18, 3, 2, 2, false, "clear", ILI9341_RED);

        // vertical pointer Lines
        int shownLines = 257 / phraseSegmentLength;
        for (int f = 0; f < shownLines; f++)
        {                                                                                                 // do this for all phrases
            tft->drawFastVLine((f * phraseSegmentLength) + 32, STEP_FRAME_H + 4, STEP_FRAME_H * 12, 360); //(x, y-start, y-length, color)
            if (f % 4 == 0)
            {
                tft->drawFastVLine((f * phraseSegmentLength) + 32, STEP_FRAME_H + 4, STEP_FRAME_H * 12, 370); //(x, y-start, y-length, color)
            }
        }
    }

    void draw_arrangment_lines(byte n, byte b) // b= active page
    {
        for (int i = 0; i < 16; i++)
        {
            draw_arrangment_line(n, i + (BARS_PER_PAGE * (b - SONGMODE_PAGE_1)));
            Serial.printf("active page = %d, which bar = %d\n", b, i + (16 * (b - SONGMODE_PAGE_1)));
        }
    }
    void draw_arrangment_line(byte n, byte b) // b= 0-255; which bar
    {
        if (clip_to_play[b] == 8)
        {
            for (int thickness = -7; thickness < 7; thickness++)
            {
                tft->drawFastHLine(((b - (16 * arrangerpage)) * STEP_FRAME_W + STEP_FRAME_W * 2) + 1, ((my_Arranger_Y_axis)*TRACK_FRAME_H + thickness) + 12, STEP_FRAME_W - 1, ILI9341_DARKGREY); //(x-start, y, length, color)
            }
        }
        else
        {
            // for other clips
            for (int thickness = -7; thickness < 7; thickness++)
            {
                tft->drawFastHLine(((b - (16 * arrangerpage)) * STEP_FRAME_W + STEP_FRAME_W * 2) + 1, ((my_Arranger_Y_axis)*TRACK_FRAME_H + thickness) + 12, STEP_FRAME_W - 1, trackColor[my_Arranger_Y_axis - 1] + (clip_to_play[b] * 20)); //(x-start, y, length, color)
            }
            draw_clipNr_arranger(n, b);
            draw_offset_arranger(n, b);
        }
    }
    void draw_clipNr_arranger(byte n, byte b)
    {
        // draw clipnumber in the arranger
        tft->setFont(Arial_8);
        tft->setTextColor(ILI9341_BLACK);
        tft->setCursor((b - (16 * arrangerpage)) * STEP_FRAME_W + STEP_FRAME_W * 2 + 2, (my_Arranger_Y_axis)*TRACK_FRAME_H + 6);
        tft->print(clip_to_play[b]);
    }
    byte get_clip_to_play(byte when)
    {
        return clip_to_play[when];
    }
    // note offset / note transpose
    void set_note_offset(byte n, int b)
    {
        if (gridTouchY == my_Arranger_Y_axis)
        {
            byte when = ((b - SEQ_GRID_LEFT) / STEP_FRAME_W) + (BARS_PER_PAGE * arrangerpage);
            if (enc_moved[n])
            {
                noteOffset[when] = constrain(noteOffset[when] + encoded[n], -24, +24);
                draw_noteOffset(n, when);
                enc_moved[n] = false;
            }
        }
    }
    void draw_noteOffset(byte n, int b)
    {
        draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "ofSet", noteOffset[b], n);
        draw_arrangment_line(n, b);
    }
    void draw_offset_arranger(byte n, byte b)
    {
        int xoffset;
        if (noteOffset[b] < 0)
            xoffset = 1;
        else
            xoffset = 5;
        // draw clipnumber in the arranger
        tft->setFont(Arial_8);
        tft->setTextColor(ILI9341_BLACK);
        tft->setCursor((b - (16 * arrangerpage)) * STEP_FRAME_W + STEP_FRAME_W * 2 + xoffset, (my_Arranger_Y_axis)*TRACK_FRAME_H + 11);
        tft->print(noteOffset[b]);
    }
    // display stuff
    //  songmode / arranger
};
