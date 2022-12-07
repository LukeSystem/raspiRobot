# raspiRobot

Arduinoで動作するロボット（以下、Arduinoロボ）のプログラムを簡単にするため、Scratchで作ることができるようにします。

Arduinoロボは、NPO法人岡山ロボット技術子供育成協会が講習会で利用しているものと同等です。　このArduinoロボにRaspberryPI4を組み合わせて利用します。  
https://orja.org/

![image](https://user-images.githubusercontent.com/98453574/206096896-7ca5f9d6-7b1b-41fb-b91b-211ed7f9ca88.png)
![image](https://user-images.githubusercontent.com/98453574/206096926-e444788f-7654-4bb6-93b6-b3281f53e46f.png)


## 機器の構成  

![image](https://user-images.githubusercontent.com/13915550/205480286-6dc652c7-ebb1-4f3c-a0a5-13ade80a2dac.png)

ArduinoロボとRaspberryPI4をUSBケーブルで接続します。

## ソフトウエアの構成

### RaspberryPI4のOS
Liteは使えないため、desktopを選択して下さい。
- Raspberry Pi OS with desktop
- Raspberry Pi OS with desktop and recommended software

### Arduinoのソフトウエア
serialClient.ino を書き込みます。

![image](https://user-images.githubusercontent.com/13915550/205480717-7b85d777-b089-4999-9d69-b31efb0d69a7.png)

### RaspberryPI4のソフトウエア（Python）
serialServer.py を Python で実行します。  
` python serialServer.py `  
start と表示されれば実行中です。

![image](https://user-images.githubusercontent.com/13915550/205480781-3f6727b5-36f2-4b50-9ab0-f7fe3b9188d6.png)

### RaspberryPI4のソフトウエア（Scratch）

#### 以下のサイトにアクセスします。  
https://lukesys.co.jp/scratchrobot/

![image](https://user-images.githubusercontent.com/13915550/205480962-aeec2efc-5939-4609-acc8-baae03b8845d.png)

#### 拡張機能を選択します
![image](https://user-images.githubusercontent.com/13915550/205481026-2a64d644-a8f8-4833-8828-c541cd8d9f81.png)

#### Robot Lukesys を選択します
![image](https://user-images.githubusercontent.com/13915550/205481053-758fc951-7dc9-4846-8fe6-4980cc7e9eae.png)

#### 拡張機能が利用できるようになります
![image](https://user-images.githubusercontent.com/13915550/205481085-736ff618-4bc3-447d-bb63-e533384b1459.png)

### Scratch の拡張機能の使い方

#### アナログ0にチェックを入れると、アナログ0の値が表示されます
他のアナログも同様です。　Arduinoロボのアナログポートに接続されたセンサー値が表示されます。

![image](https://user-images.githubusercontent.com/13915550/205481147-216c6e71-2c90-4c28-9919-42fa144725d9.png)

#### モーター
Arduinoロボの左モーター（m1）が左、右モーター（m2）が右になります。
下記の例だと、左モーターは100、右モーターは200で回します。

![image](https://user-images.githubusercontent.com/13915550/205481296-33e94c34-754b-4f5c-8bf6-b03480fecdc3.png)

※モーターの値は-255～255までです。

#### 例1　左右のモーターを100で動かし、1秒後に止まるプログラム

![image](https://user-images.githubusercontent.com/13915550/205481371-fe1dd112-e7f9-4289-b567-33cf508522bb.png)

#### 例2　アナログ0の値が500より小さくなったとき、上記例1のプログラムを動かす

![image](https://user-images.githubusercontent.com/13915550/205481509-a2751f94-c4df-4c81-9992-94ec372ff4f1.png)


