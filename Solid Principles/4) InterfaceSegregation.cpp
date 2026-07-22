#include <iostream>
using namespace std;

// "Clients should not be forced to depend on interfaces they do not use."
/*
    Bad Design
*/

class Machine{
public:
    virtual void print() = 0;
    virtual void scan() = 0;
};
class Printer : public Machine{
public:
    void print() override{
        cout<<"Printing document\n";
    }
    void scan() override{
        cout<<"Printer cannot scan!\n";
    }
};
int main(){
    Printer p;
    p.print();
    p.scan();   // Forced to implement unnecessary function
}



// -------------------------------------------------------------------------------------------
/*
    Good Design
*/

class Printable{
public:
    virtual void print() = 0;
};
class Scannable{
public:
    virtual void scan() = 0;
};

class Printer : public Printable{
public:
    void print() override{
        cout<<"Printing document\n";
    }
};
class Scanner : public Scannable{
public:
    void scan() override{
        cout<<"Scanning document\n";
    }
};

class MultiFunctionPrinter : public Printable, public Scannable{
public:
    void print() override{
        cout<<"Printing document\n";
    }

    void scan() override{
        cout<<"Scanning document\n";
    }
};

int main(){
    Printer p;
    p.print();

    Scanner s;
    s.scan();

    MultiFunctionPrinter m;
    m.print();
    m.scan();

    return 0;
}