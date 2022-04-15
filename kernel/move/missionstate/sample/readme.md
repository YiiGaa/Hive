# Move模块：missionstate

## 简述
missionstate是接受改变设置的模块，主要功能包括
- 监听stop命令，工作线程一次工作结束
- 监听pause命令，工作线程挂起

## 参数说明
本sample的参数配置在sample.json配置

| 参数               | 必要 | 默认参数 | 类型   | 说明                    |
| ------------------ | ---- | -------- | ------ | ----------------------- |
| mesqstate_channel |    | 1 | int | 连接channel，多线程连接rabbitmq时需要甚至不一样的channel |
| mesqstate_exchange | 是 | | sting | exchange名 |
| mesqstate_bindingkey | 是 | | sting | bindingkey名，绑定exchange和queue |
| mesqstate_queuenamestr | 是 | | sting | state queue名 |

## sample程序编译

编译sample文件，在samle文件加中，如果一个sample程序不够用，可以手动创建一个带有main的cpp文件，此cpp文件也会自动编译。

cmake会自动加载已有的think模块，但其他move模块不会加载。move模块最好是独立的，如果与其他move模块联系，请通过kernel/common/transferstation/transferstation.hpp做联系。

```
cd kernel/move/missionstate/sample
mkdir build
cmake ..
make
```

## 模拟测试

### 依赖环境

依赖环境：rabbitmq服务

配置：kernel/config/config.hpp

### sample程序

sample：./sample

### 结果检验

执行kernel/kernel/think/rabbitmq里面的sample程序

执行./samle testAllMissionState
或执行./samle testSingleMissionState