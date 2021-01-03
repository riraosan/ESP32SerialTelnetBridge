# ESP32 Serial WiFi Bridge(step2)

## ✅ Features

- [x] スマートフォンのブラウザ画面よりWiFi接続設定する。
- [x] シリアル出力をTelnetに表示する。
- [x] Telnet端末からコマンドを実行する。
- [ ] 接続時に接続したIPアドレスを表示する。
- [ ] 初期化ボタンで、WiFiのログイン情報を消去する。
- [ ] 切り替えボタンでAPモード、STAモードを切り替える。
- [ ] REST APIにPUTリクエストを送信して、リレーをON／OFFする。
- [ ] REST APIにPUTリクエストを送信して、PWM波をポートに出力する。（サーボ駆動用）
- [x] REST APIにGETリクエストを送信して、BME280センサーの情報を取得する。
- [ ] デバイスの状態をステータスLEDで表示する。
- [ ] コマンドより通信状態を確認する。
- [ ] リセットボタンでデバイスをハードウェアリセットする。

[要求仕様書はこちら](./SPEC.md)

## ✅ 必要とするもの、要件

- ESP32開発ボード
- リレー
- サーボ

## ✅ 簡単な回路図
（仮）
![回路図](https://www.quadcept.com/ja/manual/common/images/20200904104154565-e8.png)

## ✅ 部品一覧

- Wemos lolin32 board(ESP-WROOM-32)
- 筐体(タカチ製)
- リレーモジュール
- センサー類

### Wemos　lolin32 board)

Wemos Lolin32 Pinout
![lolin32](http://blog.pagefault-limited.co.uk/wp-content/uploads/2019/04/lolon32_v1.0.1_pinout.png)


## esp32 libraries

- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer.git)
- [ESPAsyncWiFiManager](https://github.com/alanswx/ESPAsyncWiFiManager.git)
- [Telnetspy](https://github.com/yasheena/telnetspy.git)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson.git)
- [ServoESP32](https://github.com/RoboticsBrno/ServoESP32.git)
- [SimpleCLI](https://github.com/spacehuhn/SimpleCLI.git)
<!--
[ArduinoStreamUtils](https://github.com/bblanchon/ArduinoStreamUtils.git)
-->

## 使用例

- 場所を選ばず、IoT機器から出力されるログをPCで取得する。
- 対象機器が電源OFFONで復旧した直後からのシリアルログをLAN経由で取得する。
- ロボットのログ取得に使用する。（動き回っているロボットから無線でログ取得　etc）
- テレメトリー用途で使用。（模型ロケット、ラジコン飛行機 etc）
- REST APIを叩いて、遠隔で情報を取得or機器を設定or機器を操作。

## 🚀 How to use

```zsh
$ git clone https://github.com/riraosan/ESP32SerialWiFiBridge.git
```

TODO:　インクルードする

TODO:　ESP32SerialTelnetBridgeクラスを派生元にしてアプリケーションクラスを作成する。

TODO:　インスタンスを作成

TODO: クラスヘッダ表示

## 👤 Author

- Github: [riraosan](https://github.com/riraosan)
- Qiita: []

## 📝 License

This software is released under the [MIT](https://github.com/riraosan/ESP32SerialWiFiBridge/blob/master/LICENSE) License.
