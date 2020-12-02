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

#ifndef __SerialWiFiBridgeApp_h
#define __SerialWiFiBridgeApp_h

//#include <memory>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <TelnetSpy.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <Ticker.h>
#include <HardwareSerial.h>
#include <SimpleCLI.h>

#define HOSTNAME "esp32"
#define MONITOR_SPEED 115200
#define AP_NAME "ESP32-G-AP"
#define AP_PASSWORD ""
#define PORTAL_TIMEOUT 180
#define COMMAND_PROMPT "~ esp$ "

//NOTE: The PIN assignment has changed and may not look straigt forward (other PINs are marke as Rx/Tx),
//but this assignment allows to flash via USB also with hooked MAX3232 serial drivers.
//https://github.com/AlphaLima/ESP32-Serial-Bridge.git
/*************************  COM Port 0 *******************************/
#define UART_BAUD0 115200        // Baudrate UART0
#define SERIAL_PARAM0 SERIAL_8N1 // Data/Parity/Stop UART0
#define SERIAL0_RXPIN 3          // receive Pin UART0
#define SERIAL0_TXPIN 1          // transmit Pin UART0
#define SERIAL0_TCP_PORT 55550   // Wifi Port UART0
/*************************  COM Port 1 *******************************/
#define UART_BAUD1 115200        // Baudrate UART1
#define SERIAL_PARAM1 SERIAL_8N1 // Data/Parity/Stop UART1
#define SERIAL1_RXPIN 16         // receive Pin UART1
#define SERIAL1_TXPIN 17         // transmit Pin UART1
#define SERIAL1_TCP_PORT 55551   // Wifi Port UART1
/*************************  COM Port 2 *******************************/
#define UART_BAUD2 115200        // Baudrate UART2
#define SERIAL_PARAM2 SERIAL_8N1 // Data/Parity/Stop UART2
#define SERIAL2_RXPIN 4          // receive Pin UART2
#define SERIAL2_TXPIN 2          // transmit Pin UART2
#define SERIAL2_TCP_PORT 55552   // Wifi Port UART2

//Message ID
typedef enum message_id
{
    MSG_COMMAND_RESET = 0,
    MSG_COMMAND_CLOCK,
    MSG_NOTHING = 0xFFFF
} MESSAGE_ID;

class SerialWiFiBridgeClass
{
private:
    //std::unique_ptr<DNSServer> _dns;
    //std::unique_ptr<AsyncWebServer> _server;
    DNSServer *_dns;
    AsyncWebServer *_server; 
    AsyncWiFiManager *_wifiManager;

    //TelnetSpy *_telnet0;
    TelnetSpy *_telnet1;
    TelnetSpy *_telnet2;

    //HardwareSerial *_Serial0;
    HardwareSerial *_Serial1;
    HardwareSerial *_Serial2;

    //SimpleCLI _cli0;
    SimpleCLI _cli1;
    SimpleCLI _cli2;

    //Command _command0;
    Command _command1;
    Command _command2;

    Ticker _clocker;

    static MESSAGE_ID _message_id;

    SerialWiFiBridgeClass()
    {   //new gcc...
        //_dns = std::make_unique<DNSServer>();
        //_server = std::make_unique<AsyncWebServer>(80);
        //_wifiManager = new AsyncWiFiManager(_server.get(), _dns.get());
        //old gcc...
        _dns = new DNSServer();
        _server = new AsyncWebServer(80);
        _wifiManager = new AsyncWiFiManager(_server, _dns);

        //_telnet0 = new TelnetSpy();
        _telnet1 = new TelnetSpy();
        _telnet2 = new TelnetSpy();

        //_Serial0 = new HardwareSerial(0);
        _Serial1 = new HardwareSerial(1);
        _Serial2 = new HardwareSerial(2);
    }

    ~SerialWiFiBridgeClass() = default;

    SerialWiFiBridgeClass(const SerialWiFiBridgeClass &);
    SerialWiFiBridgeClass &operator=(const SerialWiFiBridgeClass &);

    static void _telnetConnected();
    static void _telnetDisconnected();

    void printEspState();

public:
    static SerialWiFiBridgeClass &getInstance()
    {
        static SerialWiFiBridgeClass inst;
        return inst;
    }

    static void sendClockMessage();
    static String processor(const String &var);
    static void onRequest(AsyncWebServerRequest *request);
    static void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    static void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void commandErrorCallback(cmd_error *cmdError); //SimpleCLI
    static void commandCalllback(cmd *cmdline);

    virtual void initWiFi();
    virtual void initOTA();
    virtual void initSerial();
    virtual void initTelnet();
    virtual void initEEPROM();
    virtual void initServer();
    virtual void initConsole();
    virtual void initFS();
    virtual void initPort();
    virtual void initClock();
    virtual void printClock();

    virtual void messageHandle(MESSAGE_ID msg_id);
    virtual void consoleHandle(TelnetSpy *telnet, HardwareSerial *serial, SimpleCLI *cli);

    virtual void setup();
    virtual void handle();
};

#endif
