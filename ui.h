#pragma once
#include <Adafruit_ILI9341.h>

void StudentScreen(Adafruit_ILI9341 &tft);
void drawMain1_no_wifi(Adafruit_ILI9341 &tft);
void drawMain2_no_wifi(Adafruit_ILI9341 &tft);
void drawLoading(Adafruit_ILI9341 &tft);
void drawCredits(Adafruit_ILI9341 &tft);
void NewStudent(Adafruit_ILI9341 &tft, String uid);
