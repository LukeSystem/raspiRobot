/** ===============================================
# serialClient.ino
#	s.kawano @ lukesystem Inc.
#	Ver.001 2022/12/4	初版
#		データ形式（最後にカンマを付ける）
#		i=1,0=0,1=0,2=0,3=0,4=0,5=0,6=0,7=0,
#
#	機能
#		Arduino と Scratch間のデータ連携
#		1.Arduino アナログポート0～7　→　Scratch
#		2.Scratch モーター [m1, m2]　→　Arduino
#		3.Scratch サーボ [,, sv10]　→　Arduino
# ===============================================*/
#include <Servo.h>

// サーボ
Servo servo10;		// 10番に設定するのは setup()内

// グローバル変数
char buffer[32];	// 受信バッファ
int iPos;			// 受信バッファの位置
int iCount;			// 送信データの抜けを確認するためのカウンタ

unsigned long old_time_data = 0;

/**
*	モーター制御(Arduino Romeo V2 all in one controller)
*		https://akizukidenshi.com/catalog/g/gM-07045/
*
*		モータードライバにより、接続ポートや操作方法を変更して下さい。
**/
void setRomeoMotor(int motorId, int speed)
{
	int speedPin, directionPin;
	if (motorId == 1) {
		speedPin = 5;
		directionPin = 4;
	} 
	else {
		if (motorId == 2) {
			speedPin = 6;
			directionPin = 7;
		}
		else {
			return;
		}
	}

	if (speed == 0) {
		digitalWrite(speedPin, LOW);
	}
	if (speed > 0) {
		digitalWrite(directionPin, HIGH);
		analogWrite(speedPin, speed);
	}
	else {
		digitalWrite(directionPin, LOW);
		analogWrite(speedPin, -speed);
	}
}

/**
*	初期処理
*/
void setup() {
	servo10.attach(10);		// サーボが接続されているのはデジタル10番

	pinMode( 4 , OUTPUT);	// モーター用定義（Romeo用の定義）
	pinMode( 5 , OUTPUT);
	pinMode( 6 , OUTPUT);
	pinMode( 7 , OUTPUT);
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
	digitalWrite(6, LOW);
	digitalWrite(7, LOW);

	pinMode(13, OUTPUT);	// LED
	Serial.begin(115200);	// UART通信

	iPos = 0;		// グローバル変数の初期値
	iCount = 0;
}

/**
*	タイマー処理
*		アナログデータをシリアルポートから出力します。
**/
void interrupt() {

	unsigned long time_data = 0;
	unsigned long sa = 0;
	unsigned long mil = 0;

	char send_data[200];	// 送信データ
	char sInt[10];			// 数値を文字列へ変換するワーク

	time_data = millis();					// 現在のミリ秒を取得
	sa = time_data - old_time_data;			// 経過ミリ秒を計算
	if (sa > 19) {							// 経過ミリ秒が19を超えたとき（20ms 間隔で送信）
		old_time_data = time_data;			// 次の経過ミリ秒を計算するため、現在のミリ秒を保存
		strcpy(send_data, "i=");
		itoa(iCount++, sInt, 10);			// カウンタ値
		strcat(send_data, sInt);

		strcat(send_data, ",0=");
		itoa(analogRead(A0), sInt, 10);		// A0
		strcat(send_data, sInt);

		strcat(send_data, ",1=");
		itoa(analogRead(A1), sInt, 10);		// A1
		strcat(send_data, sInt);

		strcat(send_data, ",2=");
		itoa(analogRead(A2), sInt, 10);		// A2
		strcat(send_data, sInt);

		strcat(send_data, ",3=");
		itoa(analogRead(A3), sInt, 10);		// A3
		strcat(send_data, sInt);

		strcat(send_data, ",4=");
		itoa(analogRead(A4), sInt, 10);		// A4
		strcat(send_data, sInt);

		strcat(send_data, ",5=");
		itoa(analogRead(A5), sInt, 10);		// A5
		strcat(send_data, sInt);

		strcat(send_data, ",6=");
		itoa(analogRead(A6), sInt, 10);		// A6
		strcat(send_data, sInt);

		strcat(send_data, ",7=");
		itoa(analogRead(A7), sInt, 10);		// A7
		strcat(send_data, sInt);

		strcat(send_data, ",");
		Serial.println(send_data);
	}

	if (iCount > 10000) {
		iCount = 0;
	}
}


