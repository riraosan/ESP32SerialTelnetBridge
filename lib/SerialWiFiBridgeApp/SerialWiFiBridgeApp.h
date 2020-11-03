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

#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <TelnetSpy.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <Ticker.h>

#define HOSTNAME "esp32"
#define MONITOR_SPEED 115200
#define AP_NAME "ESP32-G-AP"
#define AP_PASSWORD ""
#define PORTAL_TIMEOUT 180

/*************************  COM Port 0 *******************************/
#define UART_BAUD0 115200        // Baudrate UART0
#define SERIAL_PARAM0 SERIAL_8N1 // Data/Parity/Stop UART0
#define SERIAL0_RXPIN 3          // receive Pin UART0
#define SERIAL0_TXPIN 1          // transmit Pin UART0
#define SERIAL0_TCP_PORT 8880    // Wifi Port UART0
/*************************  COM Port 1 *******************************/
#define UART_BAUD1 115200        // Baudrate UART1
#define SERIAL_PARAM1 SERIAL_8N1 // Data/Parity/Stop UART1
#define SERIAL1_RXPIN 15         // receive Pin UART1
#define SERIAL1_TXPIN 2          // transmit Pin UART1
#define SERIAL1_TCP_PORT 8881    // Wifi Port UART1
/*************************  COM Port 2 *******************************/
#define UART_BAUD2 115200        // Baudrate UART2
#define SERIAL_PARAM2 SERIAL_8N1 // Data/Parity/Stop UART2
#define SERIAL2_RXPIN 16         // receive Pin UART2
#define SERIAL2_TXPIN 17         // transmit Pin UART2
#define SERIAL2_TCP_PORT 8882    // Wifi Port UART2

/* Baud-rates available: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200, 256000, 512000, 962100
 *  
 *  Protocols available:
 * SERIAL_5N1   5-bit No parity 1 stop bit
 * SERIAL_6N1   6-bit No parity 1 stop bit
 * SERIAL_7N1   7-bit No parity 1 stop bit
 * SERIAL_8N1 (the default) 8-bit No parity 1 stop bit
 * SERIAL_5N2   5-bit No parity 2 stop bits 
 * SERIAL_6N2   6-bit No parity 2 stop bits
 * SERIAL_7N2   7-bit No parity 2 stop bits
 * SERIAL_8N2   8-bit No parity 2 stop bits 
 * SERIAL_5E1   5-bit Even parity 1 stop bit
 * SERIAL_6E1   6-bit Even parity 1 stop bit
 * SERIAL_7E1   7-bit Even parity 1 stop bit 
 * SERIAL_8E1   8-bit Even parity 1 stop bit 
 * SERIAL_5E2   5-bit Even parity 2 stop bit 
 * SERIAL_6E2   6-bit Even parity 2 stop bit 
 * SERIAL_7E2   7-bit Even parity 2 stop bit  
 * SERIAL_8E2   8-bit Even parity 2 stop bit  
 * SERIAL_5O1   5-bit Odd  parity 1 stop bit  
 * SERIAL_6O1   6-bit Odd  parity 1 stop bit   
 * SERIAL_7O1   7-bit Odd  parity 1 stop bit  
 * SERIAL_8O1   8-bit Odd  parity 1 stop bit   
 * SERIAL_5O2   5-bit Odd  parity 2 stop bit   
 * SERIAL_6O2   6-bit Odd  parity 2 stop bit    
 * SERIAL_7O2   7-bit Odd  parity 2 stop bit    
 * SERIAL_8O2   8-bit Odd  parity 2 stop bit    
*/

typedef enum message_id
{
    MSG_COMMAND_RESET = 0,
    MSG_COMMAND_CLOCK,
    MSG_NOTHING = 0xFFFF
} MESSAGE_ID;

class SerialWiFiBridgeClass
{
private:

    DNSServer *_dns;
    AsyncWebServer *_server;
    AsyncWiFiManager *_wifiManager;
    TelnetSpy *_telnet0;
    TelnetSpy *_telnet1;
    TelnetSpy *_telnet2;
    Ticker clocker;

    static MESSAGE_ID _message_id;

    SerialWiFiBridgeClass()
    {
        _dns = new DNSServer();
        _server = new AsyncWebServer(80);
        _wifiManager = new AsyncWiFiManager(_server, _dns);
        _telnet0 = new TelnetSpy();
        _telnet1 = new TelnetSpy();
        _telnet2 = new TelnetSpy();
    }

    SerialWiFiBridgeClass(const SerialWiFiBridgeClass &);
    SerialWiFiBridgeClass &operator=(const SerialWiFiBridgeClass &);
    ~SerialWiFiBridgeClass()
    {
        delete _wifiManager;
        delete _dns;
        delete _server;
        delete _telnet0;
        delete _telnet1;
        delete _telnet2;
    }

    static void telnetConnected();
    static void telnetDisconnected();
    void _initWiFi();
    void _initOTA();
    void _initTelnet();
    void _initFS();
    void _initPort();
    void _initEEPROM();
    void _initServer();
    void _initClock();    
    void _printClock();

    static String processor(const String &var);
    static void onRequest(AsyncWebServerRequest *request);
    static void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    static void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void message_handle(MESSAGE_ID msg_id);

public:
    static SerialWiFiBridgeClass &getInstance()
    {
        static SerialWiFiBridgeClass inst;
        return inst;
    }

    void setup();
    void handle();
    static void printClock();

};

#endif
