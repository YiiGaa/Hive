# Hive 监控程序

## 简述

监控程序是hive引导引导程序的监视器

- 自动创建引导程序进程
- 检测引导程序进程状态，一段时间无反馈的进程自动kill掉

## 基本原理

- 一个机器的同类型hive只有一个监控程序，此程序会一直运行
- 监控程序创建引导程序进程
- 引导程序只会执行一次任务，执行完会自动退出
- 引导程序运行过程中，会向redis更新状态（wait，start，complete，stop）
- 监控程序通过监控redis的状态，把长时间不更新的进程kill掉

## 参数说明

参数在config.json中，包含两部分:

- selfConfig：监控程序本身的设置
- processConfig：work进程启动时的参数，同 bootstrap/standalone/properties/testing.json内容

### selfConfig参数说明

| 参数              | 必要 | 类型   | 说明                                                         |
| ----------------- | ---- | ------ | ------------------------------------------------------------ |
| hiveName          | 是   | string | 当前hive的名字                                               |
| logRoot           | 是   | string | log的根目录，目前程序内部设定，1天一个，保留15天的运行日志，文件名为logRoot+"监控程序RunLog."+hiveName+"."+IP，如/home/log/监控程序RunLog.bootes.192.168.2.101 |
| watchdogKeyName   | 是   | string | 单台服务器中所有引导程序状态信息的key，+IP会自动替换成当前服务器的IP |
| statePoolAdress   | 是   | string | 用作监控hive状态信息的Redis的地址，如127.0.0.1:6379，如果是集群部署，用英文逗号隔开，如127.0.0.1:6379,127.0.0.1:6380 |
| statePoolPassword | 是   | string | 用作监控hive状态信息的Redis的密码                            |
| workerBootStrap   | 是   | string | hive的引导程序路径                                           |
| intervalTime      | 是   | int    | 监控程序循环之间停顿的秒数                                   |
| watchDogDeathTime | 是   | int    | 状态信息多长没更新（秒数），则认为该引导程序出现问题，可强制Kill掉 |
| theoreticalMemory | 是   | int    | 理论单个任务的内存使用率，理论使用率为50%写作50              |
| theoreticalCpu    | 是   | int    | 理论单个任务的CPU使用率，理论使用率为50%写作50               |
| freeNum           | 是   | int    | 保持空闲引导程序个数                                         |
| limitNum          | 是   | int    | 引导程序最大个数（执行任务最大个数）                         |

### processConfig参数说明

详情参照bootstrap/standalone/readme.md

## 依赖环境

- reids，对应配置是selfConfig中的statePoolAdress和statePoolPassword
- python3

## 程序测试

把selfConfig中的引导程序BootStrap设置为python3 ./mission-test.py

再启动程序：python3 main.py

程序会启动mission-test.py这个测试用的引导程序

## 程序启动

```
python3 main.py
```
