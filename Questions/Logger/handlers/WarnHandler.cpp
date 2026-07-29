#include "LogHandler.cpp"

class WarnHandler : public LogHandler{
public:

    bool canHandle(LogLevel level) override{
        return level == LogLevel::WARN;
    }
};