#include <iostream>
using namespace std;

/*
    Pass a request along a chain of handlers until one handles it.
    
    Level 1
      ↓
    Level 2
      ↓
    Manager
*/

class Handler{
protected:
    Handler* next = nullptr;
public:
    void setNext(Handler* h){
        next = h;
    }
    virtual void handle(int level){
        if(next){
            next->handle(level);
        }
    }
};

class Level1 : public Handler{
public:
    void handle(int level) override{
        if(level==1) cout<<"Handled by Level 1";
        else if(next) next->handle(level);
    }
};

class Level2 : public Handler{
public:
    void handle(int level) override{
        if(level==2) cout<<"Handled by Level 2";
        else if(next) next->handle(level);
    }
};

int main(){
    Level1 l1;
    Level2 l2;
    l1.setNext(&l2);
    l1.handle(2);
}