#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include <ILI9341_t3n.h>
#include <ili9341_t3n_font_Arial.h> // from ILI9341_t3
// Teensy 4.1 PINOUT
// Pinout for screen
// #define TIRQ_PIN 15                                                                  // alternate Pins: any digital pin
#define CS_PIN 14                                                                    // alternate Pins: any digital pin
#define TFT_DC 9                                                                     // alternate Pins 9, 10, 20, 21
#define TFT_CS 10                                                                    // alternate Pins 9, 15, 20, 21
#define TFT_RST 255                                                                  // 255 = unused, connect to 3.3V
#define TFT_MOSI 11                                                                  // shareable
#define TFT_SCK 13                                                                   // shareable
#define TFT_MISO 12                                                                  // shareable
XPT2046_Touchscreen ts(CS_PIN);                                                      // initiate Touchscreen
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO); // initiate TFT-Srceen

#include <SPI.h>
#include <Adafruit_Keypad.h>
// #include <Key.h>
// #include <Keypad.h>
#include <RotaryEncoder.h>
#include <Wire.h>
#include <Bounce2.h>
#include <MIDI.h>
#include "global_stuff.h"
#include <AudioSamples.h>
#include <Output.h>
#include <Track.h>
#include <plugin_List.h>
#include <fx_List.h>
Plugin_1 plugin_1("Strng", 17);
Plugin_2 plugin_2("1OSC", 18);
Plugin_3 plugin_3("2FM", 19);
Plugin_4 plugin_4("MDrm", 20);
Plugin_5 plugin_5("Drum", 21);
Plugin_6 plugin_6("Draw", 22);
Plugin_7 plugin_7("Boom", 23);
Plugin_8 plugin_8("Dtune", 24);
PluginControll *allPlugins[NUM_PLUGINS] = {&plugin_1, &plugin_2, &plugin_3, &plugin_4, &plugin_5, &plugin_6, &plugin_7, &plugin_8};
FX_1 fx_1("Rev", 1);
FX_2 fx_2("Bit", 2);
FX_3 fx_3("Nix", 3);

Output MasterOut(3);

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
const char *playstate[3] = {"Mute", "Play", "Solo"};
// notenumber to frequency chart

#define SAMPLE_ROOT 69
extern byte lastPotRow;
extern int pixelTouchX;
extern int gridTouchY;
extern byte arrangerpage;
extern byte active_track;
bool otherCtrlButtons = true;
bool buttonPressed[NUM_BUTTONS];

// calibrate your Screen
//  This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 440
#define TS_MINY 300
#define TS_MAXX 3850
#define TS_MAXY 3800
bool display_touched = false;
int touchedX;
int touchedY;

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

RotaryEncoder Enc1(31, 32);
RotaryEncoder Enc2(27, 28);
RotaryEncoder Enc3(24, 25);
RotaryEncoder Enc4(2, 3);
RotaryEncoder *allEncoders[NUM_ENCODERS]{&Enc1, &Enc2, &Enc3, &Enc4};

long oldEnc[4] = {-999, -999, -999, -999};
// Encoder Buttons
const uint8_t BUTTON_PINS[NUM_ENCODERS] = {30, 29, 26, 4};
Bounce *encButtons = new Bounce[NUM_ENCODERS];
// micros object for midiclock
elapsedMicros msecsclock;

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
    if (xPos != last_xPos || yPos != last_yPos)
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
  bool clock_is_on_tick = false;
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
        clock_is_on_tick = true;
        stop_once = true;
        return true;
      }
      else
        clock_is_on_tick = false;
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
        bar_tick = start_of_loop;
        bar_tick_display = -1;
        tft->fillRect(STEP_FRAME_W * 2, SONG_POSITION_POINTER_Y, STEP_FRAME_W * 16, 4, ILI9341_DARKGREY);
        tft->fillRect(STEP_FRAME_W * 2, GRID_POSITION_POINTER_Y, STEP_FRAME_W * 16, 4, ILI9341_DARKGREY);
        tft->fillRect(STEP_FRAME_W * 2, STEP_POSITION_POINTER_Y, STEP_FRAME_W * 16, 4, ILI9341_DARKGREY);
        tft->asyncUpdateActive();
        clock_is_on_tick = false;
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
  bool Clock_is_on_tick()
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

Track track1(&tft, &MasterOut, 1, 10, 10);
Track track2(&tft, &MasterOut, 2, 1, 1);
Track track3(&tft, &MasterOut, 3, 2, 2);
Track track4(&tft, &MasterOut, 4, 3, 3);

Track track5(&tft, &MasterOut, 5, 4, 4);
Track track6(&tft, &MasterOut, 6, 5, 5);
Track track7(&tft, &MasterOut, 7, 6, 6);
Track track8(&tft, &MasterOut, 8, 7, 7);

Track *allTracks[8]{&track1, &track2, &track3, &track4, &track5, &track6, &track7, &track8};

// put function declarations here:
void readEncoders();
void encoder_SetCursor(byte maxY);

void readTouchinput();
void readMainButtons();
void buttons_SetPlayStatus();
void buttons_SetCorsor();
void buttons_SelectTrack();
void buttons_SelectPlugin();
void buttons_SelectMixer();
void buttons_SelectArranger();
void buttons_SelectSequencerMode();
void buttons_SetNoteOnTick(int x, byte y);
void buttons_Set_potRow();
void input_behaviour();
void clock_to_notes();

void myDrawLine(int x0, int y0, int x1, int y1, uint16_t color);
void myDrawRect(int x, int y, int w, int h, uint16_t color);
void drawActiveRect(int xPos, byte yPos, byte xsize, byte ysize, bool state, const char *name, int color);
void clearWorkSpace();
void startUpScreen();
void drawsongmodepageselector();
void gridSongMode(int songpageNumber);

