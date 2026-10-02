#include "../formatter/LogFormatter.cpp"

class LogAppender{
protected:
    LogFormatter* formatter;

public:
    LogAppender(LogFormatter* formatter){
        this->formatter = formatter;
    }

    virtual void append(LogMessage log)=0;
    virtual ~LogAppender(){}
};