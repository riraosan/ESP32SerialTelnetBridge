#include <Arduino.h>
#include <unity.h>
#include <SerialTelnetBridge.h>

SerialTelnetBridgeClass serialTelnetBridge;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_serial0(void)
{
    SerialSettings port;

    port.UART_BAUD = 0;
    port.SERIAL_PARAM = SERIAL_8N1;
    port.SERIAL_RXPIN = 0;
    port.SERIAL_TXPIN = 0;
    port.SERIAL_TCP_PORT = 0;
    port.SERIAL_BUFFER_SIZE = 0;

    serialTelnetBridge.setSerialPort0(port);

    //serialTelnetBridge.initSerial();

    TEST_ASSERT_EQUAL(13, 13);
}

void test_serial1(void)
{
}

void test_serial2(void)
{
}

/*
void test_led_builtin_pin_number(void)
{
    TEST_ASSERT_EQUAL(13, LED_BUILTIN);
}

void test_led_state_high(void)
{
    digitalWrite(LED_BUILTIN, HIGH);
    TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_BUILTIN));
}

void test_led_state_low(void)
{
    digitalWrite(LED_BUILTIN, LOW);
    TEST_ASSERT_EQUAL(LOW, digitalRead(LED_BUILTIN));
}
*/

void setup()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    //(ボードがsoftwareリセットに対応していない場合に必要なdelay．)
    delay(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_serial0);

    UNITY_END(); // stop unit testing
}

void loop()
{
}