void draw_mixer();
void set_mixer_gain(byte XPos, byte YPos, const char *name, byte trackn);
void set_mixer(byte row);
void set_mixer_mute(byte XPos, byte YPos, const char *name, int min, int max);
void draw_mixer_FX_page1();
void draw_mixer_FX_page2();
void set_mixer_FX_page1(byte row);
void set_mixer_FX_page2(byte row);
void set_mixer_dry(byte XPos, byte YPos, const char *name, byte trackn);
void set_mixer_FX1(byte XPos, byte YPos, const char *name, byte trackn);
void set_mixer_FX2(byte XPos, byte YPos, const char *name, byte trackn);
void set_mixer_FX3(byte XPos, byte YPos, const char *name, byte trackn);
void myNoteOn(byte channel, byte note, byte velocity);
void myNoteOff(byte channel, byte note, byte velocity);

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200); // set MIDI baud
                        // while the serial stream is not open, do nothing:
  // while (!Serial) ;
  // initialize the TFT- and Touchscreen

  tft.begin();
  tft.setRotation(3);
  ts.begin();
  ts.setRotation(1);
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

  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleNoteOn(myNoteOn);
  AudioMemory(200);
  MasterOut.setup();
  note_frequency = new float[128];
  for (int r = 0; r < 128; r++)
  {
    note_frequency[r] = pow(2.0, ((double)(r - SAMPLE_ROOT) / 12.0));
  }

  usbMIDI.begin(); // Launch MIDI and listen to channel 4
  startUpScreen();
}

