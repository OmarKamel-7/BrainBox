#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Adafruit_ILI9341.h>

// Core control
void wifiBegin();
void wifiLoop();

// UI helpers
void drawWiFiStatus(Adafruit_ILI9341 &tft);
void disconnectWiFi();

// optional
bool isWiFiConnected();
String getWiFiIP();



extern Preferences prefs;