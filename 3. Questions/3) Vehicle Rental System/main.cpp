#include <iostream>
using namespace std;

// ==========================================
// ENUMS
// ==========================================
enum class VehicleType{ BIKE, CAR, TRUCK };

// ==========================================
// 1. VEHICLE ENTITIES & FACTORY
// ==========================================
class Vehicle{
protected:
    string number;
    bool isBooked;
public:
    Vehicle(string number){
        this->number = number;
        this->isBooked = false;
    }
    virtual ~Vehicle() = default;

    virtual VehicleType getType() = 0;
    string getNumber(){ return number; }
    
    bool getIsBooked(){ return isBooked; }
    void setIsBooked(bool status){ this->isBooked = status; }
};

class Car : public Vehicle{
public:
    Car(string number) : Vehicle(number){}
    VehicleType getType() override{
        return VehicleType::CAR;
    }
};

class Bike : public Vehicle{
public:
    Bike(string number) : Vehicle(number){}
    VehicleType getType() override{
        return VehicleType::BIKE;
    }
};

class Truck : public Vehicle{
public:
    Truck(string number) : Vehicle(number){}
    VehicleType getType() override{
        return VehicleType::TRUCK;
    }
};

class VehicleFactory{
public:
    static Vehicle* func(VehicleType type, string number){
        switch (type){
            case VehicleType::BIKE: return new Bike(number);
            case VehicleType::CAR: return new Car(number);
            case VehicleType::TRUCK: return new Truck(number);
            default: return nullptr;
        }
    };
};

// ==========================================
// 2. PRICING STRATEGY
// ==========================================
class PricingStrategy{
public:
    virtual ~PricingStrategy() = default;
    virtual double calPrice(int hours, VehicleType type) = 0;
};

class DurationBasedPrice : public PricingStrategy{
public:
    double calPrice(int hours, VehicleType type) override{
        double rate = 0.0;
        switch (type){
            case VehicleType::BIKE: rate = 10.0; break;
            case VehicleType::CAR: rate = 20.0; break;
            case VehicleType::TRUCK: rate = 30.0; break;
        }
        int billableHours = (hours < 1) ? 1 : hours;
        return billableHours * rate;
    }
};

// ==========================================
// 3. PAYMENT STRATEGY
// ==========================================
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

// ==========================================
// 4. BRANCH & BOOKING ENTITIES
// ==========================================
class Branch{
private:
    string name;
    unordered_map<string, Vehicle*> vehicles; 
public:
    Branch(string name){
        this->name = name;
    }
    
    void addVehicle(Vehicle* vehicle){
        this->vehicles[vehicle->getNumber()] = vehicle;
    }
    
    unordered_map<string, Vehicle*>& getVehicles(){
        return vehicles;
    }
    
    string getName(){ return name; }
};

class Booking{
private:
    string bookingId;
    Vehicle* vehicle;
    int hours;
    double amount;
    bool isActive;
public:
    Booking(string id, Vehicle* v, int h, double amt){
        this->bookingId = id;
        this->vehicle = v;
        this->hours = h;
        this->amount = amt;
        this->isActive = true;
    }
    
    Vehicle* getVehicle(){ return vehicle; }
    double getAmount(){ return amount; }
    void completeBooking(){ this->isActive = false; }
};

// ==========================================
// 5. BOOKING STRATEGY
// ==========================================
class BookingStrategy{
public:
    virtual ~BookingStrategy() = default;
    virtual Vehicle* findVehicle(Branch* branch, VehicleType type) = 0;
};

// Returns the first available vehicle of the requested type
class DefaultBookingStrategy : public BookingStrategy{
public:
    Vehicle* findVehicle(Branch* branch, VehicleType type) override{
        for (auto const& [number, vehicle] : branch->getVehicles()){
            if(vehicle->getType() == type && !vehicle->getIsBooked()){
                return vehicle;
            }
        }
        return nullptr;
    }
};

// ==========================================
// 6. BOOKING SERVICE (MANAGER)
// ==========================================
class BookingService{
private:
    unordered_map<string, Branch*> branches;
    unordered_map<string, Booking*> bookings;
    
    PricingStrategy* pricingStrategy;
    BookingStrategy* bookingStrategy;
    int bookingCounter;

public:
    BookingService(PricingStrategy* pStrategy, BookingStrategy* bStrategy){
        this->pricingStrategy = pStrategy;
        this->bookingStrategy = bStrategy;
        this->bookingCounter = 1;
    }

    void addBranch(Branch* branch){
        this->branches[branch->getName()] = branch;
    }

    Booking* bookVehicle(string branchName, VehicleType type, int hours){
        if(branches.find(branchName) == branches.end()){
            cout << "Branch not found.\n";
            return nullptr;
        }

        Branch* branch = branches[branchName];
        Vehicle* vehicle = bookingStrategy->findVehicle(branch, type);

        if(vehicle != nullptr){
            vehicle->setIsBooked(true);
            double price = pricingStrategy->calPrice(hours, type);
            
            string bId = "BKG-" + to_string(bookingCounter++);
            Booking* booking = new Booking(bId, vehicle, hours, price);
            this->bookings[bId] = booking;
            
            cout << "Booking successful! ID: " << bId << " | Vehicle: " << vehicle->getNumber() << " | Cost: Rs." << price << "\n";
            return booking;
        }
        
        cout << "No available vehicles of this type at " << branchName << ".\n";
        return nullptr;
    }

    void returnVehicle(string bookingId, PaymentMethod* payment){
        if(bookings.find(bookingId) != bookings.end()){
            Booking* booking = bookings[bookingId];
            booking->getVehicle()->setIsBooked(false);
            booking->completeBooking();
            
            cout << "Processing return for " << bookingId << "...\n";
            payment->pay(booking->getAmount());
            cout << "Vehicle " << booking->getVehicle()->getNumber() << " successfully returned.\n";
        } else{
            cout << "Invalid Booking ID.\n";
        }
    }
};

// ==========================================
// DRIVER CODE
// ==========================================
int main(){
    PricingStrategy* pricing = new DurationBasedPrice();
    BookingStrategy* bookingStrat = new DefaultBookingStrategy();

    BookingService* service = new BookingService(pricing, bookingStrat);

    Branch* downtown = new Branch("Downtown");
    downtown->addVehicle(VehicleFactory::func(VehicleType::CAR, "CAR-123"));
    downtown->addVehicle(VehicleFactory::func(VehicleType::BIKE, "BIKE-999"));
    
    service->addBranch(downtown);

    Booking* b1 = service->bookVehicle("Downtown", VehicleType::CAR, 5);
    Booking* b2 = service->bookVehicle("Downtown", VehicleType::CAR, 2);

    if(b1 != nullptr){
        PaymentMethod* card = new CardPayment();
        service->returnVehicle("BKG-1", card);
    }
    
    Booking* b3 = service->bookVehicle("Downtown", VehicleType::CAR, 2); 

    return 0;
}