void loop()
{
  // See if there's any  touch data for us

  usbMIDI.read();
  Masterclock.process_MIDItick();
  clock_to_notes();
  readEncoders();
  readMainButtons();
  readTouchinput();
  input_behaviour();

  // placeholder for debugging
  /*if (millis() % 200 == 0)
  {
    // Serial.println(Masterclock.playing ? "True" : "False");
  }*/

  // placeholder for debugging
  if (millis() % 50 == 0)
  {
    tft.updateScreenAsync();
    if (encoder_function == INPUT_FUNCTIONS_FOR_ARRANGER)
      cursor.update(pixelTouchX, gridTouchY, ARRANGER_FRAME_H);
    else
      cursor.update(pixelTouchX, gridTouchY, STEP_FRAME_H);
    tft.fillRect(70, lastPotRow * 4, 10, 3, ILI9341_RED);
    // Serial.printf("pl2: %f, fx2: %f, pl3: %f, fx3: %f\n", MasterOut.fx_section.plugin_2.peak.read(),MasterOut.fx_section.peak2.read(), plugin_3.peak.read(), MasterOut.fx_section.peak3.read());
  }
  if (buttonPressed[BUTTON_ENTER])
  {
    // MasterOut.fx_section.plugin_2.get_peak();
    plugin_3.get_peak();
    MasterOut.fx_section.get_peak();
  }
   // tft.updateScreen();
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
  buttons_SelectSequencerMode();
  buttons_SelectMixer();
  // if none of the bottom buttons are pressed with the upper ones
  if (!otherCtrlButtons)
  {
    buttons_SetCorsor();
    buttons_Set_potRow();
    buttons_SetPlayStatus();
  }
  // if we are in one of the sequencer pages
  if (encoder_function == INPUT_FUNCTIONS_FOR_SEQUENCER)
  {
    // if Shift button is NOT pressed
    if (!buttonPressed[BUTTON_SHIFT])
    {
      encoder_SetCursor(14);                        // Encoder: 0,1
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
  if (encoder_function == INPUT_FUNCTIONS_FOR_ARRANGER)
  {
    // if Shift button is NOT pressed
    if (!buttonPressed[BUTTON_SHIFT])
    {
      // gridTouchY = 0;
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
  // if we are in one of the pluginpages
  if (encoder_function == INPUT_FUNCTIONS_FOR_PLUGIN)
  {
    MasterOut.set_parameters(active_track, lastPotRow);
  }
  if (encoder_function == INPUT_FUNCTIONS_FOR_SEQUENCER_MODES)
  {
    // if Shift button is NOT pressed
    if (!buttonPressed[BUTTON_SHIFT])
    {
      for (int i = 0; i < NUM_TRACKS; i++)
      {
        allTracks[i]->set_SeqMode_parameters(lastPotRow);
      }
    }
  }

  if (encoder_function == INPUT_FUNCTIONS_FOR_MIXER1)
    set_mixer(lastPotRow);
  if (encoder_function == INPUT_FUNCTIONS_FOR_MIXER2)
    set_mixer_FX_page1(lastPotRow);
  if (encoder_function == INPUT_FUNCTIONS_FOR_MIXER3)
    set_mixer_FX_page2(lastPotRow);
  if (encoder_function == INPUT_FUNCTIONS_FOR_FX1)
    fx_1.set_parameters(lastPotRow);
  if (encoder_function == INPUT_FUNCTIONS_FOR_FX2)
    fx_2.set_parameters(lastPotRow);
}
void clock_to_notes()
{
  if (Masterclock.is_playing())
  {
    if (Masterclock.clock_is_on_tick)
    {
      // Serial.println(Masterclock.MIDItick);
      for (int t = 0; t < NUM_TRACKS; t++)
      {
        allTracks[t]->play_sequencer_mode(Masterclock.MIDItick, Masterclock.start_of_loop, Masterclock.end_of_loop);
      }
    }
  }
}

void readTouchinput()
{

  // tft.waitUpdateAsyncComplete();
  //  Serial.println("would like to be touched");
  if (!tft.asyncUpdateActive() && ts.touched())
  {

    TS_Point touch = ts.getPoint();
    touch.x = map(touch.x, TS_MINX, TS_MAXX, 0, 320);
    touch.y = map(touch.y, TS_MINY, TS_MAXY, 0, 240);
    touchedX = touch.x;
    touchedY = touch.y;
    // pixelTouchX = touch.x;
    // gridTouchY = touch.y / 16;
    display_touched = true;
    // if (millis() % 50 == 0)
    // Serial.printf("touched @ x %d, y %d \n", touchedX, touchedY);
  }

  else
    display_touched = false;
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
  }
  if (enc_moved[1])
  {
    gridTouchY = constrain(gridTouchY + encoded[1], 0, maxY);
    enc_moved[1] = false;
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
      allTracks[t]->internal_clock_bar = 0;
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
        encoder_function = INPUT_FUNCTIONS_FOR_SEQUENCER;
        // buttonPressed[BUTTON_TRACK] = false;
        allTracks[active_track]->drawStepSequencerStatic();
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
    for (int i = 0; i < BUTTONS_PER_ROW; i++)
    {
      if (buttonPressed[i])
      {
        active_track = i;
        MasterOut.draw_plugin(i, allTracks[i]->MIDI_channel_out);
        encoder_function = INPUT_FUNCTIONS_FOR_PLUGIN;
        // Serial.printf("plugin selected Track: %d on channel: %d\n", i,allTracks[i]->MIDI_channel_out );
        //  buttonPressed[BUTTON_PLUGIN] = false;
        buttonPressed[i] = false;
      }
    }
  }
}
void buttons_SelectMixer()
{
  if (buttonPressed[BUTTON_MIXER])
  {
    if (buttonPressed[BUTTON_LEFT])
    {
      change_plugin_row = true;
      buttonPressed[BUTTON_LEFT] = false;
      encoder_function = INPUT_FUNCTIONS_FOR_MIXER1;
      clearWorkSpace();
      draw_mixer();
    }
    if (buttonPressed[BUTTON_RIGHT])
    {
      change_plugin_row = true;
      buttonPressed[BUTTON_RIGHT] = false;
      encoder_function = INPUT_FUNCTIONS_FOR_MIXER2;
      clearWorkSpace();
      draw_mixer_FX_page1();
    }
    if (buttonPressed[BUTTON_UP])
    {
      change_plugin_row = true;
      buttonPressed[BUTTON_UP] = false;
      encoder_function = INPUT_FUNCTIONS_FOR_MIXER3;
      clearWorkSpace();
      draw_mixer_FX_page2();
    }
    if (buttonPressed[BUTTON_DOWN])
    {
      change_plugin_row = true;
      buttonPressed[BUTTON_DOWN] = false;
      encoder_function = INPUT_FUNCTIONS_FOR_FX1;
      clearWorkSpace();
      fx_1.draw_plugin();
    }
    if (buttonPressed[BUTTON_ROW])
    {
      change_plugin_row = true;
      buttonPressed[BUTTON_ROW] = false;
      encoder_function = INPUT_FUNCTIONS_FOR_FX2;
      clearWorkSpace();
      fx_2.draw_plugin();
    }
  }
}

void buttons_SelectSequencerMode()
{ // select active plugin from choosen track
  if (buttonPressed[BUTTON_FX])
  {
    for (int i = 0; i < BUTTONS_PER_ROW; i++)
    {
      if (buttonPressed[i])
      {
        allTracks[i]->draw_sequencer_modes(allTracks[i]->sequencer_mode);
        encoder_function = INPUT_FUNCTIONS_FOR_SEQUENCER_MODES;
        Serial.println("SeqMode selected");
        // buttonPressed[BUTTON_PLUGIN] = false;
        buttonPressed[i] = false;
      }
    }
  }
}
void buttons_SelectArranger()
{
  if (buttonPressed[BUTTON_SONG])
  {
    encoder_function = INPUT_FUNCTIONS_FOR_ARRANGER;
    arrangerpage = SONGMODE_PAGE_11;
    if (buttonPressed[BUTTON_LEFT])
    {
      arrangerpage = SONGMODE_PAGE_1;
      buttonPressed[BUTTON_LEFT] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_RIGHT])
    {
      arrangerpage = SONGMODE_PAGE_2;
      buttonPressed[BUTTON_RIGHT] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_UP])
    {
      arrangerpage = SONGMODE_PAGE_3;
      buttonPressed[BUTTON_UP] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_DOWN])
    {
      arrangerpage = SONGMODE_PAGE_4;
      buttonPressed[BUTTON_DOWN] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_ROW])
    {
      arrangerpage = SONGMODE_PAGE_5;
      buttonPressed[BUTTON_ROW] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_REC])
    {
      arrangerpage = SONGMODE_PAGE_6;
      buttonPressed[BUTTON_REC] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_PLAY])
    {
      arrangerpage = SONGMODE_PAGE_7;
      buttonPressed[BUTTON_PLAY] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_STOP])
    {
      arrangerpage = SONGMODE_PAGE_8;
      buttonPressed[BUTTON_STOP] = false;
      buttonPressed[BUTTON_SONG] = false;
    }
    if (buttonPressed[BUTTON_TRACK])
    {
      arrangerpage = SONGMODE_PAGE_9;
      buttonPressed[BUTTON_TRACK] = false;
      buttonPressed[BUTTON_SONG] = false;
    }

    Serial.println(arrangerpage);
    gridSongMode(arrangerpage);
    for (int i = 0; i < NUM_TRACKS; i++)
      allTracks[i]->draw_arrangment_lines(3, arrangerpage);
  }
}
void buttons_Set_potRow()
{
  if (buttonPressed[BUTTON_ROW])
  {
    change_plugin_row = true;
    lastPotRow++;
    if (lastPotRow >= 4)
    {
      lastPotRow = 0;
    }
    tft.fillRect(70, 0, 10, 16, ILI9341_DARKGREY);
    buttonPressed[BUTTON_ROW] = false;
  }
}
// void drawPot(int XPos, byte YPos, int dvalue, int min, int max, const char *dname, int color)

