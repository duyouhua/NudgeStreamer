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
- Pic：存放关于项目的序列图、分层架构图以及数据流图等
- lib：项目用到的第三方库的头文件以及库文件，lib下有各种非自行编译的第三方库的32位以及64位的库，默认为64位
- Log：项目日志功能模块
- Plugins：项目框架以及模块实现方式生成的库
- Streamer：界面层有关的源代码

注:
- lib下第三方库某些库需要自行编译，如librtmp
- 本程序非自行编译的第三方库均是64位的库，在32位环境下编译请将第三方库换成32位对应的库

### English

## 序列图 Sequence Diagram
![Sequence Diagram](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Pic/NudgeStreamer_Sequence_Diagram.png)

## 分层架构图 Layer Architecture
### 中文 Chinese
![Layer Architecture](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Pic/NudgeStreamer_Layer_Architecture.png)

### English
![Layer Architecture Englis](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Pic/NudgeStreamer_Layer_Architecture_English.png)

## 数据流图 Data-Flow Graph
### 软件数据流图 NudgeStreamer Data-Flow Graph
![NudgeStreamer Data-Flow Graph](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Pic/NudgeStreamer_Data-Flow_Graph.png)

### 异常或参数数据流图 Parament Data-Flow Graph
![Parament Data-Flow Graph](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Pic/Parament_Data_Flow_Graph.png)

### 日志数据流图 Log Data-Flow Graph
![Log Data-Flow Graph](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Pic/Log_Data-Flow_Graph.png)

