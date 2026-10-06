#include <iostream>
#include <string>

using namespace std;

// ==========================================
// 1. FORWARD DECLARATIONS
// ==========================================
class VendingMachine;

// ==========================================
// 2. STATE INTERFACE (Using your diagram's exact methods)
// ==========================================
class VendingMachineState {
public:
    virtual ~VendingMachineState() = default;
    
    // Default error messages prevent the "Abstract Class" instantiation error
    virtual void insertCoin(VendingMachine* m, int amount) { cout << "[ERROR] Cannot insert coin right now.\n"; }
    virtual void selectItem(VendingMachine* m) { cout << "[ERROR] Cannot select item right now.\n"; }
    virtual void dispense(VendingMachine* m) { cout << "[ERROR] Cannot dispense right now.\n"; }
    virtual void refill(VendingMachine* m, int qty) { cout << "[ERROR] Cannot refill right now.\n"; }
};

// ==========================================
// 3. CONCRETE STATES (Declarations)
// ==========================================
class NoCoinState : public VendingMachineState {
public:
    void insertCoin(VendingMachine* m, int amount) override;
    void refill(VendingMachine* m, int qty) override;
};

class HasCoinState : public VendingMachineState {
public:
    void insertCoin(VendingMachine* m, int amount) override;
    void selectItem(VendingMachine* m) override;
};

class DispenseState : public VendingMachineState {
public:
    void dispense(VendingMachine* m) override;
};

class SoldOutState : public VendingMachineState {
public:
    void refill(VendingMachine* m, int qty) override;
};

// ==========================================
// 4. CONTEXT (Vending Machine Manager)
// ==========================================
class VendingMachine {
private:
    VendingMachineState* state;
    int inventory;
    int currentBalance;
    const int ITEM_PRICE = 15; // Hardcoded price for MVP simplicity

public:
    VendingMachine(int initialInventory);

    void setState(VendingMachineState* newState) { this->state = newState; }
    
    int getInventory() { return inventory; }
    void setInventory(int qty) { this->inventory = qty; }
    
    int getBalance() { return currentBalance; }
    void addBalance(int amount) { this->currentBalance += amount; }
    void clearBalance() { this->currentBalance = 0; }
    
    int getPrice() { return ITEM_PRICE; }

    // Delegate to current state
    void insertCoin(int amount) { state->insertCoin(this, amount); }
    void selectItem() { state->selectItem(this); }
    void dispense() { state->dispense(this); }
    void refill(int qty) { state->refill(this, qty); }
};

// ==========================================
// 5. IMPLEMENTATIONS (Resolving Circular Dependencies)
// ==========================================
VendingMachine::VendingMachine(int initialInventory) {
    this->inventory = initialInventory;
    this->currentBalance = 0;
    
    if (initialInventory > 0) {
        this->state = new NoCoinState();
    } else {
        this->state = new SoldOutState();
    }
}

// --- NoCoinState ---
void NoCoinState::insertCoin(VendingMachine* m, int amount) {
    m->addBalance(amount);
    cout << "Coin inserted: Rs. " << amount << ". Current Balance: Rs. " << m->getBalance() << "\n";
    m->setState(new HasCoinState());
}

void NoCoinState::refill(VendingMachine* m, int qty) {
    m->setInventory(m->getInventory() + qty);
    cout << "Machine refilled. Total inventory: " << m->getInventory() << "\n";
}

// --- HasCoinState ---
void HasCoinState::insertCoin(VendingMachine* m, int amount) {
    m->addBalance(amount);
    cout << "Coin inserted: Rs. " << amount << ". Current Balance: Rs. " << m->getBalance() << "\n";
}

void HasCoinState::selectItem(VendingMachine* m) {
    if (m->getInventory() == 0) {
        cout << "[DECLINED] Item is sold out. Refunding Rs. " << m->getBalance() << "\n";
        m->clearBalance();
        m->setState(new SoldOutState());
        return;
    }

    if (m->getBalance() < m->getPrice()) {
        cout << "[DECLINED] Insufficient funds. Price is Rs. " << m->getPrice() 
             << ", but you only have Rs. " << m->getBalance() << "\n";
        return;
    }

    cout << "Item selected successfully.\n";
    m->setState(new DispenseState());
    m->dispense(); // Auto-trigger dispense after successful selection
}

// --- DispenseState ---
void DispenseState::dispense(VendingMachine* m) {
    m->setInventory(m->getInventory() - 1);
    int change = m->getBalance() - m->getPrice();
    
    cout << "\n--- DISPENSING ITEM ---\n";
    if (change > 0) {
        cout << "Returning change: Rs. " << change << "\n";
    }
    
    m->clearBalance();
    
    if (m->getInventory() > 0) {
        m->setState(new NoCoinState());
    } else {
        cout << "[NOTICE] Machine is now SOLD OUT.\n";
        m->setState(new SoldOutState());
    }
}

// --- SoldOutState ---
void SoldOutState::refill(VendingMachine* m, int qty) {
    m->setInventory(m->getInventory() + qty);
    cout << "Machine refilled with " << qty << " items. Back in business!\n";
    m->setState(new NoCoinState());
}

// ==========================================
// 6. MAIN DRIVER
// ==========================================
int main() {
    // Initialize with 2 items
    VendingMachine* machine = new VendingMachine(2);

    cout << "=== SCENARIO 1: SUCCESSFUL PURCHASE ===\n";
    machine->insertCoin(10);
    machine->insertCoin(10); // Total Rs 20
    machine->selectItem();   // Item costs 15, should return 5 change

    cout << "\n=== SCENARIO 2: INSUFFICIENT FUNDS ===\n";
    machine->insertCoin(5);
    machine->selectItem();   // Fails

    cout << "\n=== SCENARIO 3: BUYING LAST ITEM ===\n";
    machine->insertCoin(10); // Total now Rs 15
    machine->selectItem();   // Succeeds, machine becomes Sold Out

    cout << "\n=== SCENARIO 4: SOLD OUT REJECTION ===\n";
    machine->insertCoin(10); // Rejected by SoldOutState

    cout << "\n=== SCENARIO 5: REFILL ===\n";
    machine->refill(5);
    machine->insertCoin(20);
    machine->selectItem();

    return 0;
}