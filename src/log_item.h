#ifndef LOG_ITEM_H
#define LOG_ITEM_H

#include <QString>

enum class LogItemType
{
    Log,
    Clear,
};

enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
};

class LogItem
{
public:
    LogItemType Type;
    LogLevel Level;
    double Timestamp;
    QString Source;
    QString Message;
};

#endif // LOG_ITEM_H
