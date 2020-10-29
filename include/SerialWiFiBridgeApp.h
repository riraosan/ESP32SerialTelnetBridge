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

#ifndef SerialWiFiBridgeApp_h
#define SerialWiFiBridgeApp_h

#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <Ticker.h>
//#include <ESP32_SPIFFS_ShinonomeFNT.h>
//#include <ESP32_SPIFFS_UTF8toSJIS.h>
#include <esp32-hal-log.h>

//#define HOSTNAME "esp32"
//#define MONITOR_SPEED 115200
//#define AP_NAME "ESP32-G-AP"
//#define MSG_CONNECTED "        WiFi Started."

class SerialWiFiBridgeApp
{
private:
    DNSServer dns;
    //AsyncWebServer server(80);
    //AsyncWiFiManager wifiManager(&server, &dns);

    AsyncWebServer server;
    AsyncWiFiManager wifiManager;


    Ticker clocker;
    Ticker blinker;
    Ticker checker;

public:
    SerialWiFiBridgeApp();
    ~SerialWiFiBridgeApp();
    void setup();
    void handle();
};

#endif
