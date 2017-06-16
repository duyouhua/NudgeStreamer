#include "Logger.h"
#include "Streamer.h"
#include "ui_Streamer.h"
#include "Factory/PushFactory.h"
#include "Factory/EncodeFactory.h"
#include "Factory/DisplayFactory.h"
#include "Factory/NetworkFactory.h"
#include "Factory/InterceptFactory.h"

#include <QPainter>
#include <QDateTime>
#include <QMessageBox>

Streamer::Streamer(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::Streamer),
    DEFAULT_WIDTH(1280),
    DEFAULT_HEIGHT(720)
{
    ui->setupUi(this);
    setFixedSize(this->width(), this->height());
    ui->StopButton->setEnabled(false);
    setObjectName("Streamer");

    // 注册自定义数据类型
    qRegisterMetaType<QImage>("QImage");
    qRegisterMetaType<NetworkParament>("NetworkParament");
    connect(&DataToUI::GetInstance(), &DataToUI::sendError, this, &Streamer::HandleError, Qt::AutoConnection);
    connect(&DataToUI::GetInstance(), &DataToUI::sendJitter, this, &Streamer::ShowJitter, Qt::AutoConnection);
    connect(&DataToUI::GetInstance(), &DataToUI::sendImage, this, &Streamer::DisplayImage, Qt::AutoConnection);
}

Streamer::~Streamer()
{
    // 关闭客户端时解除信号槽的绑定
    disconnect(&DataToUI::GetInstance(), &DataToUI::sendError, this, &Streamer::HandleError);
    disconnect(&DataToUI::GetInstance(), &DataToUI::sendJitter, this, &Streamer::ShowJitter);
    disconnect(&DataToUI::GetInstance(), &DataToUI::sendImage, this, &Streamer::DisplayImage);

    delete ui;
}

// 初始化插件
void Streamer::InitialPlugins()
{
    /*
     * 界面层持有每个模块抽象类的指针，
     * 界面层利用工厂模式获取每个模块
     * 实现类的对象来进行初始化，并开
     * 启子线程来实现每个模块的功能
     */

    Logger::Log::GetInstance().start();

    Logger::LogMessage* log
        = new Logger::LogMessage
            (
            Logger::INFO,
            objectName().toStdString(),
            "initial successfully",
            QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
            );
    DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

    // 初始化截屏模块
    Catpure::InterceptFactory catpure_factory;
    _catpure = catpure_factory.CreateInterceptClass(std::string("GdiGrab"), DEFAULT_WIDTH, DEFAULT_HEIGHT);
    if (_catpure->Initial())
    {
        _catpure->start();
    }

    // 初始化网络质量测试模块
    Network::NetworkFactory network_factory;
    _network = network_factory.CreateNetworkClass(std::string("Tcping"), std::string("172.18.102.254"), 12678);
    if (_network->Initial())
    {
        _network->start();
    }

    // 初始化视频播放模块
    Display::DisplayFactory display_factory;
    _display = display_factory.CreateDisplayClass(std::string("PicSendUI"), DEFAULT_WIDTH, DEFAULT_HEIGHT, NULL);
    if (_display->Initial())
    {
        _display->start();
    }

    // 初始化视频编码模块
    Encode::EncodeFactory encode_factory;
    _encode = encode_factory.CreateEncodeClass(std::string("AvEncode"), DEFAULT_WIDTH, DEFAULT_HEIGHT);
    if (_encode->Initial())
    {
        _encode->start();
    }

    // 初始化视频推流模块
    Push::PushFactory push_factory;
    _push = push_factory.CreatePushClass("RtmpPush", "rtmp://172.18.102.254:12678/live/text");
    _push->Initial();
}

// 重绘视频显示界面区域
void Streamer::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (this->image.size().width() <= 0)
        return;

    // 将图像按比例缩放成和窗口一样大小
    QImage image = this->image.scaled(ui->VideoDisplay->size(), Qt::KeepAspectRatio);

    int x = ui->VideoDisplay->width() - image.width();
    int y = ui->VideoDisplay->height() - image.height();

    x /= 2;
    y /= 2;

    x += ui->VideoDisplay->x();
    y += ui->VideoDisplay->y();

    painter.drawImage(QPoint(x, y), image);// 画出图像
    QMainWindow::paintEvent(event);
}

