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

MESSAGE_ID SerialWiFiBridgeClass::_message_id = MSG_NOTHING;

HardwareSerial _Serial(0);
HardwareSerial _Serial1(1);
HardwareSerial _Serial2(2);

/*
    log_e("")
    log_w("")
    log_i("")
    log_d("")
    log_v("")
*/

void SerialWiFiBridgeClass::initPort()
{
    _Serial.println("Initializing Ports...");
    //TODO:Initializing Ports
}

void SerialWiFiBridgeClass::initEEPROM()
{
    _Serial.println("Initializing EEPROM...");
    //TODO:Initializing EEPROM
}

void SerialWiFiBridgeClass::initFS()
{
    _Serial.println("Mounting SPIFFS...");
    if (!SPIFFS.begin())
    {
        _Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
}

void SerialWiFiBridgeClass::initWiFi()
{
    _wifiManager->setDebugOutput(false);
    _wifiManager->setConfigPortalTimeout(PORTAL_TIMEOUT);
    _wifiManager->setAPCallback([](AsyncWiFiManager *myWiFiManager) {
        _Serial.println("- No known wifi found");
        _Serial.print("- Starting AP: ");
        _Serial.println(myWiFiManager->getConfigPortalSSID());
        _Serial.println(WiFi.softAPIP());
    });

    // enable autoconnect
    if (!(String(AP_PASSWORD).isEmpty() ? _wifiManager->autoConnect(AP_NAME) : _wifiManager->autoConnect(AP_NAME, AP_PASSWORD)))
    {
        _Serial.println("- Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    }
    else
    {
        _Serial.println("- WiFi: " + WiFi.SSID());
        _Serial.println("-  Mac: " + WiFi.macAddress());
        _Serial.println("-   IP: " + WiFi.localIP().toString());
    }

    _Serial.println("- WiFi Started");
}

void SerialWiFiBridgeClass::_telnetConnected()
{
    _Serial.println("- Telnet connection established.");
}

void SerialWiFiBridgeClass::_telnetDisconnected()
{
    _Serial.println("- Telnet connection closed.");
}

void SerialWiFiBridgeClass::initTelnet()
{
    _Serial.println("- Initializing Telnet...");

    _Serial.setDebugOutput(false);
    _Serial1.setDebugOutput(false);
    _Serial2.setDebugOutput(false);

    _telnet0->setWelcomeMsg((char *)"\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial0 <-> 8880)\n");
    _telnet0->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet0->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet0->setPort(SERIAL0_TCP_PORT);
    _telnet0->setSerial(&_Serial);
    _telnet0->begin(UART_BAUD0, SERIAL_8N1, SERIAL0_RXPIN, SERIAL0_TXPIN);

    _telnet1->setWelcomeMsg((char *)"\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial1 <-> 8881)\n");
    _telnet1->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet1->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet1->setPort(SERIAL1_TCP_PORT);
    _telnet1->setSerial(&_Serial1);
    _telnet1->begin(UART_BAUD1, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);

    _telnet2->setWelcomeMsg((char *)"\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. (Serial2<->8882)\n");
    _telnet2->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet2->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet2->setPort(SERIAL2_TCP_PORT);
    _telnet2->setSerial(&_Serial2);
    _telnet2->begin(UART_BAUD2, SERIAL_8N1, SERIAL2_RXPIN, SERIAL2_TXPIN);

    delay(1000);

    _Serial.println("Serial0 Txd is on pin: " + String(SERIAL0_TXPIN));
    _Serial.println("Serial0 Rxd is on pin: " + String(SERIAL0_RXPIN));

    _Serial.println("Serial1 Txd is on pin: " + String(SERIAL1_TXPIN));
    _Serial.println("Serial1 Rxd is on pin: " + String(SERIAL1_RXPIN));

    _Serial.println("Serial2 Txd is on pin: " + String(SERIAL2_TXPIN));
    _Serial.println("Serial2 Rxd is on pin: " + String(SERIAL2_RXPIN));
}

void SerialWiFiBridgeClass::initConsole()
{
    //_console0.begin(256, 8, atoi(ARDUHAL_LOG_COLOR_CYAN), 0);
    //_console.setConsoleConfig(256, 8, atoi(LOG_COLOR_CYAN), 0);
    //_console.setHistoryLength(10);
    //_console.setPromptString("[SWB]> ");

    //_console.begin();

    // add a new function "pin" to bitlash
    //_console.addFunction("pin", (bitlash_function)pin_func);
    //_console.consoleTaskStart(); // will start a task waiting for input and execute
}

void SerialWiFiBridgeClass::initOTA()
{
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else
            type = "filesystem";

        _Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        _Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        _Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        _Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            _Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            _Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            _Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            _Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            _Serial.println("End Failed");
    });

    ArduinoOTA.setHostname(HOSTNAME);

    _Serial.print("- Hostname: ");
    _Serial.println(ArduinoOTA.getHostname() + ".local");

    ArduinoOTA.begin();
    _Serial.println("- OTA Started");
}

void SerialWiFiBridgeClass::onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    _Serial.println("onBody()");
}

void SerialWiFiBridgeClass::initServer()
{
    _server->on(
        "/", HTTP_POST, [](AsyncWebServerRequest *request) {
            _Serial.println("[HTTP_POST] /");
            request->send(200);
        },
        NULL, onBody);

    //REST API exsample
    _server->on("/A", HTTP_GET, [](AsyncWebServerRequest *request) {
        _Serial.println("/A");
    });

    _server->on("/B", HTTP_GET, [](AsyncWebServerRequest *request) {
        _Serial.println("/B");
    });
}

void SerialWiFiBridgeClass::sendClockMessage()
{
    SerialWiFiBridgeClass::_message_id = MSG_COMMAND_CLOCK;
}

void SerialWiFiBridgeClass::initClock()
{
    //Get NTP time
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
    delay(2000);

    _clocker.attach(1, SerialWiFiBridgeClass::sendClockMessage);
}

void SerialWiFiBridgeClass::setup()
{
    initPort();
    initTelnet();
    initConsole();
    initEEPROM();
    initServer();
    initFS();
    initWiFi();
    initOTA();
    //initClock();
    _Serial.println("setup() end");
}

#define _SERIAL
#define _TELNET

//for test
void SerialWiFiBridgeClass::printClock()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

#ifndef _TELNET
    _telnet0->printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet1->printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet2->printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif

#ifndef _SERIAL
    _Serial.printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _Serial1.printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _Serial2.printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);
    log_i("HH:MM:SS = %02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif
}

void SerialWiFiBridgeClass::messageHandle(MESSAGE_ID msg_id)
{
    switch (msg_id)
    {
    case MSG_COMMAND_CLOCK:
        printClock();
        break;
    case MSG_COMMAND_RESET:
        ESP.restart();
        delay(2000);
        break;
    default:;
    }

    SerialWiFiBridgeClass::_message_id = MSG_NOTHING;
}

void SerialWiFiBridgeClass::_serialHandle(TelnetSpy *telnet, HardwareSerial *serial)
{
    // read from serial, send to telnet
    if (serial->available())
    {
        int byte = serial->read();
        telnet->write(byte);
    }

    // read from telnet, send to serial
    if (telnet->available())
    {
        int byte = telnet->read();
        serial->write(byte);
    }

    telnet->handle();
}

void SerialWiFiBridgeClass::handle()
{
    ArduinoOTA.handle();

    _serialHandle(_telnet0, &_Serial);
    _serialHandle(_telnet1, &_Serial1);
    _serialHandle(_telnet2, &_Serial2);

    messageHandle(SerialWiFiBridgeClass::_message_id);

    yield();
}
