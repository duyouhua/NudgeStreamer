#include "Logger.h"
#include "LogMessage.h"
#include "DataBufferPointer.h"

#include <QFile>
#include <QTextStream>

namespace Logger
{
    Log::Log() :
        class_name("Log"),
        shut_down(true)
    {
        DataBufferPointer::GetInstance().GetLog()->RegistCustomer(ClassName());

        _file_log = new QFile("Log");
        _file_log->open(QIODevice::WriteOnly | QIODevice::Text);
    }

    Log::~Log()
    {
        if (_file_log)
        {
            _file_log->close();
            delete _file_log;
            _file_log = NULL;
        }
    }

    void Log::run()
    {
        while (shut_down)
        {
            if (DataBufferPointer::GetInstance().GetLog()->HaveData(ClassName()))
            {
                std::shared_ptr<LogMessage> log = DataBufferPointer::GetInstance().GetLog()->PopTop(ClassName());
                WriteLog(log->log_level, log->class_name, log->message, log->time);
            }
            msleep(1);
        }
    }

    Log& Log::GetInstance()
    {
        static Log inc;
        return inc;
    }

    void Log::WriteLog(const int& level, const std::string& class_name, const std::string& message, const std::string& time)
    {
        QTextStream text(_file_log);
        switch (level)
        {
        case INFO:
            text << "INFO: ";
            break;
        case WARNING:
            text << "WARNING: ";
            break;
        case ERRORINFOR:
            text << "ERROR: ";
            break;
        }

        text << class_name.c_str() << " " << message.c_str() << " at " << time.c_str() << endl;
        text.flush();
    }
}
