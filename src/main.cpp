/*
The MIT License (MIT)

Copyright (c) 2020 riraotech.com

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
#if defined(ESP32)
#include <WiFi.h>
#include <SPIFFS.h>
#include <DNSServer.h>
#define MONITOR_SPEED 115200
#define AP_NAME "ESP32-G-AP"
#define EEPROM_SIZE 512
#define _FS SPIFFS
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncDNSServer.h>
#define MONITOR_SPEED 74880
#define AP_NAME "ESP8266-G-AP"
#define EEPROM_SIZE 512
#define _FS LittleFS
#endif

#define HOSTNAME "esp"

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <TelnetSpy.h>
#include <Servo.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

AsyncWebServer server(80);
#if defined(ESP32)
DNSServer dns;
#elif defined(ESP8266)
AsyncDNSServer dns;
#endif
AsyncWiFiManager wifiManager(&server, &dns);
AsyncEventSource events("/events");

static const int RELAY_NUM = 15;
static const int SERVO_NUM = 16;
static const int RED_PIN = 13;
static const int GREEN_PIN = 12;
static const int BLUE_PIN = 14;

Servo myservo;
Ticker countDown;
Ticker mode;
TelnetSpy SerialAndTelnet;

const size_t capacity = JSON_OBJECT_SIZE(3) + 50;
DynamicJsonDocument doc(capacity);

#define Serial SerialAndTelnet

//Event ID
#define MSG_NOTHING 0x00
#define MSG_START_TIMER 0x01
#define MSG_RESET_TIMER 0x02
#define MSG_COUNT_TIMER 0x03
#define MSG_SERVO_ON 0x04
#define MSG_IGNAITER_ON 0x05
#define MSG_CHANGE_AP_MODE 0x06
#define MSG_CHANGE_STA_MODE 0x07
#define MSG_SET_TIME 0x08
#define MSG_ENTER_SETTING 0x09
#define MSG_ENTER_MAIN 0x0A
#define MSG_BLYNK_LED 0x0B
#define MSG_RESET_ESP 0xFF

int gMsgEventID = MSG_NOTHING;

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

void telnetConnected()
{
    Serial.println("Telnet connection established.");
}

void telnetDisconnected()
{
    Serial.println("Telnet connection closed.");
}

void sendCountDownMsg(int state)
{
    SendMessage(MSG_COUNT_TIMER);
}

void blynkModeLed(int state)
{
    SendMessage(MSG_BLYNK_LED);
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

    String jsonBody;

    for (size_t i = 0; i < len; i++)
    {
        jsonBody += (char)data[i];
    }

    //jsonBody to doc
    deserializeJson(doc, jsonBody);

    String mode = String((const char *)doc["MODE"]);
    int SPERK_TIME = doc["SPERK_TIME"];
    int RELEASE_TIME = doc["RELEASE_TIME"];

    Serial.printf("MODE = %s ", mode.c_str());
    Serial.printf("SPERK_TIME = %d ", SPERK_TIME);
    Serial.printf("RELEASE_TIME = %d \n", RELEASE_TIME);

    if (mode == "SUBMIT")
    {
        SendMessage(MSG_SET_TIME);
    }
    else if (mode == "STA_MODE")
    {
        SendMessage(MSG_CHANGE_STA_MODE);
    }
    else if (mode == "AP_MODE")
    {
        SendMessage(MSG_CHANGE_AP_MODE);
    }
}

void initLeds()
{
    Serial.println("Initializing RGB Led...");
}

//for anti-sperking-noise
void closeLedPort()
{
    pinMode(RED_PIN, OUTPUT_OPEN_DRAIN);
    pinMode(GREEN_PIN, OUTPUT_OPEN_DRAIN);
    pinMode(BLUE_PIN, OUTPUT_OPEN_DRAIN);
}
//to normal
void openLedPort()
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
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

void initTelnet()
{
    Serial.println("Initializing Telnet...");

    Serial.begin(MONITOR_SPEED);
    delay(500); // Wait for serial port
    Serial.setDebugOutput(false);
    delay(1000);

    SerialAndTelnet.setWelcomeMsg((char *)"Welcome to ESP Terminal.\n");
    SerialAndTelnet.setCallbackOnConnect(telnetConnected);
    SerialAndTelnet.setCallbackOnDisconnect(telnetDisconnected);
    Serial.println("log ================================================");
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

void configModeCallback(AsyncWiFiManager *myWiFiManager)
{
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
}

void saveConfigCallback()
{
    Serial.println("Should save config");
}

void initWiFi()
{
    EepromStream settings(0, EEPROM_SIZE / 2);

    wifiManager.setDebugOutput(true);
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    String mode = String((const char *)doc["MODE"]);
    Serial.printf("MODE = %s\n", mode.c_str());

    if (mode == "AP_MODE")
    {   
        wifiManager.setConfigPortalTimeout(1);
        wifiManager.setTryConnectDuringConfigPortal(false);
        wifiManager.startConfigPortal(AP_NAME);
    }
    else
    {
        wifiManager.setConfigPortalTimeout(120);     
        if (!wifiManager.autoConnect(AP_NAME))
        {
            doc["MODE"] = "AP_MODE";
            serializeJson(doc, settings);
            settings.flush(); //write to eeprom
            ESP.restart();
        }
    }

    Serial.println("WiFi Started");
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

void initOta()
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

void initFS()
{
    // Initialize FS
    Serial.println("Mounting FS...");
    if (!_FS.begin())
    {
        Serial.println("An Error has occurred while mounting FS");
        return;
    }
}

void setup()
{
    //initLeds();
    initTelnet();
    initEEPROM();
    initWiFi();
    initFS();
    initPort();
    initServer();
    initOta();
}

//Event Message Loop
void loop()
{
    char p[32] = {0};
    static int nCount = 10;

    SerialAndTelnet.handle();
    ArduinoOTA.handle();

    switch (gMsgEventID)
    {
    case MSG_BLYNK_LED:
        SendMessage(MSG_NOTHING);
        break;
    case MSG_ENTER_MAIN:
        mode.detach();
        SendMessage(MSG_NOTHING);
        break;
    case MSG_ENTER_SETTING:
        mode.attach_ms(1000, blynkModeLed, 0);
        SendMessage(MSG_NOTHING);
        break;
    case MSG_CHANGE_AP_MODE:
    case MSG_CHANGE_STA_MODE:
    {
        Serial.print("Change Mode to ");
        Serial.println((const char *)doc["MODE"]);

        EepromStream settings(0, EEPROM_SIZE / 2);
        //doc to settings(eeprom)
        serializeJson(doc, settings);
        settings.flush(); //write to eeprom

        Serial.println("Reset...");
        SendMessage(MSG_RESET_ESP);
    }
    break;
    case MSG_SET_TIME:
    {
        Serial.println("Set times.");

        EepromStream settings(0, EEPROM_SIZE / 2);
        //doc to settings(eeprom)
        serializeJson(doc, settings);
        settings.flush(); //write to eeprom

        SendMessage(MSG_NOTHING);
    }
    break;
    case MSG_START_TIMER:
        countDown.attach_ms(1000, sendCountDownMsg, 0);

        SendMessage(MSG_NOTHING);
        break;
    case MSG_COUNT_TIMER:

        nCount--;

        Serial.printf("Timer = %d\r\n", nCount);

        sprintf(p, "%d", nCount);
        events.send(p, "count");

        if (nCount == 0)
        {
            countDown.detach();
            SendMessage(MSG_IGNAITER_ON);
        }
        else
        {
            SendMessage(MSG_NOTHING);
        }
        break;
    case MSG_SERVO_ON:
    {
        int release_time = doc["RELEASE_TIME"];
        Serial.printf("RELEASE_TIME = %d[ms]\n", release_time);

        delay(release_time); // wait from MSG_IGNAITER_ON

        //Serial.println("Servo ON!");
        myservo.write(150);
        delay(1000);
        myservo.write(90);

        SendMessage(MSG_RESET_TIMER);
    }
    break;
    case MSG_IGNAITER_ON:
    {
        int sperk_time = doc["SPERK_TIME"];
        Serial.printf("SPERK_TIME = %d[ms]\n", sperk_time);

        pinMode(RELAY_NUM, OUTPUT);

        digitalWrite(RELAY_NUM, HIGH);
        Serial.println("Ignaiter ON!");
        delay(sperk_time);
        digitalWrite(RELAY_NUM, LOW);
        Serial.println("Ignaiter OFF!");

        pinMode(RELAY_NUM, INPUT_PULLUP);

        SendMessage(MSG_SERVO_ON);
    }
    break;
    case MSG_RESET_TIMER:
        Serial.println("Reset");
        countDown.detach();

        nCount = 10;

        sprintf(p, "%d", nCount);
        events.send(p, "count");

        SendMessage(MSG_NOTHING);
        break;
    case MSG_RESET_ESP:
        delay(1000);
        ESP.restart();
        break;
    default:
        //MSG_NOTHING
        break;
    }
}
