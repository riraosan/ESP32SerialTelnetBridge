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

#include <SPIFFS.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <TelnetSpy.h>
#include <EEPROM.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <Ticker.h>
#include <HardwareSerial.h>
#include <SimpleCLI.h>

class SerialTelnetBridgeClass
{
private:
    String _HOSTNAME;
    String _TARGET_HOSTNAME;
    String _AP_PASSWORD;

    String _welcome0;
    String _welcome1;
    String _welcome2;
    String _COMMAND_PROMPT;
    String _AP_NAME;

    const uint32_t _MONITOR_SPEED = 115200;
    const uint16_t _PORTAL_TIMEOUT = 180;

    /*************************  COM Port 0 *******************************/
    const uint32_t UART_BAUD0 = 115200;        // Baudrate UART0
    const uint32_t SERIAL_PARAM0 = SERIAL_8N1; // Data/Parity/Stop UART0
    const uint8_t SERIAL0_RXPIN = 3;           // receive Pin UART0
    const uint8_t SERIAL0_TXPIN = 1;           // transmit Pin UART0
    const uint16_t SERIAL0_TCP_PORT = 55550;   // Telnet Port UART0
    const size_t SERIAL0_BUFFER_SIZE = 1024;   // RX Buffer Size UART0
    /*************************  COM Port 1 *******************************/
    const uint32_t UART_BAUD1 = 115200;        // Baudrate UART1
    const uint32_t SERIAL_PARAM1 = SERIAL_8N1; // Data/Parity/Stop UART1
    const uint8_t SERIAL1_RXPIN = 16;          // receive Pin UART1
    const uint8_t SERIAL1_TXPIN = 17;          // transmit Pin UART1
    const uint16_t SERIAL1_TCP_PORT = 55551;   // Telnet Port UART1
    const size_t SERIAL1_BUFFER_SIZE = 1024;   // RX Buffer Size UART1
    /*************************  COM Port 2 *******************************/
    const uint32_t UART_BAUD2 = 115200;        // Baudrate UART2
    const uint32_t SERIAL_PARAM2 = SERIAL_8N1; // Data/Parity/Stop UART2
    const uint8_t SERIAL2_RXPIN = 4;           // receive Pin UART2
    const uint8_t SERIAL2_TXPIN = 2;           // transmit Pin UART2
    const uint16_t SERIAL2_TCP_PORT = 55552;   // Telnet Port UART2
    const size_t SERIAL2_BUFFER_SIZE = 1024;   // RX Buffer Size UART2

    DNSServer *_dns;
    AsyncWebServer *_server;
    AsyncWiFiManager *_wifiManager;

    TelnetSpy *_telnet0;
    TelnetSpy *_telnet1;
    TelnetSpy *_telnet2;

    HardwareSerial *_Serial0;
    HardwareSerial *_Serial1;
    HardwareSerial *_Serial2;

public:
    SerialTelnetBridgeClass(const SerialTelnetBridgeClass &);
    SerialTelnetBridgeClass &operator=(const SerialTelnetBridgeClass &);

    SerialTelnetBridgeClass()
    {
        _dns = new DNSServer();
        _server = new AsyncWebServer(80);
        _wifiManager = new AsyncWiFiManager(_server, _dns);

        _telnet0 = new TelnetSpy();
        _telnet1 = new TelnetSpy();
        _telnet2 = new TelnetSpy();

        _Serial0 = new HardwareSerial(0);
        _Serial1 = new HardwareSerial(1);
        _Serial2 = new HardwareSerial(2);

        _HOSTNAME = "esp32_001";
        _TARGET_HOSTNAME = "esp32_gw";
        _AP_PASSWORD = "1234";
        _COMMAND_PROMPT = "~esp$ ";
        _AP_NAME = "ESP-G-AP";
    }

    ~SerialTelnetBridgeClass() = default;

    SimpleCLI _cli0;
    SimpleCLI _cli1;
    SimpleCLI _cli2;

    Command _command0;
    Command _command1;
    Command _command2;

    static void telnet0Connected();
    static void telnet0Disconnected();
    static void telnet1Connected();
    static void telnet1Disconnected();
    static void telnet2Connected();
    static void telnet2Disconnected();

    static void printEspState();

    virtual bool begin();
    virtual void initWiFi();
    virtual void initOTA();
    virtual void initSerial();
    virtual void initTelnet();
    virtual void initConsole();
    virtual void initClock();
    virtual void printClock();

    void setHostname(String hostname);
    void setApPassword(String password);
    void setTargetHostname(String targetHostname);
    void setCommandPrompt(String prompt);

    //Message loop
    virtual void consoleHandle(TelnetSpy *telnet, HardwareSerial *serial, SimpleCLI *cli);
    virtual void handle();

    AsyncWebServer *getAsyncWebServerPtr();
};
