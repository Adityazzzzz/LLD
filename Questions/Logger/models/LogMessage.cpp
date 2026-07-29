#include <string>
using namespace std;

class LogMessage{
public:
    Loglevel level;
    string message;
    long timestamp;

    LogMessage(LogLevel level,string message,long timestamp){
        this->level = level;
        this->message = message;
        this->timestamp = timestamp;
    }
};