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

//static ENUM_MESSAGE_ID msg_id = ENUM_MESSAGE_ID::MSG_COMMAND_NOTHING;

void SerialTelnetBridgeClass::initPort()
{
    log_d("- Initializing Ports...");
    //TODO:Initializing Ports
}

void SerialTelnetBridgeClass::initEEPROM()
{
    log_d("- Initializing EEPROM...");
    //TODO:Initializing EEPROM
}

void SerialTelnetBridgeClass::initFS()
{
    log_d("- Mounting SPIFFS...");
    if (!SPIFFS.begin())
    {
        log_e("- An Error has occurred while mounting SPIFFS");
        return;
    }
}

void SerialTelnetBridgeClass::initWiFi()
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

void SerialTelnetBridgeClass::_telnetConnected()
{
    log_d("- Telnet connection established.");
    //TODO LED ON
}

void SerialTelnetBridgeClass::_telnetDisconnected()
{
    log_d("- Telnet connection closed.");
    //TODO LED OFF
}

void SerialTelnetBridgeClass::initSerial()
{
    log_d("- Initializing Serial...");

    //_Serial0->setDebugOutput(true);
    _Serial1->setDebugOutput(true);
    _Serial2->setDebugOutput(true);

    //set buffer size
    _Serial0->setRxBufferSize(SERIAL0_BUFFER_SIZE)
        _Serial1->setRxBufferSize(SERIAL1_BUFFER_SIZE);
    _Serial2->setRxBufferSize(SERIAL2_BUFFER_SIZE);

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

void SerialTelnetBridgeClass::initTelnet()
{
    log_d("- Initializing Telnet...");

    String prompt(_COMMAND_PROMPT);

    welcome0 += prompt;
    welcome1 += prompt;
    welcome2 += prompt;

    _telnet0->setWelcomeMsg((char *)_welcome0.c_str());
    _telnet0->setCallbackOnConnect(SerialTelnetBridgeClass::_telnetConnected);
    _telnet0->setCallbackOnDisconnect(SerialTelnetBridgeClass::_telnetDisconnected);
    _telnet0->setPort(SERIAL0_TCP_PORT);
    //to begin telnet only
    _telnet0->setSerial(nullptr);
    _telnet0->begin(UART_BAUD0);

    _telnet1->setWelcomeMsg((char *)_welcome1.c_str());
    _telnet1->setCallbackOnConnect(SerialTelnetBridgeClass::_telnetConnected);
    _telnet1->setCallbackOnDisconnect(SerialTelnetBridgeClass::_telnetDisconnected);
    _telnet1->setPort(SERIAL1_TCP_PORT);
    _telnet1->setBufferSize(SERIAL1_BUFFER_SIZE);
    //to begin telnet only
    _telnet1->setSerial(nullptr);
    _telnet1->begin(UART_BAUD1);

    _telnet2->setWelcomeMsg((char *)_welcome2.c_str());
    _telnet2->setCallbackOnConnect(SerialTelnetBridgeClass::_telnetConnected);
    _telnet2->setCallbackOnDisconnect(SerialTelnetBridgeClass::_telnetDisconnected);
    _telnet2->setPort(SERIAL2_TCP_PORT);
    _telnet2->setBufferSize(SERIAL2_BUFFER_SIZE);
    //to begin telnet only
    _telnet2->setSerial(nullptr);
    _telnet2->begin(UART_BAUD2);

    delay(500);
}

void SerialTelnetBridgeClass::initConsole()
{
}

void SerialTelnetBridgeClass::initOTA()
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


void SerialTelnetBridgeClass::initClock()
{
    //Get NTP time
    configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
    delay(2000);
}

void SerialTelnetBridgeClass::printEspState()
{
    log_d("- WiFi: %s", WiFi.SSID().c_str());
    log_d("-  Mac: %s", WiFi.macAddress().c_str());
    log_d("-   IP: %s", WiFi.localIP().toString().c_str());
}

//for test
void SerialTelnetBridgeClass::printClock()
{
    time_t t = time(nullptr);
    struct tm *tm = localtime(&t);

    _telnet0->printf("\n[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet1->printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet2->printf("\n[ %02d:%02d:%02d ]", tm->tm_hour, tm->tm_min, tm->tm_sec);

    _telnet1->printf("\r\n");
    _telnet2->printf("\r\n");

    log_d("HH:MM:SS = %02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void SerialTelnetBridgeClass::consoleHandle(TelnetSpy *telnet, HardwareSerial *serial, SimpleCLI *cli)
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

AsyncWebServer *SerialTelnetBridgeClass::getAsyncWebServerPtr()
{
    return _server;
}

void SerialTelnetBridgeClass::setup()
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

void SerialTelnetBridgeClass::handle()
{
    ArduinoOTA.handle();

    consoleHandle(_telnet0, _Serial0, &_cli0);
    consoleHandle(_telnet1, _Serial1, &_cli1);
    consoleHandle(_telnet2, _Serial2, &_cli2);

    yield();
}
