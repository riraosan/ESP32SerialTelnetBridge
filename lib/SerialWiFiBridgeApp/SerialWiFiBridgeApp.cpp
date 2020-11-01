/*
The MIT License (MIT)

Copyright (c) 2020-2021 riraotech.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <esp32-hal-log.h>
#include "SerialWiFiBridgeApp.h"

/*
void SendMessage(int nMessageID)
{
    gMsgEventID = nMessageID;
}

String processor(const String &var)
{
    Serial.println("processor()");

    if (var == "SPERK_TIME")
    {
        int SPERK_TIME = doc["SPERK_TIME"];
        String value = String(SPERK_TIME);
        return value;
    }
    else if (var == "RELEASE_TIME")
    {
        int RELEASE_TIME = doc["RELEASE_TIME"];
        String value = String(RELEASE_TIME);
        return value;
    }

    return "0";
}

void onRequest(AsyncWebServerRequest *request)
{
    Serial.println("onRequest() Handle Unknown Request");
    //Handle Unknown Request
    request->send(404);
}

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    Serial.println("onUpload() Handle upload");
    //Handle upload
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    Serial.println("onBody()");
}

void initLeds()
{
    Serial.println("Initializing RGB Led...");
}


void initPort()
{
    Serial.println("Initializing Ports...");

    pinMode(SERVO_NUM, OUTPUT);
    pinMode(RELAY_NUM, INPUT_PULLUP);

    myservo.attach(SERVO_NUM);
    myservo.write(90);

    //init open IO for anti-noise
    //pinMode(4, OUTPUT_OPEN_DRAIN);
    //pinMode(16, OUTPUT_OPEN_DRAIN);
    //pinMode(1, OUTPUT_OPEN_DRAIN);
    //pinMode(3, OUTPUT_OPEN_DRAIN);
    //pinMode(RED_PIN, OUTPUT_OPEN_DRAIN);
    //pinMode(GREEN_PIN, OUTPUT_OPEN_DRAIN);
    //pinMode(BLUE_PIN, OUTPUT_OPEN_DRAIN);
}

void initEEPROM()
{
    Serial.println("Initializing EEPROM...");
    EEPROM.begin(EEPROM_SIZE);

    EepromStream settings(0, EEPROM_SIZE / 2);

    //settings(eeprom) to doc
    deserializeJson(doc, settings);

    if (doc.isNull() == true)
    {
        Serial.println("doc is null");

        const char *json = "{\"MODE\":\"AP_MODE\",\"SPERK_TIME\":50,\"RELEASE_TIME\":10}";
        deserializeJson(doc, json);
    }
}

void initServer()
{
    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Route to load style.css file");
        request->send(_FS, "/style.css", "text/css");
    });

    server.on("/jquery-3.5.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Route to load jquery-3.5.1.min.js file");
        request->send(_FS, "/jquery-3.5.1.min.js", "text/javascript");
    });

    // Route to load favicon.ico file
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Route to load favicon.ico file");
        request->send(_FS, "/favicon.ico", "icon");
    });

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("[HTTP_GET] /");
        request->send(_FS, "/index.html", String(), false, processor);
        SendMessage(MSG_ENTER_MAIN);
    });

    server.on(
        "/", HTTP_POST, [](AsyncWebServerRequest *request) {
            Serial.println("[HTTP_POST] /");
            request->send(200);
        },
        NULL, onBody);

    // Route for /settings web page
    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("[HTTP_GET] /settings");
        request->send(_FS, "/settings.html", String(), false, processor);
        SendMessage(MSG_ENTER_SETTING);
    });

    //REST API
    //Start timer
    server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("/start");
        request->send(_FS, "/index.html", String(), false, processor);
        SendMessage(MSG_START_TIMER);
    });

    //Reset timer
    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("/reset");
        request->send(_FS, "/index.html", String(), false, processor);
        SendMessage(MSG_RESET_TIMER);
    });

    events.onConnect([](AsyncEventSourceClient *client) {
        client->send("10", NULL, millis(), 1000);
    });

    server.addHandler(&events);

    // Catch-All Handlers
    // Any request that can not find a Handler that canHandle it
    // ends in the callbacks below.
    server.onNotFound(onRequest);
    server.onFileUpload(onUpload);

    server.begin();
    Serial.println("Server Started");
}
*/

