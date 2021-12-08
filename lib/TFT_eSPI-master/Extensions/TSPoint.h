/*
  TSPoint.h - TSPoint-Library for Airduino.
*/

#ifndef TSPoint_h
#define TSPoint_h

#include "Arduino.h"

class TSPoint {
  public:
    TSPoint() {}
    TSPoint(uint16_t x, uint16_t y) : x(x), y(y) {}

    uint16_t x;
    uint16_t y;
};

#endif
