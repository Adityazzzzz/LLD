#include <iostream>
using namespace std;

class VendingMachine;

class VendingMachineState{
public:
    virtual ~VendingMachineState() = default;

    virtual VendingMachine* insertCoin(VendingMachine* m, int amount){ 
        cout << "err"; 
        return m; 
    }
    virtual VendingMachine* selectItem(VendingMachine* m){ 
        cout << "err"; 
        return m; 
    }
    virtual VendingMachine* dispense(VendingMachine* m){ 
        cout << "err"; 
        return m; 
    }
    virtual VendingMachine* refill(VendingMachine* m, int qty){ 
        cout << "err"; 
        return m; 
    }
    virtual VendingMachine* returnCoin(VendingMachine* m){ 
        cout << "err"; 
        return m; 
    }
};

class NoCoinState:public VendingMachineState{
public:
    VendingMachine* insertCoin(VendingMachine* m,int amount) override;
};

class HasCoinState:public VendingMachineState{
public:
    VendingMachine* selectItem(VendingMachine* m) override;
};

class DispenseState:public VendingMachineState{
public:
    VendingMachine* dispense(VendingMachine* m) override;
};

class SoldOutState:public VendingMachineState{
public:
    VendingMachine* refill(VendingMachine* m,int qty) override;
};

class VendingMachine{
private:
    VendingMachineState* state;
    int inventory;
    int currentBalance;
    const int ITEM_PRICE = 15;
public:
    VendingMachine(int initialInventory);

    void setState(VendingMachineState* newState){
        this->state = newState;
    }
    int getInventory(){ return inventory; }
    void setInventory(int qty){ this->inventory = qty; }
    
    int getBalance(){ return currentBalance; }
    void addBalance(int amount){ this->currentBalance += amount; }
    void clearBalance(){ this->currentBalance = 0; }


    VendingMachine* insertCoint(int amount){
        return this->state->insertCoin(this,amount);
    }
    VendingMachine* selectItem(){
        return this->state->selectItem(this);
    }
    VendingMachine* dispense(){
        return this->state->dispense(this);
    }
    VendingMachine* refill(int qty){
        return this->state->refill(this,qty);
    }
    VendingMachine* returnCoin(){
        return this->state->returnCoin(this);
    }
};

VendingMachine::VendingMachine(int initialInventory){
    this->inventory = initialInventory;
    this->currentBalance = 0;

    if(initialInventory > 0) this->state = new NoCoinState();
    else this->state = new SoldOutState();
}

VendingMachine* NoCoinState::insertCoin(VendingMachine* m,int amount){
    m->addBalance(amount);
    m->setState(new HasCoinState());
    return m;
}

VendingMachine* HasCoinState::selectItem(VendingMachine* m){
    if(m->getInventory() == 0){
        m->clearBalance();
        m->setState(new SoldOutState());
        return m;
    }
    if(m->getBalance() < m->getPrice()){
        return m;
    }

    m->setState(new DispenseState());
    return m->dispense();
}

VendingMachine* DispenseState::dispense(VendingMachine* m){
    m->setInventory(m->getInventory() - 1);
    int change = m->getBalance() - m->getPrice();
    
    if(change > 0){
        //return cancellation
    }
    
    m->clearBalance();
    
    if(m->getInventory() > 0){
        m->setState(new NoCoinState());
    } 
    else{
        m->setState(new SoldOutState());
    }
    return m;
}

VendingMachine* SoldOutState::refill(VendingMachine* m, int qty){
    m->setInventory(m->getInventory() + qty);
    m->setState(new NoCoinState());
    return m;
}

int main() {
    VendingMachine* machine = new VendingMachine(2);
    
    //chaining
    machine->insertCoin(10)->insertCoin(10)->selectItem();
    
    //cancellation
    machine->insertCoin(10)->insertCoin(5)->returnCoin();

    //last item
    machine->insertCoin(10)->insertCoin(10)->selectItem();
    
    //sold out
    machine->insertCoin(10)->returnCoin();
    return 0;
}