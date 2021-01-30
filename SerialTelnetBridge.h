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

#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <TelnetSpy.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <HardwareSerial.h>
#include <SimpleCLI.h>

class SerialSettings
{
public:
    uint32_t UART_BAUD;        // Baudrate UART
    uint32_t SERIAL_PARAM;     // Data/Parity/Stop UART
    uint8_t SERIAL_RXPIN;      // receive Pin UART
    uint8_t SERIAL_TXPIN;      // transmit Pin UART
    uint16_t SERIAL_TCP_PORT;  // Telnet Port UART
    size_t SERIAL_BUFFER_SIZE; // RX Buffer Size UART

    SerialSettings()
    {
        UART_BAUD = 115200;
        SERIAL_PARAM = SERIAL_8N1;
        SERIAL_RXPIN = 0;
        SERIAL_TXPIN = 0;
        SERIAL_TCP_PORT = 0;
        SERIAL_BUFFER_SIZE = 1024;
    }
};

class SerialTelnetBridgeClass
{
public:
    SerialTelnetBridgeClass();
    ~SerialTelnetBridgeClass() = default;

    SerialTelnetBridgeClass(const SerialTelnetBridgeClass &);
    SerialTelnetBridgeClass &operator=(const SerialTelnetBridgeClass &);

    //TODO　構造体の変数を変更するメソッドを作成する。

    virtual void setSerialPort0(SerialSettings &port0);
    virtual void setSerialPort1(SerialSettings &port1);
    virtual void setSerialPort2(SerialSettings &port2);

    virtual void getSerialPort0(SerialSettings &port0);
    virtual void getSerialPort1(SerialSettings &port1);
    virtual void getSerialPort2(SerialSettings &port2);

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
    virtual void initSerialPorts();

    void setHostname(String hostname);
    void setApName(String apName);
    void setApPassword(String password);
    void setTargetHostname(String targetHostname);
    void setCommandPrompt(String prompt);

    //Message loop
    virtual void consoleHandle(TelnetSpy *telnet, HardwareSerial *serial, SimpleCLI *cli);
    virtual void handle();

    AsyncWebServer *getAsyncWebServerPtr();

protected:
    SimpleCLI _cli0;
    SimpleCLI _cli1;
    SimpleCLI _cli2;

    Command _command0;
    Command _command1;
    Command _command2;

private:
    String _HOSTNAME;
    String _TARGET_HOSTNAME;
    String _AP_PASSWORD;

    String _welcome0;
    String _welcome1;
    String _welcome2;
    String _COMMAND_PROMPT;
    String _AP_NAME;

    uint16_t _PORTAL_TIMEOUT;

    DNSServer *_dns;
    AsyncWebServer *_server;
    AsyncWiFiManager *_WiFiManager;

    TelnetSpy *_telnet0;
    TelnetSpy *_telnet1;
    TelnetSpy *_telnet2;

    HardwareSerial *_Serial0;
    HardwareSerial *_Serial1;
    HardwareSerial *_Serial2;

    SerialSettings _port0;
    SerialSettings _port1;
    SerialSettings _port2;
};