void drawActiveRect(int xPos, byte yPos, byte xsize, byte ysize, bool state, const char *name, int color)
{
  if (state)
  {
    tft.fillRect(STEP_FRAME_W * xPos, STEP_FRAME_H * yPos, STEP_FRAME_W * xsize, STEP_FRAME_W * ysize, color);
    tft.drawRect(STEP_FRAME_W * xPos, STEP_FRAME_H * yPos, STEP_FRAME_W * xsize, STEP_FRAME_W * ysize, color);
    tft.setFont(Arial_8);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(STEP_FRAME_W * xPos + 4, STEP_FRAME_H * yPos + 3);
    tft.print(name);
  }
  if (!state)
  {
    tft.fillRect(STEP_FRAME_W * xPos, STEP_FRAME_H * yPos, STEP_FRAME_W * xsize, STEP_FRAME_W * ysize, ILI9341_DARKGREY);
    tft.drawRect(STEP_FRAME_W * xPos, STEP_FRAME_H * yPos, STEP_FRAME_W * xsize, STEP_FRAME_W * ysize, color);
    tft.setFont(Arial_8);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(STEP_FRAME_W * xPos + 4, STEP_FRAME_H * yPos + 3);
    tft.print(name);
  }
}
void myDrawLine(int x0, int y0, int x1, int y1, uint16_t color)
{
  tft.drawLine(x0, y0, x1, y1, color);
}
void myDrawRect(int x, int y, int w, int h, uint16_t color)
{
  tft.drawRect(x, y, w, h, color);
}
void clearWorkSpace()
{                                                                                                       // clear the whole grid from Display
  tft.fillRect(STEP_FRAME_W, STEP_FRAME_H, STEP_FRAME_W * 21, STEP_FRAME_H * 13 + 4, ILI9341_DARKGREY); // Xmin, Ymin, Xlength, Ylength, color
                                                                                                        // tft->fillRect(STEP_FRAME_W, STEP_FRAME_H, STEP_FRAME_W, STEP_FRAME_H * 12, ILI9341_DARKGREY);
}
// songmode / arranger
void drawsongmodepageselector()
{
  // draw 16 rects of 16x16px in the 13th row
  for (int pages = 2; pages < 18; pages++)
  {
    // drawActiveRect(pages, 13, 1, 1, selectPage == pages + 8, "", ILI9341_LIGHTGREY);
    tft.drawRect(STEP_FRAME_W * pages, STEP_FRAME_H * 13 + 4, STEP_FRAME_W, STEP_FRAME_H, ILI9341_WHITE);
    tft.setFont(Arial_8);
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(STEP_FRAME_W * pages + 3, STEP_FRAME_H * 13 + 8);
    tft.print((pages - 1));
  }
}
void gridSongMode(int songpageNumber)
{ // static Display rendering
  // int page_phrase_start = songpageNumber * 16;
  // int page_phrase_end = (songpageNumber + 1) * 16;
  clearWorkSpace();
  drawsongmodepageselector();
  // drawActiveRect(18, 3, 2, 2, false, "clear", ILI9341_RED);

  // vertical pointer Lines
  int shownLines = 257 / phraseSegmentLength;
  for (int f = 0; f < shownLines; f++)
  {                                                                                              // do this for all phrases
    tft.drawFastVLine((f * phraseSegmentLength) + 32, STEP_FRAME_H + 4, STEP_FRAME_H * 12, 360); //(x, y-start, y-length, color)
    if (f % 4 == 0)
    {
      tft.drawFastVLine((f * phraseSegmentLength) + 32, STEP_FRAME_H + 4, STEP_FRAME_H * 12, 370); //(x, y-start, y-length, color)
    }
  }
}

