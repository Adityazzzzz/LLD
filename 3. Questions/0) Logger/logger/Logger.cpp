#include "../handlers/LogHandler.cpp"
#include "../models/LogMessage.cpp"
#include <chrono>
using namespace std;

class Logger{
private:
    LogHandler* handler;

    Logger(){
        handler = nullptr;
    }

public:
    static Logger& getInstance(){
        static Logger logger;
        return logger;
    }

    void setHandler(LogHandler* h){
        handler = h;
    }

    long getCurrentTime(){
        return chrono::system_clock::to_time_t(
            chrono::system_clock::now()
        );
    }

    void log(LogLevel level, string message){
        LogMessage log(level, message, getCurrentTime());

        if(handler){
            handler->handle(log);
        }
    }

    void info(string message){
        log(LogLevel::INFO, message);
    }

    void warn(string message){
        log(LogLevel::WARN, message);
    }

    void error(string message){
        log(LogLevel::ERROR, message);
    }
};