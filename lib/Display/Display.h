#ifndef Display_h
#define Display_h

#include "../Util.h"
#include "TFT_eSPI.h"
#include "DataCollection.h"

class Display
{
private:
   static void dPrint(String text, int x, int y, int size, int color, int datum = 0, int backgroundColor = -1, String oldText = "", int oldTextSize = -1, boolean redraw = false, int padding = 0);
   static void dPrint(int text, int x, int y, int size, int color, int datum = 0, int backgroundColor = -1, int oldText = -1, int oldTextSize = -1, boolean redraw = false, int padding = 0);
public:
   static void setup();
   static void loop();
};

#endif