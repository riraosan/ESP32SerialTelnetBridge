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
    _server = new AsyncWebServer(80);

#ifdef MPL3115A2
    _baro = new Adafruit_MPL3115A2();
#endif
#ifdef BME280
    _bme = new Adafruit_BME280();
#endif
}

MyApplication::~MyApplication()
{
}

void MyApplication::initWebServer()
{

    //BME280 API(GET)                                lambda-introducer... I dont understand.
    _server->on("/esp/sensor/temperatur", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_n("/esp/sensor/temperatur");
    });

    _server->on("/esp/sensor/pressur", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_n("/esp/sensor/pressur");
    });

    _server->on("/esp/sensor/humidity", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_n("/esp/sensor/humidity");
    });

    _server->on("/esp/sensor/altitude", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_n("/esp/sensor/altitude");
    });

    _server->begin();
    log_n("Server Started");
}

#ifdef MPL3115A2
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
#endif

#ifdef BME280
float MyApplication::getTemperature(void)
{
    float value = _bme->readTemperature();
    log_n("Temperature = %2.1f*C", value);

    return value;
}

float MyApplication::getPressure(void)
{
    float pascals = _bme->readPressure();
    log_n("Pressure = %4.1f (hPa)", pascals / 100.0F);

    return pascals / 100.0F;
}

float MyApplication::getHumidity(void)
{
    float humidity = _bme->readHumidity();
    log_n("Humidity = %2.1f %", humidity);

    return humidity;
}

float MyApplication::getAltitude(float seaLevel)
{
    float altitude = _bme->readAltitude(SEALEVELPRESSURE_HPA);
    log_n("Altitude = %4.1f m", altitude);

    return altitude;
}

uint32_t MyApplication::getSensorID(void)
{
    uint32_t id = _bme->sensorID();
    log_n("Sensor ID = %d", id);

    return id;
}
#endif

void MyApplication::setup()
{
    SerialWiFiBridgeClass::setup();
#ifdef MPL3115A2
    if (!_baro->begin())
    {
        log_n("ESP32 couldn't find sensor.");
        return;
    }
    else
    {
        log_n("ESP32 could find sensor.");
    }

    setSeaPressure(SEALEVELPRESSURE_HPA);
    getPressure();
    getTemperature();
#endif
#ifdef BME280

    if (!_bme->begin(BME280_ADDRESS_ALTERNATE))
    {
        log_n("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        log_n("SensorID was: 0x%x", _bme->sensorID());
        log_n("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        log_n("   ID of 0x56-0x58 represents a BMP 280,\n");
        log_n("        ID of 0x60 represents a BME 280.\n");
        log_n("        ID of 0x61 represents a BME 680.\n");
        while (1)
            delay(10);
    }
    else
    {
        log_n("ESP could find a BME280 sensor!");
    }

    getTemperature();
    getPressure();
    getHumidity();
    getAltitude(SEALEVELPRESSURE_HPA);
    getSensorID();
#endif
    initWebServer();
}

void MyApplication::handle()
{
    SerialWiFiBridgeClass::handle();
}
