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

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Ticker.h>
#include <SerialTelnetBridge.h>

//Message ID
enum class ENUM_MESSAGE_ID
{
    MSG_COMMAND_RESET,
    MSG_COMMAND_CLOCK,
    MSG_COMMAND_CHECK_SENSOR,
    MSG_COMMAND_RELAY_0_ON,
    MSG_COMMAND_RELAY_0_OFF,
    MSG_COMMAND_RELAY_1_ON,
    MSG_COMMAND_RELAY_1_OFF,
    MSG_COMMAND_NOTHING
};

class Application : public SerialTelnetBridgeClass
{
public:
    static Application &getInstance()
    {
        static Application instance;
        return instance;
    }
    static void sendSensorInfo(void);

    void initBME280HumiditySensing(void);
    void initBME280WeatherStation(void);
    void initUnifiedBME280(void);
    void initWebServer(void);
    void initConsole(void);
    void initPorts(void);
    void initClock(void);

    float getTemperature(void);
    float getPressure(void);
    float getHumidity(void);
    float getAltitude(float seaLevel);
    uint32_t getSensorID(void);

    String makeTime(void);
    void checkSensor(void);

    void messageHandler(ENUM_MESSAGE_ID message_id);

    static void commandErrorCallbackSerial0(cmd_error *cmdError);
    static void commandErrorCallbackSerial1(cmd_error *cmdError);
    static void commandErrorCallbackSerial2(cmd_error *cmdError);

    static void commandCallbackSerial0(cmd *cmdline);
    static void commandCallbackSerial1(cmd *cmdline);
    static void commandCallbackSerial2(cmd *cmdline);

    void setup(void);
    void handle(void);

private:
    AsyncWebServer *_server;

    Adafruit_BME280 *_bme;
    Adafruit_Sensor *_pressur;
    Adafruit_Sensor *_temperature;
    Adafruit_Sensor *_humidity;
    uint32_t _sensor_ID;
    Ticker sensorChecker;

    String _sensors_responseJson;
    String _relays_responseJson;
    String _servos_responseJson;

    Application();
    ~Application();

    Application(const Application &);
    Application &operator=(const Application &);

    constexpr static PROGMEM prog_char *_PARAM_DEVICE_ID = "device_id";
    constexpr static PROGMEM prog_char *_ENDPOINT_URI_sensor = "/api/v1/sensors/all";
    constexpr static PROGMEM prog_char *_ENDPOINT_URI_relay = "/api/v1/relays/operations";
    constexpr static PROGMEM prog_char *_ENDPOINT_URI_servo = "/api/v1/servos/operations";

    String _getESP32ChipID(void);
    String _byteToHexString(uint8_t *buf, uint8_t length, String strSeperator = ":");

    static void _handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

    static void _checkSensor(void);
};
