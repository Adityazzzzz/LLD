#include <iostream>
using namespace std;

/*
    Factory creates objects without exposing the object creation logic to the client
*/

class Shape{
public:
    virtual void draw() = 0;
};

class Circle : public Shape{
public:
    void draw() override{
        cout<<"Circle";
    }
};

class Square : public Shape{
public:
    void draw() override{
        cout<<"Square";
    }
};

class ShapeFactory{
public:
    static Shape* createShape(string type){
        if(type == "Circle") return new Circle();
        return new Square();
    }
};

int main(){
    Shape* s = ShapeFactory::createShape("Circle");
    s->draw();
}