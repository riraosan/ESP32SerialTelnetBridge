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
#include <SerialTelnetBridge.h>

/*
    log_e("")：エラー
    log_w("")：警告
    log_i("")：情報
    log_d("")：デバッグ
    log_v("")：詳細
    log_d("")：通常
*/

SerialTelnetBridgeClass::SerialTelnetBridgeClass()
{
    _dns = new DNSServer();
    _server = new AsyncWebServer(80);
    _WiFiManager = new AsyncWiFiManager(_server, _dns);

    _telnet0 = new TelnetSpy();
    _telnet1 = new TelnetSpy();
    _telnet2 = new TelnetSpy();

    _Serial0 = new HardwareSerial(0);
    _Serial1 = new HardwareSerial(1);
    _Serial2 = new HardwareSerial(2);

    _HOSTNAME = "esp32_001";
    _TARGET_HOSTNAME = "esp32_gw";
    _AP_PASSWORD = "";
    _COMMAND_PROMPT = "~esp$ ";
    _AP_NAME = "ESP-G-AP";
    _PORTAL_TIMEOUT = 180;

    initSerialPorts();
}

void SerialTelnetBridgeClass::initSerialPorts()
{
    _port0.SERIAL_RXPIN = 3;
    _port0.SERIAL_TXPIN = 1;
    _port0.SERIAL_TCP_PORT = 55550;

    _port1.SERIAL_RXPIN = 16;
    _port1.SERIAL_TXPIN = 17;
    _port1.SERIAL_TCP_PORT = 55551;

    _port2.SERIAL_RXPIN = 4;
    _port2.SERIAL_TXPIN = 2;
    _port2.SERIAL_TCP_PORT = 55552;
}

void SerialTelnetBridgeClass::setSerialPort0(SerialSettings &port0)
{
    _port0 = port0;
}

void SerialTelnetBridgeClass::setSerialPort1(SerialSettings &port1)
{
    _port1 = port1;
}

void SerialTelnetBridgeClass::setSerialPort2(SerialSettings &port2)
{
    _port2 = port2;
}

void SerialTelnetBridgeClass::getSerialPort0(SerialSettings &port0)
{
    port0 = _port0;
}

void SerialTelnetBridgeClass::getSerialPort1(SerialSettings &port1)
{
    port1 = _port1;
}

void SerialTelnetBridgeClass::getSerialPort2(SerialSettings &port2)
{
    port2 = _port2;
}

void SerialTelnetBridgeClass::initWiFi()
{
    log_d("- Initializing Connecting to WiFi AP...");

    _WiFiManager->setDebugOutput(false);
    _WiFiManager->setConfigPortalTimeout(_PORTAL_TIMEOUT);

    _WiFiManager->setAPCallback([](AsyncWiFiManager *myWiFiManager) {
        log_d("- No known WiFi found");
        log_d("- Starting AP: ");
        log_d("%s", myWiFiManager->getConfigPortalSSID().c_str());
        log_d("%s", WiFi.softAPIP().toString().c_str());
    });

    if (_AP_PASSWORD.isEmpty())
    {
        if (!_WiFiManager->autoConnect(_AP_NAME.c_str()))
        {
            log_e("- Failed to connect and hit timeout");
            ESP.restart();
            delay(1000);
        }
    }
    else
    {
        if (!_WiFiManager->autoConnect(_AP_NAME.c_str(), _AP_PASSWORD.c_str()))
        {
            log_e("- Failed to connect and hit timeout");
            ESP.restart();
            delay(1000);
        }
    }

    log_d("- Success to connect AP!!");
    log_d("- address to _dns = 0x%x", _dns);
    log_d("- address to _server = 0x%x", _server);

    printEspState();

    log_i("- WiFi Started.");
}

void SerialTelnetBridgeClass::telnet0Connected()
{
    log_d("- Telnet0 connection established.");
    //TODO LED0 ON
}

void SerialTelnetBridgeClass::telnet0Disconnected()
{
    log_d("- Telnet0 connection closed.");
    //TODO LED0 OFF
}

void SerialTelnetBridgeClass::telnet1Connected()
{
    log_d("- Telnet1 connection established.");
    //TODO LED1 ON
}

void SerialTelnetBridgeClass::telnet1Disconnected()
{
    log_d("- Telnet1 connection closed.");
    //TODO LED1 OFF
}

void SerialTelnetBridgeClass::telnet2Connected()
{
    log_d("- Telnet2 connection established.");
    //TODO LED2 ON
}

void SerialTelnetBridgeClass::telnet2Disconnected()
{
    log_d("- Telnet2 connection closed.");
    //TODO LED2 OFF
}

