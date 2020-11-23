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
#include <esp32-hal-log.h>
#include "SerialWiFiBridgeApp.h"

MESSAGE_ID SerialWiFiBridgeClass::_message_id = MSG_NOTHING;

/*
    log_e("")
    log_w("")
    log_i("")
    log_d("")
    log_v("")
*/

void SerialWiFiBridgeClass::initPort()
{
    log_n("Initializing Ports...");
    //TODO:Initializing Ports
}

void SerialWiFiBridgeClass::initEEPROM()
{
    log_n("Initializing EEPROM...");
    //TODO:Initializing EEPROM
}

void SerialWiFiBridgeClass::initFS()
{
    log_n("Mounting SPIFFS...");
    if (!SPIFFS.begin())
    {
        log_e("An Error has occurred while mounting SPIFFS");
        return;
    }
}

void SerialWiFiBridgeClass::initWiFi()
{
    _wifiManager->setDebugOutput(false);
    _wifiManager->setConfigPortalTimeout(PORTAL_TIMEOUT);
    _wifiManager->setAPCallback([](AsyncWiFiManager *myWiFiManager) {
        log_n("- No known wifi found");
        log_n("- Starting AP: ");
        log_n("%s \n", myWiFiManager->getConfigPortalSSID().c_str());
        log_n("%s \n", WiFi.softAPIP().toString().c_str());
    });

    // enable autoconnect
    if (!(String(AP_PASSWORD).isEmpty() ? _wifiManager->autoConnect(AP_NAME) : _wifiManager->autoConnect(AP_NAME, AP_PASSWORD)))
    {
        log_n("- Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    }
    else
    {
        log_n("- WiFi: %s", WiFi.SSID().c_str());
        log_n("-  Mac: %s", WiFi.macAddress().c_str());
        log_n("-   IP: %s", WiFi.localIP().toString().c_str());
    }

    log_n("- WiFi Started");
}

void SerialWiFiBridgeClass::_telnetConnected()
{
    log_n("- Telnet connection established.");
    //TODO LED ON
}

void SerialWiFiBridgeClass::_telnetDisconnected()
{
    log_n("- Telnet connection closed.");
    //TODO LED OFF
}

void SerialWiFiBridgeClass::initSerial()
{
    log_n("- Initializing Serial...");

    _Serial0->setDebugOutput(true);
    _Serial1->setDebugOutput(true);
    _Serial2->setDebugOutput(true);

    _Serial0->begin(UART_BAUD0, SERIAL_8N1, SERIAL0_RXPIN, SERIAL0_TXPIN);
    _Serial1->begin(UART_BAUD1, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
    _Serial2->begin(UART_BAUD2, SERIAL_8N1, SERIAL2_RXPIN, SERIAL2_TXPIN);

    _Serial0->println("Serial0 Txd is on pin: " + String(SERIAL0_TXPIN));
    _Serial0->println("Serial0 Rxd is on pin: " + String(SERIAL0_RXPIN));

    _Serial1->println("Serial1 Txd is on pin: " + String(SERIAL1_TXPIN));
    _Serial1->println("Serial1 Rxd is on pin: " + String(SERIAL1_RXPIN));

    _Serial2->println("Serial2 Txd is on pin: " + String(SERIAL2_TXPIN));
    _Serial2->println("Serial2 Rxd is on pin: " + String(SERIAL2_RXPIN));

    _Serial0->flush();
    _Serial1->flush();
    _Serial2->flush();

    delay(500);
}

void SerialWiFiBridgeClass::initTelnet()
{
    //TODO to gain buffer size
    log_n("- Initializing Telnet...");

    String welcome0("\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial0 <-> esp32.local:55550)\n");
    String welcome1("\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial1 <-> esp32.local:55551)\n");
    String welcome2("\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial2 <-> esp32.local:55552)\n");

    String prompt(COMMAND_PROMPT);

    welcome0 += prompt;
    welcome1 += prompt;
    welcome2 += prompt;

    _telnet0->setWelcomeMsg((char *)welcome0.c_str());
    _telnet0->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet0->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet0->setPort(SERIAL0_TCP_PORT);
    //to begin telnet only
    _telnet0->setSerial(nullptr);
    _telnet0->begin(UART_BAUD0);

    _telnet1->setWelcomeMsg((char *)welcome1.c_str());
    _telnet1->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet1->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet1->setPort(SERIAL1_TCP_PORT);
    _telnet1->set
    //to begin telnet only
    _telnet1->setSerial(nullptr);
    _telnet1->begin(UART_BAUD1);

    _telnet2->setWelcomeMsg((char *)welcome2.c_str());
    _telnet2->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet2->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet2->setPort(SERIAL2_TCP_PORT);
    //to begin telnet only
    _telnet2->setSerial(nullptr);
    _telnet2->begin(UART_BAUD2);

    delay(500);
}

void SerialWiFiBridgeClass::commandErrorCallback(cmd_error *cmdError)
{
    CommandError commandError(cmdError); // Create wrapper object

    log_e("ERROR: %s", commandError.toString().c_str());
}

void SerialWiFiBridgeClass::commandCalllback(cmd *cmdline)
{
    Command command(cmdline); // Create wrapper object

    log_n("cmmand line = %s", command.toString());

    // Get first (and only) Argument
    Argument arg = command.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();

    if (arg.getValue() == "clock")
    {
        SerialWiFiBridgeClass::_message_id = MSG_COMMAND_CLOCK;
    }
    else if (arg.getValue() == "reset")
    {
        SerialWiFiBridgeClass::_message_id = MSG_COMMAND_RESET;
    }
}

void SerialWiFiBridgeClass::initConsole()
{
    _cli0.setOnError(SerialWiFiBridgeClass::commandErrorCallback);
    _cli1.setOnError(SerialWiFiBridgeClass::commandErrorCallback);
    _cli2.setOnError(SerialWiFiBridgeClass::commandErrorCallback);

    _command0 = _cli0.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllback);
    _command1 = _cli1.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllback);
    _command2 = _cli2.addSingleArgCmd("esp", SerialWiFiBridgeClass::commandCalllback);
}

void SerialWiFiBridgeClass::initOTA()
{
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else
            type = "filesystem";

        log_n("Start updating %s", type.c_str());
    });

    ArduinoOTA.onEnd([]() {
        log_n("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        log_n("Progress: %u%%\r", (progress / (total / 100)));
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

    log_n("- Hostname: ");
    log_n("%s.local", ArduinoOTA.getHostname().c_str());

    ArduinoOTA.begin();
    log_n("- OTA Started");
}

void SerialWiFiBridgeClass::onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    ;
}

void SerialWiFiBridgeClass::initServer()
{
    _server->on(
        "/", HTTP_POST, [](AsyncWebServerRequest *request) {
            log_n("[HTTP_POST] /");
            request->send(200);
        },
        nullptr, onBody);

    //TODO add REST API
    //REST API exsample
    _server->on("/test1/do.test1", HTTP_GET, [](AsyncWebServerRequest *request) {
        log_n("/test1/do.test1");
        request->send(200);
    });

    _server->on("/test2/do.test2", HTTP_GET, [](AsyncWebServerRequest *request) {
        log_n("/test2/do.test2");
        request->send(200);
    });
}

void SerialWiFiBridgeClass::sendClockMessage()
{
    SerialWiFiBridgeClass::_message_id = MSG_COMMAND_CLOCK;
}

void SerialWiFiBridgeClass::initClock()
{
    //Get NTP time
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
    delay(2000);

    //_clocker.attach(1, SerialWiFiBridgeClass::sendClockMessage);
}

void SerialWiFiBridgeClass::setup()
{
    initPort();
    initConsole();
    initSerial();
    initTelnet();
    initEEPROM();
    initServer();
    initFS();
    initWiFi();
    initOTA();
    initClock();
    _Serial0->println("Serial0 setup() end.");
    _Serial1->println("Serial1 setup() end.");
    _Serial2->println("Serial2 setup() end.");
}

//for test
void SerialWiFiBridgeClass::printClock()
{
    time_t t = time(nullptr);
    struct tm *tm = localtime(&t);

    _telnet0->printf("\n[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet1->printf("\n[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet2->printf("\n[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);

    log_d("HH:MM:SS = %02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void SerialWiFiBridgeClass::_serialHandle(TelnetSpy *telnet, HardwareSerial *serial, SimpleCLI *cli)
{
    // read from serial, send to telnet
    if (serial->available())
    {
        telnet->write(serial->read());
    }

    // read from telnet, send to serial
    if (telnet->available())
    {
        String line = telnet->readStringUntil('\n');
        line += "\n"; //add Line Feed
        serial->write(line.c_str());

        cli->parse(line); //include command execute

        telnet->write(COMMAND_PROMPT);
    }

    telnet->handle();
}

void SerialWiFiBridgeClass::messageHandle(MESSAGE_ID msg_id)
{
    switch (msg_id)
    {
    case MSG_COMMAND_CLOCK:
        printClock();
        break;
    case MSG_COMMAND_RESET:
        ESP.restart();
        delay(2000);
        break;
    default:;
    }

    SerialWiFiBridgeClass::_message_id = MSG_NOTHING;
}

void SerialWiFiBridgeClass::handle()
{
    ArduinoOTA.handle();

    _serialHandle(_telnet0, _Serial0, &_cli0);
    _serialHandle(_telnet1, _Serial1, &_cli1);
    _serialHandle(_telnet2, _Serial2, &_cli2);

    messageHandle(SerialWiFiBridgeClass::_message_id);

    yield();
}
