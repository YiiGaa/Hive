<img src="https://github.com/YiiGaa/Trick/blob/master/Prop/Common/Img/logo.png" width="300"/>

Hive是通用型的云计算服务架构，把脚本放进指定目录即可扩展云计算功能。适合各开发语言的云计算脚本。

- 最新稳定版本: v1.0
- 使用文档详见官网，[点击跳转使用手册](https://stoprefactoring.com/#content@content#framework/hive/)

## 只需放入脚本，即可拥有云计算能力

Hive是通用型的云计算服务架构，把脚本放入指定目录即可扩展云计算功能。

原则上，脚本的开发语言是不限制的，只要脚本能在服务器上正常运行即可，这里推荐使用Python。

云计算的应用场景是，处理一些比较长的任务，如视频转码、数据整理、统计整理等任务。

云计算程序是受后端调度的，云计算与后端应用程序的区别是，后端应用程序处理的是耗时比较短的任务（如数据库操作、redis操作等），云计算程序处理的是耗时比较长的任务，如视频转码、数据整理、统计整理等。

理论上，后端应用程序也可以完成耗时较长的任务，但实际上，如果后端应用程序处理耗时较长的任务（如视频转码、负责的数据整理等）的话，则会容易造成后端服务器的性能阻塞，阻碍其他请求。并且，一些长时间任务是十分损耗性能的，一台服务器也只能同时执行几个任务，如果把这些任务交由后端应用程序完成，则可能服务器会超负荷。

云计算部分不是每个网站系统都需要的，而且由于其复杂性，一般使用第三方提供的云计算服务（SaaS服务）会更省力一些。但是，如果自身的网站系统拥有自己的云计算部分，将会大大提升网站系统的可能性。

## 基础技术

此框架是一种扩展型程序，框架解决了云计算程序的任务下发、任务进度更新、监听任务状态变更等通用问题。框架本身是一个独立程序，云计算脚本放入到指定目录，即可被后端应用程序调度这个云计算脚本。

原则上，脚本的开发语言是不限制的，只要脚本能在服务器上正常运行即可。不过，我们推荐使用Python。

框架采用的基础技术如下，但一般情况下，框架本身是不需要二次开发的，所以`不懂框架的技术，也可以正常使用`：

- 开发语言：C++、Python3
- 监控软件：Supervisor

如果你的云计算脚本不是Python编写，且出现了问题的话，可以联系官方解决。

## 设计思想

Hive架构的设计思想是，Hive解决通用的云计算任务下发、任务进度更新、监听任务状态变更等问题。开发者只需要把云计算功能脚本放入指定目录即可扩展云计算功能。Hive架构与云计算脚本在程序上是完全分离的，业务开发人员无需关注诸如任务下发、消息队列等机制，只需要关心云计算功能脚本即可。

## 工作原理

云计算服务部分主要是处理一些运行时间较长、逻辑或计算较为复杂的任务，例如：视频转码、直播转码、统计、数据挖掘等。云计算服务部分的通用应用场景如下所示。

![](https://github.com/YiiGaa/Hive/blob/main/design/design2.png) 

以上是简化了的云计算应用场景，实际上，后端应用程序与云计算服务软件是相互独立的，且是多对多的关系。


## Hive的组成部分

默认情况下，Hive由三部分组成：监控程序、引导程序、任务脚本。在一台服务器内，监控程序、引导程序的进程、任务脚本，是一对多对多的关系。也就是说，一个监控软件会启动可以启动多个引导程序进程，每个引导程序进程根据任务信息可以执行不同的任务脚本。

> 我们提供Docker版本的镜像，可快速安装，[点击跳转使用手册](https://stoprefactoring.com/#content@content#framework/hive/)。

> 实际上，一台服务器也可以运行多个监控软件以划分不同的云计算区域。但是，如果有多种云计算区域的划分需要，则更推荐以服务器为单位划分，这样，能更好地规划服务器的物理性能。

- 监控程序：监控程序是python3编写的，主要是监控引导程序，监控程序有以下2个职责。监控程序本身受Supervisor监控，监控程序意外退出后，会自动重启。监控程序意外退出，不影响引导程序运行。

  1. 监控引导程序，监控程序通过Redis的引导程序状态信息监控引导程序，引导程序退出后（完成任务退出或意外退出）会自动启动新的引导程序；
  2. 启动引导程序，监控程序根据配置启动多个引导程序进程，引导程序的相关配置（如RabbitMQ地址、Redis地址等），也是由监控程序传入。

- 引导程序：引导程序是C++编写的，主要是负责一些通用机制，有以下几个职责。引导程序接到任务后，会执行任务脚本。

  1. 接收任务，监听RabbitMQ。引导程序接收到任务和任务处理结束，都会调用Restful-API通知后端应用程序；

  2. 执行任务，默认情况下，引导程序会启动任务脚本（命令行指令）以完成任务。若引导程序意外退出，任务脚本进程也会同步退出；

  3. 接收任务变更指令，监听RabbitMQ。默认情况下，命令变更指令只支持停止任务，引导程序接收到停止命令后，会停止任务脚本。

  4. 报告任务进度或中间结果，引导程序启动任务脚本后，会捕获任务脚本的终端log，并定期更新到Redis中。后端应用程序想要获取任务进度时，只需要根据任务ID读取Redis即可；

  5. 报告引导程序状态，引导程序定期向Redis更新自身状态（如等待任务、执行任务中等状态）。监控程序通过Redis查看本机多个引导程序进程的状态，后端应用程序可以通过Redis获取到云计算集群的繁忙状态。

- 任务脚本：由于引导程序时通过类似于命令行的方式启动任务脚本的，所以原则上任务脚本使用任何语言编写都是允许的。但是，由于一些规范问题，我们建议使用Python3编写，我们也给出了Python3的任务脚本模板。从工作原理上，任务脚本是完全独立的，所以在调试时，是可以直接通过命令行单独调试的。

  > 默认情况下，通过任务脚本的方式能满足大多数场景。如果存在一些特殊场景，需要频繁接收任务变更指令，如接收车辆位置信息描画行程图等，则可以自行对引导程序进行改造，在引导程序内部完成功能。当然，也可联系我们做定制开发。

![](https://github.com/YiiGaa/Hive/blob/main/design/design1.png) 

> 如果单云计算服务器不够需要扩容，则直接添加服务器即可。
