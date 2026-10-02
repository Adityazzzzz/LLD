#include "LogFormatter.cpp"

class JsonFormatter : public LogFormatter{
public:
    string format(LogMessage log) overrid{
        return "{ \"message\" : \"" + log.message + "\" }";
    }
};