#include "LogHandler.cpp"

class ErrorHandler : public LogHandler{
public:
    bool canHandle(LogLevel level) override{
        return level == LogLevel::ERROR;
    }
};