#include "Display.h"

Display::Display(int csPin, int dcPin, int rstPin) {
    cs = csPin;
    dc = dcPin;
    rst = rstPin;

    tft = new Adafruit_ILI9341(cs, dc, rst);
}

void Display::begin() {
    SPI.begin(12, -1, 11, cs);
    tft->begin();
    tft->setRotation(3);
    tft->fillScreen(ILI9341_BLACK);
}

void Display::showHello() {
    tft->setTextColor(ILI9341_GREEN);
    tft->setTextSize(3);
    tft->setCursor(40, 100);
    tft->println("Hello Omar");
}

void Display::testColors() {
    tft->fillScreen(ILI9341_RED);
    delay(500);

    tft->fillScreen(ILI9341_GREEN);
    delay(500);

    tft->fillScreen(ILI9341_BLUE);
    delay(500);

    tft->fillScreen(ILI9341_BLACK);
}