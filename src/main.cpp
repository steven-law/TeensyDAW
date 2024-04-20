#include <Arduino.h>
#include <ILI9341_t3n.h>
#include <ili9341_t3n_font_Arial.h> // from ILI9341_t3
#include <SPI.h>
#include <Adafruit_Keypad.h>
// #include <Key.h>
// #include <Keypad.h>
#include <RotaryEncoder.h>
#include <Wire.h>
#include <Bounce2.h>
#include <MIDI.h>
// #include <USB-MIDI.h>
// #include <MIDIUSB.h>
// MIDI_CREATE_DEFAULT_INSTANCE();
// #include <Adafruit_MotorShield.h>

// sizes and positions
#define STEP_QUANT 16
#define STEP_FRAME_W 16
#define STEP_FRAME_H 16
#define ARRANGER_FRAME_H 24
#define TRACK_FRAME_H 24
#define SEQ_GRID_LEFT 30
#define SEQ_GRID_RIGHT (18 * STEP_FRAME_W) - 2
#define SEQ_GRID_TOP 1
#define SEQ_GRID_BOTTOM 12
#define GRID_LENGTH_HOR 256
#define GRID_LENGTH_VERT 192

#define POSITION_BAR_BUTTON 5
#define DOT_OFFSET_X 40 // STEP_FRAME_W * 2 + 8
#define DOT_OFFSET_Y 24 // STEP_FRAME_H + 8
#define DOT_RADIUS 5
#define POSITION_START_LOOP_BUTTON 16
#define POSITION_END_LOOP_BUTTON 18
#define SONG_POSITION_POINTER_Y 228
#define STEP_POSITION_POINTER_Y 236
#define GRID_POSITION_POINTER_Y 232
#define POSITION_POINTER_THICKNESS 3
#define POTPICKUP 3

#define NUM_STEPS 16
#define NUM_NOTES 12
#define MAX_VOICES 4
#define MAX_TICKS 96
#define MAX_CLIPS 9
#define NUM_USER_CLIPS 7
#define NO_NOTE 128
#define NUM_TRACKS 8
#define VELOCITY_NOTE_OFF 0
#define VELOCITY_NOTE_ON 127

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
#define SONGMODE_PAGE_1 10
#define SONGMODE_PAGE_2 11
#define SONGMODE_PAGE_3 12
#define SONGMODE_PAGE_4 13
#define SONGMODE_PAGE_5 14
#define SONGMODE_PAGE_6 15
#define SONGMODE_PAGE_7 16
#define SONGMODE_PAGE_8 17
#define SONGMODE_PAGE_9 18
#define SONGMODE_PAGE_10 19
#define SONGMODE_PAGE_11 20
#define SONGMODE_PAGE_12 21
#define SONGMODE_PAGE_13 22
#define SONGMODE_PAGE_14 23
#define SONGMODE_PAGE_15 24
#define SONGMODE_PAGE_16 25

// encoder functions
#define INPUT_FUNCTIONS_FOR_CURSOR 0
#define INPUT_FUNCTIONS_FOR_SEQUENCER 1
#define INPUT_FUNCTIONS_FOR_SEQUENCER_ALT 2
#define INPUT_FUNCTIONS_FOR_ARRANGER 3
#define INPUT_FUNCTIONS_FOR_ARRANGER_ALT 4

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
bool buttonPressed[NUM_BUTTONS];
bool cursor_moved;
bool otherCtrlButtons = true;
int pixelTouchX = 0;
int gridTouchY = 0;
byte active_page = TRACK_1_PAGE;
byte active_track = ACTIVE_TRACK_1;
byte lastPotRow = 0;

// individual trackcolors
int trackColor[9]{6150246, 8256638, 1095334, 12643941, 2583100, 9365295, 12943157, 5678954, ILI9341_WHITE};
// Teensy 4.1 PINOUT
// Pinout for screen
#define TIRQ_PIN 15                                                                  // alternate Pins: any digital pin
#define CS_PIN 14                                                                    // alternate Pins: any digital pin
#define TFT_DC 9                                                                     // alternate Pins 9, 10, 20, 21
#define TFT_CS 10                                                                    // alternate Pins 9, 15, 20, 21
#define TFT_RST 255                                                                  // 255 = unused, connect to 3.3V
#define TFT_MOSI 11                                                                  // shareable
#define TFT_SCK 13                                                                   // shareable
#define TFT_MISO 12                                                                  // shareable
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO); // initiate TFT-Srceen

// Let's allocate the frame buffer ourself.
DMAMEM uint16_t tft_frame_buffer[ILI9341_TFTWIDTH * ILI9341_TFTHEIGHT];
#define ENABLE_ILI9341_FRAMEBUFFER
// /keypad button Pins

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
// define the cymbols on the buttons of the keypads
char keys[ROWS][COLS] = {
    {'P', 'H', 'O', 'G'},
    {'M', 'F', 'N', 'E'},
    {'L', 'C', 'K', 'D'},
    {'J', 'A', 'I', 'B'}};
byte rowPins[ROWS] = {37, 36, 35, 34}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {41, 38, 39, 40}; // connect to the column pinouts of the keypad
Adafruit_Keypad kpd = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//  Encoder Pins
#define NUM_ENCODERS 4
RotaryEncoder Enc1(31, 32);
RotaryEncoder Enc2(27, 28);
RotaryEncoder Enc3(24, 25);
RotaryEncoder Enc4(2, 3);
RotaryEncoder *allEncoders[NUM_ENCODERS]{&Enc1, &Enc2, &Enc3, &Enc4};
// Encoder Pins
bool enc_moved[4]{0, 0, 0, 0};
int encoded[4];
long oldEnc[4] = {-999, -999, -999, -999};
// Encoder Buttons
const uint8_t BUTTON_PINS[NUM_ENCODERS] = {30, 29, 26, 4};
Bounce *encButtons = new Bounce[NUM_ENCODERS];
// micros object for midiclock
elapsedMicros msecsclock;

