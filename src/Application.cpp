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

#include "Application.h"
#include <esp32-hal-log.h>

//MyApplication class is exsample to use SerialWiFiBridgeClass Library.
MyApplication::MyApplication()
{
    _pServer = getAsyncWebServerPtr();
    _baro = new Adafruit_MPL3115A2();
}

MyApplication::~MyApplication()
{
}

void MyApplication::initWebServer()
{   
    //REST API(GET)
    _pServer->on("/esp/sensor/get/temperatur", HTTP_GET, [](AsyncWebServerRequest *request) {
        log_n("/esp/sensor/get/temperatur");
        request->send(200);
    });

    _pServer->on("/esp/sensor/get/pressur", HTTP_GET, [](AsyncWebServerRequest *request) {
        log_n("/esp/sensor/get/pressur");
        request->send(200);
    });

    _pServer->begin();
    log_n("Server Started");
}

float MyApplication::getPressure()
{
    float pascals = _baro->getPressure();
    log_n("%4.1f (hPa)", pascals * 0.01);
    
    return pascals * 0.01;
}

float MyApplication::getTemperature()
{
    float tempC = _baro->getTemperature();
    log_n("%2.1f *C", tempC);
    
    return tempC;
}

/*!
 *  @brief  Set the local sea level barometric pressure
 *  @param pascal the pressure (hPa) to use as the baseline
 */
void MyApplication::setSeaPressure(float hPascal)
{
    _baro->setSeaPressure(hPascal * 100);
}

void MyApplication::setup()
{
    SerialWiFiBridgeClass::setup();

    if (!_baro->begin())
    {
        log_n("ESP32 couldn't find sensor.");
        return;
    }
    else
    {
        log_n("ESP32 could find sensor.");
    }

    setSeaPressure(1014.9);//令和元年　平均海面気圧(hPa)

    initWebServer();

    getPressure();
    getTemperature();
}

void MyApplication::handle()
{
    SerialWiFiBridgeClass::handle();
}

