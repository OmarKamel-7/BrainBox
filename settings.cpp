#include "settings.h"
#include "ui.h"
#include "wifi_manager.h"

static unsigned long lastActivityTime = 0;
static const unsigned long SCREEN_TIMEOUT = 30000;
static bool screenSleeping = false;

static uint8_t touchIRQPin;
static uint8_t tftBacklightPin;



void sleepScreen(Adafruit_ILI9341 &tft) {
    if (screenSleeping) return;

    screenSleeping = true;

    digitalWrite(tftBacklightPin, LOW);
    tft.fillScreen(ILI9341_BLACK);
}

void wakeScreen(Adafruit_ILI9341 &tft, ScreenState currentScreen) {
    if (!screenSleeping) return;

    screenSleeping = false;

    digitalWrite(tftBacklightPin, HIGH);
    tft.fillScreen(ILI9341_BLACK);

    switch (currentScreen) {
        case MAIN_SCREEN:
            drawMain1_no_wifi(tft);
            break;

        case STUDENT_SCREEN:
            StudentScreen(tft);
            break;

        case MAIN2_SCREEN:
            drawMain2_no_wifi(tft);
            break;

        case WIFI_SCREEN:
            drawWiFiStatus(tft);
            break;

        case LOADING:
            drawLoading(tft);
            break;
    }
}

void initPowerManager(uint8_t touchIrqPin, uint8_t backlightPin) {
    touchIRQPin = touchIrqPin;
    tftBacklightPin = backlightPin;

    pinMode(touchIRQPin, INPUT);
    pinMode(tftBacklightPin, OUTPUT);

    digitalWrite(tftBacklightPin, HIGH);

    lastActivityTime = millis();
}

void updateScreenTimeout(
    Adafruit_ILI9341 &tft,
    ScreenState currentScreen
) {
    if (!screenSleeping &&
        millis() - lastActivityTime > SCREEN_TIMEOUT) {
        sleepScreen(tft);
    }
}

void registerActivity() {
    lastActivityTime = millis();
}

bool handleWakeOnTouchIRQ(
    Adafruit_ILI9341 &tft,
    ScreenState currentScreen
) {
    if (screenSleeping) {
        if (digitalRead(touchIRQPin) == LOW) {
            wakeScreen(tft, currentScreen);
            registerActivity();
            delay(200);
        }
        return true;
    }

    return false;
}