void startUpScreen()
{

  // static Display rendering
  tft.fillScreen(ILI9341_DARKGREY);

  tft.setFont(Arial_9);

  // songmode button
  tft.setTextColor(ILI9341_BLACK);
  tft.fillRect(1, 1, 15, ARRANGER_FRAME_H, ILI9341_MAGENTA); // Xmin, Ymin, Xlength, Ylength, color
  tft.setCursor(4, 3);
  tft.print("S");

  // Drumtrack button
  tft.fillRect(1, STEP_FRAME_H + 8, 15, TRACK_FRAME_H, trackColor[0]); // Xmin, Ymin, Xlength, Ylength, color
  tft.setCursor(4, TRACK_FRAME_H + 8);
  tft.print("D");

  // other tracks buttons
  for (int otherTracks = 2; otherTracks <= 8; otherTracks++)
  {
    tft.fillRect(1, TRACK_FRAME_H * otherTracks, 15, TRACK_FRAME_H, trackColor[otherTracks - 1]); // Xmin, Ymin, Xlength, Ylength, color
    tft.setCursor(4, TRACK_FRAME_H * otherTracks + 6);
    tft.print(otherTracks);
  }
  // Mixer button
  tft.fillRect(1, STEP_FRAME_H * 13 + 8, 15, TRACK_FRAME_H, ILI9341_LIGHTGREY); // Xmin, Ymin, Xlength, Ylength, color
  tft.setCursor(3, STEP_FRAME_H * 13 + 14);
  tft.print("M");

  // scale Select
  tft.drawRect(STEP_FRAME_W * POSITION_SCALE_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);

  // arrangment Select
  tft.drawRect(STEP_FRAME_W * POSITION_ARR_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);

  // record button & Rectangle
  tft.drawRect(STEP_FRAME_W * POSITION_RECORD_BUTTON, 0, STEP_FRAME_W, STEP_FRAME_H, ILI9341_WHITE);
  tft.fillCircle(STEP_FRAME_W * POSITION_RECORD_BUTTON + 7, 7, DOT_RADIUS + 1, ILI9341_LIGHTGREY);

  // Play button & Rectangle
  tft.drawRect(STEP_FRAME_W * POSITION_PLAY_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);                                                                     // PLAY RECT FRAME
  tft.fillTriangle(STEP_FRAME_W * POSITION_PLAY_BUTTON + 12, 3, STEP_FRAME_W * POSITION_PLAY_BUTTON + 12, 13, STEP_FRAME_W * POSITION_PLAY_BUTTON + 22, 8, ILI9341_GREEN); // x1, y1, x2, y2, x3, y3

  // stop button & Rectangle
  tft.drawRect(STEP_FRAME_W * POSITION_STOP_BUTTON, 0, STEP_FRAME_W, STEP_FRAME_H, ILI9341_WHITE);
  tft.fillRect(STEP_FRAME_W * POSITION_STOP_BUTTON + 3, 3, 10, 10, ILI9341_LIGHTGREY);

  // barcounter & Rectangle
  tft.drawRect(STEP_FRAME_W * POSITION_BAR_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);

  // tempo button & Rectangle
  tft.drawRect(STEP_FRAME_W * POSITION_BPM_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);

  // save button & Rectangle
  tft.drawRect(STEP_FRAME_W * POSITION_SAVE_BUTTON, 0, STEP_FRAME_W * 2, STEP_FRAME_H, ILI9341_WHITE);
  tft.fillRect(STEP_FRAME_W * POSITION_SAVE_BUTTON + 1, 1, STEP_FRAME_W * 2 - 2, STEP_FRAME_H - 2, ILI9341_ORANGE);
  tft.setTextColor(ILI9341_BLACK);
  tft.setFont(Arial_9);
  tft.setCursor(STEP_FRAME_W * POSITION_SAVE_BUTTON + 2, 3);
  tft.print("SAV");

  // load button & Rectangle
  tft.drawRect(STEP_FRAME_W * POSITION_LOAD_BUTTON, 0, STEP_FRAME_W, STEP_FRAME_H, ILI9341_WHITE);
  tft.fillRect(STEP_FRAME_W * POSITION_LOAD_BUTTON + 1, 1, STEP_FRAME_W - 2, STEP_FRAME_H - 2, ILI9341_GREENYELLOW);
  tft.setTextColor(ILI9341_BLACK);
  tft.setFont(Arial_9);
  tft.setCursor(STEP_FRAME_W * POSITION_LOAD_BUTTON + 4, 3);
  tft.print("L");
}
void myNoteOn(byte channel, byte note, byte velocity)
{
  if (channel < 9 && !allTracks[channel - 1]->muted)
    MasterOut.noteOn(note, VELOCITY_NOTE_ON, allTracks[channel - 1]->MIDI_channel_out, 0);
}
void myNoteOff(byte channel, byte note, byte velocity)
{
  if (channel < 9 && !allTracks[channel - 1]->muted)
    MasterOut.noteOff(note, 0, allTracks[channel - 1]->MIDI_channel_out, 0);
}

