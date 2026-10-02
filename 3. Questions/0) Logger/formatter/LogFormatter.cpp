#include <string>
#include "../models/LogMessage.cpp"
using namespace std;

class LogFormatter{
public:
    virtual string format(LogMessage message) = 0;

    virtual ~LogFormatter(){}
};