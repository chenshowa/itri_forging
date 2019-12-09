from flask import Flask
from flask import render_template
import jinja2
import math
import numpy as np
import pymysql
import random as ra
import struct

# float to binary
def binary(num):
    return ''.join('{:0>8b}'.format(c) for c in struct.pack('!f', num))
def float_to_hex(f):
    return hex(struct.unpack('<I', struct.pack('<f', f))[0])

InsertListtoSQL = []
for i in range(10) :
    InsertListtoSQL.append(float_to_hex(ra.uniform(0,5)))

InsertstringtoSQL = ' '.join(InsertListtoSQL)
# 插入BLOB list到資料庫
count = 1
db = pymysql.connect("localhost", "root", "rootroot", "itri_forging")
cursor = db.cursor()    
sql_insert = "INSERT customsenorsn1 (`ShotSN`, `MaxPressure`) VALUES ('%s', '%s')" % (count, InsertstringtoSQL)
print(sql_insert)
cursor.execute(sql_insert)
db.commit()

# 從資料庫取出完整的讀值list
cursor = db.cursor()    
sql_select = "SELECT MaxPressure from customsenorsn1"
cursor.execute(sql_select)
db.commit()
temp = list(cursor.fetchall())

import struct
struct.unpack('!f', '0x400fe95a'.decode('hex'))[0]
RawDataString = temp[3][0].decode('utf-8')
RawDataList = RawDataString.split()
convert(RawDataList[1])

0x1.ap+3

float.fromhex('0x1.ap+3')

from ctypes import *

def convert(s):
    i = int(s, 16)                   # convert from hex to a Python int
    cp = pointer(c_int(i))           # make this into a c integer
    fp = cast(cp, POINTER(c_float))  # cast the int pointer to a float pointer
    return fp.contents.value         # dereference the pointer, get the float

print convert("41973333")    # returns 1.88999996185302734375E1

print convert("41995C29")    # returns 1.91700000762939453125E1

print convert("470FC614")    # returns 3.6806078125E4


print(Jack)

# import base64
# 
# abc = base64.encodestring(Jack[0])
# 
# cac = base64.decodestring(Jack[2])
# Jack[2][0].decode(encoding="utf-8")
# print(Jack)
# cursor.fetchall()
# 
# base_str = "10.1234"
# bytes_utf_8 = base_str.encode(encoding="utf-8")
# print(bytes_utf_8)
# bytes_gb2312 = base_str.encode(encoding="gb2312")
# print(bytes_gb2312)
# str_from_utf_8 = bytes_utf_8.decode(encoding="utf-8")
# print(str_from_utf_8)
# str_from_gb2312 = bytes_gb2312.decode(encoding="gb2312")
# print(str_from_gb2312)
# 
# 
# 
# 
# 
# 
# 
# 
# 
InsertStringtoSQL = " ".join(InsertListtoSQL) 
f = ra.uniform(0,5)
aa = binary(f)
# 
    # 
# 
# for i in 
# aa = [9598595, 9598669]
# bin(aa[0])
# C = ''+str(bin(aa[0]))


