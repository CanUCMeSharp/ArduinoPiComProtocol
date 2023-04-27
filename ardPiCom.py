#To Do:
#Create User mpgwetter

#Redis DBs overview:
#0  Live dump
#1  Minute Average
#2  Hourly Average
#3  Daily Average

import json
import serial
import time
import redis
import math
baud = 57600
minute = 0


def checksum(data):
    s = 0
    MOD_Checksum = 2 << 32
    for i in data:
        s = (s + i) % MOD_Checksum
    return s

def returnToArduino(data):
    dataValues = [ int(data.temp), int(data.light), int(data.loudness), int(data.wind), int(data.wDirection), int(data.uv), int(data.o2), int(data.humidity), int(data.pressure), int(data.co2), int(data.co), int(data.no2), int(data.smog), int(data.ph1), int(data.ph2), int(data.ph3)]
    if int(data.checksum) != checksum(dataValues):
         #1 = not Acknowledge
        ser.write(str(1).encode() + 0x0A.to_bytes(1, 'little'))
        print("Data from Arduino not valid")
    current_time = int(time.time())
    delta = current_time - int(data.time)
    if  delta > 1 or delta < -1:
        ser.write(str(current_time).encode() + 0x0A.to_bytes(1, 'little'))
        print("Time updated to: " + str(current_time))
    else:
        #0 = Acknowledge
        ser.write(str(0).encode() + 0x0A.to_bytes(1, 'little'))

def addToDB(data):
    redis.set(data.time, data, ex=90)
            

#Converts JSON String to data Structure
class Payload(object):
        def __init__(self, j):
            self.__dict__ = json.loads(j)

pool = redis.ConnectionPool(host='localhost', port=6379, db=0)
redis = redis.Redis(connection_pool=pool)

while True:
    try:
        ser = serial.Serial()
        ser.baudrate = baud	
        ser.port =  input("Serial port: ")
        ser.open()
        break
    except:
        print("No connection to Arduino!")
        time.sleep(0.5)
while True:
    try:
        #Raw JSON
        j = ser.readline()
        #JSON processed to object
        data = Payload(j) 
        returnToArduino(data)
        addToDB(data)
    except:
        try:
            #Not Acknowledge
            ser.write(str(1).encode() + 0x0A.to_bytes(1, 'little'))
        except:
            try:
                ser.close()
                ser.open()
            except:
                print("Error in connection to Arduino")
                time.sleep(0.5)