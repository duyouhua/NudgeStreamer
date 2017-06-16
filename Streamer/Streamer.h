/*
 * 界面类，有信号槽的绑定，
 * 重绘事件和关闭事件的重
 * 写，各个模块插件的初始
 * 化等
 */

#ifndef _STREAMER_H
#define _STREAMER_H

#include <QMainWindow>

class PushFlow;
class VideoEncode;
class VideoDisplay;
class NetworkQuality;
class InterceptScreen;
struct NetworkParament;

namespace Ui
{
class Streamer;
}

class Streamer : public QMainWindow
{
    Q_OBJECT

public:
    explicit Streamer(QWidget* parent = 0);
    ~Streamer();

public:
    void InitialPlugins();// 初始化插件 Initial Plugins

protected:
    // 重写画面绘制事件，当视频显示模块发送图片过来时就会进行一次重新绘制并进行显示
    void paintEvent(QPaintEvent* event);
    // 重写关闭事件，先关闭子线程，释放子线程资源，再关闭父线程，防止程序崩溃
    void closeEvent(QCloseEvent* event);

private slots:
    void HandleError(const QString& err);// 异常处理信号
    void ShowJitter(const NetworkParament& parament);// 网络质量信息显示
    void DisplayImage(const QImage& image);// 获取图片进行视频播放

private slots:
    void on_GdiGrabAction_triggered();// 菜单栏Gdi抓屏的槽函数
    void on_TcpingAction_triggered();// 菜单栏Tcping的槽函数
    void on_PicSendUIAction_triggered();// 菜单栏PicSendUI的槽函数
    void on_SDLAction_triggered();// 菜单栏SDL的槽函数
    void on_AvEncodeAction_triggered();// 菜单栏AvEncode的槽函数
    void on_RtmpPushAction_triggered();// 菜单栏RtmpFFmpeg的槽函数
    void on_StartButton_clicked();// 菜单栏开始推流按钮的槽函数
    void on_StopButton_clicked();// 菜单栏停止推流按钮的槽函数

private:
    // 截屏后存储的图片宽和高
    const int DEFAULT_WIDTH;
    const int DEFAULT_HEIGHT;

private:
    Ui::Streamer* ui;
    PushFlow* _push;// 视频推流抽象类的指针
    VideoEncode* _encode;// 视频编码抽象类的指针
    VideoDisplay* _display;// 视频显示抽象类的指针
    NetworkQuality* _network;// 网络计算模块抽象类的指针
    InterceptScreen* _catpure;// 截屏模块抽象类的指针
    QImage image;// 记录当前的图像
};

#endif // _STREAMER_H
