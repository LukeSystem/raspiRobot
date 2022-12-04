#!/usr/bin/env python3

# ===============================================
# serialServer.py
#	s.kawano @ lukesystem Inc.
#	Ver.001 2022/12/4	初版
#	機能
#		Arduino と Scratch間のデータ連携
#		1.Arduino アナログポート0～7　→　Scratch	"http://localhost:38601/analogRead?port=" + No;
#		2.Scratch モーター[m1, m2]　→　Arduino		`/motor?m1=${args.M1}&m2=${args.M2}`
#		3.Scratch サーボ [,, sv10]　→　Arduino		`/servo?sv10=${args.SV10}`
# ===============================================

from http.server import BaseHTTPRequestHandler, HTTPServer
import serial
import threading

PORT = 38601	# Scratch からの要求を受け付けるポート
sensor = [0, 1, 2, 3, 4, 5, 6, 7]	# アナログセンサーの初期値
oldNo = 0

###
# 文字列が数値に変換できるか調べる（true=数値, false=数値でない)
##
def isInt(s):
	try:
		int(s, 10)
	except ValueError:
		return False
	else:
		return True

###
# シリアルポートを定期的に監視する
##
def th_serial():

	global sensor
	global oldNo

	res = sr.readline()
	ss = res.decode()
	sp = ss.split(',')

	for dat in sp:
		sdat = dat.split('=')
		if (len(sdat) == 2):
			if (sdat[0] == 'i'):
				if (isInt(sdat[1])):
					sensorNo = int(sdat[1])
					flag = 0
					if (sensorNo != oldNo + 1):
						flag = 1
					oldNo = sensorNo
			elif (sdat[0] >= '0' and sdat[0] <= '9'):
				if (isInt(sdat[0]) and (isInt(sdat[1]))):
					no = int(sdat[0])
					value = int(sdat[1])
					sensor[no] = value

	t = threading.Timer(0.01, th_serial)	# 10msで監視する
	t.start()

###
# Scratch からの要求を処理
##
class S(BaseHTTPRequestHandler):

	# 応答のヘッダ情報をセット
	def _set_response(self):
		self.send_response(200)
		self.send_header("Content-Type", "application/json; charset=utf-8")
		self.send_header("Access-Control-Allow-Headers", "Content-Type")
		# CORS 対応。　ドメイン名で指定したいができないので、全て許可している
		self.send_header("Access-Control-Allow-Origin", "*")
		self.send_header("Access-Control-Allow-Methods", "OPTIONS,POST,GET")
		self.end_headers()

	# アナログデータ取得の要求
	def do_GET(self):
		self._set_response()
		req = self.path
		reqsp = req.split('?')

		if (reqsp[0] == '/analogRead'):	# アナログセンサーの読み出し
			port = reqsp[1].split('=')
			sRes = ""
			if (port[1] == '0'):
				sRes = '"result":{0}'.format(sensor[0])
			elif (port[1] == '1'):
				sRes = '"result":{0}'.format(sensor[1])
			elif (port[1] == '2'):
				sRes = '"result":{0}'.format(sensor[2])
			elif (port[1] == '3'):
				sRes = '"result":{0}'.format(sensor[3])
			elif (port[1] == '4'):
				sRes = '"result":{0}'.format(sensor[4])
			elif (port[1] == '5'):
				sRes = '"result":{0}'.format(sensor[5])
			elif (port[1] == '6'):
				sRes = '"result":{0}'.format(sensor[6])
			elif (port[1] == '7'):
				sRes = '"result":{0}'.format(sensor[7])

			sRes = "{" + sRes + "}"
			#print("sRes=[{0}]".format(sRes))
			self.wfile.write(sRes.encode('utf-8'))

		elif (reqsp[0] == '/servo'):
			sv10 = reqsp[1].split('=')
			sCom = "[,,{0}]".format(sv10[1])
			sr.write(bytes(sCom, 'UTF-8'))

			sRes = '"result":{0}'.format(sv10[1])
			sRes = "{" + sRes + "}"
			#print("sRes=[{0}]".format(sRes))
			self.wfile.write(sRes.encode('utf-8'))

		elif (reqsp[0] == '/motor'):
			motor = reqsp[1].split('&')
			if (len(motor) == 2):
				m1 = motor[0].split('=')
				m2 = motor[1].split('=')
				sCom = "[{0},{1}]".format(m1[1], m2[1])
				sr.write(bytes(sCom, 'UTF-8'))
				
				sRes = '"m1":{0},"m2":{1}'.format(m1[1], m2[1])
				sRes = "{" + sRes + "}"
				#print("sRes=[{0}]".format(sRes))
				self.wfile.write(sRes.encode('utf-8'))

###
# Webのアクセスを開始
##
def run(server_class=HTTPServer, handler_class=S, port=8080):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()

###
# Main
##
if __name__ == '__main__':
	dev = "/dev/ttyACM0"	# romeo 0
	#dev = "/dev/ttyACM1"	# romeo 1 Arduinoの状態で接続ポートが変わることがあります
	rate = 115200
	sr = serial.Serial(dev, rate, timeout=10)

	print("start")
	oldNo = 0
	t = threading.Thread(target=th_serial)
	t.start()

	run(port=PORT)

