#include "LogFormatter.cpp"

class JsonFormatter:public LogFormatter{
public:
    string format(LogMessage log) override{
        return "{ \"message\" : \"" + log.message + "\" }";
    }
};