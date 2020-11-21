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
    _Serial0->println("Initializing Ports...");
    //TODO:Initializing Ports
}

void SerialWiFiBridgeClass::initEEPROM()
{
    _Serial0->println("Initializing EEPROM...");
    //TODO:Initializing EEPROM
}

void SerialWiFiBridgeClass::initFS()
{
    _Serial0->println("Mounting SPIFFS...");
    if (!SPIFFS.begin())
    {
        _Serial0->println("An Error has occurred while mounting SPIFFS");
        return;
    }
}

void SerialWiFiBridgeClass::initWiFi()
{
    _wifiManager->setDebugOutput(false);
    _wifiManager->setConfigPortalTimeout(PORTAL_TIMEOUT);
    _wifiManager->setAPCallback([](AsyncWiFiManager *myWiFiManager) {
        //_Serial0->println("- No known wifi found");
        //_Serial0->print("- Starting AP: ");
        //_Serial0->println(myWiFiManager->getConfigPortalSSID());
        //_Serial0->println(WiFi.softAPIP());
    });

    // enable autoconnect
    if (!(String(AP_PASSWORD).isEmpty() ? _wifiManager->autoConnect(AP_NAME) : _wifiManager->autoConnect(AP_NAME, AP_PASSWORD)))
    {
        _Serial0->println("- Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    }
    else
    {
        _Serial0->println("- WiFi: " + WiFi.SSID());
        _Serial0->println("-  Mac: " + WiFi.macAddress());
        _Serial0->println("-   IP: " + WiFi.localIP().toString());
    }

    _Serial0->println("- WiFi Started");
}

void SerialWiFiBridgeClass::_telnetConnected()
{
    //_Serial0->println("- Telnet connection established.");
}

void SerialWiFiBridgeClass::_telnetDisconnected()
{
    //_Serial0->println("- Telnet connection closed.");
}

void SerialWiFiBridgeClass::initSerial()
{
    _Serial0->println("- Initializing Serial...");

    _Serial0->setDebugOutput(true);
    _Serial1->setDebugOutput(true);
    _Serial2->setDebugOutput(true);

    _Serial0->begin(UART_BAUD0, SERIAL_8N1, SERIAL0_RXPIN, SERIAL0_TXPIN);
    _Serial1->begin(UART_BAUD1, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
    _Serial2->begin(UART_BAUD2, SERIAL_8N1, SERIAL2_RXPIN, SERIAL2_TXPIN);

    _Serial0->println("Serial0 Txd is on pin: " + String(SERIAL0_TXPIN));
    _Serial0->println("Serial0 Rxd is on pin: " + String(SERIAL0_RXPIN));

    _Serial0->println("Serial1 Txd is on pin: " + String(SERIAL1_TXPIN));
    _Serial0->println("Serial1 Rxd is on pin: " + String(SERIAL1_RXPIN));

    _Serial0->println("Serial2 Txd is on pin: " + String(SERIAL2_TXPIN));
    _Serial0->println("Serial2 Rxd is on pin: " + String(SERIAL2_RXPIN));

    _Serial0->flush();
    _Serial1->flush();
    _Serial2->flush();

    delay(500);
}

void SerialWiFiBridgeClass::initTelnet()
{
    _Serial0->println("- Initializing Telnet...");

    char *welcome0 = "\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial0 <-> esp32.local:55550)\n";
    char *welcome1 = "\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial1 <-> esp32.local:55551)\n";
    char *welcome2 = "\n\nWelcome to ESP32 Serial WiFi Bridge Terminal. \n(Serial2 <-> esp32.local:55552)\n";

    _telnet0->setWelcomeMsg(welcome0);
    _telnet0->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet0->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet0->setPort(SERIAL0_TCP_PORT);
    //to begin telnet only
    _telnet0->setSerial(NULL);
    _telnet0->begin(UART_BAUD0);

    _telnet1->setWelcomeMsg(welcome1);
    _telnet1->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet1->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet1->setPort(SERIAL1_TCP_PORT);
    //to begin telnet only
    _telnet1->setSerial(NULL);
    _telnet1->begin(UART_BAUD1);

    _telnet2->setWelcomeMsg(welcome2);
    _telnet2->setCallbackOnConnect(SerialWiFiBridgeClass::_telnetConnected);
    _telnet2->setCallbackOnDisconnect(SerialWiFiBridgeClass::_telnetDisconnected);
    _telnet2->setPort(SERIAL2_TCP_PORT);
    //to begin telnet only
    _telnet2->setSerial(NULL);
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
    Command cmd(cmdline); // Create wrapper object

    // Get first (and only) Argument
    Argument arg = cmd.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();

    if (argVal == "on")
    {
        SerialWiFiBridgeClass::_message_id = MSG_COMMAND_CLOCK;
    }
}

void SerialWiFiBridgeClass::initConsole()
{
    _cli1.setOnError(SerialWiFiBridgeClass::commandErrorCallback);
    _command = _cli1.addSingleArgCmd("clock", commandCalllback);
}

void SerialWiFiBridgeClass::initOTA()
{
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else
            type = "filesystem";

        //_Serial0->println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        //_Serial0->println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        //_Serial0->printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        //_Serial0->printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            //_Serial0->println("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            //_Serial0->println("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            //_Serial0->println("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            //_Serial0->println("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            //_Serial0->println("End Failed");
        }
    });

    ArduinoOTA.setHostname(HOSTNAME);

    _Serial0->print("- Hostname: ");
    _Serial0->println(ArduinoOTA.getHostname() + ".local");

    ArduinoOTA.begin();
    _Serial0->println("- OTA Started");
}

void SerialWiFiBridgeClass::onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    //_Serial0->println("onBody()");
}

void SerialWiFiBridgeClass::initServer()
{
    _server->on(
        "/", HTTP_POST, [](AsyncWebServerRequest *request) {
            //_Serial0->println("[HTTP_POST] /");
            request->send(200);
        },
        NULL, onBody);

    //REST API exsample
    _server->on("/A", HTTP_GET, [](AsyncWebServerRequest *request) {
        //_Serial0->println("/A");
    });

    _server->on("/B", HTTP_GET, [](AsyncWebServerRequest *request) {
        //_Serial0->println("/B");
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

    _clocker.attach(1, SerialWiFiBridgeClass::sendClockMessage);
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
    //initClock();
    _Serial0->println("Serial0 setup() end.");
    _Serial1->println("Serial1 setup() end.");
    _Serial2->println("Serial2 setup() end.");
}

//for test
void SerialWiFiBridgeClass::printClock()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    _telnet0->printf("[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet1->printf("[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet2->printf("[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);

    //log_i("HH:MM:SS = %02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
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

void SerialWiFiBridgeClass::_serialHandle(TelnetSpy *telnet, HardwareSerial *serial)
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

        telnet->write(COMMAND_PROMPT);
        _cli1.parse(line);
    }

    telnet->handle();
}

void SerialWiFiBridgeClass::handle()
{
    ArduinoOTA.handle();

    _serialHandle(_telnet0, _Serial0);
    _serialHandle(_telnet1, _Serial1);
    _serialHandle(_telnet2, _Serial2);

    messageHandle(SerialWiFiBridgeClass::_message_id);

    yield();
}
