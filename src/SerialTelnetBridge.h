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

#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <AsyncJson.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <HardwareSerial.h>
#include <TelnetSpy.h>
//#include <LinenoiseBitlash.h>
#include <Ticker.h>

class SerialSettings {
   public:
    uint32_t UART_BAUD;         // Baudrate UART
    uint32_t SERIAL_PARAM;      // Data/Parity/Stop UART
    uint8_t SERIAL_RXPIN;       // receive Pin UART
    uint8_t SERIAL_TXPIN;       // transmit Pin UART
    uint16_t SERIAL_TCP_PORT;   // Telnet Port UART
    size_t SERIAL_BUFFER_SIZE;  // RX Buffer Size UART
    String welcomeMsg;

    SerialSettings() {
        UART_BAUD          = 115200;
        SERIAL_PARAM       = SERIAL_8N1;
        SERIAL_RXPIN       = 0;
        SERIAL_TXPIN       = 0;
        SERIAL_TCP_PORT    = 0;
        SERIAL_BUFFER_SIZE = 1024;
        welcomeMsg         = "";
    }
};
#if 0
class BitlashCLI : public Console
{
public:
    void begin(int baud, String p, int hlen)
    {
        _prompt = p;
        Console::begin(baud, p, hlen);
        initBitlash(baud);
    }

    void addFunction(const char *func_name, bitlash_function func)
    {
        _btl.addFunction(func_name, func);
    }

    virtual void consoleTask()
    {
        log_d("consoleTask start!");
        char *line;

        while (1)
        {
            if ((line = lineNoise()) != NULL)
            {
                sendTelnet(line);
                historyAdd(line);
                doCommand(line);
                freeLine(line);
            }
        }
    }

    void bitlashConsoleHandle(void)
    {
        if (_telnet->available())
        {
            String buffer(_telnet->readStringUntil('\n'));
            sp(buffer.c_str());
            sp("\n");

            historyAdd((char *)buffer.c_str());
            doCommand((char *)buffer.c_str());

            sp(_prompt.c_str());

            _telnet->write(_prompt.c_str());
        }

        if (_telnet)
            _telnet->handle();
    }

    void bindTelnet(TelnetSpy *telnet)
    {
        if (telnet)
            _telnet = telnet;
    }

    void sendTelnet(char *line)
    {
        log_d("send to telnet: %s", line);

        if (_telnet)
        {
            while (*line)
                _telnet->write(*line++);

            _telnet->write("\n");
            _telnet->write(_prompt.c_str());
        }
    }

private:
    TelnetSpy *_telnet;
    LinenoiseBitlash _btl;
    String _prompt;
};
#endif

class SerialTelnetBridgeClass {
   public:
    SerialTelnetBridgeClass();
    ~SerialTelnetBridgeClass() = default;

    SerialTelnetBridgeClass(const SerialTelnetBridgeClass &);
    SerialTelnetBridgeClass &operator=(const SerialTelnetBridgeClass &);

    typedef void (*callback_c)(void);

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

    virtual bool begin(bool serial1, bool serial2);
    virtual void initWiFi();
    virtual void initOTA();
    virtual void initSerial(bool serial0, bool serial1, bool serial2);
    virtual void initTelnet(bool telnet0, bool telnet1, bool telnet2);
    virtual void initConsole();
    virtual void initClock();
    virtual void printClock();
    virtual void initSerialPorts();
    virtual void initTelnetPorts();

    void setHostname(String hostname);
    void setApName(String apName);
    void setApPassword(String password);
    void setTargetHostname(String targetHostname);
    void setCommandPrompt(String prompt);
    void setPortalTimeout(uint16_t portalTimeout);
    void bindTelnet0(void);
    void bindTelnet1(void);
    void bindTelnet2(void);

    void setWiFiConnectChecker(callback_c callback);

    //Message loop
    virtual bool handle();

    AsyncWebServer *getAsyncWebServerPtr();

   protected:
#ifdef SIMPLE_CLI
    SimpleCLI _cli0;
    SimpleCLI _cli1;
    SimpleCLI _cli2;

    Command _command0;
    Command _command1;
    Command _command2;
#endif

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

    HardwareSerial *_serial0;
    HardwareSerial *_serial1;
    HardwareSerial *_serial2;

    SerialSettings _port0;
    SerialSettings _port1;
    SerialSettings _port2;

    void setSerialPort(HardwareSerial *serial, SerialSettings *port);
    void setTelnetPort(TelnetSpy *telent, SerialSettings *port, void (*callbackOnConnect)(), void (*callbackOnDisconnect)());

    //BitlashCLI _bcli;
    Ticker _connectChecker;
    callback_c _connectCheckerCallback;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIALTELNETBRIDGE)
extern SerialTelnetBridgeClass STB;
#endif
