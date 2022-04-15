# hive引导程序

## 简述

启动程序，联合move模块，配置例子properties/testing.json

## properties参数说明

```
{
    "common":{
        "identity":"5c70489e-a04e-4186-9e51-93bd0ea69dba",  #hive程序的身份码（uuid），hive监控程序会自动生成
        "hostName":"",                                      #当前服务器name，hive监控程序会自动获取
        "hostIp":"192.168.1.100",														#当前服务器IP，hive监控程序会自动获取
        "endWaitTime":4                                     #引导程序执行完程序后，过多久（秒）后退出，必须大于progress_timer，以保证状态信息已经更新完成
    },
    "missionget":{
        "isMain":1,                                         #引导程序的主线程，不用修改
        "mesqmission_adress":"127.0.0.1:5672",              #用作接收任务的RabbitMQ的地址
        "mesqmission_user":"guest",                         #用作接收任务的RabbitMQ的用户名
        "mesqmission_pwd":"guest",                          #用作接收任务的RabbitMQ的密码
        "mesqmission_channel":1,                            #RabbitMQ连接的channel，不用修改
        "mesqmission_queuename":"hive.mission"              #RabbitMQ中用作接收任务的队列名
    },
    "missionstate":{
        "mesqstate_adress":"127.0.0.1:5672",                #用作接收任务变更指令的RabbitMQ的地址
        "mesqstate_user":"guest",                           #用作接收任务变更指令的RabbitMQ的用户名
        "mesqstate_pwd":"guest",                            #用作接收任务变更指令的RabbitMQ的密码
        "mesqstate_channel":2,                              #RabbitMQ连接的channel，不用修改
        "mesqstate_exchange":"hive",                        #用作接收任务变更指令的RabbitMQ的exchange
        "mesqstate_bindingkey":"hive.state",                #用作接收任务变更指令的RabbitMQ的bindingkey
        "mesqstate_queuenamestr":"hive.state.+UUID"         #引导程序专属的用于作接收任务变更指令的队列名称，+UUID会自动替换
    },
    "missionprogress":{
        "progress_adress":"127.0.0.1:6383",                 #用作更新引导程序状态信息的Redis的地址
        "progress_pwd":"YiiGaa@progress0201!",              #用作更新引导程序状态信息的Redis的密码
        "watchdog_adress":"127.0.0.1:6383",                 #用作更新任务进度的Redis的地址，必须与statePoolAdress相同
        "watchdog_pwd":"YiiGaa@progress0201!",              #用作更新任务进度的Redis的密码
        "progress_keyname":"hive.progress-",                #任务进度数据的key，后面会自动添加任务ID
        "watchdog_keyname":"hive.watchdog-+IP",             #状态信息数据的key，+IP会自动替换当前服务器的IP
        "progress_timer":2                                  #每次更新的时间间隔（秒），此值需要小于watchDogDeathTime
    },
    "taskcenter":{
    }
}
```

`common`:通用变量

missionget/missionstate/missionprogress/taskcenter:move模块，自动创建对应move线程

## 任务下达

```
{
		"missionId":"uuid",
		"acceptUrl":"http://ip:host/task/worker/accept",
		"responseUrl":"http://ip:host/task/worker/response",
		"target":{
				"command":"python3 -u /home/hive/run/mission/sample.py \"{\\\"requestId\\\":\\\"123\\\", \\\"action\\\":{\\\"loop\\\":12}}\" 2>&1",
				"loop":1,
				"interval":1,
				"isMarkAllProgress":0,
				"markProgressLength":1,
				"progressLength":1
		}
}
```

| 参数               | 必要 | 默认参数 | 类型   | 说明                                                         |
| ------------------ | ---- | -------- | ------ | ------------------------------------------------------------ |
| missionId          | 是   |          | string | 任务id，需要唯一                                             |
| acceptUrl          | 否   |          | string | 任务开始执行时，回调的Restful-API的URL。目标Restful-API的请求方式必需是post，请求体必需是json字符串的格式。回调有失败重试机会，最多重试3次，每次间隔1秒，3次失败会记录log |
| responseUrl        | 否   |          | string | 任务完成时，回调的Restful-API的URL。目标Restful-API的请求方式必需是post，请求体必需是json字符串的格式。回调有失败重试机会，最多重试3次，每次间隔1秒，3次失败会记录log |
| target             | 是   |          | json   | 任务相关配置                                                 |
| command            | 是   |          | string | 任务脚本运行命令，`不可缺省`，如果是python3的任务脚本，命令中必需含有`-u`和`2>&1`，不然，hive的引导程序无法捕获屏幕log。如果命令中含有特殊字符，需要转义，如果是json，请按以上例子方式转义。 |
| loop               | 否   | 1        | int    | 任务脚本命令执行多少次，设置成-1的话是无限运行，但不推荐这样做 |
| interval           | 否   | 0        | int    | 任务脚本执行多次间的时间间隔（秒），loop设置为1的话，此值无意义 |
| isMarkAllProgress  | 否   | 0        | int    | 任务完成时，回调的消息体中，是否包含任务脚本多次运行的log，0（只记录最后一次运行命令的log）/1（记录多次运行命令的log） |
| markProgressLength | 否   | 1        | int    | 任务完成时，回调的消息体中，每次运行命令的log的数组长度，默认是1（记录单词运行命令的log的最后一条），设置为0则不记录所有log，-1是记录所有log，但我们不推荐记录所有log，因为可能会造成消息体过大而出错 |
| progressLength     | 否   | 1        | int    | 记录到用作更新任务进度Redis中的log数组长度，`可缺省`，默认是1。hive的引导程序捕获任务脚本的log后，会作为数组更新到Redis中，此值为数组长度的限制。此值不应过大，不然可能会造成Redis阻塞。另外，hive的引导程序更新Redis任务进度信息是定时触发的（如每两秒，具体有[hive的配置文件]()而定），所以如果progressLength设置1，则可能日志是不连续的 |

## 依赖库

请参照[官网使用手册中的环境安装](https://stoprefactoring.com/#content@content#framework/hive/)

## 程序说明

- startup.cpp :正式的引导程序main文件，配置需要命令行参数传入；
- standalone.cpp :单独测试的引导程序main文件，自动使用properties/testing.json配置文件；

## 程序编译

```
cd bootstrap/standalone
mkdir build
cmake ..
make
```

