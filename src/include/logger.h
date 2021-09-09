#pragma once

#include"include/lockQueue.h"
#include<string>
enum LogLevel{
    INFO,
    ERR,
};

class Logger{
public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel level);
    void Log(std::string);
private:
    int m_logLevel;
    lockQueue<std::string> m_lckQueue;  //日志缓冲队列

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

//定义宏

#define LOG_INFO(logmsg,...)\
    do{\
        Logger& ins = Logger::GetInstance();\
        ins.SetLogLevel(INFO);\
        char buf[1024];\
        sprintf(buf,1024,logmsg,##__VA_ARGS__);\
        ins.Log(buf);\
    }while(0)


#define LOG_ERR(logmsg,...)\
    do{\
        Logger& ins = Logger::GetInstance();\
        ins.SetLogLevel(ERR);\
        char buf[1024];
        sprintf(buf,1024,logmsg,##__VA__ARGS__);\
        ins.Log(buf);\
    }while(0)






