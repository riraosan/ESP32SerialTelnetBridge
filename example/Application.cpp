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

static ENUM_MESSAGE_ID msg_id = ENUM_MESSAGE_ID::MSG_COMMAND_NOTHING;

// Application class is exsample to use SerialTelnetBridgeClass Library.
Application::Application() {
    _sensor_ID = 0;
    _server    = getAsyncWebServerPtr();
    _bme       = new Adafruit_BME280();
}

Application::~Application() {}

void Application::initPorts() {
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);

    digitalWrite(25, LOW);
    digitalWrite(26, LOW);
}

void Application::initClock(void) {
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
    delay(2000);
}

void Application::_checkSensor() {
    msg_id = ENUM_MESSAGE_ID::MSG_COMMAND_CHECK_SENSOR;
}

void Application::checkSensor(void) {
    StaticJsonDocument<512> local;

    String chipID(_getESP32ChipID());
    String sensorID(getSensorID());

    local["chip_id"]    = chipID;
    local["created_at"] = makeTime();
    local["status"]     = "online";

    JsonObject _links      = local.createNestedObject("_links");
    _links["self"]["href"] = String(_ENDPOINT_URI_sensor);
    _links["next"]["href"] = String(_ENDPOINT_URI_relay);

    JsonObject _embedded_sensor =
        local["_embedded"]["sensor"].createNestedObject(sensorID);
    _embedded_sensor["device_name"] = "BME280";
    _embedded_sensor["temperature"] = getTemperature();
    _embedded_sensor["humidity"]    = getHumidity();
    _embedded_sensor["pressure"]    = getPressure();

    serializeJson(local, _sensors_responseJson);

    String debug;
    serializeJsonPretty(local, debug);
    log_d("%s", debug.c_str());
}

void Application::commandErrorCallbackSerial0(cmd_error *cmdError) {
    CommandError commandError(cmdError);  // Create wrapper object
    log_e("ERROR: (Serial1) %s", commandError.toString().c_str());
}

void Application::commandErrorCallbackSerial1(cmd_error *cmdError) {
    CommandError commandError(cmdError);  // Create wrapper object
    log_e("ERROR: (Serial1) %s", commandError.toString().c_str());
}

void Application::commandErrorCallbackSerial2(cmd_error *cmdError) {
    CommandError commandError(cmdError);  // Create wrapper object
    log_e("ERROR: (Serial2) %s", commandError.toString().c_str());
}

void Application::commandCallbackSerial0(cmd *cmdline) {}

void Application::commandCallbackSerial1(cmd *cmdline) {
    Command command(cmdline);  // Create wrapper object

    log_d("cmmand line = %s", command.toString());

    // Get first (and only) Argument
    Argument arg = command.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();

    if (arg.getValue() == "clock") {
        // sendClockMessage();
    } else if (arg.getValue() == "reset") {
        // sendResetMessage();
    } else if (arg.getValue() == "state") {
        printEspState();
    }
}

void Application::commandCallbackSerial2(cmd *cmdline) {}

void Application::initConsole() {
    _cli0.setOnError(Application::commandErrorCallbackSerial0);
    _cli1.setOnError(Application::commandErrorCallbackSerial1);
    _cli2.setOnError(Application::commandErrorCallbackSerial2);

    _command0 = _cli0.addSingleArgCmd("esp", Application::commandCallbackSerial0);
    _command1 = _cli1.addSingleArgCmd("esp", Application::commandCallbackSerial1);
    _command2 = _cli2.addSingleArgCmd("esp", Application::commandCallbackSerial2);
}
// template
void Application::_handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    log_d("Application::handleBody");
    String localJson;

    StaticJsonDocument<512> _local;

    log_d("BodyStart: %u Byte", total);
    for (size_t i = 0; i < len; i++) {
        localJson += (char)data[i];
    }
    log_d("BodyEnd: %u Byte", index + len);

    DeserializationError error = deserializeJson(_local, localJson);

    if (error) {
        log_d("deserializeJson() failed: %s", error.f_str());
        return;
    }
}

