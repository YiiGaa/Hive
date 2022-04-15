#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys
import json
import time
#Command line check init
def CommandLineCheck():
    try:
        argvParams = json.loads(sys.argv[1])
        argvParams["requestId"]
        argvParams["action"]
    except Exception as e:
        print('You should run the script like this: python3 sample.py "{\\"requestId\\":\\"id\\", \\"action\\":{...}}"')
        exit()
    return argvParams

'''
@Description: sample script
@Run: run this script like this: python3 sample.py "{\"requestId\":\"id\", \"action\":{\"loop\":2}}"
@The description of the command line parameters as follow:
    {
        "requestId":"request Id (uuid)",
        "action":{

        }
    }
'''

import redis  #pip install redis

Config = {
    "connect":"10.139.17.164",
    "port":"6383",
    "password":"pCloud@progress0401!"
}
if __name__ == '__main__':
    #STEP: get command line parameters
    argvParams = CommandLineCheck()
    actionConfig = argvParams["action"]

    print('Worker start!, params:'+ json.dumps(argvParams))

    #The Running body as follow
    try:
        loop = int(actionConfig["loop"])
        index = 0
        while loop>index :
            index = index + 1
            print('loop index:'+ str(index))
            time.sleep(1)
    except Exception as e:
        print('Exception: '+ repr(e))

    