// Mixer
void draw_mixer()
{

  if (change_plugin_row)
  {
    change_plugin_row = false;
    drawPot(0, 0, allTracks[0]->mixGainPot, "Tr D");
    drawActiveRect(3, 5, 1, 1, allTracks[0]->muted, "M", ILI9341_RED);
    drawActiveRect(4, 5, 1, 1, allTracks[0]->soloed, "S", ILI9341_WHITE);
    drawPot(1, 0, allTracks[1]->mixGainPot, "Tr 2");
    drawActiveRect(7, 5, 1, 1, allTracks[1]->muted, "M", ILI9341_RED);
    drawActiveRect(8, 5, 1, 1, allTracks[1]->soloed, "S", ILI9341_WHITE);
    drawPot(2, 0, allTracks[2]->mixGainPot, "Tr 3");
    drawActiveRect(11, 5, 1, 1, allTracks[2]->muted, "M", ILI9341_RED);
    drawActiveRect(12, 5, 1, 1, allTracks[2]->soloed, "S", ILI9341_WHITE);
    drawPot(3, 0, allTracks[3]->mixGainPot, "Tr 4");
    drawActiveRect(15, 5, 1, 1, allTracks[3]->muted, "M", ILI9341_RED);
    drawActiveRect(16, 5, 1, 1, allTracks[3]->soloed, "S", ILI9341_WHITE);

    drawPot(0, 2, allTracks[4]->mixGainPot, "Tr 5");
    drawActiveRect(3, 11, 1, 1, allTracks[4]->muted, "M", ILI9341_RED);
    drawActiveRect(4, 11, 1, 1, allTracks[4]->soloed, "S", ILI9341_WHITE);
    drawPot(1, 2, allTracks[5]->mixGainPot, "Tr 6");
    drawActiveRect(7, 11, 1, 1, allTracks[5]->muted, "M", ILI9341_RED);
    drawActiveRect(8, 11, 1, 1, allTracks[5]->soloed, "S", ILI9341_WHITE);
    drawPot(2, 2, allTracks[6]->mixGainPot, "Tr 7");
    drawActiveRect(11, 11, 1, 1, allTracks[6]->muted, "M", ILI9341_RED);
    drawActiveRect(12, 11, 1, 1, allTracks[6]->soloed, "S", ILI9341_WHITE);
    drawPot(3, 2, allTracks[7]->mixGainPot, "Tr 8");
    drawActiveRect(15, 11, 1, 1, allTracks[7]->muted, "M", ILI9341_RED);
    drawActiveRect(16, 11, 1, 1, allTracks[7]->soloed, "S", ILI9341_WHITE);
  }
}
void set_mixer(byte row)
{
  draw_mixer();
  if (row == 0)
  {

    set_mixer_gain(0, 0, "Tr D", 0);
    set_mixer_mute(0, 0, "Tr D", 0, 1);
    set_mixer_gain(1, 0, "Tr 2", 1);
    set_mixer_mute(1, 0, "Tr 2", 0, 1);

    set_mixer_gain(2, 0, "Tr 3", 2);
    set_mixer_mute(2, 0, "Tr 3", 0, 1);
    set_mixer_gain(3, 0, "Tr 4", 3);
    set_mixer_mute(3, 0, "Tr 4", 0, 1);
  }

  if (row == 1)
  {
  }

  if (row == 2)
  {
    set_mixer_gain(0, 2, "Tr 5", 4);
    set_mixer_mute(0, 2, "Tr 5", 0, 1);
    set_mixer_gain(1, 2, "Tr 6", 5);
    set_mixer_mute(1, 2, "Tr 6", 0, 1);

    set_mixer_gain(2, 2, "Tr 7", 6);
    set_mixer_mute(2, 2, "Tr 7", 0, 1);
    set_mixer_gain(3, 2, "Tr 8", 7);
    set_mixer_mute(3, 2, "Tr 8", 0, 1);
  }

  if (row == 3)
  {
  }
}
void set_mixer_gain(byte XPos, byte YPos, const char *name, byte trackn)
{

  if (!buttonPressed[BUTTON_SHIFT])
  {
    if (enc_moved[XPos])
    {

      allTracks[trackn]->mixGainPot = constrain(allTracks[trackn]->mixGainPot + encoded[XPos], 0, MIDI_CC_RANGE);
      allTracks[trackn]->mixGain = (float)(allTracks[trackn]->mixGainPot / MIDI_CC_RANGE_FLOAT);
      /*for (int i = 0; i < NUM_PLUGINS; i++)
     {
       if (allTracks[trackn]->MIDI_channel_out == i+17)
         allPlugins[i]->MixGain.gain(allTracks[trackn]->mixGain);
     }*/
      if (allTracks[trackn]->MIDI_channel_out == 17)
        plugin_1.MixGain.gain(allTracks[trackn]->mixGain);
      if (allTracks[trackn]->MIDI_channel_out == 18)
        plugin_2.MixGain.gain(allTracks[trackn]->mixGain);
      if (allTracks[trackn]->MIDI_channel_out == 19)
        plugin_3.MixGain.gain(allTracks[trackn]->mixGain);
      if (allTracks[trackn]->MIDI_channel_out == 20)
        plugin_4.MixGain.gain(allTracks[trackn]->mixGain);
      if (allTracks[trackn]->MIDI_channel_out == 21)
        plugin_5.MixGain.gain(allTracks[trackn]->mixGain);
      if (allTracks[trackn]->MIDI_channel_out == 22)
        plugin_6.MixGain.gain(allTracks[trackn]->mixGain);
      if (allTracks[trackn]->MIDI_channel_out == 23)
        plugin_7.MixGain.gain(allTracks[trackn]->mixGain);
      if (allTracks[trackn]->MIDI_channel_out == 24)
        plugin_8.MixGain.gain(allTracks[trackn]->mixGain);

      drawPot(XPos, YPos, allTracks[trackn]->mixGainPot, name);
    }
  }
}
void set_mixer_mute(byte XPos, byte YPos, const char *name, int min, int max)
{
  int nYPos;
  if (YPos == 0)
    nYPos = 0;

  if (YPos == 2)
    nYPos = 1;

  if (buttonPressed[BUTTON_SHIFT])
  {
    if (enc_moved[XPos])
    {
      int n = XPos + (nYPos * NUM_ENCODERS);
      allTracks[n]->mute_norm_solo_pot = constrain(allTracks[n]->mute_norm_solo_pot + encoded[XPos], 0, 2);
      Serial.printf("track: %d, mute state: %d\n", n, allTracks[n]->mute_norm_solo_pot);
      if (allTracks[n]->mute_norm_solo_pot == 0)
      {
        allTracks[n]->muted = true;
      }

      if (allTracks[n]->mute_norm_solo_pot == 1)
      {
        allTracks[n]->muted = false;
        allTracks[n]->soloed = false;
        for (int i = 0; i < NUM_TRACKS; i++) // play other tracks
          allTracks[i]->muted = false;
      }

      if (allTracks[n]->mute_norm_solo_pot == 2) // solo state
      {
        for (int i = 0; i < NUM_TRACKS; i++) // mute other tracks
          allTracks[i]->muted = true;
        allTracks[n]->soloed = true;
        allTracks[n]->muted = false;

        // drawPot(XPos, YPos, allTracks[n]->mixGainPot, name);
      }
      change_plugin_row = true;
      draw_mixer();
    }
  }
}