## 主要实现思路
### 中文 Chinese
![Connection for UI and Logic Layer in Chinese](https://raw.githubusercontent.com/Eric7Coding/NudgeStreamer/master/Pic/Connection_for_UI_and_Logic_Layer_in_Chinese.png)

### English

## 各个模块的实现
### 截屏模块
InterceptScreen抽象类，继承QThread
成员：bool shut_down，控制线程的变量

方法
公有：void ShutDown，将成员变量置为false
保护：void ThrowError(QString)，发送错误信息到界面层
公有接口：void Initial，初始化

命名空间Catpure的GdiGrab类，继承InterceptScreen抽象类，实现截屏模块功能，实现Initial以及run方法
该模块功能主要运用ffmpeg库调用GDI的方式实现截屏功能，截屏后将会将数据存到PicYUV自定义结构体中，并放入消息队列供其他线程使用

#### struct PicYUV
PicYUV，存放图片数据的自定义结构体
成员：NUM_DATA_POINTERS = 3、width、height、linesize数组、data数组
方法：无参与有参构造函数、析构函数
无参构造函数在使用前必须给data开辟内存空间
析构主要做data内存的释放        

### 网络质量计算木块
NetworkQuality抽象类，继承QThread
成员：bool shut_down，控制线程的变量

方法
公有：void ShutDown，将成员变量置为false
保护：void ThrowError(QString)，发送错误信息到界面层
void SendMessage(NetworkParament)，发送网络质量参数到界面层
公有接口：void Initial，初始化

命名空间Network的TcpTest类，继承NetworkQuality抽象类，实现网络质量计算模块功能，实现Initial以及run方法
该模块功能主要运用Qt库中的QTcpSocket进行TCP连接，每次测量均会连接40次，经历40次连接与断开，每次只会计算TCP三次握手的时间，在这40次连接中会得到最短连接时间、最长连接时间、平均延时、丢包率从而可计算出网络抖动值，每次得出参数后就会发送到界面层进行显示，也会将该数据到消息队列中供视频编码模块使用

#### struct NetworkParament
NetworkParament，自定义存放网络质量参数的结构体
变量：网络连接最短时间、最长时间、平均延时、丢包率、抖动值
方法：NetworkParament
NetworkParament(int, int, int, int) 最小、最大、延时、丢包率
int JitterRange 计算网络抖动区间

### 视频显示模块
VideoDisplay抽象类，继承QThread
成员：bool shut_down，控制线程的变量
string class_name，表示该类名字

方法
构造函数中将会向消息队列进行注册该模块是PicYUV数据的消费者
公有：void ShutDown，将成员变量置为false
保护：void ThrowError(QString)，发送错误信息到界面层
void ShowImage(QImage)，发送图片到界面层进行显示
string ClassName，获得类名
公有接口：void Initial，初始化

命名空间Display的PicSendUI类，继承VideoDisplay抽象类，实现显示模块功能，实现Initial以及run方法
该模块功能主要是从消息队列中获取图片数据（存放在PicYUV结构体中的数据），获取YUV格式数据后利用ffmpeg库将其转成RGB格式，封装成QImage，传给界面层进行显示

### 视频编码模块
VideoEncode抽象类，继承QThread
成员：bool shut_down，控制线程的变量
string class_name，表示该类名字

方法
构造函数会向消息队列注册该模块是PicYUV以及NetworkParament的消费者
公有：void ShutDown，将成员变量置为false
保护：void ThrowError(QString)，发送错误信息到界面层
string ClassName，获得类名
公有接口：void Initial，初始化

命名空间Encode的AvEncode类，继承VideoEncode抽象类，实现编码模块功能，实现Initial以及run方法

动态调整编码码率的实现
运用享元模式编写一个编码池，在该模块初始化的时候，就会初始化编码池，编码池就会初始化4个编码对象，这四个编码对象的码率都不同，分别对应不同网络质量下的编码码率，每次AvEncode对象都通过编码池来获取不同码率的编码对象
编码池运用单例模式来保证对象的唯一性

该模块的实现主要是在一段时间内从消息队列中获取最新的网络质量参数，判断网络质量后可获取编码对象的ID，通过这个ID可从编码池中获取对应码率的编码对象；该模块的编码使用ffmpeg库对YUV格式的图片进行编码，编码成H264格式，存放到自定义数据结构Frame中去

#### struct Frame
Frame，存放H264格式数据的自定义数据结构
成员主要有size以及data，data是裸数据，size是裸数据的大小
方法主要有无参与有参构造函数以及析构函数
无参构造在使用前必须对data进行内存开辟，析构主要释放内存

### 视频推流模块
PushFlow抽象类，继承QThread
成员：bool isPush，控制线程的变量
string class_name，表示该类名字

方法
构造函数中将会向消息队列进行注册该模块是Frame数据的消费者
公有：void StartPush，将成员变量置为true
void StopPush，将成员变量置为false
保护：void ThrowError(QString)，发送错误信息到界面层
string ClassName，获得类名
公有接口：void Initial，初始化

命名空间Push的RtmpPush类，继承PushFlow抽象类，实现推流模块功能，实现Initial以及run方法
该模块主要运用librtmp库来进行H264格式数据的推流，当系统软件初始化的时候，该模块并不会启动线程，而当用户点击“开始推流”按钮时，isPush变量将会变为true，此时将会启动线程进行推流，当用户点击“停止推流”时，isPush变量将会变为false，线程将会调用exit()，此时线程不会立即停止，而是等到完全执行完run函数才会停止

### Log
Log 单例类 继承QThread 线程类		
成员：bool shut_down  控制线程的变量		
QFile _file_log  打开关闭日志文件		
string class_name  类名		

方法
构造函数中将会向消息队列进行注册该模块是LogMessage数据的消费者
公有：void ShutDown  将成员变量置为false
string ClassName 获得类名
私有：void WriteLog(int, string, string, string)  参数是日志等级，类名，日志信息，时间

命名空间Logger中的Log类，是个线程类，在客户端初始化时就会对该线程进行初始化，初始化时就会打开日志文件，并一直保持开着的状态，其他模块需要写日志时会将日志信息存入LogMessage结构体中，并放入消息队列，该线程类会在一段时间内从消息队列中获取日志信息并写入日志文件，当客户端关闭时，将会关闭日志文件。

#### enum LogLevel
LogLevel  枚举 日志等级信息	
成员主要有	
INFO	
WARNING	
ERRORINFO	
三种日志等级	

#### struct LogMessage
LogMessage 结构体 存储日志信息		
成员主要有日志等级、类名、日志信息、时间	

### 消息队列
#### DataBufferQueue
DataBufferQueue  类模板
私有成员：mutex，用于加锁的对象，存放数据以及获取数据都在该类中上锁
map<string, deque<shared_ptr<T>> > 保存数据的映射，用string标记消费者，每个消费者都有属于自己的一条队列，队列中存的是数据
公有方法
void RegistCustomer(string) 消费者在注册时将会进行注册，map变量将会为注册的消费者新开一条属于注册消费者的数据队列
bool HavaData(string) 查看该消费者对应的队列是否有数据
void PushBack(shared_ptr<T>) 生产者存放数据，每当生产者存放数据时，都会对map中的每一条队列都插入数据对应的智能指针
shared_ptr<T> PopTop(string) 根据string先在map中找到消费者的队列，然后再将该队列中的头元素弹出来；而对于NetworkParament数据结构，采用的是弹出尾部元素，再把整条队列的数据清除掉，因为对于其他数据来说获取的是时间比较久的数据，而网络数据需要最新的数据

#### DataBufferPointer
DataBufferPointer 单例类
将DataBufferQueue需要存放的三种数据结构的对象指针封装在该类中，模块每次访问都只能从该类中获取到对应数据消息队列的指针来进行数据的访问；利用单例模式设计保证了对象的唯一性

成员：DataBufferQueue<Frame>*	
DataBufferQueue<PicYUV>*	
DataBufferQueue<NetworkParament>*	
DataBufferQueue<Logger::LogMessage>*	
公有方法主要有获取该类的地址，获取四种数据消息队列的指针；
私有方法主要有构造函数和析构函数，构造函数是对四个成员变量进行初始化，析构函数是对四个指针的内存进行释放

##### 该类存在的必要性
主要解决程序关闭时崩溃的情况
当程序关闭时，程序首先释放DataBufferQueue的内存，这时某些线程可能还没关闭，仍在运行中，这些线程可能刚好从DataBufferQueue中获取数据，由于已经释放掉相应的内存，正在获取数据的线程运用了已经释放掉内存的地址，这时就会导致程序崩溃，所以就写了这个持有几种存放不同数据的DataBufferQueue，等到给单例类释放内存的时候才释放这几个消息队列的内存，相当于延长了消息队列的生命周期，等到其他线程都关闭了，没有线程获取数据了才释放消息队列的内存。

### DataToUI
DataToUI  中间件  单例类  每个模块与UI线程的通信类
主要用于给每个模块发送信息到界面层

自定义信号；
void sendImage(QImage)	
void sendError(QString)	
void sendJitter(NetworkParament)	

公有方法
void setParament(QString)  模块传入错误信息，由该类发送sendError信号到界面层处理 
void setParament(QImage)  主要由显示模块传入图片信息，由该类发送sendImage信号到界面层进行显示
void setParament(NetworkParament)  主要由网络质量计算模块传入网络质量参数，由该类发送sendJitter信号到界			                面层进行显示



