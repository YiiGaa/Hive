# Think模块：redis

## 简述

redis模块操作的是redis数据，主要功能包括

- 更新或插入redis数据库中的数据（哈希map结构）

## moduleParam参数说明

| 参数  | 必要 | 默认参数 | 类型   | 说明                   |
| ----- | ---- | -------- | ------ | ---------------------- |
| field | 是   |          | string | redis数据库中的field名 |
| key   | 是   |          | string | redis数据库中的key     |
| value | 是   |          | string | redis数据库中的value   |

## action说明

| 值                        | 必要的moduleParam值 | action前提             | 说明                         |
| ------------------------- | ------------------- | ---------------------- | ---------------------------- |
| think::Credis::CONNECT    |                     |                        | 连接数据库                   |
| think::Credis::DISCONNECT |                     |                        | 断开连接                     |
| think::Credis::UPDATE     | field,key,value     | think::Credis::CONNECT | 更新数据                     |
| think::Credis::ONCE       | field,key,value     |                        | 更新数据，自动连接和断开连接 |

## 依赖库

详情请看kernel/think/redis/devinclude/install.txt

## sample程序说明

- sample.cpp :用的是think::Credis::ONCE（更新数据，自动连接和断开连接）
- passParam.cpp:moduleParam相关参数的配置

## sample程序编译

编译sample文件，在samle文件加中，如果一个sample程序不够用，可以手动创建一个带有main的cpp文件，此cpp文件也会自动编译。

cmake会自动加载本think模块，但其他think模块不会加载。think模块与其他think模块是独立的。

```
cd kernel/think/redis/sample
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

获取数据

```
redis-cli
hget mission.bootes.progress d03743b53844473e8129f11837088633
```

- redis-cli：本示例是连接本机redis，若是其他机器，redis-cli需要设置连接的服务
- mission.bootes.progress：redis更新的field，kernel/think/redis/sample/passParam.json中的field配置
- d03743b53844473e8129f11837088633：redis更新的key，kernel/think/redis/sample/passParam.json中的key配置