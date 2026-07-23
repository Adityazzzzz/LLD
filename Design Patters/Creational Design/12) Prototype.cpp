#include <iostream>
using namespace std;

/*
    Create new objects by cloning an existing object instead of creating them from scratch
*/

class Car{
public:
    string color;
    Car(string c){
        color = c;
    }
    Car* clone(){
        return new Car(*this);
    }
};

int main(){
    Car car1("Red");
    Car* car2 = car1.clone();
    cout<<car2->color;
}