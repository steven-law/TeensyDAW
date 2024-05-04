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
#include <global_stuff.h>
#include <AudioSamples.h>
#include <Output.h>
#include <Track.h>
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

// notenumber to frequency chart

#define SAMPLE_ROOT 69

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
        bar_tick = start_of_loop;
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
void drawPot(int XPos, byte YPos, int dvalue, const char *dname);
void clearWorkSpace();
void startUpScreen();
void drawsongmodepageselector();
void gridSongMode(int songpageNumber);

void draw_mixer();
void set_mixer_gain(byte XPos, byte YPos, const char *name, int min, int max);
void set_mixer(byte row);

void myNoteOn(byte channel, byte note, byte velocity);
void myNoteOff(byte channel, byte note, byte velocity);
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
  usbMIDI.read();
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

    if (encoder_function == INPUT_FUNCTIONS_FOR_ARRANGER)
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
    // if Shift button is NOT pressed
    if (!buttonPressed[BUTTON_SHIFT])
    {

      MasterOut.set_parameters(active_track, lastPotRow);
    }
  }
  if (encoder_function == INPUT_FUNCTIONS_FOR_MIXER1)
  {
    // if Shift button is NOT pressed
    if (!buttonPressed[BUTTON_SHIFT])
    {
      set_mixer(lastPotRow);
    }
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
        allTracks[t]->play_sequencer_mode(Masterclock.MIDItick, Masterclock.start_of_loop, Masterclock.end_of_loop);
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
    change_plugin_row = true;
    buttonPressed[BUTTON_MIXER] = false;
    encoder_function = INPUT_FUNCTIONS_FOR_MIXER1;
     clearWorkSpace();
    draw_mixer();
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
void drawPot(int XPos, byte YPos, int dvalue, const char *dname)
{
  enc_moved[XPos] = false;
  // xposition, yposition, value 1-100, value to draw, name to draw, color
  // drawPot Variables
  static float circlePos[4];
  static float circlePos_old[4];
  static int dvalue_old[4];
  // byte fvalue = map(dvalue, 0, 127, min, max);
  int xPos;
  int color;
  if (XPos == 0)
  {
    xPos = 3;
    color = ILI9341_BLUE;
  }
  if (XPos == 1)
  {
    xPos = 7;
    color = ILI9341_RED;
  }
  if (XPos == 2)
  {
    xPos = 11;
    color = ILI9341_GREEN;
  }
  if (XPos == 3)
  {
    xPos = 15;
    color = ILI9341_WHITE;
  }
  if (YPos != lastPotRow)
    color = ILI9341_LIGHTGREY;

  int yPos = (YPos + 1) * 3;

  circlePos[XPos] = dvalue / 63.5;

  tft.setFont(Arial_8);
  tft.setTextColor(ILI9341_DARKGREY);
  tft.setCursor(STEP_FRAME_W * xPos + 4, STEP_FRAME_H * yPos - 3);
  tft.print(dvalue_old[XPos]);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(STEP_FRAME_W * xPos + 4, STEP_FRAME_H * yPos - 3);
  tft.print(dvalue);
  tft.setCursor(STEP_FRAME_W * xPos, STEP_FRAME_H * (yPos + 1) + 3);
  tft.print(dname);

  tft.fillCircle(STEP_FRAME_W * (xPos + 1) + 16 * cos((2.5 * circlePos_old[XPos]) + 2.25), STEP_FRAME_H * yPos + 16 * sin((2.5 * circlePos_old[XPos]) + 2.25), 4, ILI9341_DARKGREY);
  tft.drawCircle(STEP_FRAME_W * (xPos + 1), STEP_FRAME_H * yPos, 16, ILI9341_LIGHTGREY);
  tft.fillCircle(STEP_FRAME_W * (xPos + 1) + 16 * cos((2.5 * circlePos[XPos]) + 2.25), STEP_FRAME_H * yPos + 16 * sin((2.5 * circlePos[XPos]) + 2.25), 4, color);
  circlePos_old[XPos] = circlePos[XPos];
  dvalue_old[XPos] = dvalue;
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
  if (channel < 9)
    MasterOut.noteOn(note, VELOCITY_NOTE_ON, allTracks[channel - 1]->MIDI_channel_out, 0);
}
void myNoteOff(byte channel, byte note, byte velocity)
{
  if (channel < 9)
    MasterOut.noteOff(note, 0, allTracks[channel - 1]->MIDI_channel_out, 0);
}

// Mixer
void draw_mixer()
{
 
  if (change_plugin_row)
  {
    change_plugin_row = false;
    drawPot(0, 0, allTracks[0]->mixGainPot, "Tr 1");
    drawPot(1, 0, allTracks[1]->mixGainPot, "Tr 2");
    drawPot(2, 0, allTracks[2]->mixGainPot, "Tr 3");
    drawPot(3, 0, allTracks[3]->mixGainPot, "Tr 4");

    drawPot(0, 1, allTracks[4]->mixGainPot, "Tr 5");
    drawPot(1, 1, allTracks[5]->mixGainPot, "Tr 6");
    drawPot(2, 1, allTracks[6]->mixGainPot, "Tr 7");
    drawPot(3, 1, allTracks[7]->mixGainPot, "Tr 8");
  }
}
void set_mixer(byte row)
{
  draw_mixer();
  if (row == 0)
  {

    set_mixer_gain(0, 0, "Tr 1", 0, 1);
    set_mixer_gain(1, 0, "Tr 2", 0, 1);
    set_mixer_gain(2, 0, "Tr 3", 0, 1);
    set_mixer_gain(3, 0, "Tr 4", 0, 1);
  }

  if (row == 1)
  {
    set_mixer_gain(0, 1, "Tr 5", 0, 1);
    set_mixer_gain(1, 1, "Tr 6", 0, 1);
    set_mixer_gain(2, 1, "Tr 7", 0, 1);
    set_mixer_gain(3, 1, "Tr 8", 0, 1);
  }

  if (row == 2)
  {
  }

  if (row == 3)
  {
  }
}
void set_mixer_gain(byte XPos, byte YPos, const char *name, int min, int max)
{
  if (enc_moved[XPos])
  {
    int n = XPos + (YPos * NUM_ENCODERS);
    allTracks[n]->mixGainPot = constrain(allTracks[n]->mixGainPot + encoded[XPos], 0, MIDI_CC_RANGE);
    allTracks[n]->mixGain = (float)(allTracks[n]->mixGainPot / MIDI_CC_RANGE_FLOAT);
    if (allTracks[n]->MIDI_channel_out == 17)
      MasterOut.plugin_1.MixGain.gain(allTracks[n]->mixGain);
    if (allTracks[n]->MIDI_channel_out == 18)
      MasterOut.plugin_2.MixGain.gain(allTracks[n]->mixGain);
    if (allTracks[n]->MIDI_channel_out == 19)
      MasterOut.plugin_3.MixGain.gain(allTracks[n]->mixGain);
    if (allTracks[n]->MIDI_channel_out == 20)
      MasterOut.plugin_4.MixGain.gain(allTracks[n]->mixGain);
    if (allTracks[n]->MIDI_channel_out == 21)
      MasterOut.plugin_5.MixGain.gain(allTracks[n]->mixGain);

    drawPot(XPos, YPos, allTracks[n]->mixGainPot, name);
  }
}