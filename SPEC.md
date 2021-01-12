<!--

REST API

GET：データを取得します。
POST：新しいデータを作成します。
PUT：データを更新します。
DELETE：データを削除します。

# 要求仕様書には以下を含めること
## How どうやって
## What なにを　／　Why　なぜ
## Who どの機能が / Where どこの機能が　どのぐらいの範囲で
## When いつ、どのような順序で、どのぐらいの期間

合理主義、経験主義、選択的無知

テンプレート

### [swb-x]要求機能概要（要求機能で何をするのかを概要で書く）

| 要求の補足 | 内容 |
|:---|:---|
|理由|なぜ、その要求を実現したいのか？|
|説明|要求についての詳細な説明|

* [ ] [swb-1-1] 要求機能をさらに具体的書く。何をどうやってどのような機能をどのような順序で行うのかを書く。５W1Hで書く。

-->
# ESP32 UART-Telnetブリッジ 要求仕様書
## 目次

<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [ESP32 UART-Telnetブリッジ 要求仕様書](#esp32-uart-telnetブリッジ-要求仕様書)
  - [目次](#目次)
    - [[swb-1]Telnet端末からコマンドを実行する](#swb-1telnet端末からコマンドを実行する)
    - [[swb-2]REST APIでポートの制御をする](#swb-2rest-apiでポートの制御をする)
    - [[swb-3]ESP32の状態をコマンドで表示する](#swb-3esp32の状態をコマンドで表示する)
    - [[swb-4]初期化ボタンを押下するとSSID、パスワード情報を消去する](#swb-4初期化ボタンを押下するとssidパスワード情報を消去する)
    - [[swb-5]現在のESP32の状態をLEDで表示する](#swb-5現在のesp32の状態をledで表示する)
    - [[swb-6]リセットボタンでESP32をリセットする](#swb-6リセットボタンでesp32をリセットする)
    - [[swb-7]REST APIでBME280センサーのデータを取得する](#swb-7rest-apiでbme280センサーのデータを取得する)
    - [[swb-8]コマンド入力によりUART0の出力先をTelnet1またはTelnet2へ変更する](#swb-8コマンド入力によりuart0の出力先をtelnet1またはtelnet2へ変更する)

<!-- /code_chunk_output -->

##　要求仕様一覧

### [swb-1]Telnet端末からコマンドを実行する

| 要求の補足 | 内容 |
|:---|:---|
|理由（機能目的？）|コマンドを入力してサーボやリレーを動作させたいから。サーボを動作させるために、ポートにPWMを出力させたり、リレーを動作させるために、ポートをON／OFFしたい。|
|説明（機能要件）|コマンドの種類は別のファイルで定義する。|

【機能分解】
* [ ] [swb-1-1] ポートをON／OFFする
* [ ] [swb-1-2] リレーをON／OFF
* [ ] [swb-1-3] サーボを任意に操作する

### [swb-2]REST APIでポートの制御をする

| 要求の補足 | 内容 |
|:---|:---|
|理由|REST APIのフォーマットでポートを制御して周辺機器を動作させたい。|
|説明|コマンドの種類は[別のファイル]で定義する|

* [swb-3-1] ポートをON／OFFする
* [swb-3-2] リレーを動作させる
* [swb-3-3] ポートにPWM信号を出力する（サーボを動作させる）

### [swb-3]ESP32の状態をコマンドで表示する

| 要求の補足 | 内容 |
|:---|:---|
|理由|IPアドレスを確認したい。ポートの状態を表示したい。|
|説明|コマンドの種類は[別のファイル]で定義する|

* [swb-4-1]　IPアドレスを表示する。
* [swb-4-2]　ポートの状態を表示する。
* [swb-4-3]　電波の強度を表示する。
* [swb-4-4]　接続したアクセスポイントのSSIDを表示する　　

### [swb-4]初期化ボタンを押下するとSSID、パスワード情報を消去する

| 要求の補足 | 内容 |
|:---|:---|
|理由|新しいアクセスポイントに接続する必要がる場合に、すばやく作業を行いたい。|
|説明|接続するアクセスポイントを変更する場合に、一度、パスワード情報を消して、アクセスポイント接続モードにする。再度、SSIDとパスワードを入力する画面を表示させる。|

### [swb-5]現在のESP32の状態をLEDで表示する

| 要求の補足 | 内容 |
|:---|:---|
|理由|現在、ESP32に電源が入っているのかどうなのか、アクセスポイントに接続したのかどうなのか、ひと目で確認したいから。|
|説明|２つのLED（緑、赤）でESP32を表現する|

### [swb-6]リセットボタンでESP32をリセットする

| 要求の補足 | 内容 |
|:---|:---|
|理由|リセットボタンを押下することで、ESP32を再起動したい|
|説明|単純に、ESP32のリセット端子をプルダウンすることで、ESP32をリセットする|

### [swb-7]REST APIでBME280センサーのデータを取得する

| 要求の補足 | 内容 |
|:---|:---|
|理由|REST APIからBME280センサーの温度、湿度、気圧のデータを取得したい。|
|説明|REST APIにGETリクエストを送信して、JSONデータで温度、湿度、気圧データを取得する。レスポンスボディはJSON形式でリクエスト元へ送信する。|

* [x] [swb-7-0]WebServerを立てる。
* [x] [swb-7-1]デバイスID、温度、湿度、気圧をREST APIで取得する。 
* [x] [swb-7-2]ホスト名は「esp32」とする。
* [x] [swb-7-3]温度の取得URI「GET /esp/sensor/temparature」とする。
* [x] [swb-7-4]湿度の取得URI「GET /esp/sensor/humidity」とする。
* [x] [swb-7-5]気圧の取得URI「GET /esp/sensor/pressure」とする。
* [x] [swb-7-6]すべての情報取得URI「GET /esp/sensor/all」とする。
* [x] [swb-7-7]JSONフォーマットは以下のフォーマットとする。GETリクエストを受信するとjson形式のBodyをレスポンスで応答する。
  * [ ] JSONレスポンスフォーマットは次のように定める：
    * [ ] 温度の取得
      * [ ] {"id": xx, "devices": "BME280", data: {"temparature": xx.xx}}
    * [ ] 湿度の取得
      * [ ] {"id": xx, "devices": "BME280", data: {"humidity": xx.xx}}
    * [ ] 気圧の取得
      * [ ] {"id": xx, "devices": "BME280", data: {"pressure": xx.xx}}
    * [ ] すべての情報取得
      * [ ] {"id": xx, "devices": "BME280", data: {"temparature": xx.xx, "humidity": xx.xx, "pressure": xx.xx}}
  * [x] 各データの小数点以下の有効桁数は特に定めない。（取得した側が調整する課題とする）

### [swb-8]コマンド入力によりUART0の出力先をTelnet1またはTelnet2へ変更する

| 要求の補足 | 内容 |
|:---|:---|
|理由|log出力がHARDWAREシリアル0に固定されているため、その出力をコマンドを使ってTelnet1, Telnet2に動的に変更したい。|
|説明|Telnet1またはTelnet2に接続している状態で、Telnetからコマンドを入力し、UART0の出力をTelnet1またはTelnet2に出力させる。<br> また、もとの状態に直ちに戻すことができる。|

* [ ] [swb-8-0] 要求機能をさらに具体的書く。何をどうやってどのような機能をどのような順序で行うのかを書く。５W1Hで書く。
* [ ] [swb]

//HAL(JSON Hypertext Application Language)
//https://tools.ietf.org/html/draft-kelly-json-hal-08
//POST リクエスト
//esp32.local デバイスが　esp32_clock.local デバイスへ温度、湿度、気圧情報をPOSTリクエストで
送る。

<pre>
{
    "device_id": 12345678, //MACアドレスをキーにしてデバイスIDを送信する
    "created_at": "2021-01-10 12:01:16",
    "_links": { //エンベロープ？
        "self": {
            "href": "http://esp32.local"
        },
        "next": {
            "href": ""
        }
    },
    "_embedded": {
        "sensor": {
            "id": 93, //センサーID
            "name": "BME280",
            "temperature": 15.41,
            "humidity": 72.4638671875,
            "pressure": 1021.46828125,
            "status": "processing"
        },
        "servo": {
            "1": {
                "id": null,
                "first_angle": 0, //degree
                "second_angle": 90, //degree
                "duration": 1000 //ms
            },
            "2": {
                "id": null,
                "first_angle": 0, //degree
                "second_angle": 90, //degree
                "duration": 1000 //ms
            }
        },
        "relay": {}
    }
}
</pre>



//esp32からesp32_clockへデータを送信する
//bme280 POST
//esp32_clcok.localに実装する。
//BME280 POST(esp32->esp32_clock)
* [x] "href": "/sensor/V1/all"
* [x] "href": "/sensor/V1/temperature"
* [x] "href": "/sensor/V1/humidity"
* [x] "href": "/sensor/V1/pressure"

//esp32のサーボの角度を変更する
//Servo PUT(esp32_clock->esp32)
* [x] "href": "/servo/V1/1/operation"
* [x] "href": "/servo/V1/2/operation"

//Servo GET(esp32_clock->esp32)
* [x] "href": "/servo/V1/1/status"
* [x] "href": "/servo/V1/2/status"

//Relay PUT(esp32_clock->esp32)
* [x] "href": "/relay/V1/1/operation"
* [x] "href": "/relay/V1/2/operation"

//Clock GET(esp32_clock->esp32)
* [x] "href": "/clock/V1/now"

//Reset PUT(esp32_clock->esp32)
* [x] "href": "/reset/V1/now"

//Clock GET(esp32_clock->esp32)
* [x] "href": "/stat/V1/now"
