#include "LogAppender.cpp"
#include <fstream>
using namespace std;

class FileAppender : public LogAppender{
private:
    string fileName;

public:
    FileAppender(LogFormatter* formatter, string fileName) : LogAppender(formatter){
        this->fileName=fileName;
    }

    void append(LogMessage log) override{
        ofstream fout(fileName,ios::app);

        fout<<formatter->format(log)<<endl;

        fout.close();
    }
};