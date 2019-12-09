import time
from datetime import datetime

def TimeStampcutter (DBtimestamp):
    # '''datatime convert to usec(float) '''
    d_in_ms = int(datetime.strptime(DBtimestamp, "%Y-%m-%d %H:%M:%S.%f").timestamp()*1000000)
    # d_in_ms = int(d.timestamp()*1000000)
    # d = bb
    # d_in_ms = int(float(d)*1000000)

    # print(datetime.fromtimestamp(float(d)))
    # print(d_in_ms)

    # '''usec(float) to datatime'''
    # print(datetime.fromtimestamp(1575538760399999/1000000.0))
    # '''first_datatime = timestamp - sectionlength/clockrate(usec)'''
    fst_datatime = d_in_ms - 39999
    # print(fst_datatime)

    # '''先計算first＿timestamp,再去計算40個點的時間'''
    tempblock = 0
    tempdatatime = []
    while tempblock < 40:
        fst_datatime = fst_datatime + 2500
        tempdatatime.append(fst_datatime)
        tempblock += 1

    for i in tempdatatime:
     TimeStamp = (datetime.fromtimestamp(i/1000000.0))
    
    return TimeStamp