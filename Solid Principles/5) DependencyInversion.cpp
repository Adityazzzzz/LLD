#include <iostream>
using namespace std;

// "High-level modules should not depend on low-level modules. Both should depend on abstractions"
/*
    Bad Design
*/

class Keyboard{
public:
    void type(){
        cout << "Typing using Keyboard";
    }
};
class Computer{
    Keyboard keyboard;

public:
    void work(){
        keyboard.type();
    }
};
int main(){
    Computer c;
    c.work();
}


// -------------------------------------------------------------------------------------------
/*
    Good Design
*/

#include <iostream>
using namespace std;

class Keyboard{
public:
    virtual void type() = 0;
    virtual ~Keyboard(){}
};
class WiredKeyboard : public Keyboard{
public:
    void type() override{
        cout << "Typing using Wired Keyboard";
    }
};
class WirelessKeyboard : public Keyboard{
public:
    void type() override{
        cout << "Typing using Wireless Keyboard";
    }
};

class Computer{
    Keyboard* keyboard;

public:
    Computer(Keyboard* k){
        keyboard = k;
    }
    void work(){
        keyboard->type();
    }
};

int main(){
    WiredKeyboard wired;
    WirelessKeyboard wireless;

    Computer c1(&wired);
    c1.work();

    Computer c2(&wireless);
    c2.work();

    return 0;
}