void SerialTelnetBridgeClass::initSerial()
{
    log_d("- Initializing Serial...");

    _Serial0->setDebugOutput(false);
    _Serial1->setDebugOutput(false);
    _Serial2->setDebugOutput(false);

    //set buffer size
    _Serial0->setRxBufferSize(_port0.SERIAL_BUFFER_SIZE);
    _Serial1->setRxBufferSize(_port1.SERIAL_BUFFER_SIZE);
    _Serial2->setRxBufferSize(_port2.SERIAL_BUFFER_SIZE);

    _Serial0->begin(_port0.UART_BAUD, _port0.SERIAL_PARAM, _port0.SERIAL_RXPIN, _port0.SERIAL_TXPIN);
    _Serial1->begin(_port1.UART_BAUD, _port1.SERIAL_PARAM, _port1.SERIAL_RXPIN, _port1.SERIAL_TXPIN);
    _Serial2->begin(_port2.UART_BAUD, _port2.SERIAL_PARAM, _port2.SERIAL_RXPIN, _port2.SERIAL_TXPIN);

    _Serial0->println("Serial0 Txd is on pin: " + String(_port0.SERIAL_TXPIN));
    _Serial0->println("Serial0 Rxd is on pin: " + String(_port0.SERIAL_RXPIN));

    _Serial1->println("Serial1 Txd is on pin: " + String(_port1.SERIAL_TXPIN));
    _Serial1->println("Serial1 Rxd is on pin: " + String(_port1.SERIAL_RXPIN));

    _Serial2->println("Serial2 Txd is on pin: " + String(_port2.SERIAL_TXPIN));
    _Serial2->println("Serial2 Rxd is on pin: " + String(_port2.SERIAL_RXPIN));

    _Serial0->flush();
    _Serial1->flush();
    _Serial2->flush();
}

void SerialTelnetBridgeClass::initTelnet()
{
    log_d("- Initializing Telnet...");

    String prompt(_COMMAND_PROMPT);

    _welcome0 += prompt;
    _welcome1 += prompt;
    _welcome2 += prompt;

    _telnet0->setWelcomeMsg((char *)_welcome0.c_str());
    _telnet0->setCallbackOnConnect(SerialTelnetBridgeClass::telnet0Connected);
    _telnet0->setCallbackOnDisconnect(SerialTelnetBridgeClass::telnet0Disconnected);
    _telnet0->setPort(_port0.SERIAL_TCP_PORT);
    _telnet0->setBufferSize(_port0.SERIAL_BUFFER_SIZE);
    //to begin telnet only
    _telnet0->setSerial(nullptr);
    _telnet0->begin(_port0.UART_BAUD);

    _telnet1->setWelcomeMsg((char *)_welcome1.c_str());
    _telnet1->setCallbackOnConnect(SerialTelnetBridgeClass::telnet1Connected);
    _telnet1->setCallbackOnDisconnect(SerialTelnetBridgeClass::telnet1Disconnected);
    _telnet1->setPort(_port1.SERIAL_TCP_PORT);
    _telnet1->setBufferSize(_port1.SERIAL_BUFFER_SIZE);
    //to begin telnet only
    _telnet1->setSerial(nullptr);
    _telnet1->begin(_port1.UART_BAUD);

    _telnet2->setWelcomeMsg((char *)_welcome2.c_str());
    _telnet2->setCallbackOnConnect(SerialTelnetBridgeClass::telnet2Connected);
    _telnet2->setCallbackOnDisconnect(SerialTelnetBridgeClass::telnet2Disconnected);
    _telnet2->setPort(_port2.SERIAL_TCP_PORT);
    _telnet2->setBufferSize(_port2.SERIAL_BUFFER_SIZE);
    //to begin telnet only
    _telnet2->setSerial(nullptr);
    _telnet2->begin(_port2.UART_BAUD);

    delay(500);
}

void SerialTelnetBridgeClass::initConsole()
{
    log_d("- Initializing Console...");
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

    ArduinoOTA.setHostname(_HOSTNAME.c_str());

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
    _telnet1->printf("\n[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _telnet2->printf("\n[ %02d:%02d:%02d ]\n", tm->tm_hour, tm->tm_min, tm->tm_sec);

    log_i("HH:MM:SS = %02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
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
        telnet->write(_COMMAND_PROMPT.c_str());
    }

    telnet->handle();
}

AsyncWebServer *SerialTelnetBridgeClass::getAsyncWebServerPtr()
{
    if (_server)
    {
        return _server;
    }

    return nullptr;
}

void SerialTelnetBridgeClass::setHostname(String hostname)
{
    _HOSTNAME = hostname;
}

void SerialTelnetBridgeClass::setApPassword(String password)
{
    _AP_PASSWORD = password;
}

void SerialTelnetBridgeClass::setTargetHostname(String targetHostname)
{
    _TARGET_HOSTNAME = targetHostname;
}

void SerialTelnetBridgeClass::setCommandPrompt(String prompt)
{
    _COMMAND_PROMPT = prompt;
}

void SerialTelnetBridgeClass::setApName(String apName)
{
    _AP_NAME = apName;
}

bool SerialTelnetBridgeClass::begin()
{
    initWiFi();
    initSerial();
    initTelnet();
    initConsole();
    initOTA();

    return true;
}

void SerialTelnetBridgeClass::handle()
{
    ArduinoOTA.handle();

    consoleHandle(_telnet0, _Serial0, &_cli0);
    consoleHandle(_telnet1, _Serial1, &_cli1);
    consoleHandle(_telnet2, _Serial2, &_cli2);

    yield();
}
