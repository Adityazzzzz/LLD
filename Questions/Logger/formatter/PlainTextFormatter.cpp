#include "LogFormatter.cpp"

class PlainTextFormatter : public LogFormatter{
public:
    string format(LogMessage log) override{
        return log.message;
    }
};