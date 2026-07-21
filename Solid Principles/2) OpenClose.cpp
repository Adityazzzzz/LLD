#include <iostream>  // Payment
using namespace std;

/*
    Bad Design 
*/
class Payment{
public:
    void pay(string method){
        if (method == "CreditCard"){
            cout<<"Paid using Credit Card";
        }
        else if (method == "UPI"){
            cout<<"Paid using UPI";
        }
    }
};
int main(){
    Payment payment;
    payment.pay("UPI");
}

// --------------------------------------------------------------------------------------------
/*
    Good Design - Employee has 1 responsibility
*/


class Payment{
public:
    virtual void pay() = 0;
};

class CreditCard : public Payment{
public:
    void pay() override{
        cout<<"Paid using Credit Card";
    }
};

class UPI : public Payment{
public:
    void pay() override{
        cout<<"Paid using UPI";
    }
};

int main(){
    Payment* payment;
    payment = new UPI();
    payment->pay();
    delete payment;
    return 0;
}