// 关闭客户端程序事件
void Streamer::closeEvent(QCloseEvent* event)
{
    /*
     * 当点击关闭按钮关闭客户端时，
     * 先关闭子线程，释放掉子线程
     * 的资源，再关闭父线程并释放
     * 资源；关闭子线程通过改变抽
     * 象类中的bool变量值，每个子
     * 线程运行时都会判断该bool变
     * 量值才进入循环，当程序未关
     * 闭时，该bool变量一直为真，
     * 当程序关闭时将该值置为假，
     * 从而退出循环，关闭线程释放
     * 资源；而每一个子线程都要调
     * 用deleteLater则是因为当将
     * bool变量置为假时，子线程有
     * 可能会在循环中间，强行关闭
     * 则会导致程序崩溃，所以才需
     * 要调用deleteLater
     */

    hide();

    _network->ShutDown();
    _network->quit();
    _network->wait();
    _network->deleteLater();

    _catpure->ShutDown();
    _catpure->quit();
    _catpure->wait();
    _catpure->deleteLater();

    _display->ShutDown();
    _display->quit();
    _display->wait();
    _display->deleteLater();

    _encode->ShutDown();
    _encode->quit();
    _encode->wait();
    _encode->deleteLater();

    if (_push->isRunning())
    {
        _push->StopPush();
    }
    _push->quit();
    _push->wait();
    _push->deleteLater();

    Logger::Log::GetInstance().ShutDown();
    Logger::Log::GetInstance().quit();
    Logger::Log::GetInstance().wait();
    Logger::Log::GetInstance().deleteLater();

    QMainWindow::closeEvent(event);
}

// 异常处理
void Streamer::HandleError(const QString& err)
{
    QMessageBox(QMessageBox::Warning, "Error", err, QMessageBox::Ok).exec();
}

// 网络质量信息显示
void Streamer::ShowJitter(const NetworkParament& parament)
{
    ui->MinValue->setText(QString::number(parament._min) + QString(" ms"));
    ui->MaxValue->setText(QString::number(parament._max) + QString(" ms"));
    ui->DelayValue->setText(QString::number(parament._delay) + QString(" ms"));
    ui->LossValue->setText(QString::number(parament._loss) + " %");
    ui->JitterValue->setText(QString::number(parament._minus_jitter) + " to " + QString::number(parament._plus_jitter) + QString(" ms"));
}

// 获取图片进行视频播放
void Streamer::DisplayImage(const QImage& image)
{
    this->image = image;
    update();// 调用update将执行paintEvent函数
}

// 菜单栏Gdi抓屏的槽函数
void Streamer::on_GdiGrabAction_triggered()
{
    if (ui->GdiGrabAction->isChecked())
    {
        ui->GdiGrabAction->setChecked(true);
    }
    else
    {
        ui->GdiGrabAction->setChecked(true);
    }
}

// 菜单栏Tcping的槽函数
void Streamer::on_TcpingAction_triggered()
{
    if (ui->TcpingAction->isChecked())
    {
        ui->TcpingAction->setChecked(true);
    }
    else
    {
        ui->TcpingAction->setChecked(true);
    }
}

// 菜单栏PicSendUI的槽函数
void Streamer::on_PicSendUIAction_triggered()
{
    if (ui->PicSendUIAction->isChecked())
    {
        ui->PicSendUIAction->setChecked(true);
    }
    else
    {
        ui->PicSendUIAction->setChecked(true);
        ui->SDLAction->setChecked(false);
    }
}

// 菜单栏SDL的槽函数
void Streamer::on_SDLAction_triggered()
{
    if (ui->SDLAction->isChecked())
    {
        ui->SDLAction->setChecked(true);
    }
    else
    {
        ui->SDLAction->setChecked(true);
        ui->PicSendUIAction->setChecked(false);
    }
}

// 菜单栏AvEncode的槽函数
void Streamer::on_AvEncodeAction_triggered()
{
    if (ui->AvEncodeAction->isChecked())
    {
        ui->AvEncodeAction->setChecked(true);
    }
    else
    {
        ui->AvEncodeAction->setChecked(true);
    }
}

// 菜单栏RtmpFFmpeg的槽函数
void Streamer::on_RtmpPushAction_triggered()
{
    if (ui->RtmpPushAction->isChecked())
    {
        ui->RtmpPushAction->setChecked(true);
    }
    else
    {
        ui->RtmpPushAction->setChecked(true);
    }
}

// 菜单栏开始推流按钮的槽函数
void Streamer::on_StartButton_clicked()
{
    _push->StartPush();
    _push->start();

    Logger::LogMessage* log
        = new Logger::LogMessage
            (
            Logger::INFO,
            "Push Module",
            "is starting to push",
            QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
            );
    DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

    ui->StartButton->setEnabled(false);
    ui->StopButton->setEnabled(true);
}

// 菜单栏停止推流按钮的槽函数
void Streamer::on_StopButton_clicked()
{
    _push->StopPush();
    // 当点击停止推流按钮后，执行exit函数线程不会立即停止，线程将会等到完全执行完毕才停止
    _push->exit();
    _push->wait();

    Logger::LogMessage* log
        = new Logger::LogMessage
            (
            Logger::INFO,
            "Push Module",
            "is stoping to push",
            QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
            );
    DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

    ui->StopButton->setEnabled(false);
    ui->StartButton->setEnabled(true);
}