void draw_mixer_FX_page1()
{
  if (change_plugin_row)
  {
    change_plugin_row = false;
    drawPot(0, 0, allTracks[0]->mixDryPot, "Dry D");
    drawPot(1, 0, allTracks[1]->mixDryPot, "Dry 2");
    drawPot(2, 0, allTracks[2]->mixDryPot, "Dry 3");
    drawPot(3, 0, allTracks[3]->mixDryPot, "Dry 4");

    drawPot(0, 1, allTracks[0]->mixFX1Pot, "FX1 D");
    drawPot(1, 1, allTracks[1]->mixFX1Pot, "FX1 2");
    drawPot(2, 1, allTracks[2]->mixFX1Pot, "FX1 3");
    drawPot(3, 1, allTracks[3]->mixFX1Pot, "FX1 4");

    drawPot(0, 2, allTracks[0]->mixFX2Pot, "FX2 D");
    drawPot(1, 2, allTracks[1]->mixFX2Pot, "FX2 2");
    drawPot(2, 2, allTracks[2]->mixFX2Pot, "FX2 3");
    drawPot(3, 2, allTracks[3]->mixFX2Pot, "FX2 4");

    drawPot(0, 3, allTracks[0]->mixFX3Pot, "FX3 D");
    drawPot(1, 3, allTracks[1]->mixFX3Pot, "FX3 2");
    drawPot(2, 3, allTracks[2]->mixFX3Pot, "FX3 3");
    drawPot(3, 3, allTracks[3]->mixFX3Pot, "FX3 4");
  }
}
void draw_mixer_FX_page2()
{

  if (change_plugin_row)
  {
    change_plugin_row = false;
    drawPot(0, 0, allTracks[4]->mixDryPot, "Dry 5");
    drawPot(1, 0, allTracks[5]->mixDryPot, "Dry 6");
    drawPot(2, 0, allTracks[6]->mixDryPot, "Dry 7");
    drawPot(3, 0, allTracks[7]->mixDryPot, "Dry 8");

    drawPot(0, 1, allTracks[4]->mixFX1Pot, "FX1 5");
    drawPot(1, 1, allTracks[5]->mixFX1Pot, "FX1 6");
    drawPot(2, 1, allTracks[6]->mixFX1Pot, "FX1 7");
    drawPot(3, 1, allTracks[7]->mixFX1Pot, "FX1 8");

    drawPot(0, 2, allTracks[4]->mixFX2Pot, "FX1 5");
    drawPot(1, 2, allTracks[5]->mixFX2Pot, "FX1 6");
    drawPot(2, 2, allTracks[6]->mixFX2Pot, "FX1 7");
    drawPot(3, 2, allTracks[7]->mixFX2Pot, "FX1 8");

    drawPot(0, 3, allTracks[4]->mixFX3Pot, "FX1 5");
    drawPot(1, 3, allTracks[5]->mixFX3Pot, "FX1 6");
    drawPot(2, 3, allTracks[6]->mixFX3Pot, "FX1 7");
    drawPot(3, 3, allTracks[7]->mixFX3Pot, "FX1 8");
  }
}
void set_mixer_FX_page1(byte row)
{
  draw_mixer_FX_page1();
  if (row == 0)
  {

    set_mixer_dry(0, 0, "Dry D", 0);
    set_mixer_dry(1, 0, "Dry 2", 1);
    set_mixer_dry(2, 0, "Dry 3", 2);
    set_mixer_dry(3, 0, "Dry 4", 3);
  }

  if (row == 1)
  {
    set_mixer_FX1(0, 1, "FX1 D", 0);
    set_mixer_FX1(1, 1, "FX1 2", 1);
    set_mixer_FX1(2, 1, "FX1 3", 2);
    set_mixer_FX1(3, 1, "FX1 4", 3);
  }

  if (row == 2)
  {
    set_mixer_FX2(0, 2, "FX2 D", 0);
    set_mixer_FX2(1, 2, "FX2 2", 1);
    set_mixer_FX2(2, 2, "FX2 3", 2);
    set_mixer_FX2(3, 2, "FX2 4", 3);
  }

  if (row == 3)
  {
    set_mixer_FX3(0, 3, "FX2 D", 0);
    set_mixer_FX3(1, 3, "FX2 2", 1);
    set_mixer_FX3(2, 3, "FX2 3", 2);
    set_mixer_FX3(3, 3, "FX2 4", 3);
  }
}
void set_mixer_FX_page2(byte row)
{
  draw_mixer_FX_page1();
  if (row == 0)
  {

    set_mixer_dry(0, 0, "Dry 5", 4);
    set_mixer_dry(1, 0, "Dry 6", 5);
    set_mixer_dry(2, 0, "Dry 7", 6);
    set_mixer_dry(3, 0, "Dry 8", 7);
  }

  if (row == 1)
  {
    set_mixer_FX1(0, 1, "FX1 5", 4);
    set_mixer_FX1(1, 1, "FX1 6", 5);
    set_mixer_FX1(2, 1, "FX1 7", 6);
    set_mixer_FX1(3, 1, "FX1 8", 7);
  }

  if (row == 2)
  {
    set_mixer_FX2(0, 2, "FX2 5", 4);
    set_mixer_FX2(1, 2, "FX2 6", 5);
    set_mixer_FX2(2, 2, "FX2 7", 6);
    set_mixer_FX2(3, 2, "FX2 8", 7);
  }

  if (row == 3)
  {
    set_mixer_FX3(0, 3, "FX3 5", 4);
    set_mixer_FX3(1, 3, "FX3 6", 5);
    set_mixer_FX3(2, 3, "FX3 7", 6);
    set_mixer_FX3(3, 3, "FX3 8", 7);
  }
}

