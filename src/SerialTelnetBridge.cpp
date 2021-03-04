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

SerialTelnetBridgeClass::SerialTelnetBridgeClass()
{
    _dns = new DNSServer();
    _server = new AsyncWebServer(8080);
    _WiFiManager = new AsyncWiFiManager(_server, _dns);

    _telnet0 = new TelnetSpy();
    _telnet1 = new TelnetSpy();
    _telnet2 = new TelnetSpy();

    _Serial0 = new HardwareSerial(0);
    _Serial1 = new HardwareSerial(1);
    _Serial2 = new HardwareSerial(2);

    setHostname("esp32_001");
    setTargetHostname("esp32_gw");
    setApPassword("");
    setCommandPrompt("~esp$ ");
    setApName("ESP-G-AP");
    setPortalTimeout(180);

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
}

void SerialTelnetBridgeClass::telnet0Disconnected()
{
    log_d("- Telnet0 connection closed.");
}

void SerialTelnetBridgeClass::telnet1Connected()
{
    log_d("- Telnet1 connection established.");
}

void SerialTelnetBridgeClass::telnet1Disconnected()
{
    log_d("- Telnet1 connection closed.");
}

void SerialTelnetBridgeClass::telnet2Connected()
{
    log_d("- Telnet2 connection established.");
}

void SerialTelnetBridgeClass::telnet2Disconnected()
{
    log_d("- Telnet2 connection closed.");
}

void SerialTelnetBridgeClass::setSerialPort(HardwareSerial *serial, SerialSettings *port)
{
    serial->setDebugOutput(false);
    serial->setRxBufferSize(port->SERIAL_BUFFER_SIZE);
    serial->begin(port->UART_BAUD, port->SERIAL_PARAM, port->SERIAL_RXPIN, port->SERIAL_TXPIN);
    serial->println("Serial Txd is on pin: " + String(port->SERIAL_TXPIN));
    serial->println("Serial Rxd is on pin: " + String(port->SERIAL_RXPIN));
    serial->flush();
}

void SerialTelnetBridgeClass::initSerial(bool serial0, bool serial1, bool serial2)
{
    log_d("- Initializing Serial...");

    if (serial0)
        setSerialPort(_Serial0, &_port0);

    if (serial1)
        setSerialPort(_Serial1, &_port1);

    if (serial2)
        setSerialPort(_Serial2, &_port2);
}

void SerialTelnetBridgeClass::setTelnet()

void SerialTelnetBridgeClass::initTelnet(String welcomMsg, TelnetSpy *telnet, void (*callbackOnConnect)(), void (*callbackOnDisconnect)())
{
    log_d("- Initializing Telnet...");

    String prompt(_COMMAND_PROMPT);

    telnet->setWelcomeMsg(welcomMsg.c_str());
    telnet->setCallbackOnConnect(callbackOnConnect);
    telnet->setCallbackOnDisconnect(callbackOnDisconnect);
    telnet->setPort(_port0.SERIAL_TCP_PORT);
    telnet->setBufferSize(_port0.SERIAL_BUFFER_SIZE);
    //to begin telnet only
    telnet->setSerial(nullptr);
    telnet->begin(_port0.UART_BAUD);

    delay(500);
}

// could be used as:
// > pin(13,0)  # set pin 13 to 0
// > print pin(13)
// 0
numvar pin_func(void)
{
    // two args this is a set
    if (getarg(0) == 2)                     ///  getarg(0) is the number of args
        digitalWrite(getarg(1), getarg(2)); // getarg(1): 1st arg, getarg(2) 2nd arg

    // always return the value read
    return digitalRead(getarg(1));
}

void SerialTelnetBridgeClass::initConsole()
{
    log_d("- Initializing Console...");

    _CON.begin(115200, _COMMAND_PROMPT, 10);

    if (_CON.termProbe())
    { /* zero indicates success */
        printf("\n"
               "Your terminal application does not support escape sequences.\n"
               "Line editing and history features are disabled.\n"
               "On linux , try screen.\n"
               "On Windows, try using Putty instead.\n");
        _CON.termDumb(true);
    }

    // add a new function "pin" to bitlash
    _CON.addFunction("pin", (bitlash_function)pin_func);

    _CON.consoleTaskStart(); // will start a task waiting for input and execute
                             // it with Bitlash doCommand()
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
#ifdef SIMPLE_CLI
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
#endif

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

void SerialTelnetBridgeClass::setPortalTimeout(uint16_t portalTimeout)
{
    _PORTAL_TIMEOUT = portalTimeout;
}

bool SerialTelnetBridgeClass::begin(bool serial0, bool serial1, bool serial2)
{
    initWiFi();
    initSerial(serial0, serial1, serial2);
    initTelnet();
    initConsole();
    initOTA();

    return true;
}

void SerialTelnetBridgeClass::handle()
{
    ArduinoOTA.handle();

    //consoleHandle(_telnet0, _Serial0, &_cli0);
    //consoleHandle(_telnet1, _Serial1, &_cli1);
    //consoleHandle(_telnet2, _Serial2, &_cli2);

    yield();
}
