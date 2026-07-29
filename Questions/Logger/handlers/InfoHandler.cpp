#include "LogHandler.cpp"

class InfoHandler : public LogHandler{
public:
    bool canHandle(LogLevel level) override{
        return level == LogLevel::INFO;
    }
};