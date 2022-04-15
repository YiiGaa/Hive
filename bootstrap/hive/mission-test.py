#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import time
import random
import json
import redis
import sys

import socket
import fcntl,struct
def GetHostIp():
    ip = '0:0:0:0'
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    finally:
        s.close()

    return ip

import os
def GetSelfPid():
    return os.getpid()



if __name__ == '__main__':
    print ("Testing tool for hive's worker.")

    print(sys.argv[1])
    argvParams = json.loads(sys.argv[1])

    redisName = argvParams['missionprogress']['watchdog_fieldname']
    workerState = {
        'hostName':argvParams['common']['hostName'], 
        'hostIp':argvParams['common']['hostIp'],
        'pid':GetSelfPid(),
        'state':'wait',
        'identity':argvParams['common']['identity'],
        'timeStamp':0
    }
    

    redisPool = redis.ConnectionPool(host='192.168.3.72', port=6382, password='YiiGaa@progress0303')
    connect = redis.Redis(connection_pool=redisPool)

    missionGetTime = random.randint(0, 6)
    missionRunTime = random.randint(10, 15)

    #STEP waiting mission
    workerState['state'] = 'wait'
    print ("    - Mission waiting: ", missionGetTime,"s")
    while missionGetTime > 0:
        workerState['timeStamp'] = int(time.time())
        connect.hset(redisName, workerState['identity'], json.dumps(workerState))
        #print ("Start:", time.ctime())
        time.sleep(1)
        missionGetTime = missionGetTime - 1

    #STEP run mission
    workerState['state'] = 'run'
    print ("    - Mission running: ", missionRunTime,"s")
    while missionRunTime > 0:
        workerState['timeStamp'] = int(time.time())
        connect.hset(redisName, workerState['identity'], json.dumps(workerState))
        time.sleep(1)
        missionRunTime = missionRunTime - 1

    #STEP end mission
    workerState['state'] = 'end'
    workerState['timeStamp'] = int(time.time())
    connect.hset(redisName, workerState['identity'], json.dumps(workerState))
    print ("    - Mission end.")



