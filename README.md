# ESP32 Serial Telnet Bridge (Setp2)

## Contents

- ESP32 Seria Telnet Bridge (Setp2)
  - [Contents](#contents)
  - [✅ Features](#-features)
  - [✅ 開発環境](#-開発環境)
  - [✅ 回路図](#-回路図)
  - [✅ Wemos lolin32 board Pinout](#-wemos-lolin32-board-pinout)
  - [✅ Using ESP32 libraries](#-using-esp32-libraries)
  - [✅ How to use](#-how-to-use)
  - [👤 Author](#-author)
  - [📝 License](#-license)

## ✅ Features

- [x] スマートフォンのブラウザ画面で、SSID、パスワードを入力してWiFiアクセスポイントに接続する
- [x] REST APIにGETリクエストを送信して、BME280センサーの情報を取得する
  - [x] BME280で取得した、気温、湿度、気圧をレスポンスで返却する。
- [x] シリアル入出力(UART1, UART2)をTelnetへリダイレクトする
- [x] Telnetからコマンドを入力してデバイスの操作をする
- [x] リレーモジュール制御
  - [x] REST APIにPUTリクエストを送信して、リレーを制御する
- [ ] サーボモーター制御
  - [ ] REST APIにPUTリクエストを送信して、サーボを制御する
- [ ] 初期化ボタンで、WiFiのログイン情報（SSID、パスワード）を消去する
- [ ] ステータスLEDでデバイスの状態を表示・表現する
- [ ] コマンドよりWiFi通信状態を確認する
  - [ ] 電波強度確認
- [ ] リセットボタンでデバイスをハードウェアリセットする
- [ ] ESP32へTelnet接続時に、そのデバイスのIPアドレスを表示する

## ✅ 開発環境

- [ESP-WROOM-32(Wemos lolin32 Board)]()
- [VSCode]()
- [PlatformIO]()
- [Arduino]()
- [Relay Modules]()
- [Sensors(BME280)]()
- [USB-Serial Bridge]()

## ✅ 回路図

![回路図]()

## ✅ Wemos lolin32 board Pinout

![Wemos lolin32 board Pinout](http://blog.pagefault-limited.co.uk/wp-content/uploads/2019/04/lolon32_v1.0.1_pinout.png)

## ✅ Using ESP32 libraries

- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer.git)
- [ESPAsyncWiFiManager](https://github.com/alanswx/ESPAsyncWiFiManager.git)
- [Telnetspy](https://github.com/yasheena/telnetspy.git)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson.git)
- [ServoESP32](https://github.com/RoboticsBrno/ServoESP32.git)
- [SimpleCLI](https://github.com/spacehuhn/SimpleCLI.git)
<!--
[ArduinoStreamUtils](https://github.com/bblanchon/ArduinoStreamUtils.git)
-->

## ✅ How to use

TODO:　ESP32SerialTelnetBridgeクラスを派生元にしてアプリケーションクラスを作成する。

TODO:　インスタンスを作成

TODO: クラスヘッダ表示

TODO: APIの説明

## 👤 Author

- Github: [riraosan](https://github.com/riraosan)
- Qiita: [riaosan](https://qiita.com/riraosan)

## 📝 License

This software is released under the [MIT](https://github.com/riraosan/ESP32SerialWiFiBridge/blob/master/LICENSE) License.

## Donete
