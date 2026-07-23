#include <iostream>
using namespace std;

/*
    Encapsulate a request as an object.
    
    Remote Button
        ↓
      TV ON
*/

class TV{
public:
    void ON(){
        cout<<"TV ON";
    }
};

class Command{
public:
    virtual void execute() = 0;
};

class TVOnCommand : public Command{
    TV* tv;
public:
    TVOnCommand(TV* t){
        tv = t;
    }
    void execute() override{
        tv->ON();
    }
};

class Remote{
    Command* cmd;
public:
    Remote(Command* c){
        cmd = c;
    }
    void pressButton(){
        cmd->execute();
    }
};

int main(){
    TV tv;
    TVOnCommand command(&tv);
    Remote remote(&command);
    remote.pressButton();
}