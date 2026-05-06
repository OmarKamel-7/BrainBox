#include "wifi_manager.h"

Preferences prefs;
WebServer server(80);

// ================= STATE =================
bool wifiConnected = false;

// ================= START AP =================
void wifiBegin() {
    WiFi.mode(WIFI_AP_STA);

    // Always running hotspot
    WiFi.softAP("BrainBox_Setup");

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    // Load saved WiFi
    prefs.begin("wifi", true);

    String ssid = prefs.getString("ssid", "");
    String pass = prefs.getString("pass", "");

    prefs.end();

    if (ssid != "") {
        WiFi.begin(ssid.c_str(), pass.c_str());
    }

    // ================= WEB SERVER =================

    server.on("/", []() {
        server.send(200, "text/html",
            "<h2>BrainBox WiFi Setup</h2>"
            "<form action='/save'>"
            "SSID:<br><input name='ssid'><br>"
            "PASS:<br><input name='pass' type='password'><br><br>"
            "<button type='submit'>Connect</button>"
            "</form>"
        );
    });

    server.on("/save", []() {

        String ssid = server.arg("ssid");
        String pass = server.arg("pass");

        prefs.begin("wifi", false);
        prefs.putString("ssid", ssid);
        prefs.putString("pass", pass);
        prefs.end();

        WiFi.begin(ssid.c_str(), pass.c_str());

        server.send(200, "text/html", "<h3>Connecting...</h3>");
    });

    server.begin();
}

// ================= LOOP (IMPORTANT) =================
void wifiLoop() {
    server.handleClient();

    wifiConnected = (WiFi.status() == WL_CONNECTED);
}

// ================= STATUS =================
void drawWiFiStatus(Adafruit_ILI9341 &tft) {
    tft.fillScreen(ILI9341_BLACK);

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(20, 20);
    tft.print("WiFi Panel");

    tft.setTextSize(1);
    tft.setCursor(20, 60);
    tft.print("AP: BrainBox_Setup");

    tft.setCursor(20, 80);
    tft.print("Open: 192.168.4.1");

    if (WiFi.status() == WL_CONNECTED) {

        tft.setTextColor(ILI9341_GREEN);
        tft.setCursor(20, 120);
        tft.print("CONNECTED");

        tft.setCursor(20, 140);
        tft.print("SSID: ");
        tft.print(WiFi.SSID());

        tft.setCursor(20, 160);
        tft.print("IP: ");
        tft.print(WiFi.localIP());

    } else {
        tft.setTextColor(ILI9341_RED);
        tft.setCursor(20, 120);
        tft.print("NOT CONNECTED");
    }

    // Disconnect button
    tft.drawRect(60, 180, 200, 40, ILI9341_RED);
    tft.setTextColor(ILI9341_RED);
    tft.setCursor(100, 205);
    tft.print("DISCONNECT");
}

// ================= DISCONNECT =================
void disconnectWiFi() {
    WiFi.disconnect(true, true);

    prefs.begin("wifi", false);
    prefs.clear();
    prefs.end();
}

// ================= HELPERS =================
bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String getWiFiIP() {
    return WiFi.localIP().toString();
}