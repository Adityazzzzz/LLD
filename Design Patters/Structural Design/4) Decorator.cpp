#include <iostream>
using namespace std;

/*
    Decorator Pattern allows you to add new functionality to an object dynamically without modifying its class.
*/

// Component
class Coffee{
public:
    virtual void makeCoffee() = 0;
    virtual ~Coffee(){}
};

// Concrete Component
class SimpleCoffee:public Coffee{
public:
    void makeCoffee() override{
        cout<<"Plain Coffee";
    }
};

// Decorator Base Class
class CoffeeDecorator:public Coffee{
protected:
    Coffee* coffee;
public:
    CoffeeDecorator(Coffee* c){
        coffee = c;
    }
};

// Concrete Decorator
class MilkDecorator:public CoffeeDecorator{
public:
    MilkDecorator(Coffee* c) : CoffeeDecorator(c){}

    void makeCoffee() override{
        coffee->makeCoffee();
        cout<<" + Milk";
    }
};

// Another Decorator
class SugarDecorator:public CoffeeDecorator{
public:
    SugarDecorator(Coffee* c) : CoffeeDecorator(c){}

    void makeCoffee() override{
        coffee->makeCoffee();
        cout<<" + Sugar";
    }
};

int main(){
    Coffee* coffee = new SimpleCoffee();
    coffee = new MilkDecorator(coffee);
    coffee = new SugarDecorator(coffee);
    coffee->makeCoffee();
    return 0;
}