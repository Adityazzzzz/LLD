#include <vector>
#include "../appenders/LogAppender.cpp"
#include "../models/LogMessage.cpp"
using namespace std;

class LogHandler{
protected:
    LogHandler* next = nullptr;
    vector<LogAppender*> appenders;

public:
    void setNext(LogHandler* handler){
        next = handler;
    }
    void addAppender(LogAppender* appender){
        appenders.push_back(appender);
    }

    virtual bool canHandle(LogLevel level)=0;

    void handle(LogMessage log){
        if(canHandle(log.level)){
            for(auto appender : appenders){
                appender->append(log);
            }
        }
        if(next){
            next->handle(log);
        }
    }

    virtual ~LogHandler(){}
};