void SerialWiFiBridgeClass::_initFS()
{
    Serial.println("Mounting SPIFFS...");
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
}

void SerialWiFiBridgeClass::_initWiFi()
{
    // wifiManager.resetSettings();  // this will delete all credentials
    _wifiManager->setDebugOutput(false);
    _wifiManager->setConfigPortalTimeout(PORTAL_TIMEOUT);
    _wifiManager->setAPCallback([](AsyncWiFiManager *myWiFiManager) {
        Serial.println("- No known wifi found");
        Serial.print("- Starting AP: ");
        Serial.println(myWiFiManager->getConfigPortalSSID());
        Serial.println(WiFi.softAPIP());
    });

    // enable autoconnect
    if (!(AP_PASSWORD == "" ? _wifiManager->autoConnect(AP_NAME) : _wifiManager->autoConnect(AP_NAME, AP_PASSWORD)))
    {
        Serial.println("- Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
        // known wifi found & connected
    }
    else
    {
        Serial.println("- WiFi: " + WiFi.SSID());
        Serial.println("-  Mac: " + WiFi.macAddress());
        Serial.println("-   IP: " + WiFi.localIP().toString());
    }

    Serial.println("WiFi Started");
}

void SerialWiFiBridgeClass::telnetConnected()
{
    Serial.println("Telnet connection established.");
}

void SerialWiFiBridgeClass::telnetDisconnected()
{
    Serial.println("Telnet connection closed.");
}

void SerialWiFiBridgeClass::_initTelnet()
{
    //Serial.println("Initializing Telnet...");

    //Serial.begin(MONITOR_SPEED);
    //delay(500); // Wait for serial port
    //Serial.setDebugOutput(false);
    //delay(1000);

    _telnet0->setWelcomeMsg((char*)"\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. (COM0<->8880)\n");
    _telnet0->setCallbackOnConnect(telnetConnected);
    _telnet0->setCallbackOnDisconnect(telnetDisconnected);
    _telnet0->setPort(SERIAL0_TCP_PORT);
    _telnet0->setSerial(&Serial1);
    _telnet0->begin(UART_BAUD0, SERIAL_PARAM0, SERIAL0_RXPIN, SERIAL0_TXPIN);

    _telnet1->setWelcomeMsg((char*)"\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. (COM1<->8881)\n");
    _telnet1->setCallbackOnConnect(telnetConnected);
    _telnet1->setCallbackOnDisconnect(telnetDisconnected);
    _telnet1->setPort(SERIAL1_TCP_PORT);
    _telnet1->begin(UART_BAUD1, SERIAL_PARAM1, SERIAL1_RXPIN, SERIAL1_TXPIN);
}

void SerialWiFiBridgeClass::_initOTA()
{
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else
            type = "filesystem";

        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
    });

    ArduinoOTA.setHostname(HOSTNAME);

    Serial.print("Hostname: ");
    Serial.println(ArduinoOTA.getHostname() + ".local");

    ArduinoOTA.begin();
    Serial.println("OTA Started");
}

void SerialWiFiBridgeClass::_initLEDS() {}
void SerialWiFiBridgeClass::_initEEPROM() {}
void SerialWiFiBridgeClass::_initServer() {}

void SerialWiFiBridgeClass::setup()
{
    _initLEDS();
    _initTelnet();
    _initEEPROM();
    _initWiFi();
    _initFS();
    _initServer();
    _initOTA();
}

void SerialWiFiBridgeClass::handle()
{
    _telnet0->handle();
    _telnet1->handle();
    ArduinoOTA.handle();
}
