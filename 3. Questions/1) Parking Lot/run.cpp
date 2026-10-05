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
    string getNumber(){ return number; }
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

    Ticket* processVehicle(Vehicle* vehicle){
        cout << vehicle->getNumber() << endl;
        return lot->parkVehicle(vehicle); 
    }
};
class ExitGate:public Gate{
public:
    ExitGate(int id,ParkingLot* lot) : Gate(id,lot){}

    double processTicket(Ticket* ticket,PaymentMethod* payment){
        cout << ticket->getId() <<endl;
        return lot->exitVehicle(ticket,payment);
    }
};

class Floor{
private:
    int id;
    map<int,ParkingSpot*> spots;
public:
    Floor(int id){
        this->id = id;
    }

    void addSpot(ParkingSpot* spot){
        spots[spot->getId()] = spot;
    }
    ParkingSpot* findAvailableSpot(Vehicle* vehicle){
        for(auto const& [spotId,spot] : spots){
            if(spot->canFit(vehicle)){
                return spot;
            }
        }
        return nullptr;
    }
};

class ParkingSpot{
protected:
    int id;
    VehicleType type;
    bool occupied;
    Vehicle* vehicle;
public:
    ParkingSpot(int id,VehicleType type){
        this->id = id;
        this->type = type;
        this->occupied = false;
        this->vehicle = NULL;
    }
    
    bool canFit(Vehicle* v){
        return !occupied && v->getType() == type;
    }
    void park(Vehicle* v){
        vehicle = v;
        occupied = true;
    }
    void freeSpot(){
        vehicle = nullptr;
        occupied = false;
    }
    int getId(){ return id; }
};

class Ticket{
private:
    int id;
    Vehicle* vehicle;
    ParkingSpot* spot;
    time_t entryTime;
    double fee;
public:
    Ticket(int id,Vehicle* vehicle,ParkingSpot* spot) 
        : id(id),vehicle(vehicle),spot(spot),fee(0){
        entryTime = time(nullptr);
    }

    void close(double calculatedFee){
        this->fee = calculatedFee;
    }

    int getId(){ return id; }
    Vehicle* getVehicle(){ return vehicle; }
    ParkingSpot* getSpot(){ return spot; }
    time_t getEntryTime(){ return entryTime; }
};

//strategy
class PricingStrategy{
public:
    virtual ~PricingStrategy() = default;
    virtual double calPrice(int hours,VehicleType type) = 0;
};
class DurationBasedPrice:public PricingStrategy{
public:
    double calPrice(int hours,VehicleType type) override{
        double rate = 0.0;
        switch(type){
            case VehicleType::BIKE: rate = 10.0; break;
            case VehicleType::CAR: rate = 20.0; break;
            case VehicleType::TRUCK: rate = 30.0; break;
        }
        int billableHours =(hours < 1) ? 1 : hours;
        
        return billableHours * rate;
    }
};
class EventBasedPrice:public PricingStrategy{
    //-------------
};

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
    map<int,Floor*> floors;
    PricingStrategy* pricingStrategy;
    int ticketCounter = 1;

public:
    ParkingLot(PricingStrategy* strategy) : pricingStrategy(strategy){}

    void addFloor(Floor* floor){
        // Assuming floor IDs are 1, 2, 3...
        floors[floors.size() + 1] = floor; 
    }

    Ticket* parkVehicle(Vehicle* vehicle){
        for(auto const& [floorId, floor] : floors){
            ParkingSpot* spot = floor->findAvailableSpot(vehicle);
            if(spot != nullptr){
                spot->park(vehicle);
                Ticket* ticket = new Ticket(ticketCounter++, vehicle, spot);
                cout << "Parked at Spot: " << spot->getId() << "\n";
                return ticket;
            }
        }
        cout << "No spots available.\n";
        return nullptr;
    }

    double exitVehicle(Ticket* ticket, PaymentMethod* payment){
        // Mocking 2 hours for the interview output
        int hoursParked = 2; 
        
        double price = pricingStrategy->calPrice(hoursParked, ticket->getVehicle()->getType());
        ticket->close(price);
        
        payment->pay(price);
        ticket->getSpot()->freeSpot();
        
        cout << "Spot " << ticket->getSpot()->getId() << " is now free.\n";
        return price;
    }
};

int main() {
    // 1. Initialize Strategy and Main System
    PricingStrategy* standardPricing = new DurationBasedPrice();
    ParkingLot* lot = new ParkingLot(standardPricing);

    // 2. Build Infrastructure (Floors and Spots)
    Floor* floor1 = new Floor(1);
    floor1->addSpot(new ParkingSpot(101, VehicleType::BIKE));
    floor1->addSpot(new ParkingSpot(102, VehicleType::CAR));
    floor1->addSpot(new ParkingSpot(103, VehicleType::CAR));

    Floor* floor2 = new Floor(2);
    floor2->addSpot(new ParkingSpot(201, VehicleType::TRUCK));

    lot->addFloor(floor1);
    lot->addFloor(floor2);

    // 3. Initialize Gates
    EntryGate* entryGate = new EntryGate(1, lot);
    ExitGate* exitGate = new ExitGate(2, lot);

    // 4. Create Vehicles using your Factory
    Vehicle* myCar = VehicleFactory::func(VehicleType::CAR, "MP-09-AB-1234");
    Vehicle* myTruck = VehicleFactory::func(VehicleType::TRUCK, "MP-09-XY-9876");

    // 5. Execute Entry Flow
    cout << "--- ENTRY ---\n";
    Ticket* carTicket = entryGate->processVehicle(myCar);
    Ticket* truckTicket = entryGate->processVehicle(myTruck);

    // 6. Execute Exit Flow with Strategy/Payment Processing
    cout << "\n--- EXIT ---\n";
    if (carTicket != nullptr) {
        PaymentMethod* cardPayment = new CardPayment();
        exitGate->processTicket(carTicket, cardPayment);
    }

    return 0;
}