void Application::initWebServer() {
    _server->on(
        _ENDPOINT_URI_sensor, HTTP_GET, [this](AsyncWebServerRequest *request) {
            log_d("[HTTP_GET] %s", _ENDPOINT_URI_sensor);

            if (_sensors_responseJson.isEmpty()) {
                request->send(204, "application/json; charset=UTF-8", "{\"message\":\"No Content\"}");
                return;
            } else if (request->hasParam(Application::_PARAM_DEVICE_ID)) {
                String chip_id(request->getParam(_PARAM_DEVICE_ID)->value());

                if (chip_id == _getESP32ChipID()) {
                    request->send(200, "application/json; charset=UTF-8", _sensors_responseJson);
                    return;
                }
            }
            request->send(400, "application/json; charset=UTF-8", "{\"message\":\"Bad Request\"}");
        });

    _server->on(
        _ENDPOINT_URI_relay, HTTP_PUT,
        [this](AsyncWebServerRequest *request) {
            log_d("[HTTP_PUT] %s", _ENDPOINT_URI_relay);

            if (request->hasParam(Application::_PARAM_DEVICE_ID)) {
                String chip_id(
                    request->getParam(Application::_PARAM_DEVICE_ID)->value());

                if (chip_id == Application::_getESP32ChipID()) {
                    request->send(200, "application/json; charset=UTF-8", _relays_responseJson);
                    return;
                }
            }

            request->send(400, "application/json; charset=UTF-8", "{\"message\":\"Bad Request\"}");
        },
        nullptr,
        [this](AsyncWebServerRequest *request, uint8_t *data, size_t len,
               size_t index, size_t total) {
            log_d("Application::handleBody");
            String localJson;

            StaticJsonDocument<512> _local;

            log_d("BodyStart: %u Byte", total);
            for (size_t i = 0; i < len; i++) {
                localJson += (char)data[i];
            }
            log_d("BodyEnd: %u Byte", index + len);

            DeserializationError error = deserializeJson(_local, localJson);

            if (error) {
                log_d("deserializeJson() failed: %s", error.f_str());
                return;
            }

            if (request->hasParam(_PARAM_DEVICE_ID)) {
                String chip_id(request->getParam(_PARAM_DEVICE_ID)->value());

                if (chip_id == _getESP32ChipID()) {
                    String _embedded_relay_0_turn((
                        const char *)_local["_embedded"]["relay"]["0"]["turn"]);
                    String _embedded_relay_1_turn((
                        const char *)_local["_embedded"]["relay"]["1"]["turn"]);

                    if (_embedded_relay_0_turn == "on") {
                        digitalWrite(25, HIGH);
                    } else {
                        digitalWrite(25, LOW);
                    }

                    if (_embedded_relay_1_turn == "on") {
                        digitalWrite(26, HIGH);
                    } else {
                        digitalWrite(26, LOW);
                    }

                    serializeJsonPretty(_local, _relays_responseJson);
                }
            }
        });

    // Servo
    _server->on(_ENDPOINT_URI_servo, HTTP_GET,
                [this](AsyncWebServerRequest *request) {
                    log_d("[HTTP_GET] %s", _ENDPOINT_URI_servo);
                    request->send(200);
                });

    _server->on(_ENDPOINT_URI_servo, HTTP_PUT,
                [this](AsyncWebServerRequest *request) {
                    log_d("[HTTP_PUT] %s", _ENDPOINT_URI_servo);
                    request->send(200);
                });

    _server->onNotFound([](AsyncWebServerRequest *request) {
        log_d("onNotFound");
        request->send(404, "application/json; charset=UTF-8", "{\"message\":\"Not found\"}");
    });

    _server->begin();
    log_d("Server Started");
}

float Application::getTemperature(void) {
    float value = _bme->readTemperature();
    log_d("Temperature = %2.1f*C", value);

    return value;
}

float Application::getPressure(void) {
    float pascals = _bme->readPressure();
    log_d("Pressure = %4.1f(hPa)", pascals / 100.0f);

    return pascals / 100.0f;
}

float Application::getHumidity(void) {
    float humidity = _bme->readHumidity();
    log_d("Humidity = %2.1f%%", humidity);

    return humidity;
}

float Application::getAltitude(float seaLevel) {
    float altitude = _bme->readAltitude(SENSORS_PRESSURE_SEALEVELHPA);
    log_d("Altitude = %4.1f m", altitude);

    return altitude;
}

uint32_t Application::getSensorID(void) {
    uint32_t id = _bme->sensorID();
    log_d("Sensor ID = %d", id);

    return id;
}

// See Also.
// https://github.com/adafruit/Adafruit_BME280_Library/blob/master/examples/advancedsettings/advancedsettings.ino
// weather monitoring
void Application::initBME280WeatherStation() {
    log_i("-- Weather Station Scenario --");
    log_i("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
    log_i("filter off");
    _bme->setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X1,  // temperature
                      Adafruit_BME280::SAMPLING_X1,  // pressure
                      Adafruit_BME280::SAMPLING_X1,  // humidity
                      Adafruit_BME280::FILTER_OFF);
}

