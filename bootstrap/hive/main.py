#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys
import json
import subprocess
import psutil
import time
import redis
from rediscluster import RedisCluster
import uuid
import traceback

processMap = {}

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

import os,signal
def GetSelfPid():
    return os.getpid()

def LinkNodeGet(nodeString):
    node = []
    try:
        nodeArr = nodeString.split(",")
        for piece in nodeArr:  
            tempArr = piece.split(":")
            nodePoint = {"host": tempArr[0], "port": tempArr[1]}
            node.append(nodePoint)
    except Exception as e:
        logger.error('wrong Redis Adress:'+nodeString, exc_info=True)
        sys.exit(-1)
    return node

#Logging init
import logging
import re
from logging.handlers import TimedRotatingFileHandler
from logging.handlers import RotatingFileHandler
def LoggerInit(rootPath, hiveName):
    if not os.path.exists(rootPath):
        os.makedirs(rootPath)
    
    log_fmt = '[%(asctime)s][%(levelname)s]['+GetHostIp()+']['+hiveName+'][%(message)s] %(filename)s,%(lineno)d'
    formatter = logging.Formatter(log_fmt)

    log_file_handler = TimedRotatingFileHandler(filename= rootPath+"queenBeeRunLog."+hiveName+"."+GetHostIp(), when="D", interval=1, backupCount=15)
    log_file_handler.suffix = "%Y-%m-%d"
    log_file_handler.extMatch = re.compile(r"^\d{4}-\d{2}-\d{2}$")
    log_file_handler.setFormatter(formatter)
    logging.basicConfig(level=logging.INFO)
    log = logging.getLogger()
    log.addHandler(log_file_handler)
    return log

