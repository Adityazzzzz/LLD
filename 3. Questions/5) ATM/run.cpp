#include <iostream>
using namespace std;

class Card{
public:
    string cardNumber;
    string pin;
    double balance

    Card(string c,sting p,double b){
        this->cardNumber = c;
        this->pin = p;
        this->balance = b;
    }
};

// COR
class Dispenser{
protected:
    Dispenser* next; //imp**********
public:
    Dispenser(){
        this->next = nullptr;
    }
    virtual ~Dispenser() = default;
    virtual bool canDispense(int amount) = 0;
    virtual void dispense(int amount) = 0;

    void setNext(Dispenser* nextDispenser){
        this->next = nextDispenser;
    }
};

class Dispenser2000:public Dispenser{
public:
    bool canDispense(int amount) override{
        int remainder = amount % 2000;
        if(remainder==0) return true;
        if(next != nullptr) return next->canDispense(remainder);
        return false;
    }
    void dispense(int amount) override{
        int notes = amount / 2000;
        int remainder = amount % 2000;
        
        if(notes > 0) cout << "Dispensing " << notes << " notes of Rs. 2000\n";
        if(remainder > 0 && next != nullptr) next->dispense(remainder);
    }
}
class Dispenser500 : public Dispenser{
public:
    bool canDispense(int amount) override{
        int remainder = amount % 500;
        if(remainder==0) return true;
        if(next != nullptr) return next->canDispense(remainder);
        return false;
    }
    void dispense(int amount) override{
        int notes = amount / 500;
        int remainder = amount % 500;
        
        if(notes > 0) cout << "Dispensing " << notes << " notes of Rs. 500\n";
        if(remainder > 0 && next != nullptr) next->dispense(remainder);
    }
};
class Dispenser1000 : public Dispenser{
public:
    bool canDispense(int amount) override{
        int remainder = amount % 100;
        if(remainder==0) return true;
        if(next != nullptr) return next->canDispense(remainder);
        return false;
    }
    void dispense(int amount) override{
        int notes = amount / 100;
        int remainder = amount % 100;
        
        if(notes > 0) cout << "Dispensing " << notes << " notes of Rs. 100\n";
        if(remainder > 0 && next != nullptr) next->dispense(remainder);
    }
};

class ATMState{
public:
    virtual ~ATMState() = default;
    virtual void insertCard(Card* card) = 0
    virtual void enterPin(string pin) = 0
    virtual void selectOption(string option) = 0
    virtual void dispenseCash(int amount) = 0
    virtual void ejectCard() = 0
};
class IdleState : public ATMState{
private:
    ATMMachine* machine;
public:
    IdleState(ATMMachine* m){ this->machine = m; }
    void insertCard(Card* card) override;
};
class CardInsertedState : public ATMState{
private:
    ATMMachine* machine;
public:
    CardInsertedState(ATMMachine* m){ this->machine = m; }
    void enterPin(string pin) override;
    void ejectCard() override;
};
class AuthenticatedState : public ATMState{
private:
    ATMMachine* machine;
public:
    AuthenticatedState(ATMMachine* m){ this->machine = m; }
    void selectOption(string option) override;
    void ejectCard() override;
};
class DispenseCashState : public ATMState{
private:
    ATMMachine* machine;
public:
    DispenseCashState(ATMMachine* m){ this->machine = m; }
    void dispenseCash(int amount) override;
};