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
    _server = getAsyncWebServerPtr();

    _bme = new Adafruit_BME280();

    _pressur = _bme->getPressureSensor();
    _temperatur = _bme->getTemperatureSensor();
    _humidity = _bme->getHumiditySensor();
}

MyApplication::~MyApplication()
{
    //TODO: something to do...
}

void MyApplication::initWebServer()
{
    SerialWiFiBridgeClass::initWebServer();
    //https://qiita.com/TakahiRoyte/items/949f4e88caecb02119aa#%E3%82%A4%E3%83%B3%E3%82%BF%E3%83%BC%E3%83%95%E3%82%A7%E3%83%BC%E3%82%B9%E3%81%AE%E7%B5%B1%E4%B8%80
    _server->on("/esp/sensor/temperatur", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/temperatur");
        String response;

        if (_bme->takeForcedMeasurement())
        {
            _root["id"] = _sensor_ID;
            _root["temperatur"] = getTemperature();
        }

        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/esp/sensor/pressur", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/pressur");
        String response;

        if (_bme->takeForcedMeasurement())
        {
            _root["id"] = _sensor_ID;
            _root["pressur"] = getPressure();
        }
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/esp/sensor/humidity", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/humidity");
        String response;

        if (_bme->takeForcedMeasurement())
        {
            _root["id"] = _sensor_ID;
            _root["humidity"] = getHumidity();
        }
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/esp/sensor/altitude", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/altitude");
        String response;

        if (_bme->takeForcedMeasurement())
        {
            _root["id"] = _sensor_ID;
            _root["altitude"] = getAltitude(SEALEVELPRESSURE_HPA);
        }

        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/esp/sensor/all", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/esp/sensor/all");
        String response;

        if (_bme->takeForcedMeasurement())
        {
            _root["id"] = _sensor_ID;
            _root["temperatur"] = getTemperature();
            _root["pressur"] = getPressure();
            _root["humidity"] = getHumidity();
        }

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

float MyApplication::getTemperature(void)
{
    float value = _bme->readTemperature();
    log_d("Temperature = %2.1f*C", value);

    return value;
}

float MyApplication::getPressure(void)
{
    float pascals = _bme->readPressure();
    log_d("Pressure = %4.1f (hPa)", pascals / 100.0f);

    return pascals / 100.0f;
}

float MyApplication::getHumidity(void)
{
    float humidity = _bme->readHumidity();
    log_d("Humidity = %2.1f %", humidity);

    return humidity;
}

float MyApplication::getAltitude(float seaLevel)
{
    float altitude = _bme->readAltitude(SEALEVELPRESSURE_HPA);
    log_d("Altitude = %4.1f m", altitude);

    return altitude;
}

uint32_t MyApplication::getSensorID(void)
{
    uint32_t id = _bme->sensorID();
    log_d("Sensor ID = %d", id);

    return id;
}

void MyApplication::initBME280()
{
    if (!_bme->begin(BME280_ADDRESS_ALTERNATE))
    {
        log_e("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        log_e("SensorID was: 0x%x", _bme->sensorID());
        log_e("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        log_e("   ID of 0x56-0x58 represents a BMP 280,\n");
        log_e("        ID of 0x60 represents a BME 280.\n");
        log_e("        ID of 0x61 represents a BME 680.\n");
        while (1)
            delay(10);
    }
    else
    {
        log_d("ESP could find a BME280 sensor!");
        log_d("SensorID was: 0x%x", _bme->sensorID());

        // humidity sensing
        // See Also. https://github.com/adafruit/Adafruit_BME280_Library/blob/master/examples/advancedsettings/advancedsettings.ino
        log_i("-- Humidity Sensing Scenario --");
        log_i("forced mode, 1x temperature / 1x humidity / 0x pressure oversampling");
        log_i("= pressure off, filter off");
        _bme->setSampling(Adafruit_BME280::sensor_mode::MODE_FORCED,
                          Adafruit_BME280::sensor_sampling::SAMPLING_X2,   // temperature
                          Adafruit_BME280::sensor_sampling::SAMPLING_NONE, // pressure
                          Adafruit_BME280::sensor_sampling::SAMPLING_X4,   // humidity
                          Adafruit_BME280::sensor_filter::FILTER_OFF);

        if (_bme->takeForcedMeasurement())
        {
            _sensor_ID = getSensorID();
            getTemperature();
            getPressure();
            getHumidity();
        }

        // suggested rate is 1Hz (1s)
        delay(1000);
    }
}

void MyApplication::setup()
{
    SerialWiFiBridgeClass::setup();
    initBME280();
    initWebServer();
}
