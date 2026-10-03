#include <iostream>
using namespace std;

//enums
enum class VehicleType{
    CAR,BIKE,TRUCK
};
//entities
class Vehicle{
protected:
    string number;
public:
    Vehicle(string number){
        this->number = number;
    }
    virtual ~Vehicle() = default;

    virtual VehicleType getType() = 0;
    string getnumber(){ return number };
};

class Car:public Vehicle{
public:
    Car(string number) : Vehicle(number){}
    VehicleType getType(){
        return VehicleType::CAR;
    }
};
class Bike:public Vehicle{
public:
    Bike(string number) : Vehicle(number){}
    VehicleType getType(){
        return VehicleType::BIKE;
    }
};
class Truck:public Vehicle{
public:
    Truck(string number) : Vehicle(number){}
    VehicleType getType(){
        return VehicleType::TRUCK;
    }
};

class Gate{
protected:
    int gateId;
    ParkingLot* lot;
public:
    Gate(int id,ParkingLot* lot){
        this->gateId = id;
        this->lot = lot;
    }
    virtual ~Gate() = default;
};
class EntryGate:public Gate{
public:
    EntryGate(int id,ParkingLot* lot) : Gate(id,lot){}

    Ticket* processVehicle(Vehicle* vehicle) {
        cout << vehicle->getNumber() << endl;
        return lot->parkVehicle(vehicle); 
    }
}
class ExitGate:public Gate{
    ExitGate(int id,ParkingLot* lot) : Gate(id,lot){}

    double processTicket(Ticket* ticket, PaymentMethod* payment) {
        cout << ticket->getId() <<endl;
        return lot->exitVehicle(ticket, payment);
    }
}

class Floors{
    //have multiple spots
        map<int,Spots*> mpp;
}
class Spots{
    //accordint to fixed videhicele
}
class Ticket{
// independent class
}

//strategy
class PricingStrategy{
public:
    virtual ~PricingStrategy() = default;
    virtual double calPrice(int hours,VehicleType type) = 0;
}
class DurationBasedPrice:public PricingStrategy{
public:
    double calPrice(int hours,VehicleType type) override{
        double rate = 0.0;
        switch(type){
            case VehicleType::BIKE: rate = 10.0; break;
            case VehicleType::CAR: rate = 20.0; break;
            case VehicleType::TRUCK: rate = 30.0; break;
        }
        int billableHours = (hours < 1) ? 1 : hours;
        
        return billableHours * rate;
    }
}
class EventBasedPrice:public PricingStrategy{
    //-------------
}

class PaymentMethod{
public:
    virtual void pay(double amount) = 0;
    virtual ~PaymentMethod(){}
};
class CashPayment : public PaymentMethod{
public:
    void pay(double amount) override{
        cout << "Paid Rs. " << amount << " using Cash\n";
    }
};
class CardPayment : public PaymentMethod{
public:
    void pay(double amount) override{
        cout << "Paid Rs. " << amount << " using Card\n";
    }
};

//factory
class VehicleFactory{
public:
    static Vehicle* func(VehicleType type,string number){
        switch(type){
            case VehicleType::BIKE: return new Bike(number);
            case VehicleType::CAR: return new Car(number);
            case VehicleType::TRUCK: return new Truck(number);
            default:
                break;
        }
    }
};
//manager
class ParkingLot{
private:
    //have multiple floors
    map<int,Floors*> mpp;
public:

}