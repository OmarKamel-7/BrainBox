#include <Arduino.h>
#include "Display.h"

Display display(10, 9, 8);

void setup() {
    Serial.begin(115200);

    display.begin();
    display.testColors();
    display.showHello();
}

void loop() {
}