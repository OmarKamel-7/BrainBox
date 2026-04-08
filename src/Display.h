#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

class Display {
private:
    int cs;
    int dc;
    int rst;

    Adafruit_ILI9341* tft;

public:
    Display(int csPin, int dcPin, int rstPin);

    void begin();
    void showHello();
    void testColors();
};

#endif