if __name__ == '__main__':
    time.sleep(1)
	
    #STEP read config file
    allConfig = {}
    configFile = sys.argv[1] if len(sys.argv)>1 else './config.json'
    with open(configFile, 'r') as f:
        allConfig = json.load(f)
    config = allConfig['selfConfig']
    workerState = allConfig['processConfig']
    statePool = config['watchdogKeyName'].replace("+IP", GetHostIp())
    mesqstate_queuenamestr = ""
    if 'missionstate' in workerState:
        mesqstate_queuenamestr = workerState['missionstate']['mesqstate_queuenamestr']
    

    #STEP init logger
    logger = LoggerInit(config['logRoot'], config['hiveName'])
    logger.info('Hive start!,'+ json.dumps(allConfig))

    #STEP link redis
    LinkNode = LinkNodeGet(config['statePoolAdress'])
    if(len(LinkNode) == 1):
        redisPool = redis.ConnectionPool(host=LinkNode[0]['host'], port=LinkNode[0]['port'], password=config['statePoolPassword'])
        connect = redis.Redis(connection_pool = redisPool)
    else:
        connect = RedisCluster(startup_nodes=LinkNode, password=config['statePoolPassword'])

    while True:
        time.sleep(config['intervalTime'])

        tempRemoveList = []
        cpuRateSum = 0.0
        memoryRateSum = 0.0
        processNum = 0
        freeProcess = 0

        #STEP get statePool's keys
        processList = connect.hkeys(statePool)
        connect.expire(statePool, 600)  #10 min expire
        
        #STEP checking processMap
        for key,value in processMap.items():
            #check whether Process alive
            try:
                #check statePool (check watchdog)
                try:
                    detailInfo = connect.hget(statePool, key)
                    detailInfo = json.loads(detailInfo)
                    timeStampState = detailInfo['timeStamp']
                    pidState = detailInfo['pid']
                    stateState = detailInfo['state']
                    
                    if stateState == 'wait':
                        if config['freeNum'] <= freeProcess:
                            timeStampState = 0
                        freeProcess+=1
                    if stateState == 'end':
                        timeStampState = 0
                    if (int(time.time()) - timeStampState) >= config["watchDogDeathTime"]:   #whether need to kill process
                        try:
                            logger.info('kill process, key='+ key+', pid='+ str(pidState))
                            os.kill(pidState, signal.SIGKILL)  
                        except Exception as e:
                            logger.warning('kill process failed, pid='+ str(pidState))
                        finally:
                            tempRemoveList.append(key)
                            connect.hdel(statePool, key)
                    else:
                        processNum+=1
                except Exception as e:
                    try:
                        logger.info('kill process, key='+ key+', pid='+ str(value['pid']))
                        os.kill(value['pid'], signal.SIGKILL)  
                    except Exception as e:
                        logger.warning('kill process failed, pid='+ str(value['pid']))
                        tempRemoveList.append(key)

                    logger.warning('statePool(watchdog) without this process, key='+ key)
                
                try:
                    tempProcess = psutil.Process(value['pid'])
                    cpuRateSum += (float(tempProcess.memory_percent()) if float(tempProcess.memory_percent())>value['theoreticalMemory'] else value['theoreticalMemory'])
                    memoryRateSum += (float(tempProcess.cpu_percent(None)) if float(tempProcess.cpu_percent(None))>value['theoreticalCpu'] else value['theoreticalCpu'])
                except Exception as e:
                    tempRemoveList.append(key)

            except Exception as e:
                logger.error('unknow except,', exc_info=True)
                #logger.warning(traceback.print_exc())
            finally:
                try:
                    processList.remove(bytes(key.encode('utf-8')))
                except Exception as e:
                    pass

        #Remove death process in processMap
        for key in tempRemoveList:
            processMap.pop(key, '404')

        #Check the remaining processList 
        for key in processList:
            #check statePool (check watchdog)
            key = str(key, encoding = "utf8")
            try:
                poolInfo = connect.hget(statePool, key)
                poolInfo = json.loads(poolInfo)
                timeStampState = poolInfo['timeStamp']
                pidState = poolInfo['pid']
                stateState = poolInfo['state']
                if stateState == 'wait':
                    if config['freeNum'] <= freeProcess:
                        timeStampState = 0
                    freeProcess+=1
                if stateState == 'end':
                    timeStampState = 0
                if (int(time.time()) - timeStampState) >= config["watchDogDeathTime"]:   #whether need to kill process
                    try:
                        logger.info('kill process, key='+ key+', pid='+ str(pidState))
                        os.kill(pidState, signal.SIGKILL)  
                    except Exception as e:
                        logger.warning('kill process failed, pid='+ str(pidState))
                    finally:
                        connect.hdel(statePool, key)
                    continue
                
                tempProcessKid = {
                    'pid':pidState,
                    'theoreticalMemory':config['theoreticalMemory'],
                    'theoreticalCpu':config['theoreticalCpu']
                }
                processMap[key] = tempProcessKid

                processNum+=1
            except Exception as e:
                logger.warning('StatePool without this process, key='+ key)
                #logger.warning(traceback.print_exc())

        #STEP whether procesNum beyond limit
        if config['limitNum'] <= processNum :
            continue
        elif config['freeNum'] <= freeProcess:
            continue
        
        #STEP create process
        try:
            memoryUsage = psutil.virtual_memory().percent
            cpuUsage = psutil.cpu_percent(None)
            processNum = processNum if processNum!=0 else 1 
            afterMemoryUsage = memoryUsage + (config['theoreticalMemory'] if config['theoreticalMemory']>(memoryRateSum/processNum) else memoryRateSum/processNum)
            afterCpuUsage = cpuUsage + (config['theoreticalCpu'] if config['theoreticalCpu']>(cpuRateSum/processNum) else cpuRateSum/processNum)
            if afterMemoryUsage<=100 and afterCpuUsage<=100:
                #prepare process parameter
                identity = str(uuid.uuid4())
                workerState['common']['identity'] = identity
                workerState['common']['hostName'] = socket.gethostname()
                workerState['common']['hostIp'] = GetHostIp()
                watchdog_keyname = workerState['missionprogress']['watchdog_keyname']
                workerState['missionprogress']['watchdog_keyname'] = watchdog_keyname.replace("+IP", GetHostIp())
                if 'missionstate' in workerState:
                    workerState['missionstate']['mesqstate_queuenamestr'] = mesqstate_queuenamestr.replace("+UUID", identity)

                #create process
                workerBootStrap = config['workerBootStrap']+' \'' + json.dumps(workerState) + '\''
                item = subprocess.Popen(workerBootStrap, shell=True, stdout=None, stderr=None, preexec_fn=os.setpgrp)
                #for windows
                #item = subprocess.Popen(workerBootStrap, stdin=None, stdout=None, stderr=None, shell=True, creationflags=DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP)
                logger.info('create process, '+ workerBootStrap)
                tempProcessKid = {
                    'pid':item.pid,
                    'theoreticalMemory':config['theoreticalMemory'],
                    'theoreticalCpu':config['theoreticalCpu']
                }
                processMap[workerState['common']['identity']] = tempProcessKid
        except Exception as e:
            logger.error('unknow except,', exc_info=True)
            #logger.warning(traceback.print_exc())