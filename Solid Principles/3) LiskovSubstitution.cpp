#include <iostream>
using namespace std;

// "A derived class should be replaceable with its base class without changing the correctness of the program."
/*
    Bad Design
*/

class Bird{
public:
    virtual void fly(){
        cout << "Bird is flying\n";
    }
};

class Penguin : public Bird{
public:
    void fly() override{
        throw runtime_error("Penguins can't fly!");
    }
};

int main(){
    Bird* bird = new Penguin();

    bird->fly();   // ❌ Unexpected behavior

    delete bird;
}

// -------------------------------------------------------------------------------------------
/*
    Good Design
*/


class Bird{
public:
    virtual void eat(){
        cout << "Bird is eating\n";
    }

    virtual ~Bird(){}
};

// Only flying birds inherit this
class FlyingBird : public Bird{
public:
    virtual void fly(){
        cout << "Flying in the sky\n";
    }
};

class Sparrow : public FlyingBird{
public:
    void fly() override{
        cout << "Sparrow is flying\n";
    }
};

class Penguin : public Bird{
public:
    // No fly() function
};

int main(){
    FlyingBird* bird = new Sparrow();
    bird->fly();

    Bird* penguin = new Penguin();
    penguin->eat();

    delete bird;
    delete penguin;

    return 0;
}