/**
*	メイン処理
*		アナログデータをシリアルポートから送信（アナログポート）
*		シリアルポートからの受信データの処理（モーター、サーボ）
**/
void loop() {
	int i;
	int m1, m2, sv10;
	int iRecvPos;		// Pythonからの指示データの項目位置
	char data;
	bool flag = false;
	bool m1sign = false;
	bool m2sign = false;

	// アナログデータのシリアルポート送信
	interrupt();

	// シリアルポートからの受信データ
	while (Serial.available() > 0) {	// 受信データがあるとき
		data = Serial.read();			// シリアルポートのデータ（1バイト）読み取る

		if (data == '[') {				// 開始位置のとき（データは"["から始まる"]"で終わります）
			for (i=0; i<32; i++) {		// データを保存するバッファをクリア
				buffer[i] = 0;
			}
			iPos = 0;
		}

		buffer[iPos++] = data;			// シリアルポートのデータ（1バイト）をバッファに保存

		if (data == ']') {				// 終了位置のとき
			flag = true;				// 受信データ完了フラグをOnにする
		}
		if (iPos > 30) {				// バッファが30バイトを超えたとき
			break;						// 設計では30バイトを超えないので、以後は開始位置まで無視
		}
	}

	/** 受信したデータが揃ったとき、モーターの値として処理する
	/　[0,0] カッコで囲んであること、左がm1，右がm2
	**/
	if (flag) {							// 受信データ完了フラグがOnのとき

		flag = false;					// 受信データ完了フラグをOff（次の受信処理のため）
		m1 = 0;							// m1 の初期値は0
		m2 = 0;							// m2 の初期値は0
		sv10 = 0;						// サーボの初期値は0
		iRecvPos = 0;					// 0=m1, 1=m2,2=サーボ
		m1sign = false;					// m1の符号（プラスかマイナス）をプラスとする
		m2sign = false;					// m2の符号（プラスかマイナス）をプラスとする

		for (i=0; i<iPos; i++) {		// 受信データを1バイトずつ処理
			data = buffer[i];
			if ((data >= '0') && (data <= '9')) {		// 数字のとき
				switch (iRecvPos) {
					case 0:								// m1のデータとして処理
						m1 = m1 * 10 + (data - 48);
						break;
					case 1:								// m2のデータとして処理
						m2 = m2 * 10 + (data - 48);
						break;
					case 2:								// サーボのデータとして処理
						sv10 = sv10 * 10 + (data - 48);
				}
			}
			else if (data == '-') {		// 数字がマイナスのとき
				switch (iRecvPos) {
					case 0:				// m1の符号（プラスかマイナス）をマイナスとする
						m1sign = true;
						break;
					case 1:				// m2の符号（プラスかマイナス）をマイナスとする
						m2sign = true;
						break;
					// case 3: サーボはマイナスが無い
				}
			}
			else if (data == ',') {		// m1,m2,サーボの順に処理する
				iRecvPos++;
			}
		}

		switch (iRecvPos) {
			case 1:		// モーターのときは、データ項目が2つ
				if (m1sign) m1 = m1 * (-1);		// m1の符号がマイナスのとき、m1をマイナス値にする
				if (m2sign) m2 = m2 * (-1);		// m2の符号がマイナスのとき、m2をマイナス値にする
				setRomeoMotor(1, m1);			// m1モーターを操作
				setRomeoMotor(2, m2);			// m2モーターを操作
				break;

			case 2:		// サーボのときは、データ項目が3つ。　モーターのデータがあっても無視
				servo10.write(sv10);			// サーボを操作
				break;
		}
	}

}
