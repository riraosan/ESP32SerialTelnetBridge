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

#include <Application.h>

//Application class is exsample to use SerialTelnetBridgeClass Library.
Application::Application()
{
    _sensor_ID = 0;
    _server = getAsyncWebServerPtr();

    _bme = new Adafruit_BME280();
}

Application::~Application() {}

void Application::initWebServer()
{
    _server->on("/sensor/V1/temperature", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /sensor/V1/temperature");
        String response;

        if (_bme->takeForcedMeasurement())
        {
            _root["id"] = _sensor_ID;
            _root["temperatur"] = getTemperature();
        }

        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/sensor/V1/pressure", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /sensor/V1/pressure");
        String response;

        if (_bme->takeForcedMeasurement())
        {
            _root["id"] = _sensor_ID;
            _root["pressur"] = getPressure();
        }
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/sensor/V1/humidity", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /sensor/V1/humidity");
        String response;

        if (_bme->takeForcedMeasurement())
        {
            _root["id"] = _sensor_ID;
            _root["humidity"] = getHumidity();
        }
        serializeJson(_root, response);

        request->send(200, "application/json", response);
    });

    _server->on("/sensor/V1/all", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("/sensor/V1/all");
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

    _server->on("/clock/V1/now", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /clock/V1/now");
        request->send(200);
    });

    _server->on("/reset/V1/now", HTTP_PUT, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_PUT] /reset/V1/now");
        request->send(200);
    });

    _server->on("/state/V1/now", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /state/V1/now");
        request->send(200);
    });

    //Relay
    _server->on("/relay/V1/1/operation", HTTP_PUT, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_PUT] /relay/V1/1/operation");
        request->send(200);
    });

    _server->on("/relay/V1/2/operation", HTTP_PUT, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_PUT] /relay/V1/2/operation");
        request->send(200);
    });

    //Servo
    _server->on("/servo/V1/1/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /servo/V1/1/status");
        request->send(200);
    });

    _server->on("/servo/V1/2/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /servo/V1/2/status");
        request->send(200);
    });

    _server->on("/servo/V1/1/operation", HTTP_PUT, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_PUT] /servo/V1/1/operation");
        request->send(200);
    });

    _server->on("/servo/V1/2/operation", HTTP_PUT, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_PUT] /servo/V1/2/operation");
        request->send(200);
    });

    _server->begin();
    log_d("Server Started");
}

float Application::getTemperature(void)
{
    float value = _bme->readTemperature();
    log_d("Temperature = %2.1f*C", value);

    return value;
}

float Application::getPressure(void)
{
    float pascals = _bme->readPressure();
    log_d("Pressure = %4.1f (hPa)", pascals / 100.0f);

    return pascals / 100.0f;
}

float Application::getHumidity(void)
{
    float humidity = _bme->readHumidity();
    log_d("Humidity = %2.1f %", humidity);

    return humidity;
}

float Application::getAltitude(float seaLevel)
{
    float altitude = _bme->readAltitude(SENSORS_PRESSURE_SEALEVELHPA);
    log_d("Altitude = %4.1f m", altitude);

    return altitude;
}

uint32_t Application::getSensorID(void)
{
    uint32_t id = _bme->sensorID();
    log_d("Sensor ID = %d", id);

    return id;
}

// See Also. https://github.com/adafruit/Adafruit_BME280_Library/blob/master/examples/advancedsettings/advancedsettings.ino
// weather monitoring
void Application::initBME280WeatherStation()
{
    log_i("-- Weather Station Scenario --");
    log_i("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
    log_i("filter off");
    _bme->setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X1, // temperature
                      Adafruit_BME280::SAMPLING_X1, // pressure
                      Adafruit_BME280::SAMPLING_X1, // humidity
                      Adafruit_BME280::FILTER_OFF);

    // suggested rate is 1/60Hz (1m)
}

// humidity sensing
void Application::initBME280HumiditySensing()
{
    log_i("-- Humidity Sensing Scenario --");
    log_i("forced mode, 2x temperature / 4x humidity / 0x pressure oversampling");
    log_i("= pressure off, filter off");
    _bme->setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X2,   // temperature
                      Adafruit_BME280::SAMPLING_NONE, // pressure
                      Adafruit_BME280::SAMPLING_X4,   // humidity
                      Adafruit_BME280::FILTER_OFF);

    if (_bme->takeForcedMeasurement())
    {
        _sensor_ID = getSensorID();
        getTemperature();
        getPressure();
        getHumidity();
    }

    // suggested rate is 1Hz (1s)
}

void Application::setup()
{
    SerialTelnetBridgeClass::begin();

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
#ifdef WEATER_STATION_SENARIO
        initBME280WeatherStation();
#endif
#ifdef HUMIDITY_SENSING_SENARIO
        initBME280HumiditySensing();
#endif
    }
    initWebServer();
}

void Application::handle()
{
}

void Application::onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {}

/*
void SerialWiFiBridgeClass::messageHandle(ENUM_MESSAGE_ID message_id)
{
    switch (message_id)
    {
    case ENUM_MESSAGE_ID::MSG_COMMAND_CLOCK:
        printClock();
        break;
    case ENUM_MESSAGE_ID::MSG_COMMAND_RESET:
        ESP.restart();
        delay(2000);
        break;
    default:;
    }

    msg_id = ENUM_MESSAGE_ID::MSG_COMMAND_NOTHING;
}
*/

/*
void SerialWiFiBridgeClass::commandErrorCallbackSerial1(cmd_error *cmdError)
{
    CommandError commandError(cmdError); // Create wrapper object
    log_e("ERROR: (Serial1) %s", commandError.toString().c_str());
}

void SerialWiFiBridgeClass::commandErrorCallbackSerial2(cmd_error *cmdError)
{
    CommandError commandError(cmdError); // Create wrapper object
    log_e("ERROR: (Serial2) %s", commandError.toString().c_str());
}

void SerialWiFiBridgeClass::commandCalllbackSerial1(cmd *cmdline)
{
    Command command(cmdline); // Create wrapper object

    log_d("cmmand line = %s", command.toString());

    // Get first (and only) Argument
    Argument arg = command.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();

    if (arg.getValue() == "clock")
    {
        //sendClockMessage();
    }
    else if (arg.getValue() == "reset")
    {
        //sendResetMessage();
    }
    else if (arg.getValue() == "state")
    {
        printEspState();
    }
}

void SerialWiFiBridgeClass::commandCalllbackSerial2(cmd *cmdline)
{
}

void SerialWiFiBridgeClass::initConsole()
{
    _cli0.setOnError(SerialWiFiBridgeClass::commandErrorCallback);
    _cli1.setOnError(SerialWiFiBridgeClass::commandErrorCallbackSerial1);
    _cli2.setOnError(SerialWiFiBridgeClass::commandErrorCallbackSerial2);

    _command0 = _cli0.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllback);
    _command1 = _cli1.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllbackSerial1);
    _command2 = _cli2.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllbackSerial2);
}
*/
