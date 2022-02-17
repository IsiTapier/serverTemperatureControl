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
  display.setTextDatum(CC_DATUM);
  //temperature
  int _temp = DataCollection::getTempBme();
  display.setTextFont(7);
  display.setCursor(TFT_HEIGHT/2-23, 32);
  display.setTextColor(TFT_BLACK);
  display.drawString(String(DataCollection::getOldTempBme()));
  display.setTextColor(_temp>MAX_TEMPERATURE?TFT_RED:TFT_WHITE);
  display.drawString(String(_temp));
  display.setFreeFont(&FreeSansBold24pt7b);
  display.drawString("             c");
  display.fillCircle(122, 22, 7, _temp>MAX_TEMPERATURE?TFT_RED:TFT_WHITE);
  display.fillCircle(122, 22, 3, TFT_BLACK);
  //humidity
  display.setTextFont(7);
  display.setTextColor(TFT_BLACK);
  display.setCursor(TFT_HEIGHT/2-23, 94);
  display.drawString(String(DataCollection::getOldHumidityBme()));
  display.setTextColor(TFT_WHITE);
  display.drawString(String(DataCollection::getHumidityBme()));
  display.setFreeFont(&FreeSansBold24pt7b);
  display.drawString("            %");
}


void Display::dPrint(String text, int x, int y, int size, int color, int datum, int backgroundColor, String oldText, int oldTextSize, boolean redraw, int padding) {
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
void Display::dPrint(int text, int x, int y, int size, int color, int datum, int backgroundColor, int oldText, int oldTextSize, boolean redraw, int padding) {
  dPrint(String(text), x, y, size, color, datum, backgroundColor, (oldText == -1) ? "" : String(oldText), oldTextSize, redraw, padding);
}