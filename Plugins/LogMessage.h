#ifndef _LOG_MESSAGE_H
#define _LOG_MESSAGE_H

namespace Logger
{
    struct LogMessage
    {
        int log_level;
        std::string class_name;
        std::string message;
        std::string time;

        LogMessage() {}

        LogMessage(const int& level, const std::string& name, const std::string& msg, const std::string& time) :
            log_level(level),
            class_name(name),
            message(msg),
            time(time)
        {
        }
    };
}

#endif // _LOG_MESSAGE_H
