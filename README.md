# 桌面推流器 NudgeStreamer
Homepage:https://github.com/Eric7Coding/NudgeStreamer

## 项目简单介绍 Project Introduction
### 中文 Chinese
1. 实现2个显示器的桌面数据采集（扩展屏），采集后的格式为YUV420P
2. 计算网络质量参数，网络影响包含抖动、延时的影响
3. 编码采集到的数据为H264
4. 根据网络质量参数，动态调整编码码率及效果
5. 以rtmp协议推送到提供的流媒体服务器
6. 使用第三方播放器能播放推流的视频
7. 简单的GUI

### English

## 各文件夹简单介绍
### 中文 Chinese
- bin：项目生成文件
- lib：项目用到的第三方库的头文件以及库文件，lib下有各种非自行编译的第三方库的32位以及64位的库，默认为64位
- Log：项目日志功能模块
- Plugins：项目框架以及模块实现方式生成的库
- Streamer：界面层有关的源代码

注:
- lib下第三方库某些库需要自行编译，如librtmp
- 本程序非自行编译的第三方库均是64位的库，在32位环境下编译请将第三方库换成32位对应的库

### English

## 序列图 Sequence Diagram
![Sequence Diagram](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/NudgeStreamer_Sequence_Diagram.png)

## 分层架构图 Layer Architecture
![Layer Architecture](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/NudgeStreamer_Layer_Architecture.png)

## 数据流图 Data-Flow Graph
### 软件数据流图 NudgeStreamer Data-Flow Graph
![NudgeStreamer Data-Flow Graph](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/NudgeStreamer_Data-Flow_Graph.png)

### 异常数据流图 Parament Data-Flow Graph
![Parament Data-Flow Graph](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Error_Data-Flow_Graph.png)

### 日志数据流图 Log Data-Flow Graph
![Log Data-Flow Graph](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Log_Data-Flow_Graph.png)

## 