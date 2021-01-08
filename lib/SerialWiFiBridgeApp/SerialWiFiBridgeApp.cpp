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
#include <SerialWiFiBridgeApp.h>
#include <esp32-hal-log.h>

/*
    log_e("")：エラー
    log_w("")：警告
    log_i("")：情報
    log_d("")：デバッグ
    log_v("")：verbose?
    log_d("")：通常
*/

static ENUM_MESSAGE_ID msg_id = ENUM_MESSAGE_ID::MSG_COMMAND_NOTHING;

void SerialWiFiBridgeClass::initPort()
{
    log_d("- Initializing Ports...");
    //TODO:Initializing Ports
}

void SerialWiFiBridgeClass::initEEPROM()
{
    log_d("- Initializing EEPROM...");
    //TODO:Initializing EEPROM
}

void SerialWiFiBridgeClass::initFS()
{
    log_d("- Mounting SPIFFS...");
    if (!SPIFFS.begin())
    {
        log_e("- An Error has occurred while mounting SPIFFS");
        return;
    }
}

void SerialWiFiBridgeClass::initWiFi()
{
    _wifiManager->setDebugOutput(false);
    _wifiManager->setConfigPortalTimeout(PORTAL_TIMEOUT);
    _wifiManager->setAPCallback([](AsyncWiFiManager *myWiFiManager) {
        log_d("- No known wifi found");
        log_d("- Starting AP: ");
        log_d("%s \n", myWiFiManager->getConfigPortalSSID().c_str());
        log_d("%s \n", WiFi.softAPIP().toString().c_str());
    });

    // enable autoconnect
    if (!(String(AP_PASSWORD).isEmpty() ? _wifiManager->autoConnect(AP_NAME) : _wifiManager->autoConnect(AP_NAME, AP_PASSWORD)))
    {
        log_d("- Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    }
    else
    {
        log_d("- WiFi: %s", WiFi.SSID().c_str());
        log_d("-  Mac: %s", WiFi.macAddress().c_str());
        log_d("-   IP: %s", WiFi.localIP().toString().c_str());

        log_d("- address to _dns = 0x%x", _dns);
        log_d("- address to _server = 0x%x", _server);

        log_d("- WiFi Started");
    }
}

void SerialWiFiBridgeClass::_telnetConnected()
{
    log_d("- Telnet connection established.");
    //TODO LED ON
}

void SerialWiFiBridgeClass::_telnetDisconnected()
{
    log_d("- Telnet connection closed.");
    //TODO LED OFF
}

void SerialWiFiBridgeClass::initSerial()
{
    log_d("- Initializing Serial...");

    //_Serial0->setDebugOutput(true);
    _Serial1->setDebugOutput(true);
    _Serial2->setDebugOutput(true);

    //set buffer sizze
    _Serial1->setRxBufferSize(SERIAL1_BUFFER_SIZE);
    _Serial2->setRxBufferSize(SERIAL2_BUFFER_SIZE);

    //_Serial0->begin(UART_BAUD0, SERIAL_8N1, SERIAL0_RXPIN, SERIAL0_TXPIN);
    _Serial1->begin(UART_BAUD1, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
    _Serial2->begin(UART_BAUD2, SERIAL_8N1, SERIAL2_RXPIN, SERIAL2_TXPIN);

    //_Serial0->println("Serial0 Txd is on pin: " + String(SERIAL0_TXPIN));
    //_Serial0->println("Serial0 Rxd is on pin: " + String(SERIAL0_RXPIN));

    _Serial1->println("Serial1 Txd is on pin: " + String(SERIAL1_TXPIN));
    _Serial1->println("Serial1 Rxd is on pin: " + String(SERIAL1_RXPIN));

    _Serial2->println("Serial2 Txd is on pin: " + String(SERIAL2_TXPIN));
    _Serial2->println("Serial2 Rxd is on pin: " + String(SERIAL2_RXPIN));

    //_Serial0->flush();
    _Serial1->flush();
    _Serial2->flush();

    delay(500);
}

void SerialWiFiBridgeClass::initTelnet()
{
    //TODO to gain buffer size
    log_d("- Initializing Telnet...");

    String welcome0("\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial0 <-> esp32.local:55550)\n");
    String welcome1("\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial1 <-> esp32.local:55551)\n");
    String welcome2("\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial2 <-> esp32.local:55552)\n");

    String prompt(COMMAND_PROMPT);

    welcome0 += prompt;
    welcome1 += prompt;
    welcome2 += prompt;
    /*
    _telnet0->setWelcomeMsg((char *)welcome0.c_str());
    _telnet0->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet0->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet0->setPort(SERIAL0_TCP_PORT);
    //to begin telnet only
    _telnet0->setSerial(nullptr);
    _telnet0->begin(UART_BAUD0);
*/
    _telnet1->setWelcomeMsg((char *)welcome1.c_str());
    _telnet1->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet1->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet1->setPort(SERIAL1_TCP_PORT);
    _telnet1->setBufferSize(SERIAL1_BUFFER_SIZE);
    //to begin telnet only
    _telnet1->setSerial(nullptr);
    _telnet1->begin(UART_BAUD1);

    _telnet2->setWelcomeMsg((char *)welcome2.c_str());
    _telnet2->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet2->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet2->setPort(SERIAL2_TCP_PORT);
    _telnet2->setBufferSize(SERIAL2_BUFFER_SIZE);
    //to begin telnet only
    _telnet2->setSerial(nullptr);
    _telnet2->begin(UART_BAUD2);

    delay(500);
}

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
        sendClockMessage();
    }
    else if (arg.getValue() == "reset")
    {
        sendResetMessage();
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
    //_cli0.setOnError(SerialWiFiBridgeClass::commandErrorCallback);
    _cli1.setOnError(SerialWiFiBridgeClass::commandErrorCallbackSerial1);
    _cli2.setOnError(SerialWiFiBridgeClass::commandErrorCallbackSerial2);

    //_command0 = _cli0.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllback);
    _command1 = _cli1.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllbackSerial1);
    _command2 = _cli2.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllbackSerial2);
}

