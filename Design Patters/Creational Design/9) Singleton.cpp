#include <iostream>
using namespace std;

/*
    Ensure only one object of a class exists and provide a global access point.

    Real-life Example
        Printer Spooler
        Logger
        Database Connection
        Configuration Manager
Only one instance should exist.

*/

class Logger{
private:
    Logger(){}
public:
    static Logger& getInstance(){
        static Logger obj;
        return obj;
    }
    void log(){
        cout<<"Logging...";
    }
};

int main(){
    Logger::getInstance().log();
}