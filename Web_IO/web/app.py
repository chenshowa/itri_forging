from flask import Flask
from flask import render_template
from threading import Lock
from flask_socketio import SocketIO
import jinja2
import math
import numpy as np
import pymysql
import time
from datetime import datetime
import json


async_mode = None
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app, async_mode=async_mode)
thread = None
thread_lock = Lock()

# global dict for timestamp calculate
value_TimeStamp = {}
Stop_TimeStamp = {}
L_in_ms = 0

#要讀的最新的資料筆數
NumLatestRecords = 2

#要讀的table
Tables = ['customSenorSN1_TableSN1', 'customSenorSN1_TableSN2']

#要讀的特徵值
DisplayItem = ['shotMax', 'shotMin', 'shotAvg']
DisplayValue = ['GraghData', 'TimeStamp']

#建立一global dict包含所需的key值
for i in Tables:
    Stop_TimeStamp[i] = []

# 後端執行序產生之數據，即時推送至前端
def background_thread():
    print("---thread run---")
    global value_TimeStamp, Stop_TimeStamp, Tables, DisplayItem, DisplayValue, L_in_ms, NumLatestRecords
    #將Timestamp切割成x軸座標
    def TimeStampCutter (DBtimestamp):
        global value_TimeStamp, Stop_TimeStamp, Tables, L_in_ms

        # 引入global TempTimeStamp做前次插值的最終時間點辨識
        # '''datatime convert to usec(float) '''
        d_in_ms = int(datetime.strptime(DBtimestamp, "%Y-%m-%d %H:%M:%S.%f").timestamp()*1000000)

        ValuesofStop_TimeStamp = []
        ValuesofStop_TimeStamp.append(Stop_TimeStamp[Table])

        # '''usec(float) to datatime'''
        # print(datetime.fromtimestamp(1575538760399999/1000000.0))
        # '''first_datatime = timestamp - sectionlength/clockrate(usec)'''
        fst_datatime = d_in_ms - 39999

        # 根據工業電腦設定而估算取得的Timestamp是40個點裡的第 N 時間點
        AssumeTimeSection = 8
        AsuumeSectionLength = 5000
        
        # '''先計算first＿timestamp,再去計算40個點的時間'''
        tempdatatime = []
        # 判斷TempTimeStamp為0時，以機台設定之的估算時間點做時間戳的序列
        if L_in_ms < 1:
            print ("TimeStamp:",L_in_ms)

            for i in range(40) :
                fst_datatime2 = fst_datatime + int(i*AsuumeSectionLength)
                tempdatatime.append(fst_datatime2)

        # 當TempTimeStamp不為0時，以前次插值時的最後估算時間點做時間戳的序列
        elif L_in_ms < d_in_ms:
            # 若前次插值的最終估算時間點較本次SQL-DB的時間戳大，則
            # TimeBlock = int((d_in_ms - TempTimeStamp) / AssumeTimeSection)
            TimeBlock = int((d_in_ms) - AsuumeSectionLength * AssumeTimeSection)
            print("1:",TimeBlock)

            for i in range(40) :
                StartTimeStamp2 = L_in_ms + int(i*AsuumeSectionLength)
                tempdatatime.append(StartTimeStamp2)
        else :
            TimeBlock = ((L_in_ms - d_in_ms) // AssumeTimeSection)
            if TimeBlock > AsuumeSectionLength :
                TimeBlock = AsuumeSectionLength
            print("2:",TimeBlock)

            for i in range(40) :
                StartTimeStamp2 = d_in_ms + int(i*TimeBlock)
                tempdatatime.append(StartTimeStamp2)

        return tempdatatime

    #將特徵值list轉成字串，給SQL語法取用
    def ListToString(item):
        DBSelectString = ', '.join(item)
        return DBSelectString

    while True:
        socketio.sleep(2)

        db = pymysql.connect("localhost", "root2", "rootroot", "forging")
        cursor = db.cursor()

        passingvalue = {}

        #建立一dict包含所需的key值
        for i in DisplayItem+DisplayValue:
            passingvalue[i] = []
        passingvalue['value'] = []

        for Table in Tables:
            #SQL特徵值
            sql_select= "select " + ListToString(DisplayItem) + ", GraghData from " + Table + " ORDER BY ShotSN DESC LIMIT  " + str(NumLatestRecords)
            cursor.execute(sql_select)
            sql1sub=list(cursor.fetchall())

            #插入特徵值至passingvalue
            for i in DisplayItem:
                passingvalue[i].append([j[DisplayItem.index(i)] for j in sql1sub])

            #SQL channel讀值
            sql_select= "select "+ ListToString(DisplayValue) + " from " + Table + " ORDER BY ShotSN DESC LIMIT  " + str(NumLatestRecords)
            print(sql_select)
            cursor.execute(sql_select)
            sql1sub=list(cursor.fetchall())
            print (sql1sub)

            #插入Channel讀值至passingvalue
            value_connet = []

            for j in sql1sub:

                # value存值
                value_list = [float(i) for i in j[0].split()]
                value_connet += value_list

                # timestamp存值
                value_TimeStamp[Table] = j[1]

            print('value_TimeStamp',value_TimeStamp)
            print('Stop_TimeStamp', Stop_TimeStamp[Table])

            # for recognition type of stop_timestamp
            Astemp = []

            if type(Stop_TimeStamp[Table]) == type(Astemp):
                INTTimeStamp = TimeStampCutter (value_TimeStamp[Table])
                Stop_TimeStamp[Table] = INTTimeStamp[-1]
                print('ASSA', Stop_TimeStamp)
            else:
                L_in_ms = int(Stop_TimeStamp[Table])
                INTTimeStamp = TimeStampCutter (value_TimeStamp[Table])
                Stop_TimeStamp[Table] = INTTimeStamp[-1]
                print('BSSB', Stop_TimeStamp)

            ConvertTimeStamp = []
            for i in INTTimeStamp:
                ConvertTimeStamp.append(json.dumps(((datetime.fromtimestamp(i/1000000.0)).isoformat())))

            passingvalue['value'].append(value_connet)
            passingvalue['TimeStamp'].append(ConvertTimeStamp)
            # print(passingvalue)

        socketio.emit('server_response', passingvalue
                      , namespace='/test')
        # 注意：這裡不需要客户端連接的上下文，預設 broadcast = True


@app.route('/')
def index():
    print("get index sucessfull")
    return render_template('index.html', async_mode=socketio.async_mode)

@socketio.on('connect', namespace='/test')
def test_connect():
    print("be connected")
    global thread
    with thread_lock:
        if thread is None:
            thread = socketio.start_background_task(target=background_thread)


if __name__ == '__main__':

    socketio.run(app, port=5566, debug=True)