void SerialWiFiBridgeClass::initOTA()
{
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else
            type = "filesystem";

        log_d("Start updating %s", type.c_str());
    });

    ArduinoOTA.onEnd([]() {
        log_d("End");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        log_d("Progress: %u%%", (progress / (total / 100)));
        log_printf("\033[1F");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        log_e("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            log_e("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            log_e("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            log_e("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            log_e("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            log_e("End Failed");
        }
    });

    ArduinoOTA.setHostname(HOSTNAME);

    log_d("- Hostname: %s", ArduinoOTA.getHostname().c_str());

    ArduinoOTA.begin();
    log_d("- OTA Started.");
}

void SerialWiFiBridgeClass::onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    ;
}

void SerialWiFiBridgeClass::initWebServer()
{
    log_d("- Initializing HTTP Server...");
    //REST API(POST)
    _server->on(
        "/esp/setting", HTTP_POST, [this](AsyncWebServerRequest *request) {
            log_d("[HTTP_POST] /");
            request->send(200);
        },
        nullptr, onBody);

    //REST API(GET)
    _server->on("/esp/clock", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /esp/clock");
        sendClockMessage();
        request->send(200);
    });

    _server->on("/esp/reset", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /esp/reset");
        request->send(200);
        sendResetMessage();
    });

    _server->on("/esp/state", HTTP_GET, [this](AsyncWebServerRequest *request) {
        log_d("[HTTP_GET] /esp/state");
        printEspState();
        request->send(200);
    });
}

void SerialWiFiBridgeClass::sendClockMessage()
{
    msg_id = ENUM_MESSAGE_ID::MSG_COMMAND_CLOCK;
}

void SerialWiFiBridgeClass::sendResetMessage()
{
    msg_id = ENUM_MESSAGE_ID::MSG_COMMAND_RESET;
}

void SerialWiFiBridgeClass::initClock()
{
    //Get NTP time
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
    delay(2000);
}

void SerialWiFiBridgeClass::printEspState()
{
    log_d("- WiFi: %s", WiFi.SSID().c_str());
    log_d("-  Mac: %s", WiFi.macAddress().c_str());
    log_d("-   IP: %s", WiFi.localIP().toString().c_str());
}

//for test
void SerialWiFiBridgeClass::printClock()
{
    time_t t = time(nullptr);
    struct tm *tm = localtime(&t);

    //_telnet0->printf("\n[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet1->printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet2->printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);

    _telnet1->printf("\r\n");
    _telnet2->printf("\r\n");

    log_d("HH:MM:SS = %02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void SerialWiFiBridgeClass::consoleHandle(TelnetSpy *telnet, HardwareSerial *serial, SimpleCLI *cli)
{
    // read from serial, send to telnet
    //(not use telnetSpy method)
    if (serial->available())
    {
        telnet->write(serial->read());
    }

    telnet->handle();

    // read from telnet, send to serial
    //(not use telnetSpy method)
    if (telnet->available())
    {
        String line = telnet->readStringUntil('\n');

        cli->parse(line); //include command execute
        telnet->write(COMMAND_PROMPT);
    }

    telnet->handle();
}

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

AsyncWebServer *SerialWiFiBridgeClass::getAsyncWebServerPtr()
{
    return _server;
}

void SerialWiFiBridgeClass::setup()
{
    initPort();
    initSerial();
    initTelnet();
    initConsole();
    initEEPROM();
    //initFS();
    initWiFi();
    initClock();
    initOTA();
    initWebServer();
}

void SerialWiFiBridgeClass::handle()
{
    ArduinoOTA.handle();
    //TODO: enable serial0
    //consoleHandle(_telnet0, _Serial0, &_cli0);
    consoleHandle(_telnet1, _Serial1, &_cli1);
    consoleHandle(_telnet2, _Serial2, &_cli2);

    yield();
}
