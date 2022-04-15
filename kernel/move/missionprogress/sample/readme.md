# Move模块：missionprogress

## 简述
missionprogress是报告进度的模块，主要功能包括
- 通过redis更新任务进度，状态
- 通过系统messagequeue向beekeeper报告进程状态（看门狗）
- timer，每十秒设置一次（具体时间根据timer参数设置）

## 参数说明
本sample的参数配置在sample.json配置

| 参数               | 必要 | 默认参数 | 类型   | 说明                    |
| ------------------ | ---- | -------- | ------ | ----------------------- |
| progress_keyname | 是   |          | string | redis中的进度key名        |
| watchdog_keyname | 是   |          | string | redis中的看门狗key名        |
| progress_timer     | 否   | 10       | int    | 模块sleep时间，单位为秒 |

## sample程序编译

编译sample文件，在samle文件加中，如果一个sample程序不够用，可以手动创建一个带有main的cpp文件，此cpp文件也会自动编译。

cmake会自动加载已有的think模块，但其他move模块不会加载。move模块最好是独立的，如果与其他move模块联系，请通过kernel/common/transferstation/transferstation.hpp做联系。

```
cd kernel/move/missionprogress/sample
mkdir build
cmake ..
make
```

## 模拟测试

### 依赖环境

依赖环境：redis-tools

安装：apt-get install redis-tools

### sample程序

sample：会自动更新mission进度

### 结果检验

```
redis-cli
hget mission.bootes.progress uuidTesting
```

- redis-cli：本示例是连接本机redis，若是其他机器，redis-cli需要设置连接的服务(redis-cli -h host -p port -a password)
- mission.bootes.progress：redis更新的field，progress_fieldname配置
- uuidTesting：redis更新的uuid，sample写死在sample.cpp中