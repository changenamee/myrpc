#include"include/logger.h"
#include<thread>
#include<string>
Logger& Logger::GetInstance(){
    static Logger log;
    return log;
}

//设置log等级
void Logger::SetLogLevel(LogLevel level){
    m_logLevel = level;
}

//将log加入log缓存队列
void Logger::Log(std::string str){
    m_lckQueue.Push(str);

}

Logger::Logger(){
    std::thread logWorker(
        [&](){
            for(;;){
                time_t now = time(nullptr);
                tm* cur = localtime(&now);
                //生成日志文件
                char filename[128];
                sprintf(filename,"%d-%d-%d-log.txt",cur->tm_year+1900,cur->tm_mon+1,cur->tm_mday);

                FILE* pf = fopen(filename,"a+");
                if(nullptr == pf){
                    std::cout<<"fail to open log file"<<std::endl;
                    exit(EXIT_FAILURE);
                }

                std::string msg = m_lckQueue.Pop();

                char time_buf[1024];

                sprintf(time_buf,"%d-%d-%d => [%s]",
                    cur->tm_year+1900,
                    cur->tm_mon+1,
                    cur->tm_day,
                    (m_logLevel == INFO?"info":"err")    
                );

                msg.insert(0,time_buf);
                msg.append("\n");

                fputs(msg.c_str(),pf);
                fclose(pf);
            }
        }
    ).detach();
}