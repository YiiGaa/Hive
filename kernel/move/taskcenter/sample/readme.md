# Move模块：taskcenter

## 简述
taskcenter是执行命令行的模块，主要功能包括
- 执行系统命令行

## 参数说明
本sample的参数配置在sample.json配置-本move模块无参数

| 参数               | 必要 | 默认参数 | 类型   | 说明                    |
| ------------------ | ---- | -------- | ------ | ----------------------- |
|  |    |          |  |         |

## sample程序编译

编译sample文件，在samle文件加中，如果一个sample程序不够用，可以手动创建一个带有main的cpp文件，此cpp文件也会自动编译。

cmake会自动加载已有的think模块，但其他move模块不会加载。move模块最好是独立的，如果与其他move模块联系，请通过kernel/common/transferstation/transferstation.hpp做联系。

```
cd kernel/move/taskcenter/sample
mkdir build
cmake ..
make
```

## 模拟测试

### 依赖环境

依赖环境：无

配置：sample.json中的mission字段

### sample程序

sample：./sample

### 结果检验

FINISH(taskcenter)打印出循环十次执行