void set_mixer_dry(byte XPos, byte YPos, const char *name, byte trackn)
{
  if (!buttonPressed[BUTTON_SHIFT])
  {
    if (enc_moved[XPos])
    {
      allTracks[trackn]->mixDryPot = constrain(allTracks[trackn]->mixDryPot + encoded[XPos], 0, MIDI_CC_RANGE);
      allTracks[trackn]->mixDry = (float)(allTracks[trackn]->mixDryPot / MIDI_CC_RANGE_FLOAT);
      if (allTracks[trackn]->MIDI_channel_out == 17)
        MasterOut.fx_section.dry_1.gain(allTracks[trackn]->mixDry);
      if (allTracks[trackn]->MIDI_channel_out == 18)
        MasterOut.fx_section.dry_2.gain(allTracks[trackn]->mixDry);
      if (allTracks[trackn]->MIDI_channel_out == 19)
        MasterOut.fx_section.dry_3.gain(allTracks[trackn]->mixDry);
      if (allTracks[trackn]->MIDI_channel_out == 20)
        MasterOut.fx_section.dry_4.gain(allTracks[trackn]->mixDry);
      if (allTracks[trackn]->MIDI_channel_out == 21)
        MasterOut.fx_section.dry_5.gain(allTracks[trackn]->mixDry);
      if (allTracks[trackn]->MIDI_channel_out == 22)
        MasterOut.fx_section.dry_6.gain(allTracks[trackn]->mixDry);
      if (allTracks[trackn]->MIDI_channel_out == 23)
        MasterOut.fx_section.dry_7.gain(allTracks[trackn]->mixDry);
      if (allTracks[trackn]->MIDI_channel_out == 24)
        MasterOut.fx_section.dry_8.gain(allTracks[trackn]->mixDry);

      drawPot(XPos, YPos, allTracks[trackn]->mixDryPot, name);
    }
  }
}
void set_mixer_FX1(byte XPos, byte YPos, const char *name, byte trackn)
{
  if (!buttonPressed[BUTTON_SHIFT])
  {
    if (enc_moved[XPos])
    {
      allTracks[trackn]->mixFX1Pot = constrain(allTracks[trackn]->mixFX1Pot + encoded[XPos], 0, MIDI_CC_RANGE);
      allTracks[trackn]->mixFX1 = (float)(allTracks[trackn]->mixFX1Pot / MIDI_CC_RANGE_FLOAT);
      if (allTracks[trackn]->MIDI_channel_out == 17)
        fx_1.pl_1.gain(allTracks[trackn]->mixFX1);
      if (allTracks[trackn]->MIDI_channel_out == 18)
        fx_1.pl_2.gain(allTracks[trackn]->mixFX1);
      if (allTracks[trackn]->MIDI_channel_out == 19)
        fx_1.pl_3.gain(allTracks[trackn]->mixFX1);
      if (allTracks[trackn]->MIDI_channel_out == 20)
        fx_1.pl_4.gain(allTracks[trackn]->mixFX1);
      if (allTracks[trackn]->MIDI_channel_out == 21)
        fx_1.pl_5.gain(allTracks[trackn]->mixFX1);
      if (allTracks[trackn]->MIDI_channel_out == 22)
        fx_1.pl_6.gain(allTracks[trackn]->mixFX1);
      if (allTracks[trackn]->MIDI_channel_out == 23)
        fx_1.pl_7.gain(allTracks[trackn]->mixFX1);
      if (allTracks[trackn]->MIDI_channel_out == 24)
        fx_1.pl_8.gain(allTracks[trackn]->mixFX1);

      drawPot(XPos, YPos, allTracks[trackn]->mixFX1Pot, name);
    }
  }
}
void set_mixer_FX2(byte XPos, byte YPos, const char *name, byte trackn)
{
  if (!buttonPressed[BUTTON_SHIFT])
  {
    if (enc_moved[XPos])
    {
      allTracks[trackn]->mixFX2Pot = constrain(allTracks[trackn]->mixFX2Pot + encoded[XPos], 0, MIDI_CC_RANGE);
      allTracks[trackn]->mixFX2 = (float)(allTracks[trackn]->mixFX2Pot / MIDI_CC_RANGE_FLOAT);
      if (allTracks[trackn]->MIDI_channel_out == 17)
        fx_2.pl_1.gain(allTracks[trackn]->mixFX2);
      if (allTracks[trackn]->MIDI_channel_out == 18)
        fx_2.pl_2.gain(allTracks[trackn]->mixFX2);
      if (allTracks[trackn]->MIDI_channel_out == 19)
        fx_2.pl_3.gain(allTracks[trackn]->mixFX2);
      if (allTracks[trackn]->MIDI_channel_out == 20)
        fx_2.pl_4.gain(allTracks[trackn]->mixFX2);
      if (allTracks[trackn]->MIDI_channel_out == 21)
        fx_2.pl_5.gain(allTracks[trackn]->mixFX2);
      if (allTracks[trackn]->MIDI_channel_out == 22)
        fx_2.pl_6.gain(allTracks[trackn]->mixFX2);
      if (allTracks[trackn]->MIDI_channel_out == 23)
        fx_2.pl_7.gain(allTracks[trackn]->mixFX2);
      if (allTracks[trackn]->MIDI_channel_out == 24)
        fx_2.pl_8.gain(allTracks[trackn]->mixFX2);

      drawPot(XPos, YPos, allTracks[trackn]->mixFX2Pot, name);
    }
  }
}
void set_mixer_FX3(byte XPos, byte YPos, const char *name, byte trackn)
{

  if (!buttonPressed[BUTTON_SHIFT])
  {
    if (enc_moved[XPos])
    {

      allTracks[trackn]->mixFX3Pot = constrain(allTracks[trackn]->mixFX3Pot + encoded[XPos], 0, MIDI_CC_RANGE);
      allTracks[trackn]->mixFX3 = (float)(allTracks[trackn]->mixFX3Pot / MIDI_CC_RANGE_FLOAT);
      if (allTracks[trackn]->MIDI_channel_out == 17)
        fx_3.pl_1.gain(allTracks[trackn]->mixFX3);
      if (allTracks[trackn]->MIDI_channel_out == 18)
        fx_3.pl_2.gain(allTracks[trackn]->mixFX3);
      if (allTracks[trackn]->MIDI_channel_out == 19)
        fx_3.pl_3.gain(allTracks[trackn]->mixFX3);
      if (allTracks[trackn]->MIDI_channel_out == 20)
        fx_3.pl_4.gain(allTracks[trackn]->mixFX3);
      if (allTracks[trackn]->MIDI_channel_out == 21)
        fx_3.pl_5.gain(allTracks[trackn]->mixFX3);
      if (allTracks[trackn]->MIDI_channel_out == 22)
        fx_3.pl_6.gain(allTracks[trackn]->mixFX3);
      if (allTracks[trackn]->MIDI_channel_out == 23)
        fx_3.pl_7.gain(allTracks[trackn]->mixFX3);
      if (allTracks[trackn]->MIDI_channel_out == 24)
        fx_3.pl_8.gain(allTracks[trackn]->mixFX3);

      drawPot(XPos, YPos, allTracks[trackn]->mixFX3Pot, name);
    }
  }
}
