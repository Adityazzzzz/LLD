#include <iostream>
using namespace std;

/*
    Strategy Pattern lets you choose an algorithm (behavior) at runtime.
*/

class PaymentStrategy{
public:
    virtual void pay() = 0;
};

class UPI : public PaymentStrategy{
public:
    void pay() override{
        cout<<"Paid using UPI";
    }
};

class CreditCard : public PaymentStrategy{
public:
    void pay() override{
        cout<<"Paid using Credit Card";
    }
};

// Context
class Payment{
    PaymentStrategy* strategy;
public:
    Payment(PaymentStrategy* s){
        strategy = s;
    }
    void checkout(){
        strategy->pay();
    }
};

int main(){
    UPI upi;
    Payment p(&upi);
    p.checkout();
}