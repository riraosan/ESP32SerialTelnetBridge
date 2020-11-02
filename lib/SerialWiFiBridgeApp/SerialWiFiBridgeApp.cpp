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
#include <HardwareSerial.h>
#include <esp32-hal-log.h>
#include "SerialWiFiBridgeApp.h"

/*
void SendMessage(int nMessageID)
{
    gMsgEventID = nMessageID;
}
*/
/*
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
*/
/*
String processor(const String &var)
{
    Serial.println("processor()");

    return "";
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
*/

MESSAGE_ID SerialWiFiBridgeClass::_message_id = MSG_NOTHING;

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
    if (!(String(AP_PASSWORD).isEmpty() ? _wifiManager->autoConnect(AP_NAME) : _wifiManager->autoConnect(AP_NAME, AP_PASSWORD)))
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
    Serial.println("Initializing Telnet...");

    Serial.setDebugOutput(false);
    Serial1.setDebugOutput(false);
    Serial2.setDebugOutput(false);

    _telnet0->setWelcomeMsg((char *)"\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. (COM0<->8880)\n");
    _telnet0->setCallbackOnConnect(SerialWiFiBridgeClass::telnetConnected);
    _telnet0->setCallbackOnDisconnect(SerialWiFiBridgeClass::telnetDisconnected);
    _telnet0->setPort(SERIAL0_TCP_PORT);
    _telnet0->setSerial(&Serial);
    _telnet0->begin(UART_BAUD0, SERIAL_PARAM0, SERIAL0_RXPIN, SERIAL0_TXPIN);

    _telnet1->setWelcomeMsg((char *)"\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. (COM0<->8881)\n");
    _telnet1->setCallbackOnConnect(SerialWiFiBridgeClass::telnetConnected);
    _telnet1->setCallbackOnDisconnect(SerialWiFiBridgeClass::telnetDisconnected);
    _telnet1->setPort(SERIAL1_TCP_PORT);
    _telnet1->setSerial(&Serial1);
    _telnet1->begin(UART_BAUD1, SERIAL_PARAM1, SERIAL1_RXPIN, SERIAL1_TXPIN);

    _telnet2->setWelcomeMsg((char *)"\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. (COM1<->8882)\n");
    _telnet2->setCallbackOnConnect(SerialWiFiBridgeClass::telnetConnected);
    _telnet2->setCallbackOnDisconnect(SerialWiFiBridgeClass::telnetDisconnected);
    _telnet2->setPort(SERIAL2_TCP_PORT);
    _telnet2->setSerial(&Serial2);
    _telnet2->begin(UART_BAUD2, SERIAL_PARAM2, SERIAL2_RXPIN, SERIAL2_TXPIN);
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

void SerialWiFiBridgeClass::onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    Serial.println("onBody()");
}

void SerialWiFiBridgeClass::_initServer()
{
    _server->on(
        "/", HTTP_POST, [](AsyncWebServerRequest *request) {
            Serial.println("[HTTP_POST] /");
            request->send(200);
        },
        NULL, onBody);

    //REST API exsample
    _server->on("/A", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("/A");
        //SendMessage(MSG_START_TIMER);
    });

    _server->on("/B", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("/B");
        //SendMessage(MSG_RESET_TIMER);
    });
}

void SerialWiFiBridgeClass::printClock()
{
    SerialWiFiBridgeClass::_message_id = MSG_COMMAND_CLOCK;
}

void SerialWiFiBridgeClass::_initClock()
{
    //Get NTP Time
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

    clocker.attach(1, SerialWiFiBridgeClass::printClock);
}

void SerialWiFiBridgeClass::setup()
{
    Serial.println("[SWB]start setup()");
 
    _initLEDS();
    _initTelnet();
    _initEEPROM();
    _initServer();
    _initWiFi();
    _initFS();
    _initOTA();
    _initClock();
    Serial.println("[SWB]end setup()");
}

void SerialWiFiBridgeClass::_printClock()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    _telnet0->printf("[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet1->printf("[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet2->printf("[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);

    log_i("HH:MM:SS = %02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void SerialWiFiBridgeClass::message_handle(MESSAGE_ID msg_id)
{
    switch(msg_id)
    {
        case MSG_COMMAND_CLOCK:
            _printClock();
        break;
        case MSG_COMMAND_RESET:
        break;
        default:
         ;
    }

    SerialWiFiBridgeClass::_message_id = MSG_NOTHING;
}

void SerialWiFiBridgeClass::handle()
{
    _telnet0->handle();
    _telnet1->handle();
    _telnet2->handle();
    ArduinoOTA.handle();

    message_handle(SerialWiFiBridgeClass::_message_id);    
}