// humidity sensing
void Application::initBME280HumiditySensing() {
    log_i("-- Humidity Sensing Scenario --");
    log_i("forced mode, 2x temperature / 4x humidity / 0x pressure oversampling");
    log_i("= pressure off, filter off");
    _bme->setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X2,    // temperature
                      Adafruit_BME280::SAMPLING_NONE,  // pressure
                      Adafruit_BME280::SAMPLING_X4,    // humidity
                      Adafruit_BME280::FILTER_OFF);

    if (_bme->takeForcedMeasurement()) {
        _sensor_ID = getSensorID();
        getTemperature();
        getPressure();
        getHumidity();
    }
}

String Application::makeTime() {
    time_t t      = time(NULL);
    struct tm *tm = localtime(&t);

    char buffer[128] = {0};
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d+0900", tm->tm_year + 1900,
            tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    log_i("[time] %s", String(buffer).c_str());

    return String(buffer);
}

String Application::_byteToHexString(uint8_t *buf, uint8_t length,
                                     String strSeperator) {
    char HEX_CHAR_ARRAY[17] = "0123456789ABCDEF";
    String dataString;

    for (uint8_t i = 0; i < length; i++) {
        byte v = buf[i] / 16;
        byte w = buf[i] % 16;
        if (i > 0) {
            dataString += strSeperator;
        }
        dataString += String(HEX_CHAR_ARRAY[v]);
        dataString += String(HEX_CHAR_ARRAY[w]);
    }

    dataString.toUpperCase();

    return dataString;
}

String Application::_getESP32ChipID(void) {
    uint64_t chipid                 = 0;
    int chipid_size                 = 6;
    uint8_t chipid_arr[chipid_size] = {0};

    chipid = ESP.getEfuseMac();  // The chip ID is essentially its MAC
                                 // address(length: 6 bytes).

    for (uint8_t i = 0; i < chipid_size; i++) {
        chipid_arr[i] = (chipid >> (8 * i)) & 0xff;
    }

    return _byteToHexString(chipid_arr, chipid_size, "");
}

void Application::setup() {
    SerialTelnetBridgeClass::begin();

    if (!_bme->begin(BME280_ADDRESS_ALTERNATE)) {
        log_e("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        log_e("SensorID was: 0x%x", _bme->sensorID());
        log_e("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        log_e("   ID of 0x56-0x58 represents a BMP 280,\n");
        log_e("        ID of 0x60 represents a BME 280.\n");
        log_e("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    } else {
        log_d("ESP could find a BME280 sensor!");
        log_d("SensorID was: 0x%x", _bme->sensorID());
#ifdef WEATER_STATION_SENARIO
        initBME280WeatherStation();
#endif
#ifdef HUMIDITY_SENSING_SENARIO
        initBME280HumiditySensing();
#endif
    }
    initPorts();
    initWebServer();
    initClock();

    sensorChecker.attach(60, Application::_checkSensor);
}

void Application::messageHandler(ENUM_MESSAGE_ID message_id) {
    switch (message_id) {
        case ENUM_MESSAGE_ID::MSG_COMMAND_CLOCK:
            log_d("ENUM_MESSAGE_ID::MSG_COMMAND_CLOCK");
            break;
        case ENUM_MESSAGE_ID::MSG_COMMAND_RESET:
            log_d("ENUM_MESSAGE_ID::MSG_COMMAND_RESET");
            ESP.restart();
            delay(2000);
            break;
        case ENUM_MESSAGE_ID::MSG_COMMAND_CHECK_SENSOR:
            log_d("ENUM_MESSAGE_ID::MSG_COMMAND_CHECK_SENSOR");
            checkSensor();
            break;
            /*
        case ENUM_MESSAGE_ID::MSG_COMMAND_RELAY_0_ON:
            log_d("ENUM_MESSAGE_ID::MSG_COMMAND_RELAY_0_ON");
            digitalWrite(25, HIGH);
            break;
        case ENUM_MESSAGE_ID::MSG_COMMAND_RELAY_0_OFF:
            log_d("ENUM_MESSAGE_ID::MSG_COMMAND_RELAY_0_OFF");
            digitalWrite(25, LOW);
            break;
        case ENUM_MESSAGE_ID::MSG_COMMAND_RELAY_1_ON:
            log_d("ENUM_MESSAGE_ID::MSG_COMMAND_RELAY_1_ON");
            digitalWrite(26, HIGH);
            break;
        case ENUM_MESSAGE_ID::MSG_COMMAND_RELAY_1_OFF:
            log_d("ENUM_MESSAGE_ID::MSG_COMMAND_RELAY_1_OFF");
            digitalWrite(26, LOW);
            break;
            */
        default:;
    }

    msg_id = ENUM_MESSAGE_ID::MSG_COMMAND_NOTHING;
}

void Application::handle() {
    SerialTelnetBridgeClass::handle();
    messageHandler(msg_id);
}
