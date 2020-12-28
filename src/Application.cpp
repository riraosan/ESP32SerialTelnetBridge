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
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//MyApplication class is exsample to use SerialWiFiBridgeClass Library.
MyApplication::MyApplication()
{
    _sensor_ID = 0;
    //_server = new AsyncWebServer(80);
    _server = getAsyncWebServerPtr();
#ifdef MPL3115A2
    _baro = new Adafruit_MPL3115A2();
#endif
#ifdef BME280
    _bme = new Adafruit_BME280();

    _pressur = _bme->getPressureSensor();
    _temperatur = _bme->getTemperatureSensor();
    _humidity = _bme->getHumiditySensor();
#endif
}

MyApplication::~MyApplication()
{
}

void MyApplication::initWebServer()
{
    SerialWiFiBridgeClass::initWebServer();
    //BME280 API(GET)                                lambda-introducer... I couldn't understand...
    _server->on("/esp/sensor/temperatur", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/temperatur");
        String response;

        _root["id"] = _sensor_ID;
        _root["temperatur"] = getTemperature();
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/esp/sensor/pressur", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/pressur");
        String response;

        _root["id"] = _sensor_ID;
        _root["pressur"] = getPressure();
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/esp/sensor/humidity", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/humidity");
        String response;

        _root["id"] = _sensor_ID;
        _root["humidity"] = getHumidity();
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/esp/sensor/altitude", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/altitude");
        String response;

        _root["id"] = _sensor_ID;
        _root["altitude"] = getAltitude(SEALEVELPRESSURE_HPA);
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/esp/sensor/all", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/all");
        String response;

        _root["id"] = _sensor_ID;
        _root["temperatur"] = getTemperature();
        _root["pressur"] = getPressure();
        _root["humidity"] = getHumidity();
        _root["altitude"] = getAltitude(SEALEVELPRESSURE_HPA);
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->onNotFound([](AsyncWebServerRequest *request) {
        log_d("onNotFound");
        request->send(404, "application/json", "{\"message\":\"Not found\"}");
    });

    _server->begin();
    log_d("Server Started");
}

#ifdef MPL3115A2
float MyApplication::getPressure()
{
    float pascals = _baro->getPressure();
    log_d("%4.1f (hPa)", pascals * 0.01);

    return pascals * 0.01;
}

float MyApplication::getTemperature()
{
    float tempC = _baro->getTemperature();
    log_d("%2.1f *C", tempC);

    return tempC;
}

void MyApplication::setSeaPressure(float hPascal)
{
    _baro->setSeaPressure(hPascal * 100);
}
#endif

#ifdef BME280
float MyApplication::getTemperature(void)
{
    if (_bme->takeForcedMeasurement())
    {
        float value = _bme->readTemperature();
        log_d("Temperature = %2.1f*C", value);

        return value;
    }

    return -1;
}

float MyApplication::getPressure(void)
{
    if (_bme->takeForcedMeasurement())
    {
        float pascals = _bme->readPressure();
        log_d("Pressure = %4.1f (hPa)", pascals / 100.0F);

        return pascals / 100.0F;
    }

    return -1;
}

float MyApplication::getHumidity(void)
{
    if (_bme->takeForcedMeasurement())
    {
        float humidity = _bme->readHumidity();
        log_d("Humidity = %2.1f %", humidity);

        return humidity;
    }

    return -1;
}

float MyApplication::getAltitude(float seaLevel)
{
    if (_bme->takeForcedMeasurement())
    {
        float altitude = _bme->readAltitude(SEALEVELPRESSURE_HPA);
        log_d("Altitude = %4.1f m", altitude);

        return altitude;
    }

    return -1;
}

uint32_t MyApplication::getSensorID(void)
{
    uint32_t id = _bme->sensorID();
    log_d("Sensor ID = %d", id);

    return id;
}
#endif

void MyApplication::setup()
{
    SerialWiFiBridgeClass::setup();
#ifdef MPL3115A2
    if (!_baro->begin())
    {
        log_d("ESP32 couldn't find sensor.");
        return;
    }
    else
    {
        log_d("ESP32 could find sensor.");
    }

    setSeaPressure(SEALEVELPRESSURE_HPA);
    getPressure();
    getTemperature();
#endif
#ifdef BME280
    if (!_bme->begin(BME280_ADDRESS_ALTERNATE))
    {
        log_d("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        log_d("SensorID was: 0x%x", _bme->sensorID());
        log_d("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        log_d("   ID of 0x56-0x58 represents a BMP 280,\n");
        log_d("        ID of 0x60 represents a BME 280.\n");
        log_d("        ID of 0x61 represents a BME 680.\n");
        while (1)
            delay(10);
    }
    else
    {
        log_d("ESP could find a BME280 sensor!");
        log_d("SensorID was: 0x%x", _bme->sensorID());
    }

    _bme->setSampling(Adafruit_BME280::sensor_mode::MODE_FORCED);
    delay(100);

    _pressur = _bme->getPressureSensor();
    _temperatur = _bme->getTemperatureSensor();
    _humidity = _bme->getHumiditySensor();

    //getTemperature();
    //getPressure();
    //getHumidity();
    //getAltitude(SEALEVELPRESSURE_HPA);
    _sensor_ID = getSensorID();
#endif
    initWebServer();
}