class Background
{
public:
#define OCTAVE_CHANGE_LEFTMOST 18
#define OCTAVE_CHANGE_RIGHTMOST 20
#define OCTAVE_CHANGE_UP_TOPMOST 2
#define OCTAVE_CHANGE_UP_BOTTOMMOST 3
#define OCTAVE_CHANGE_DOWN_TOPMOST 4
#define OCTAVE_CHANGE_DOWN_BOTTOMMOST 5
#define POSITION_ARR_BUTTON 18
#define POSITION_BPM_BUTTON 11
#define POSITION_SCALE_BUTTON 16
#define POSITION_LOAD_BUTTON 15
#define POSITION_SAVE_BUTTON 13
#define POSITION_STOP_BUTTON 10
#define POSITION_PLAY_BUTTON 8
#define POSITION_RECORD_BUTTON 7
#define STARTUPSCREEN 0
#define STEP_SEQUENCER_VIEW 1
#define ARRANGMENT_VIEW 2
  ILI9341_t3n *tft; // Pointer to the display object
  byte active_page = 0;
  const char *noteNames[12]{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
  byte encoder_function = 0;
  Background(ILI9341_t3n *display)
  {
    tft = display;
  }
  // general
  void clearWorkSpace()
  {                                                                                                        // clear the whole grid from Display
    tft->fillRect(STEP_FRAME_W, STEP_FRAME_H, STEP_FRAME_W * 21, STEP_FRAME_H * 13 + 4, ILI9341_DARKGREY); // Xmin, Ymin, Xlength, Ylength, color
                                                                                                           // tft->fillRect(STEP_FRAME_W, STEP_FRAME_H, STEP_FRAME_W, STEP_FRAME_H * 12, ILI9341_DARKGREY);
  }
  void startUpScreen()
  {

    // static Display rendering
    tft->fillScreen(ILI9341_DARKGREY);

    tft->setFont(Arial_9);

    // songmode button
    tft->setTextColor(ILI9341_BLACK);
    tft->fillRect(1, 1, 15, ARRANGER_FRAME_H, ILI9341_MAGENTA); // Xmin, Ymin, Xlength, Ylength, color
    tft->setCursor(4, 3);
    tft->print("S");

    // Drumtrack button
    tft->fillRect(1, STEP_FRAME_H + 8, 15, TRACK_FRAME_H, trackColor[0]); // Xmin, Ymin, Xlength, Ylength, color
    tft->setCursor(4, TRACK_FRAME_H + 8);
    tft->print("D");

    // other tracks buttons
    for (int otherTracks = 2; otherTracks <= 8; otherTracks++)
    {
      tft->fillRect(1, TRACK_FRAME_H * otherTracks, 15, TRACK_FRAME_H, trackColor[otherTracks - 1]); // Xmin, Ymin, Xlength, Ylength, color
      tft->setCursor(4, TRACK_FRAME_H * otherTracks + 6);
      tft->print(otherTracks);
    }
    // Mixer button
    tft->fillRect(1, STEP_FRAME_H * 13 + 8, 15, TRACK_FRAME_H, ILI9341_LIGHTGREY); // Xmin, Ymin, Xlength, Ylength, color
    tft->setCursor(3, STEP_FRAME_H * 13 + 14);
    tft->print("M");

    // scale Select
    tft->drawRect(STEP_FRAME_W * POSITION_SCALE_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);

    // arrangment Select
    tft->drawRect(STEP_FRAME_W * POSITION_ARR_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);

    // record button & Rectangle
    tft->drawRect(STEP_FRAME_W * POSITION_RECORD_BUTTON, 0, STEP_FRAME_W, STEP_FRAME_H, ILI9341_WHITE);
    tft->fillCircle(STEP_FRAME_W * POSITION_RECORD_BUTTON + 7, 7, DOT_RADIUS + 1, ILI9341_LIGHTGREY);

    // Play button & Rectangle
    tft->drawRect(STEP_FRAME_W * POSITION_PLAY_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);                                                                     // PLAY RECT FRAME
    tft->fillTriangle(STEP_FRAME_W * POSITION_PLAY_BUTTON + 12, 3, STEP_FRAME_W * POSITION_PLAY_BUTTON + 12, 13, STEP_FRAME_W * POSITION_PLAY_BUTTON + 22, 8, ILI9341_GREEN); // x1, y1, x2, y2, x3, y3

    // stop button & Rectangle
    tft->drawRect(STEP_FRAME_W * POSITION_STOP_BUTTON, 0, STEP_FRAME_W, STEP_FRAME_H, ILI9341_WHITE);
    tft->fillRect(STEP_FRAME_W * POSITION_STOP_BUTTON + 3, 3, 10, 10, ILI9341_LIGHTGREY);

    // barcounter & Rectangle
    tft->drawRect(STEP_FRAME_W * POSITION_BAR_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);

    // tempo button & Rectangle
    tft->drawRect(STEP_FRAME_W * POSITION_BPM_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);

    // save button & Rectangle
    tft->drawRect(STEP_FRAME_W * POSITION_SAVE_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);
    tft->fillRect(STEP_FRAME_W * POSITION_SAVE_BUTTON + 1, 1, STEP_FRAME_W * 2 - 2, STEP_FRAME_H - 2, ILI9341_ORANGE);
    tft->setTextColor(ILI9341_BLACK);
    tft->setFont(Arial_9);
    tft->setCursor(STEP_FRAME_W * POSITION_SAVE_BUTTON + 2, 3);
    tft->print("SAV");

    // load button & Rectangle
    tft->drawRect(STEP_FRAME_W * POSITION_LOAD_BUTTON, 0, STEP_FRAME_W, STEP_FRAME_H, ILI9341_WHITE);
    tft->fillRect(STEP_FRAME_W * POSITION_LOAD_BUTTON + 1, 1, STEP_FRAME_W - 2, STEP_FRAME_H - 2, ILI9341_GREENYELLOW);
    tft->setTextColor(ILI9341_BLACK);
    tft->setFont(Arial_9);
    tft->setCursor(STEP_FRAME_W * POSITION_LOAD_BUTTON + 4, 3);
    tft->print("L");
  
  }

  // stepsequencer
  void drawStepSequencerStatic(int track)
  {

    draw_Notenames(track);
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
  void draw_Notenames(int track)
  {
    for (int n = 0; n < NUM_NOTES; n++)
    { // hor notes
      tft->fillRect(STEP_FRAME_W, STEP_FRAME_H * n + STEP_FRAME_H, STEP_FRAME_W, STEP_FRAME_H, trackColor[track]);
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

  // songmode / arranger
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
    {                                                                                               // do this for all phrases
      tft->drawFastVLine((f * phraseSegmentLength) + 32, STEP_FRAME_H + 4, STEP_FRAME_H * 12, 360); //(x, y-start, y-length, color)
      if (f % 4 == 0)
      {
        tft->drawFastVLine((f * phraseSegmentLength) + 32, STEP_FRAME_H + 4, STEP_FRAME_H * 12, 370); //(x, y-start, y-length, color)
      }
    }
  }

  void setBackgroundTo(int page)
  {
    clearWorkSpace();
    active_page = page;

    for (int i = TRACK_1_PAGE; i <= TRACK_8_PAGE; i++) // select the track/sequencer pages
    {
      if (active_page == i)
      {
        encoder_function = INPUT_FUNCTIONS_FOR_SEQUENCER; // enable cursor in this screen
        drawStepSequencerStatic(i);
      }
    }

    for (int i = SONGMODE_PAGE_1; i <= SONGMODE_PAGE_16; i++) // select the songmode/arranger pages
    {
      if (active_page == i)
      {
        encoder_function = INPUT_FUNCTIONS_FOR_ARRANGER; // enable cursor in this screen
        gridSongMode(i);
      }
    }
  }
};
Background background(&tft);

class Cursor
{
public:
  ILI9341_t3n *tft; // Pointer to the display object
  int xPos = 0;
  int yPos = 0;
  int last_xPos = 0;
  int last_yPos = 0;
  int last_button_X = 0;
  int last_button_Y = 0;
  int deltaY = STEP_FRAME_H;

  uint16_t tftRAM[16][16];
  Cursor(ILI9341_t3n *display)
  {
    tft = display;
  }
  void update(int X, int Y, int dY)
  {
    xPos = X;
    yPos = Y;
    deltaY = dY;
    showCoordinates();
    move();
    // tft->updateScreenAsync();
  }
  void move()
  {
    int arranger_offset = 0;
    if (deltaY == ARRANGER_FRAME_H)
      if (yPos >= 1)
        arranger_offset = 4;
    for (int pixel = 0; pixel < 16; pixel++)
    {
      tft->drawPixel(pixel + (last_xPos), (deltaY * last_yPos) + 1 + arranger_offset, tftRAM[0][pixel]);  // draw upper line X1
      tft->drawPixel(pixel + (last_xPos), (deltaY * last_yPos) + 15 + arranger_offset, tftRAM[1][pixel]); // draw bottom line X2
      tft->drawPixel((last_xPos) + 1, pixel + (deltaY * last_yPos) + arranger_offset, tftRAM[2][pixel]);  // draw left line Y1
      tft->drawPixel((last_xPos) + 15, pixel + (deltaY * last_yPos) + arranger_offset, tftRAM[3][pixel]); // draw right line Y2
    }
    for (int pixel = 0; pixel < 16; pixel++)
    {
      tftRAM[0][pixel] = tft->readPixel(pixel + (xPos), (deltaY * yPos) + 1 + arranger_offset);  // save upper line
      tftRAM[1][pixel] = tft->readPixel(pixel + (xPos), (deltaY * yPos) + 15 + arranger_offset); // save bottom line
      tftRAM[2][pixel] = tft->readPixel((xPos) + 1, pixel + (deltaY * yPos) + arranger_offset);  // save left line
      tftRAM[3][pixel] = tft->readPixel((xPos) + 15, pixel + (deltaY * yPos) + arranger_offset); // save right line
    }

    tft->drawRect((xPos) + 1, (deltaY * yPos) + 1 + arranger_offset, STEP_FRAME_W - 1, STEP_FRAME_H - 1, ILI9341_WHITE);

    last_xPos = xPos;
    last_yPos = yPos;
  }
  void showCoordinates()
  {
    tft->fillRect(20, 0, 60, 13, ILI9341_DARKGREY); // Xmin, Ymin, Xlength, Ylength, color
    tft->setTextColor(ILI9341_GREEN);
    tft->setFont(Arial_10);
    tft->setCursor(20, 3);
    tft->print("X");
    tft->print(xPos);
    tft->setCursor(55, 3);
    tft->print("Y");
    tft->print(yPos);
  }
};
Cursor cursor(&tft);

class Clock
{
public:
  ILI9341_t3n *tft; // Pointer to the display object
  uint32_t _next_clock = 0;
  uint32_t _clock = 160;
  uint32_t MIDItick = -1;
  uint32_t step_tick = -1;
  uint32_t bar_tick = -1;
  byte bar_tick_display = -1;
  byte start_of_loop = 0;
  byte end_of_loop = 255;
  bool seq_run = false;
  bool seq_rec = false;
  bool playing = false;
  bool stop_once = false;
  uint32_t master_tempo = 120;
  int syncPin = -1;
  uint32_t previousMillis_clock = 0;

  Clock(ILI9341_t3n *display)
  {
    tft = display;
  }

  // setup function, so we can put in an analog sync Output (Optional)
  void setAnalogSyncPin(int new_syncPin)
  {
    syncPin = new_syncPin;
    if (syncPin >= 0)
    {
      digitalWrite(syncPin, LOW); //
      pinMode(syncPin, OUTPUT);
    }
  }

  bool is_playing()
  {
    return playing;
  }
  void setPlayStatus(bool p = false)
  { // if we want to start the clock use, "clock.is_playing(true);"
    this->playing = p;
  }

  void set_start_of_loop(byte n)
  {
    if (enc_moved[n])
    {
      start_of_loop = constrain(start_of_loop + encoded[n], 0, 254);

      draw_clock_option(POSITION_START_LOOP_BUTTON, start_of_loop);

      enc_moved[n] = false;
    }
  }
  void draw_clock_option(byte x, byte v)
  {
    tft->setCursor(STEP_FRAME_W * x + 2, 3);
    tft->setFont(Arial_10);
    tft->setTextColor(ILI9341_WHITE);
    tft->setTextSize(1);
    tft->fillRect(STEP_FRAME_W * x + 2, 1, STEP_FRAME_W * 2 - 4, STEP_FRAME_H - 2, ILI9341_DARKGREY);
    tft->print(v);
    tft->updateScreenAsync();
  }
  void set_end_of_loop(byte n)
  {
    if (enc_moved[n])
    {
      end_of_loop = constrain(end_of_loop + encoded[n], 2, 255);

      draw_clock_option(POSITION_END_LOOP_BUTTON, end_of_loop);

      enc_moved[n] = false;
    }
  }

  void set_tempo(byte n)
  {
    if (enc_moved[n])
    {
      master_tempo = constrain(master_tempo + encoded[n], 1, 250);
      // midi clock messages should be sent 24 times
      // for every quarter note
      _clock = 60000000L / master_tempo / 24;
      draw_clock_option(POSITION_BPM_BUTTON, master_tempo);
      enc_moved[n] = false;
    }
  }

  int get_tempo()
  {
    return master_tempo;
  }

  void send_MIDIclock()
  {
    // spit out a MIDItick
    usbMIDI.sendRealTime(usbMIDI.Clock); // send a midiclock to usb host
    // MIDI.sendRealTime(0xF8);              //send a midiclock to serial midi
  }
  void send_sync_clock()
  {

    // spit out a MIDItick
    if (syncPin >= 0)
    {
      digitalWrite(syncPin, HIGH);
      uint32_t currentMillis = millis();
      if (currentMillis - previousMillis_clock >= 10)
      {
        previousMillis_clock = currentMillis;
        digitalWrite(syncPin, LOW);
      }
    }
  }

  bool process_MIDItick()
  {

    if (playing)
    {
      // if the clock is running
      if (msecsclock >= _clock)
      {             // compare clockvalues
        MIDItick++; // spit out a MIDItick
        send_MIDIclock();
        msecsclock = 0;
        update_step_tick();
        update_bar_tick();
        stop_once = true;
        return true;
      }
      if (MIDItick == 96)
      {
        MIDItick = 0;
      }
    }

    if (!playing)
    {
      if (stop_once)
      {
        stop_once = false;
        MIDItick = -1;
        step_tick = -1;
        bar_tick = start_of_loop ;
        bar_tick_display = -1;
        tft->fillRect(STEP_FRAME_W * 2, SONG_POSITION_POINTER_Y, STEP_FRAME_W * 16, 4, ILI9341_DARKGREY);
        tft->fillRect(STEP_FRAME_W * 2, GRID_POSITION_POINTER_Y, STEP_FRAME_W * 16, 4, ILI9341_DARKGREY);
        tft->fillRect(STEP_FRAME_W * 2, STEP_POSITION_POINTER_Y, STEP_FRAME_W * 16, 4, ILI9341_DARKGREY);
        tft->asyncUpdateActive();
      }
    }
    return false;
  }
  uint32_t get_MIDItick()
  { // returns the actual MIDItick count
    return MIDItick;
  }
  // step counter stuff
  uint32_t get_step_tick()
  { // returns the actual STEPtick count
    // int miditick;
    return MIDItick % 6;
  }
  bool clock_is_on_tick()
  {
    return MIDItick % 1 == 0;
  }
  bool clock_is_on_step()
  {
    return MIDItick % 6 == 0;
  }
  void update_step_tick()
  {
    if (MIDItick % 6 == 0)
    {
      step_tick++;
      if (step_tick == 16)
      {
        step_tick = 0;
      }
      drawstepPosition(step_tick);
    }
  }
  void drawstepPosition(int current)
  {
    if (clock_is_on_step())
    {
      // draw the songpointer positions
      for (int songPointerThickness = 0; songPointerThickness <= POSITION_POINTER_THICKNESS; songPointerThickness++)
      {
        for (int stepwidth = 1; stepwidth <= 16; stepwidth++)
        {
          tft->drawFastHLine(current * stepwidth + STEP_FRAME_W * 2, STEP_POSITION_POINTER_Y + songPointerThickness, STEP_FRAME_W, ILI9341_GREEN);
          tft->drawFastHLine((current - 1) * stepwidth + STEP_FRAME_W * 2, STEP_POSITION_POINTER_Y + songPointerThickness, STEP_FRAME_W, ILI9341_DARKGREY);
        }
        if (current == 0)
        {
          tft->drawFastHLine(STEP_FRAME_W * 17, STEP_POSITION_POINTER_Y + songPointerThickness, STEP_FRAME_W, ILI9341_DARKGREY);
        }
      }
      // Serial.print(current);
      tft->updateScreenAsync();
    }
  }
  // bar counter stuff
  uint32_t get_bar_tick()
  { // returns the actual BARtick count
    return MIDItick % 96;
  }
  bool tick_is_on_bar()
  {
    return MIDItick % 96 == 0;
  }
  void update_bar_tick()
  {
    if (MIDItick % 96 == 0)
    {

      bar_tick++;
      bar_tick_display++;
      if (bar_tick == end_of_loop)
      {
        bar_tick = start_of_loop;
      }
      // reset the bar_tick_display so the bar pointer (on the bottom display starts from the first step again
      if (bar_tick_display == 16)
      {
        bar_tick_display = 0;
      }
      Serial.println(bar_tick);
      drawbarPosition();
    }
  }
  void drawbarPosition()
  {

    // draw phrasenumber
    tft->fillRect(STEP_FRAME_W * POSITION_BAR_BUTTON + 1, 2, STEP_FRAME_W * 2 - 2, STEP_FRAME_H - 3, ILI9341_DARKGREY);
    tft->setTextColor(ILI9341_WHITE);
    tft->setFont(Arial_9);
    tft->setCursor(STEP_FRAME_W * POSITION_BAR_BUTTON + 4, 3);
    tft->print(bar_tick);

    // drawbarPosition
    for (int songPointerThickness = 0; songPointerThickness <= POSITION_POINTER_THICKNESS; songPointerThickness++)
    {
      tft->drawPixel(bar_tick + STEP_FRAME_W * 2, (SONG_POSITION_POINTER_Y + songPointerThickness), ILI9341_GREEN);

      tft->drawFastHLine((bar_tick_display)*phraseSegmentLength + STEP_FRAME_W * 2, GRID_POSITION_POINTER_Y + songPointerThickness, phraseSegmentLength, ILI9341_GREEN);
      tft->drawFastHLine((bar_tick_display - 1) * phraseSegmentLength + STEP_FRAME_W * 2, GRID_POSITION_POINTER_Y + songPointerThickness, phraseSegmentLength, ILI9341_DARKGREY);
    }
    if (bar_tick_display == 0)
    {
      // pixelphrase = 0;
      tft->fillRect(STEP_FRAME_W * 17, GRID_POSITION_POINTER_Y, STEP_FRAME_W, 4, ILI9341_DARKGREY);
      // tft->fillRect(STEP_FRAME_W * 2, GRID_POSITION_POINTER_Y, STEP_FRAME_W * 1, 4, ILI9341_GREEN);
    }
    // Serial.println(step_tick);
    // Serial.println("bar");
    tft->updateScreenAsync();
  }

  // display stuff
};
Clock Masterclock(&tft);

class Track
{
public:
#define OCTAVE_CHANGE_TEXT 3
#define NOTES_PER_OCTAVE 12
#define SEQUENCER_OPTIONS_VERY_RIGHT 18
#define BARS_PER_PAGE 16
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
  byte internal_clock = 0;
  bool internal_clock_is_on = false;
  byte internal_clock_bar = 0;
  byte step_division = 1;
  byte sequence_length = MAX_TICKS;
  byte array[MAX_CLIPS][MAX_TICKS + 1][MAX_VOICES];
  byte active_voice = 0;
  byte search_free_voice = 0;
  bool note_is_on[MAX_VOICES] = {true, true, true, true};
  bool ready_for_NoteOff[MAX_VOICES] = {false, false, false, false};
  int encoder_colour[NUM_ENCODERS] = {ILI9341_BLUE, ILI9341_RED, ILI9341_GREEN, ILI9341_WHITE};

  Track(ILI9341_t3n *display, byte Y, byte cI, byte cO)
  {
    tft = display;
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
    for (int i = 0; i < 256; i++)
    {
      clip_to_play[i] = 8;
      noteOffset[i] = 0;
    }
  }

  void play_sequencer_mode(byte cloock)
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
    if (internal_clock_bar == Masterclock.end_of_loop)
      internal_clock_bar = Masterclock.start_of_loop;
    Serial.printf("bar: %d, tick: %d\n", internal_clock_bar, internal_clock);
    // Serial.println(internal_clock_bar);
    if (internal_clock_is_on)
    {
      if (sequencer_mode == 0)
      {
        sequencer_Mode1(internal_clock);
      }
      if (sequencer_mode == 1)
      {
        sequencer_Mode2(internal_clock);
      }
    }
  }
  void sequencer_Mode1(byte cloock)
  {

    for (int v = 0; v < MAX_VOICES; v++)
    {
      if (array[clip_to_play[internal_clock_bar]][cloock][v] < NO_NOTE)
      {
        if (!note_is_on[v])
        {
          noteToPlay[v] = array[clip_to_play[internal_clock_bar]][cloock][v] + noteOffset[internal_clock_bar];
          note_is_on[v] = true;
          usbMIDI.sendNoteOn(noteToPlay[v], VELOCITY_NOTE_ON, MIDI_channel_out); // Send a Note (pitch 42, velo 127 on channel 1)
          Serial.printf("ON   tick: %d, voice: %d, note: %d\n", cloock, v, noteToPlay[v]);
        }
      }

      if (array[clip_to_play[internal_clock_bar]][cloock][v] == NO_NOTE)
      {
        if (note_is_on[v])
        {
          note_is_on[v] = false;
          usbMIDI.sendNoteOff(noteToPlay[v], VELOCITY_NOTE_OFF, MIDI_channel_out); // Send a Note (pitch 42, velo 127 on channel 1)
          Serial.printf("OFF   tick: %d, voice: %d, note: %d\n", cloock, v, noteToPlay[v]);
        }
      }
    }
  }
  void sequencer_Mode2(byte cloock)
  {

    if (array[clip_to_play[internal_clock_bar]][cloock][0] < NO_NOTE)
    {
      if (!note_is_on[0])
      {
        noteToPlay[0] = random(0, 11) + (octave * 12) + noteOffset[internal_clock_bar];
        note_is_on[0] = true;
        usbMIDI.sendNoteOn(noteToPlay[0], VELOCITY_NOTE_ON, MIDI_channel_out); // Send a Note (pitch 42, velo 127 on channel 1)
        Serial.printf("ON   tick: %d, voice: %d, note: %d\n", cloock, 0, noteToPlay[0]);
      }
    }

    if (array[clip_to_play[internal_clock_bar]][cloock][0] == NO_NOTE)
    {
      if (note_is_on[0])
      {
        note_is_on[0] = false;
        usbMIDI.sendNoteOff(noteToPlay[0], VELOCITY_NOTE_OFF, MIDI_channel_out); // Send a Note (pitch 42, velo 127 on channel 1)
        Serial.printf("OFF   tick: %d, voice: %d, note: %d\n", cloock, 0, noteToPlay[0]);
      }
    }
  }

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
      MIDI_channel_out = constrain(MIDI_channel_out + encoded[n], 1, 16);
      draw_MIDI_channel_out(n);
      enc_moved[n] = false;
    }
  }
  void draw_MIDI_channel_out(byte n)
  {
    draw_sequencer_option(SEQUENCER_OPTIONS_VERY_RIGHT, "MdCh", MIDI_channel_out, n);
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
  void draw_sequencer_option(byte x, char *nameshort, int value, byte enc)
  {
    int color;
    if (background.encoder_function == INPUT_FUNCTIONS_FOR_ARRANGER)
      color = my_Arranger_Y_axis - 1;
    else
      color = active_track;
    byte y = 6 + (enc * 2);
    // show function
    tft->setCursor(STEP_FRAME_W * x + 2, STEP_FRAME_H * (y - 1) + 2);
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

  //----------------------------------------------------------------
  // arranger stuff
  // clip to play
  void set_clip_to_play(byte n, byte b)
  {
    if (gridTouchY == my_Arranger_Y_axis)
    {
      byte when = ((b - SEQ_GRID_LEFT) / STEP_FRAME_W) + (BARS_PER_PAGE * (active_page - SONGMODE_PAGE_1));
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
        tft->drawFastHLine(((b - (16 * (active_page - SONGMODE_PAGE_1))) * STEP_FRAME_W + STEP_FRAME_W * 2) + 1, ((my_Arranger_Y_axis)*TRACK_FRAME_H + thickness) + 12, STEP_FRAME_W - 1, ILI9341_DARKGREY); //(x-start, y, length, color)
      }
    }
    else
    {
      // for other clips
      for (int thickness = -7; thickness < 7; thickness++)
      {
        tft->drawFastHLine(((b - (16 * (active_page - SONGMODE_PAGE_1))) * STEP_FRAME_W + STEP_FRAME_W * 2) + 1, ((my_Arranger_Y_axis)*TRACK_FRAME_H + thickness) + 12, STEP_FRAME_W - 1, trackColor[my_Arranger_Y_axis - 1] + (clip_to_play[b] * 20)); //(x-start, y, length, color)
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
    tft->setCursor((b - (16 * (active_page - SONGMODE_PAGE_1))) * STEP_FRAME_W + STEP_FRAME_W * 2 + 2, (my_Arranger_Y_axis)*TRACK_FRAME_H + 6);
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
      byte when = ((b - SEQ_GRID_LEFT) / STEP_FRAME_W) + (BARS_PER_PAGE * (active_page - SONGMODE_PAGE_1));
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
    tft->setCursor((b - (16 * (active_page - SONGMODE_PAGE_1))) * STEP_FRAME_W + STEP_FRAME_W * 2 + xoffset, (my_Arranger_Y_axis)*TRACK_FRAME_H + 11);
    tft->print(noteOffset[b]);
  }
};
Track track1(&tft, 1, 10, 10);
Track track2(&tft, 2, 1, 1);
Track track3(&tft, 3, 2, 2);
Track track4(&tft, 4, 3, 3);

Track track5(&tft, 5, 4, 4);
Track track6(&tft, 6, 5, 5);
Track track7(&tft, 7, 6, 6);
Track track8(&tft, 8, 7, 7);

Track *allTracks[8]{&track1, &track2, &track3, &track4, &track5, &track6, &track7, &track8};

// put function declarations here:
void readEncoders();
void encoder_SetCursor();

void readMainButtons();
void buttons_SetPlayStatus();
void buttons_SetCorsor();
void buttons_SelectTrack();
void buttons_SelectPlugin();
void buttons_SelectArranger();
void buttons_SetNoteOnTick(int x, byte y);
void buttons_Set_potRow();
void input_behaviour();
void clock_to_notes();
void USBMIDI_noteOn(byte channel, byte pitch, byte velocity);
void USBMIDI_noteOff(byte channel, byte pitch, byte velocity);

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(15200); // set MIDI baud

  // initialize the TFT- and Touchscreen
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setFrameBuffer(tft_frame_buffer);
  tft.useFrameBuffer(true);
  tft.initDMASettings();
  // tft.updateChangedAreasOnly(true);
  tft.setTextColor(ILI9341_WHITE);
  tft.setFont(Arial_8);
  tft.setCursor(0, 3);
  Serial.println("Initializing Touchscreen...");
  tft.println("Initializing Touchscreen...");
  tft.updateScreenAsync();
  delay(100);

  Serial.println("Initializing Buttons");
  tft.println("Initializing Buttons");
  kpd.begin();
  for (int i = 0; i < NUM_ENCODERS; i++)
  {
    encButtons[i].attach(BUTTON_PINS[i], INPUT_PULLUP); // setup the bounce instance for the current button
    encButtons[i].interval(25);                         // interval in ms
  }
  tft.updateScreenAsync();
  delay(100);

  Serial.println("Initializing Keypad");
  tft.println("Initializing Keypad");
  // kpd.begin();
  tft.updateScreenAsync();
  delay(1000);

  usbMIDI.begin(); // Launch MIDI and listen to channel 4
  background.startUpScreen();
}

void loop()
{

  Masterclock.process_MIDItick();

  readEncoders();
  readMainButtons();
  input_behaviour();

  clock_to_notes();

  // placeholder for debugging
  if (millis() % 200 == 0)
  {
    // Serial.println(Masterclock.playing ? "True" : "False");
  }
  // placeholder for debugging
  if (millis() % 50 == 0)
  {

    if (background.encoder_function == INPUT_FUNCTIONS_FOR_ARRANGER)
      cursor.update(pixelTouchX, gridTouchY, ARRANGER_FRAME_H);
    else
      cursor.update(pixelTouchX, gridTouchY, STEP_FRAME_H);
    tft.fillRect(70, lastPotRow * 4, 10, 3, ILI9341_RED);
  }

  tft.updateScreenAsync();
}
void input_behaviour()
{
  otherCtrlButtons = (buttonPressed[BUTTON_TRACK] ||
                      buttonPressed[BUTTON_PLUGIN] ||
                      buttonPressed[BUTTON_SONG] ||
                      buttonPressed[BUTTON_MIXER] ||
                      buttonPressed[BUTTON_FX]);

  buttons_SelectTrack();
  buttons_SelectPlugin();
  buttons_SelectArranger();

  // if none of the bottom buttons are pressed with the upper ones
  if (!otherCtrlButtons)
  {
    buttons_SetCorsor();
    buttons_Set_potRow();
    buttons_SetPlayStatus();
  }
  // if we are in one of the sequencer pages
  if (background.encoder_function == INPUT_FUNCTIONS_FOR_SEQUENCER)
  {
    // if Shift button is NOT pressed
    if (!buttonPressed[BUTTON_SHIFT])
    {
      encoder_SetCursor(14);                          // Encoder: 0,1
      allTracks[active_track]->set_octave(2);       // Encoder: 2
      allTracks[active_track]->set_clip_to_edit(3); // Encoder: 3

      buttons_SetNoteOnTick(pixelTouchX, gridTouchY);
    }
    // if Shift button is pressed
    if (buttonPressed[BUTTON_SHIFT])
    {
      allTracks[active_track]->set_sequence_length(0);  // Encoder: 0
      allTracks[active_track]->set_step_division(1);    // Encoder: 1
      allTracks[active_track]->set_sequencer_mode(2);   // Encoder: 2
      allTracks[active_track]->set_MIDI_channel_out(3); // Encoder: 3
    }
  }
  // if we are in one of the Arrangerpages
  if (background.encoder_function == INPUT_FUNCTIONS_FOR_ARRANGER)
  {
    // if Shift button is NOT pressed
    if (!buttonPressed[BUTTON_SHIFT])
    {
      gridTouchY=0;
      encoder_SetCursor(8); // Encoder: 0,1
      allTracks[gridTouchY - 1]->set_clip_to_play(2, pixelTouchX);
      allTracks[gridTouchY - 1]->set_note_offset(3, pixelTouchX);
    }
    if (buttonPressed[BUTTON_SHIFT])
    {
      Masterclock.set_tempo(1);         // Encoder: 1
      Masterclock.set_start_of_loop(2); // Encoder: 2
      Masterclock.set_end_of_loop(3);   // Encoder: 3
    }
  }
}
void clock_to_notes()
{
  if (Masterclock.is_playing())
  {
    if (Masterclock.process_MIDItick())
    {
      // Serial.println(Masterclock.MIDItick);
      for (int t = 0; t < NUM_TRACKS; t++)
      {
        allTracks[t]->play_sequencer_mode(Masterclock.MIDItick);
      }
    }
  }
}

void readEncoders()
{

  encoded[0] = 0;
  encoded[1] = 0;
  encoded[2] = 0;
  encoded[3] = 0;
  enc_moved[0] = 0;
  enc_moved[1] = 0;
  enc_moved[2] = 0;
  enc_moved[3] = 0;

  // check encoder ROTATION
  for (int e = 0; e < NUM_ENCODERS; e++)
  {
    allEncoders[e]->tick();                                   // update rotary encoders
    if (!encButtons[e].read())                                // read if the enc Button is pressed
      encoded[e] = (int)(allEncoders[e]->getDirection()) * 8; // if so, multiplicate +-1 by 8
    else
      encoded[e] = (int)(allEncoders[e]->getDirection()); // else stay with +-1

    if (encoded[e] != 0) // display encoder movement
    {
      enc_moved[e] = true;
      // Serial.printf("Enc %d moved by %d \n", e, encoded[e]);
    }
    // else
    //
  }

  // check encoder BUTTONS
  for (int i = 0; i < NUM_ENCODERS; i++)
  {
    // Update the Bounce instance :
    encButtons[i].update();
    // If it fell, flag the need to toggle the LED
    if (encButtons[i].fell())
    {
      Serial.printf("Button pressed %d\n", i);
    }
  }
}
void encoder_SetCursor(byte maxY)
{

  if (enc_moved[0])
  {
    pixelTouchX = constrain(pixelTouchX + encoded[0] * 2, 0, 320);
    enc_moved[0] = false;
    cursor_moved = true;
  }
  if (enc_moved[1])
  {
    gridTouchY = constrain(gridTouchY + encoded[1], 0, maxY);
    enc_moved[1] = false;
    cursor_moved = true;
  }
}

void readMainButtons()
{
  kpd.tick();
  while (kpd.available())
  {
    if (millis() % 10 == 0)
    {
      keypadEvent key = kpd.read();
      if (key.bit.EVENT == KEY_JUST_PRESSED)
      {
        int pressedKey = (int)(key.bit.KEY - 65);
        buttonPressed[pressedKey] = true;
      }

      if (key.bit.EVENT == KEY_JUST_RELEASED)
      {
        int pressedKey = (int)(key.bit.KEY - 65);
        buttonPressed[pressedKey] = false;
      }
    }
  }
}
void buttons_SetCorsor()
{

  if (buttonPressed[BUTTON_LEFT]) //..move the cursor left
  {
    pixelTouchX = constrain(pixelTouchX - 1, 0, 320);
    buttonPressed[BUTTON_LEFT] = false;
  }
  if (buttonPressed[BUTTON_RIGHT]) //..move the cursor right
  {
    pixelTouchX = constrain(pixelTouchX + 1, 0, 320);
    buttonPressed[BUTTON_RIGHT] = false;
  }
  if (buttonPressed[BUTTON_UP]) //..move the cursor up
  {
    gridTouchY = constrain(gridTouchY - 1, 0, 14);
    buttonPressed[2] = false;
  }

  if (buttonPressed[BUTTON_DOWN]) //..move the cursor down
  {
    gridTouchY = constrain(gridTouchY + 1, 0, 14);
    buttonPressed[BUTTON_DOWN] = false;
  }
}
void buttons_SetPlayStatus()
{
  if (buttonPressed[BUTTON_PLAY]) //..set Play status to play
  {
    Masterclock.setPlayStatus(true);
    Serial.println("Play");
    buttonPressed[BUTTON_PLAY] = false;
  }
  if (buttonPressed[BUTTON_STOP]) //..set Play status to stop
  {
    Masterclock.setPlayStatus(false);
    for (int t = 0; t < NUM_TRACKS; t++)
    {
      allTracks[t]->internal_clock = -1;
      for (int v = 0; v < MAX_VOICES; v++)
      {
        usbMIDI.sendNoteOff(allTracks[t]->array[allTracks[t]->clip_to_edit][Masterclock.MIDItick][v], VELOCITY_NOTE_OFF, allTracks[t]->MIDI_channel_out); // Send a Note (pitch 42, velo 127 on channel 1)
      }
    }
    Serial.println("Stop");
    buttonPressed[BUTTON_STOP] = false;
  }
}
void buttons_SetNoteOnTick(int x, byte y)
{

  if (buttonPressed[BUTTON_ENTER])
  {
    if (pixelTouchX >= SEQ_GRID_LEFT && pixelTouchX <= SEQ_GRID_RIGHT && gridTouchY >= SEQ_GRID_TOP && gridTouchY <= SEQ_GRID_BOTTOM)
    {
      allTracks[active_track]->set_note_on_tick(x, y);
    }
    buttonPressed[BUTTON_ENTER] = false;
  }
}
void buttons_SelectTrack()
{
  // select tracks
  if (buttonPressed[BUTTON_TRACK])
  {
    for (int i = 0; i <= TRACK_8_PAGE; i++)
    {
      if (buttonPressed[i])
      {
        active_track = i;
        active_page = i;
        // buttonPressed[BUTTON_TRACK] = false;
        background.setBackgroundTo(active_page);
        allTracks[active_track]->draw_sequencer_screen();
        allTracks[active_track]->draw_notes_in_grid();
        buttonPressed[i] = false;
      }
    }
  }
}
void buttons_SelectPlugin()
{ // select active plugin from choosen track
  if (buttonPressed[BUTTON_PLUGIN])
  {
    active_page = STARTUPSCREEN_PAGE;
    background.setBackgroundTo(active_page);
    buttonPressed[BUTTON_PLUGIN] = false;
  }
}
void buttons_SelectArranger()
{
  // select arranger pages
  if (buttonPressed[BUTTON_SONG])
  {
    active_page = SONGMODE_PAGE_11;
    // buttonPressed[BUTTON_SONG] = false;
    if (buttonPressed[BUTTON_LEFT])
    {
      // background.setBackgroundTo(SONGMODE_PAGE_1);
      active_page = SONGMODE_PAGE_1;
      buttonPressed[BUTTON_LEFT] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_RIGHT])
    {
      //  background.setBackgroundTo(SONGMODE_PAGE_2);
      active_page = SONGMODE_PAGE_2;
      buttonPressed[BUTTON_RIGHT] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_UP])
    {
      //  background.setBackgroundTo(SONGMODE_PAGE_3);
      active_page = SONGMODE_PAGE_3;
      buttonPressed[BUTTON_UP] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_DOWN])
    {
      // background.setBackgroundTo(SONGMODE_PAGE_4);
      active_page = SONGMODE_PAGE_4;
      buttonPressed[BUTTON_DOWN] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_ROW])
    {
      // background.setBackgroundTo(SONGMODE_PAGE_5);
      active_page = SONGMODE_PAGE_5;
      buttonPressed[BUTTON_ROW] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_REC])
    {
      // background.setBackgroundTo(SONGMODE_PAGE_6);
      active_page = SONGMODE_PAGE_6;
      buttonPressed[BUTTON_REC] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_PLAY])
    {
      // background.setBackgroundTo(SONGMODE_PAGE_7);
      active_page = SONGMODE_PAGE_7;
      buttonPressed[BUTTON_PLAY] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_STOP])
    {
      // background.setBackgroundTo(SONGMODE_PAGE_8);
      active_page = SONGMODE_PAGE_8;
      buttonPressed[BUTTON_STOP] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_TRACK])
    {
      // background.setBackgroundTo(SONGMODE_PAGE_9);
      active_page = SONGMODE_PAGE_9;
      buttonPressed[BUTTON_TRACK] = false;
      buttonPressed[BUTTON_SONG] = false;
    }

    Serial.println(active_page);
    background.setBackgroundTo(active_page);
    for (int i = 0; i < NUM_TRACKS; i++)
      allTracks[i]->draw_arrangment_lines(3, active_page);
  }
}
void buttons_Set_potRow()
{
  if (buttonPressed[BUTTON_ROW])
  {
    lastPotRow++;
    if (lastPotRow >= 4)
    {
      lastPotRow = 0;
      // tft.fillRect(70, 0, 10, 16, ILI9341_DARKGREY);
    }
    tft.fillRect(70, 0, 10, 16, ILI9341_DARKGREY);

    buttonPressed[BUTTON_ROW] = false;
  }
}