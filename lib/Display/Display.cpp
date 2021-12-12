#include "Display.h"

#define LETTER_LENGTH              6
#define LETTER_HEIGHT              8

void Display::setup() {
  display.init(INITR_GREENTAB128);
  display.setRotation(3);
  display.writecommand(ST7735_DISPON);
  display.fillScreen(TFT_BLACK);

}


void Display::loop() {
  display.setTextSize(1);
  //temperature
  int _temp = DataCollection::getTempBme();

  display.setTextFont(7);
  display.setCursor(TFT_HEIGHT/2-23, 32);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(CC_DATUM);
  display.drawString(String(DataCollection::getOldTempBme(), 1));
  display.setTextColor(_temp>MAX_TEMPERATURE?TFT_RED:TFT_WHITE);
  display.setCursor(TFT_HEIGHT/2-23, 32);
  display.drawString(String(_temp, 1));
  display.setFreeFont(&FreeSansBold24pt7b);
  display.drawString("             c");
  display.fillCircle(122, 22, 7, _temp>MAX_TEMPERATURE?TFT_RED:TFT_WHITE);
  display.fillCircle(122, 22, 3, TFT_BLACK);
  //humidity
  display.setTextFont(7);
  display.setTextColor(TFT_BLACK);
  display.setCursor(TFT_HEIGHT/2-23, 94);
  display.drawString(String(DataCollection::getOldHumidityBme(), 1));
  display.setTextColor(TFT_WHITE);
  display.drawString(String(DataCollection::getHumidityBme(), 1));
  display.setFreeFont(&FreeSansBold24pt7b);
  display.drawString("            %");
}


void dPrint(String text, int x, int y, int size, int color, int datum = 0, int backgroundColor = -1, String oldText = "", int oldTextSize = -1, boolean redraw = false, int padding = 0) {
  display.setTextPadding(padding);
  display.setTextDatum(datum);
  if(backgroundColor == 0)
    backgroundColor = TFT_BLACK;
  if(oldText.length() != 0) {
    if(oldTextSize < 0)
      oldTextSize = size;
    int xold = x;
    if(datum % 3 == 1) {
      x -= size*LETTER_LENGTH*text.length()/2;
      xold -= oldTextSize*LETTER_LENGTH*oldText.length()/2;
      display.setTextDatum(datum-1);
    } else if(datum % 3 == 2) {
      x -= LETTER_LENGTH*size*text.length();
      xold -= LETTER_LENGTH*oldTextSize*oldText.length();
      display.setTextDatum(datum-2);
    }
    display.setTextColor(backgroundColor);
    display.setTextSize(oldTextSize);
    if(redraw) {
      display.drawString(oldText, x, y);
      display.setTextColor(color);
      display.setTextSize(size);
      display.drawString(text, x, y);
    } else {
      for(int i = 0; i < oldText.length(); i++) {
        if(text.charAt(i) != oldText.charAt(i) || text.length() != oldText.length()) {
          display.drawString((String) oldText.charAt(i), (xold+LETTER_LENGTH*oldTextSize*i), y);
        }
      }
      display.setTextColor(color);
      display.setTextSize(size);
      for(int i = 0; i < text.length(); i++) {
        if(text.charAt(i) != oldText.charAt(i) || text.length() != oldText.length()) {
          display.drawString((String) text.charAt(i), x+LETTER_LENGTH*size*i, y);
        }
      }
    }
  } else {
    display.setTextSize(size);
    display.setCursor(x, y);
    if(backgroundColor > -1)
      display.setTextColor(color, backgroundColor);
    else
      display.setTextColor(color);
    display.drawString(text);
  }
}

//Verkürzung: Writing mit Integern
void dPrint(int text, int x, int y, int size, int color, int datum = 0, int backgroundColor = -1, int oldText = -1, int oldTextSize = -1, boolean redraw = false, int padding = 0) {
  dPrint(String(text), x, y, size, color, datum, backgroundColor, (oldText == -1) ? "" : String(oldText), oldTextSize, redraw, padding);
}