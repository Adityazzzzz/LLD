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
class Dispenser500:public Dispenser{
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
class Dispenser100:public Dispenser{
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

class ATMMachine{
public:
    ATMState* state;
    Card* currentCard;
    Dispenser* cashChain;

    ATMMachine();
    
    void setState(ATMState* newState){ 
        this->state = newState; 
    }
    
    void insertCard(Card* card){ 
        state->insertCard(card); 
    }
    void enterPin(string pin){ 
        state->enterPin(pin); 
    }
    void selectOption(string option){ 
        state->selectOption(option); 
    }
    void dispenseCash(int amount){ 
        state->dispenseCash(amount); 
    }
    void ejectCard(){ 
        state->ejectCard(); 
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

class IdleState:public ATMState{
private:
    ATMMachine* machine;
public:
    IdleState(ATMMachine* m){ 
        this->machine = m; 
    }
    void insertCard(Card* card) override;
};

class CardInsertedState:public ATMState{
private:
    ATMMachine* machine;
public:
    CardInsertedState(ATMMachine* m){ 
        this->machine = m; 
    }
    void enterPin(string pin) override;
    void ejectCard() override;
};

class AuthenticatedState:public ATMState{
private:
    ATMMachine* machine;
public:
    AuthenticatedState(ATMMachine* m){ 
        this->machine = m; 
    }
    void selectOption(string option) override;
    void ejectCard() override;
};

class DispenseCashState:public ATMState{
private:
    ATMMachine* machine;
public:
    DispenseCashState(ATMMachine* m){ 
        this->machine = m; 
    }
    void dispenseCash(int amount) override;
};

//circular dependensies
ATMMachine::ATMMachine(){
    this->currentCard = nullptr;
    this->state = new IdleState(this);
    
    // Wire up Chain of Responsibility
    Dispenser* d2000 = new Dispenser2000();
    Dispenser* d500 = new Dispenser500();
    Dispenser* d100 = new Dispenser100();
    
    d2000->setNext(d500);
    d500->setNext(d100);
    
    this->cashChain = d2000;
}

void IdleState::insertCard(Card* card){
    cout << "Card inserted: " << card->cardNumber << "\n";
    this->machine->currentCard = card;
    this->machine->setState(new CardInsertedState(this->machine));
}

void CardInsertedState::enterPin(string pin){
    if (this->machine->currentCard->pin == pin){
        cout << "PIN Verified.\n";
        this->machine->setState(new AuthenticatedState(this->machine));
    } 
    else{
        cout << "[DECLINED] Incorrect PIN.\n";
        this->ejectCard();
    }
}

void CardInsertedState::ejectCard(){
    cout << "Ejecting card.\n";
    this->machine->currentCard = nullptr;
    this->machine->setState(new IdleState(this->machine));
}

void AuthenticatedState::selectOption(string option){
    if (option == "WITHDRAW"){
        cout << "Option: WITHDRAW selected.\n";
        this->machine->setState(new DispenseCashState(this->machine));
    } 
    else{
        cout << "Option not supported.\n";
        this->ejectCard();
    }
}

void AuthenticatedState::ejectCard(){
    cout << "Ejecting card.\n";
    this->machine->currentCard = nullptr;
    this->machine->setState(new IdleState(this->machine));
}

void DispenseCashState::dispenseCash(int amount){
    if (this->machine->currentCard->balance < amount){
        cout << "[DECLINED] Insufficient balance.\n";
    } 
    else if (amount % 100 != 0){
        cout << "[DECLINED] Amount must be in multiples of 100.\n";
    } 
    else{
        cout << "\n--- WITHDRAWING RS. " << amount << " ---\n";
        this->machine->cashChain->dispense(amount);
        this->machine->currentCard->balance -= amount;
        cout << "Remaining Balance: Rs. " << this->machine->currentCard->balance << "\n";
    }
    
    // Always eject card and return to Idle after a dispense attempt
    cout << "Ejecting card.\n";
    this->machine->currentCard = nullptr;
    this->machine->setState(new IdleState(this->machine));
}