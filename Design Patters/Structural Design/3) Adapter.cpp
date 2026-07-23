#include <iostream>
using namespace std;

/*
    Adapter Pattern allows two incompatible interfaces to work together.
*/

class IPhoneCharger{
public:
    virtual void charge() = 0;
    virtual ~IPhoneCharger(){}
};

class AndroidCharger{
public:
    void chargeWithUSB(){
        cout<<"Charging with Android Charger";
    }
};

class ChargerAdapter : public IPhoneCharger{
    AndroidCharger android;
public:
    void charge() override{
        android.chargeWithUSB();
    }
};

int main(){
    IPhoneCharger* charger = new ChargerAdapter();
    charger->charge();
    delete charger;
}