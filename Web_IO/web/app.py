from flask import Flask
from flask import render_template
from threading import Lock
from flask_socketio import SocketIO
import jinja2
import math
import numpy as np
import pymysql

async_mode = None
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app, async_mode=async_mode)
thread = None
thread_lock = Lock()


# 後端執行序產生之數據，即時推送至前端
def background_thread(): 
    count = 0
    print("renew data")
    while True:
        socketio.sleep(0.1)
        count += 1
        # t = time.strftime('%M:%S', time.localtime()) # 系統時間（分:秒）
        # cpus = psutil.cpu_percent(interval=None, percpu=True) # 系統cpu使用率 non-blocking

        t = count
        data = math.sin((count * np.pi / 18.0)) + 1
        data2 = math.sin((count * np.pi / 9.0)) + 1
        socketio.emit('server_response',
                      {'scales': t, 
                      'value': data,
                      'value2': data2},
                      namespace='/test')
        # 注意：這裡不需要客户端連接的上下文，預設 broadcast = True


@app.route('/')
def index():
    db = pymysql.connect("localhost", "root", "rootroot", "itri_forging")
    cursor = db.cursor()
    sql_select= "select MaxPressure from CustomSenorSN1"
    cursor.execute(sql_select)
    #fetch出來資料格式為tuple
    sql1sub=list(cursor.fetchall())

    sql_select2= "select MaxPressure from CustomSenorSN2"
    cursor.execute(sql_select2)
    sql2sub = list(cursor.fetchall())
    # print(sql2sub)
    # 取單點特徵值
    eigenvalues = sql1sub[0][0]
    eigenvalues2 = sql2sub[0][0]
    # print(eigenvalues)
    # print(eigenvalues2)
    db.close()
    #value = [[0.456], [0.451], [0.476]]
    print("get index sucessfull")
    return render_template('index.html', async_mode=socketio.async_mode, values=str(eigenvalues), values2=str(eigenvalues2))


@socketio.on('connect', namespace='/test')
def test_connect():
    print("be connected")
    global thread
    with thread_lock:
        if thread is None:
            thread = socketio.start_background_task(target=background_thread)


if __name__ == '__main__':

    socketio.run(app, port=5566, debug=True)

