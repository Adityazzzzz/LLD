#include "LogAppender.cpp"
#include <iostream>
using namespace std;

class ConsoleAppender : public LogAppender{
public:
    ConsoleAppender(LogFormatter* formatter) : LogAppender(formatter) {}

    void append(LogMessage log) override{
        cout << formatter->format(log) << endl;
    }
};