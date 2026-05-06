#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Adafruit_ILI9341.h>

enum ScreenState {
    MAIN_SCREEN,
    STUDENT_SCREEN,
    MAIN2_SCREEN,
    WIFI_SCREEN,
    LOADING
};

void initPowerManager(uint8_t touchIrqPin, uint8_t backlightPin);

void updateScreenTimeout(
    Adafruit_ILI9341 &tft,
    ScreenState currentScreen
);

void registerActivity();

bool handleWakeOnTouchIRQ(
    Adafruit_ILI9341 &tft,
    ScreenState currentScreen
);

#endif