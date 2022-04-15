# Think模块：rabbitmq

## 简述

rabbitmq模块操作rabbitmq的模块，主要功能包括

- 创建exchange
- 创建queue
- 确认完成queue(ack)
- 

## 基础知识

https://blog.csdn.net/anzhsoft/article/details/19563091

## moduleParam参数说明

| 参数  | 必要 | 默认参数 | 类型   | 说明                   |
| ----- | ---- | -------- | ------ | ---------------------- |
| channel |  | 1 | int | 连接通道，一般无需配置 |
| durable | | 0 | int | queue是否持久化数据（保存磁盘），1：是，0，否 |
| autodelete | | 0 | int | 当最后一个连接者断开是否销毁queue，1：是，0，否 |
| noack | | 0 | int | 不需要确认消息，是否ack后才会销毁message，1：不需要ack，0，需要ack |
| isrpc | | 0 | int | 是否是rpc通信，1：是，0，否 |
| queuenamerpcstr | | 随机 | string | rpc接收信息的queue名字，默认会随机名字,rpc的queue固定durable为0,autodelete为1 |
| queuenamestr | | 随机 | string | queue名字，默认会随机名字 |
| bindingkey | | “” | string | 绑定queue的bindingkey |
| correlationid | | “” | string | 关联id，发送message时的附属信息 |
| exchangetype | | direct | string | exchange的类型，“direct”，“fanout”，“direct”，“topic” |
| returnkey | |  | string | get Message后插入passParam(数据仓库)的字段，如果参数为空，会覆盖passParam |
| sendStr | | sendStr | string | 发送信息的body |
| routingkey | |  | string | 发送信息的routingkey |
| exchange | |  | string | exchange名称 |
| correlationidget | 无需传参 |  | sting | get message时获取的correlationid，move模块可以从此参数获取得到的correlationid |

## action说明

| 值                                    | 相关的moduleParam值                                          | action前提                                              | 说明                                                         |
| ------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------- | ------------------------------------------------------------ |
| think::Crabbitmq::CONNECT             | channel                                                      |                                                         | 连接服务器                                                   |
| think::Crabbitmq::DISCONNECT          |                                                              | think::Crabbitmq::CONNECT                               | 断开服务器                                                   |
| think::Crabbitmq::DECLAREEXCHANGE     | exchange，exchangetype                                       | think::Crabbitmq::CONNECT                               | 创建exchange                                                 |
| think::Crabbitmq::DECLAREQUEUE        | durable，autodelete，exchange，bindingkey，queuenamestr      | think::Crabbitmq::CONNECT                               | 创建queue                                                    |
| think::Crabbitmq::QOSPREFETCH         |                                                              | think::Crabbitmq::CONNECT                               | 当对消息处理完后（即对消息进行了ack，并且有能力处理更多的消息）再接收来自队列的消息，这种情况需要调用 |
| think::Crabbitmq::SENDMESSAGE         | exchange，routingkey，correlationid，queuenamerpc，isrpc，sendStr | think::Crabbitmq::CONNECT                               | 发送消息                                                     |
| think::Crabbitmq::GETMESSAGE          | queuename，returnkey，noack，                                | think::Crabbitmq::CONNECT                               | 获取消息                                                     |
| think::Crabbitmq::SENDRESPONSEMESSAGE | queuename，returnkey，noack，queuenamerpc，correlationidrpc，sendStr | think::Crabbitmq::CONNECT，think::Crabbitmq::GETMESSAGE | 回复消息                                                     |
| think::Crabbitmq::SENDMESSAGEONCE     | channel，exchange，routingkey，correlationid，queuenamerpc，isrpc，sendStr |                                                         | 发送消息，包含连接，断开服务器，发送message                  |
| think::Crabbitmq::GETMESSAGEONCE      | channel，exchange，exchangetype，durable，autodelete，exchange，bindingkey，queuenamestr，queuename，returnkey，noack， |                                                         | 获取消息，包含连接，断开服务器，声明exchange，声明queue，get message |

有其他测试类的ACTION，这些目前用不到，根据一个博客教程写的测试，sample.json配有这些测试用的ACTION，sample.json中“#sampleExplain”字段为说明文档。

## 依赖库

详情请看kernel/think/rabbitmq/devinclude/install.txt

## sample程序说明

- sample.cpp :需要传一个参数，对应参数为sample.json中的一级字段，如exchangeDeclare，运行命令./sample exchangeDeclare 为执行exchangeDeclare (声明exchange)的例子
- sample.json:moduleParam相关参数的配置，一级字段是ACTION字段，程序运行只会传入ACTION字段对应的json里的变量

## sample程序编译

编译sample文件，在samle文件加中，如果一个sample程序不够用，可以手动创建一个带有main的cpp文件，此cpp文件也会自动编译。

cmake会自动加载本think模块，但其他think模块不会加载。think模块与其他think模块是独立的。

```
cd kernel/think/rabbitmq/sample
mkdir build
cmake ..
make
```

## 模拟测试

### 依赖环境

依赖环境：ratbbtmq

配置：kernel/config/config.hpp

### sample程序

sample：

### 结果检验

- 