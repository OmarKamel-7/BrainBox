#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <MFRC522.h>
#include <SD.h>
#include "ui.h"
#include "wifi_manager.h"

// ================= TFT =================
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// ================= TOUCH =================
#define TOUCH_CS 18
#define TOUCH_IRQ  17   // choose your actual IRQ pin

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

unsigned long lastActivityTime = 0;
const unsigned long SCREEN_TIMEOUT = 10000; // 30 sec

bool screenSleeping = false;


// ================= RFID =================
#define RC522_SS  4
#define RC522_RST 15
MFRC522 rfid(RC522_SS, RC522_RST);

// ================= SD =================
#define SD_CS 5

// ================= MODE =================
enum DeviceMode {
    NORMAL_MODE,
    ADD_STUDENT_MODE
};

DeviceMode currentMode = NORMAL_MODE;

// ================= SCREEN STATE =================
enum ScreenState {
    MAIN_SCREEN,
    STUDENT_SCREEN,
    MAIN2_SCREEN,
    WIFI_SCREEN,
    LOADING
};

ScreenState currentScreen = MAIN_SCREEN;

// ================= FLAGS =================
bool touchHandled = false;

// ================= BUTTON STRUCT =================
struct Button {
    int x1, y1, x2, y2;
    bool pressed(int x, int y) {
        return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
    }
};

// MAIN BUTTONS
Button btnStudent = {12, 68, 275, 118};
Button btnMain2   = {278, 172, 313, 214};
Button btnClassCondition = {12, 125, 274, 168};
Button uptomain = {280,82,312,121};
Button credits = {178,10 ,222 , 275};
// BACK BUTTONS
Button btnBack = {249, 11, 299, 52};

// WIFI
Button btnDisconnect = {60, 170, 260, 210};

// ================= TOUCH CALIBRATION =================
#define TS_MINX 200
#define TS_MAXX 3800
#define TS_MINY 200
#define TS_MAXY 3800

bool getTouch(int &x, int &y) {
    if (!ts.touched()) return false;

    TS_Point p = ts.getPoint();

    x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
    y = map(p.y, TS_MINY, TS_MAXY, 0, 240);

    x = constrain(x, 0, 319);
    y = constrain(y, 0, 239);

    return true;
}

// ================= SD SAVE =================
void saveStudentToSD(String cmd) {
    cmd.replace("SAVE_STUDENT:", "");

    int sep = cmd.indexOf(':');
    if (sep == -1) {
        Serial.println("SAVE_FAIL");
        return;
    }

    String name = cmd.substring(0, sep);
    String uid = cmd.substring(sep + 1);

    File file = SD.open("/students.csv", FILE_APPEND);

    if (!file) {
        Serial.println("SAVE_FAIL");
        return;
    }

    file.println(name + "," + uid);
    file.close();

    Serial.println("SAVE_OK");
}

// ================= SERIAL COMMANDS =================
void handleSerialCommands() {

    if (!Serial.available()) return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "ADD_MODE") {
        currentMode = ADD_STUDENT_MODE;
        Serial.println("READY");
    }

    else if (cmd == "EXIT_ADD_MODE") {
        currentMode = NORMAL_MODE;
        Serial.println("EXIT_OK");
    }

    else if (cmd.startsWith("SAVE_STUDENT:")) {
        saveStudentToSD(cmd);
    }
}

// ================= RFID HANDLER =================
void handleRFID() {

    if (currentMode != ADD_STUDENT_MODE) return;

    if (!rfid.PICC_IsNewCardPresent()) return;
    if (!rfid.PICC_ReadCardSerial()) return;

    String uid = "";

    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
    }

    uid.toUpperCase();

    Serial.println("UID:" + uid);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

// ================= NAVIGATION =================
void goHome() {
    tft.fillScreen(ILI9341_BLACK);
    drawMain1_no_wifi(tft);
    currentScreen = MAIN_SCREEN;
}

// ================= SETUP =================
void setup() {
    Serial.begin(115200);
    delay(300);

    SPI.begin();

    // SD
    if (SD.begin(SD_CS)) {
        Serial.println("SD_OK");
    } else {
        Serial.println("SD_FAIL");
    }

    // TFT
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(ILI9341_BLACK);

    // TOUCH
    ts.begin();
    ts.setRotation(1);

    // RFID
    rfid.PCD_Init();

    // WIFI
    wifiBegin();

    // UI START
    drawCredits(tft);
    delay(1000);
    drawMain1_no_wifi(tft);
}

// ================= LOOP =================
void loop() {

    wifiLoop();

    handleSerialCommands();
    handleRFID();

    int x, y;

    if (getTouch(x, y) && !touchHandled) {

        touchHandled = true;

        // MAIN SCREEN
        if (currentScreen == MAIN_SCREEN) {

            if (btnStudent.pressed(x, y)) {
                tft.fillScreen(ILI9341_BLACK);
                StudentScreen(tft);
                currentScreen = STUDENT_SCREEN;
            }

            else if (btnMain2.pressed(x, y)) {
                tft.fillScreen(ILI9341_BLACK);
                drawMain2_no_wifi(tft);
                currentScreen = MAIN2_SCREEN;
            }

            else if (btnClassCondition.pressed(x, y)) {
                tft.fillScreen(ILI9341_BLACK);
                drawLoading(tft);
                currentScreen = LOADING;
            }
        }

        // MAIN2
        else if (currentScreen == MAIN2_SCREEN) {

            if (uptomain.pressed(x,y)) {
                drawMain1_no_wifi(tft);
            }

            if (credits.pressed(x,y)) {
                drawCredits(tft);
            }


            if (btnBack.pressed(x, y)) {
                goHome();
            }
        }

        // STUDENT SCREEN
        else if (currentScreen == STUDENT_SCREEN) {

            if (btnBack.pressed(x, y)) {
                goHome();
            }
        }

        // WIFI
        else if (currentScreen == WIFI_SCREEN) {

            if (btnDisconnect.pressed(x, y)) {
                disconnectWiFi();
                drawWiFiStatus(tft);
            }

            if (btnBack.pressed(x, y)) {
                goHome();
            }
        }
    }

    if (!ts.touched()) {
        touchHandled = false;
    }
}