/*
 * 各个模块抛出的异常以及其他相关信息，
 * 都会由该对象转抛给界面层进行处理
 */

#ifndef _DATA_TO_UI_H
#define _DATA_TO_UI_H

#include "NetworkParament.h"

#include <QImage>
#include <QObject>

class DataToUI : public QObject
{
    Q_OBJECT
public:
    static DataToUI& GetInstance()
    {
        static DataToUI _instance;
        return _instance;
    }

    // 传入异常消息
    void setParament(const QString& err)
    {
        emit sendError(err);
    }

    // 传入图片信息
    void setParament(const QImage& image)
    {
        emit sendImage(image);
    }

    // 传入网络质量参数
    void setParament(const NetworkParament& parament)
    {
        emit sendJitter(parament);
    }

signals:
    void sendImage(QImage);// 发送图片信息的信号
    void sendError(QString);// 发送异常信息的信号
    void sendJitter(NetworkParament);// 发送网络质量参数信息的信号

private:
    DataToUI(const DataToUI&) = delete;
    DataToUI& operator=(const DataToUI&) = delete;
    DataToUI() {}
    ~DataToUI() {}
};

#endif // _DATA_TO_UI_H
