#include <iostream>
using namespace std;

class Payment{
public:
    void pay(){
        cout<<"Payment Done";
    }
};

class Kitchen{
public:
    void prepareFood(){
        cout<<"Food Prepared";
    }
};

class Delivery{
public:
    void deliver(){
        cout<<"Food Delivered";
    }
};

// Facade
class FoodOrderFacade{
    Payment payment;
    Kitchen kitchen;
    Delivery delivery;

public:
    void placeOrder(){
        payment.pay();
        kitchen.prepareFood();
        delivery.deliver();
    }
};

int main(){
    FoodOrderFacade order;
    order.placeOrder();
    return 0;
}