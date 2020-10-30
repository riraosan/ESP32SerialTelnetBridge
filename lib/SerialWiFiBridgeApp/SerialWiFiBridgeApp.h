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

#ifndef __SerialWiFiBridgeApp_h
#define __SerialWiFiBridgeApp_h

#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <TelnetSpy.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <Ticker.h>
//#include <ESP32_SPIFFS_ShinonomeFNT.h>
//#include <ESP32_SPIFFS_UTF8toSJIS.h>

#define HOSTNAME "esp32"
#define MONITOR_SPEED 115200
#define AP_NAME "ESP32-G-AP"
#define AP_PASSWORD ""
#define PORTAL_TIMEOUT 180
//#define MSG_CONNECTED "        WiFi Started."

// typedef enum
// {
//     OTA_IDLE,
//     OTA_WAITAUTH,
//     OTA_RUNUPDATE
// } ota_state_t;

// typedef enum
// {
//     OTA_AUTH_ERROR,
//     OTA_BEGIN_ERROR,
//     OTA_CONNECT_ERROR,
//     OTA_RECEIVE_ERROR,
//     OTA_END_ERROR
// } ota_error_t;

class SerialWiFiBridgeClass
{
private:
    DNSServer *_dns;
    AsyncWebServer *_server;
    AsyncWiFiManager *_wifiManager;
    TelnetSpy *_telnet;

    SerialWiFiBridgeClass()
    {
        _dns = new DNSServer();
        _server = new AsyncWebServer(80);
        _wifiManager = new AsyncWiFiManager(_server, _dns);
        _telnet = new TelnetSpy();
    }

    SerialWiFiBridgeClass(const SerialWiFiBridgeClass &);
    SerialWiFiBridgeClass &operator=(const SerialWiFiBridgeClass &);
    ~SerialWiFiBridgeClass()
    {
        delete _wifiManager;
        delete _dns;
        delete _server;
        delete _telnet;
    }

    Ticker _clocker;
    Ticker _blinker;
    Ticker _checker;

    static void configModeCallback(AsyncWiFiManager *myWiFiManager);
    static void saveConfigCallback();
    static void telnetConnected();
    static void telnetDisconnected();
    void _initWiFi();
    void _initOTA();
    void _initTelnet();

    void _connectToWiFi();

public:
    static SerialWiFiBridgeClass &getInstance()
    {
        static SerialWiFiBridgeClass inst;
        return inst;
    }

    void setup();
    void handle();
};

#endif
