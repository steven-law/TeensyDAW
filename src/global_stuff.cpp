#include <Arduino.h>
#include <ILI9341_t3n.h>
#include <ili9341_t3n_font_Arial.h> // from ILI9341_t3
#include "global_stuff.h"
extern ILI9341_t3n tft;
// individual trackcolors
int trackColor[9]{6150246, 8256638, 1095334, 12643941, 2583100, 9365295, 12943157, 5678954, ILI9341_WHITE};
int encoder_colour[NUM_ENCODERS] = {ILI9341_BLUE, ILI9341_RED, ILI9341_GREEN, ILI9341_WHITE};
int tuning = 440;
float *note_frequency;

// Encoder Pins
bool enc_moved[4]{0, 0, 0, 0};
int encoded[4];


int pixelTouchX = 0;
int gridTouchY = 0;
byte lastPotRow = 0;
byte encoder_function = 0;
bool change_plugin_row;



byte active_track = ACTIVE_TRACK_1;

byte arrangerpage;

void drawPot(int XPos, byte YPos, int dvalue, const char *dname)
{
  enc_moved[XPos] = false;
  // xposition, yposition, value 1-100, value to draw, name to draw, color
  // drawPot Variables
  static float circlePos[4];
  static float circlePos_old[4];
  static int dvalue_old[4];
  static const char *dname_old[4];
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
  tft.setCursor(STEP_FRAME_W * xPos, STEP_FRAME_H * (yPos + 1) + 3);
  //if (dname_old[XPos] != dname)
    tft.print(dname_old[XPos]);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(STEP_FRAME_W * xPos + 4, STEP_FRAME_H * yPos - 3);
  tft.print(dvalue);
  tft.setCursor(STEP_FRAME_W * xPos, STEP_FRAME_H * (yPos + 1) + 3);
  //if (dname_old[XPos] != dname)
    tft.print(dname);

  tft.fillCircle(STEP_FRAME_W * (xPos + 1) + 16 * cos((2.5 * circlePos_old[XPos]) + 2.25), STEP_FRAME_H * yPos + 16 * sin((2.5 * circlePos_old[XPos]) + 2.25), 4, ILI9341_DARKGREY);
  tft.drawCircle(STEP_FRAME_W * (xPos + 1), STEP_FRAME_H * yPos, 16, ILI9341_LIGHTGREY);
  tft.fillCircle(STEP_FRAME_W * (xPos + 1) + 16 * cos((2.5 * circlePos[XPos]) + 2.25), STEP_FRAME_H * yPos + 16 * sin((2.5 * circlePos[XPos]) + 2.25), 4, color);
  circlePos_old[XPos] = circlePos[XPos];
  dvalue_old[XPos] = dvalue;
  dname_old[XPos] = dname;
}

byte getEncodervalue(byte XPos, byte YPos, const char *name, byte oldValue)
{
  byte encByte = constrain(oldValue + encoded[XPos], 0, MIDI_CC_RANGE);
  drawPot(XPos, YPos, encByte, name);
  return encByte;
}

void draw_sequencer_option(byte x, const char *nameshort, int value, byte enc, const char *pluginName)
{

  int color;
  byte y = 6 + (enc * 2);
  if (encoder_function == INPUT_FUNCTIONS_FOR_ARRANGER)
    color = y - 1;
  else
    color = active_track;

  // show function
  tft.setCursor(STEP_FRAME_W * x + 2, STEP_FRAME_H * (y - 1) + 6);
  tft.setFont(Arial_8);
  tft.setTextColor(trackColor[color]);
  tft.setTextSize(1);
  tft.print(nameshort);
  // show value
  tft.drawRect(STEP_FRAME_W * x, STEP_FRAME_H * y, STEP_FRAME_W * 2, STEP_FRAME_H, encoder_colour[enc]);
  tft.fillRect(STEP_FRAME_W * x + 1, STEP_FRAME_H * y + 1, STEP_FRAME_W * 2 - 2, STEP_FRAME_H - 2, ILI9341_DARKGREY);
  tft.setCursor(STEP_FRAME_W * x + 8, STEP_FRAME_H * y + 3);
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(1);
  if (pluginName != 0)
  {
    tft.setCursor(STEP_FRAME_W * x + 2, STEP_FRAME_H * y + 4);
    tft.setFont(Arial_8);
    tft.print(pluginName);
  }
  else
    tft.print(value);
}
