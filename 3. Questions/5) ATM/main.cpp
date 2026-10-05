#include <iostream>
using namespace std;

// ==========================================
// 1. ENUMS & FORWARD DECLARATIONS
// ==========================================
enum class ATMStatus {
    IDLE,
    CARD_INSERTED,
    AUTHENTICATED,
    DISPENSE_CASH
};

class ATMMachine;
class ATM;
class CashDispenser;

// ==========================================
// 2. DOMAIN ENTITIES (Account, Card, ATM)
// ==========================================
class Account {
private:
    string accountNumber;
    double balance;
public:
    Account(string accountNumber, double balance) {
        this->accountNumber = accountNumber;
        this->balance = balance;
    }

    string getAccountNumber() { return this->accountNumber; }
    double getBalance() { return this->balance; }

    void debit(double amount) {
        this->balance -= amount;
    }
};

class Card {
private:
    string cardNumber;
    string pin;
    Account* account;
public:
    Card(string cardNumber, string pin, Account* account) {
        this->cardNumber = cardNumber;
        this->pin = pin;
        this->account = account;
    }

    string getCardNumber() { return this->cardNumber; }
    string getPin() { return this->pin; }
    Account* getAccount() { return this->account; }
};

class ATM {
private:
    string id;
    ATMStatus status;
    double cashAvailable;
    int twoThousandCount;
    int fiveHundredCount;
    int oneHundredCount;
public:
    ATM(string id, int twoThousandCount, int fiveHundredCount, int oneHundredCount) {
        this->id = id;
        this->status = ATMStatus::IDLE;
        this->twoThousandCount = twoThousandCount;
        this->fiveHundredCount = fiveHundredCount;
        this->oneHundredCount = oneHundredCount;
        this->cashAvailable = (twoThousandCount * 2000.0) + (fiveHundredCount * 500.0) + (oneHundredCount * 100.0);
    }

    string getId() { return this->id; }
    ATMStatus getStatus() { return this->status; }
    void setStatus(ATMStatus status) { this->status = status; }

    double getCashAvailable() { return this->cashAvailable; }
    int getTwoThousandCount() { return this->twoThousandCount; }
    int getFiveHundredCount() { return this->fiveHundredCount; }
    int getOneHundredCount() { return this->oneHundredCount; }

    void deductTwoThousand(int count) {
        this->twoThousandCount -= count;
        this->cashAvailable -= (count * 2000.0);
    }

    void deductFiveHundred(int count) {
        this->fiveHundredCount -= count;
        this->cashAvailable -= (count * 500.0);
    }

    void deductOneHundred(int count) {
        this->oneHundredCount -= count;
        this->cashAvailable -= (count * 100.0);
    }
};

// ==========================================
// 3. REPOSITORY LAYER
// ==========================================
class ATMRepository {
private:
    unordered_map<string, ATM*> atms;
public:
    void save(ATM* atm) {
        this->atms[atm->getId()] = atm;
    }

    ATM* getById(string id) {
        if (this->atms.find(id) != this->atms.end()) {
            return this->atms[id];
        }
        return nullptr;
    }

    void updateATMStatusById(string id, ATMStatus newStatus) {
        ATM* atm = getById(id);
        if (atm != nullptr) {
            atm->setStatus(newStatus);
        }
    }
};

// ==========================================
// 4. CASH DISPENSER (CHAIN OF RESPONSIBILITY)
// ==========================================
class CashDispenser {
protected:
    CashDispenser* nextCashDispenser;
public:
    CashDispenser() {
        this->nextCashDispenser = nullptr;
    }
    virtual ~CashDispenser() = default;

    void setNextDispenser(CashDispenser* nextCashDispenser) {
        this->nextCashDispenser = nextCashDispenser;
    }

    virtual bool canDispense(ATM* atm, int amount) = 0;
    virtual void dispense(ATM* atm, int amount) = 0;
};

class TwoThousandDispenser : public CashDispenser {
public:
    bool canDispense(ATM* atm, int amount) override {
        int notesNeeded = amount / 2000;
        int notesToUse = min(notesNeeded, atm->getTwoThousandCount());
        int remainder = amount - (notesToUse * 2000);

        if (remainder == 0) return true;
        if (this->nextCashDispenser != nullptr) {
            return this->nextCashDispenser->canDispense(atm, remainder);
        }
        return false;
    }

    void dispense(ATM* atm, int amount) override {
        int notesNeeded = amount / 2000;
        int notesToUse = min(notesNeeded, atm->getTwoThousandCount());
        int remainder = amount - (notesToUse * 2000);

        if (notesToUse > 0) {
            atm->deductTwoThousand(notesToUse);
            cout << "Dispensing " << notesToUse << " notes of Rs. 2000\n";
        }

        if (remainder > 0 && this->nextCashDispenser != nullptr) {
            this->nextCashDispenser->dispense(atm, remainder);
        }
    }
};

class FiveHundredDispenser : public CashDispenser {
public:
    bool canDispense(ATM* atm, int amount) override {
        int notesNeeded = amount / 500;
        int notesToUse = min(notesNeeded, atm->getFiveHundredCount());
        int remainder = amount - (notesToUse * 500);

        if (remainder == 0) return true;
        if (this->nextCashDispenser != nullptr) {
            return this->nextCashDispenser->canDispense(atm, remainder);
        }
        return false;
    }

    void dispense(ATM* atm, int amount) override {
        int notesNeeded = amount / 500;
        int notesToUse = min(notesNeeded, atm->getFiveHundredCount());
        int remainder = amount - (notesToUse * 500);

        if (notesToUse > 0) {
            atm->deductFiveHundred(notesToUse);
            cout << "Dispensing " << notesToUse << " notes of Rs. 500\n";
        }

        if (remainder > 0 && this->nextCashDispenser != nullptr) {
            this->nextCashDispenser->dispense(atm, remainder);
        }
    }
};

class OneHundredDispenser : public CashDispenser {
public:
    bool canDispense(ATM* atm, int amount) override {
        int notesNeeded = amount / 100;
        int notesToUse = min(notesNeeded, atm->getOneHundredCount());
        int remainder = amount - (notesToUse * 100);

        if (remainder == 0) return true;
        if (this->nextCashDispenser != nullptr) {
            return this->nextCashDispenser->canDispense(atm, remainder);
        }
        return false;
    }

    void dispense(ATM* atm, int amount) override {
        int notesNeeded = amount / 100;
        int notesToUse = min(notesNeeded, atm->getOneHundredCount());
        int remainder = amount - (notesToUse * 100);

        if (notesToUse > 0) {
            atm->deductOneHundred(notesToUse);
            cout << "Dispensing " << notesToUse << " notes of Rs. 100\n";
        }

        if (remainder > 0 && this->nextCashDispenser != nullptr) {
            this->nextCashDispenser->dispense(atm, remainder);
        }
    }
};

// ==========================================
// 5. ATM STATE PATTERN INTERFACE
// ==========================================
class ATMState {
public:
    virtual ~ATMState() = default;
    virtual void insertCard(Card* card) = 0;
    virtual void enterPin(string pin) = 0;
    virtual void selectOption(string option) = 0;
    virtual void dispenseCash(int amount) = 0;
    virtual void ejectCard() = 0;
    virtual ATMStatus getStatus() = 0;
};

// ==========================================
// 6. ATM MACHINE (CONTEXT)
// ==========================================
class ATMMachine {
private:
    ATM* atm;
    ATMState* state;
    ATMRepository* atmRepository;
    Card* currentCard;
public:
    ATMMachine(ATM* atm, ATMRepository* atmRepository);

    void setState(ATMState* newState) {
        this->state = newState;
        this->atm->setStatus(newState->getStatus());
        this->atmRepository->updateATMStatusById(this->atm->getId(), newState->getStatus());
    }

    ATM* getAtm() { return this->atm; }
    Card* getCurrentCard() { return this->currentCard; }
    void setCurrentCard(Card* card) { this->currentCard = card; }

    void insertCard(Card* card) { this->state->insertCard(card); }
    void enterPin(string pin) { this->state->enterPin(pin); }
    void selectOption(string option) { this->state->selectOption(option); }
    void dispenseCash(int amount) { this->state->dispenseCash(amount); }
    void ejectCard() { this->state->ejectCard(); }
    ATMStatus getStatus() { return this->state->getStatus(); }
};

// ==========================================
// 7. CONCRETE STATES
// ==========================================
class IdleState : public ATMState {
private:
    ATMMachine* atmMachine;
public:
    IdleState(ATMMachine* atmMachine);

    void insertCard(Card* card) override;

    void enterPin(string pin) override {
        cout << "[ERROR] Cannot enter PIN: No card inserted.\n";
    }

    void selectOption(string option) override {
        cout << "[ERROR] Cannot select option: No card inserted.\n";
    }

    void dispenseCash(int amount) override {
        cout << "[ERROR] Cannot dispense cash: No card inserted.\n";
    }

    void ejectCard() override {
        cout << "[ERROR] Cannot eject card: No card in machine.\n";
    }

    ATMStatus getStatus() override {
        return ATMStatus::IDLE;
    }
};

class CardInsertedState : public ATMState {
private:
    ATMMachine* atmMachine;
public:
    CardInsertedState(ATMMachine* atmMachine) {
        this->atmMachine = atmMachine;
    }

    void insertCard(Card* card) override {
        cout << "[ERROR] A card is already inserted in this machine.\n";
    }

    void enterPin(string pin) override;

    void selectOption(string option) override {
        cout << "[ERROR] Authenticate with your PIN before selecting options.\n";
    }

    void dispenseCash(int amount) override {
        cout << "[ERROR] Authenticate with your PIN before withdrawing cash.\n";
    }

    void ejectCard() override;

    ATMStatus getStatus() override {
        return ATMStatus::CARD_INSERTED;
    }
};

class AuthenticatedState : public ATMState {
private:
    ATMMachine* atmMachine;
public:
    AuthenticatedState(ATMMachine* atmMachine) {
        this->atmMachine = atmMachine;
    }

    void insertCard(Card* card) override {
        cout << "[ERROR] A card is already inserted in this machine.\n";
    }

    void enterPin(string pin) override {
        cout << "[NOTICE] Already authenticated successfully.\n";
    }

    void selectOption(string option) override;

    void dispenseCash(int amount) override {
        cout << "[ERROR] Choose option 'WITHDRAW' before requesting cash.\n";
    }

    void ejectCard() override;

    ATMStatus getStatus() override {
        return ATMStatus::AUTHENTICATED;
    }
};

class DispenseCashState : public ATMState {
private:
    ATMMachine* atmMachine;
    CashDispenser* chain;
public:
    DispenseCashState(ATMMachine* atmMachine) {
        this->atmMachine = atmMachine;

        // Build Dispenser Chain: 2000 -> 500 -> 100
        CashDispenser* d2000 = new TwoThousandDispenser();
        CashDispenser* d500 = new FiveHundredDispenser();
        CashDispenser* d100 = new OneHundredDispenser();

        d2000->setNextDispenser(d500);
        d500->setNextDispenser(d100);

        this->chain = d2000;
    }

    void insertCard(Card* card) override {
        cout << "[ERROR] Dispense operation in progress. Card already inside.\n";
    }

    void enterPin(string pin) override {
        cout << "[ERROR] Already authenticated.\n";
    }

    void selectOption(string option) override {
        cout << "[ERROR] Currently dispensing. Please complete withdrawal.\n";
    }

    void dispenseCash(int amount) override {
        Card* card = this->atmMachine->getCurrentCard();
        Account* account = card->getAccount();
        ATM* atm = this->atmMachine->getAtm();

        // 1. Balance check
        if (account->getBalance() < amount) {
            cout << "[DECLINED] Insufficient account balance. Available: Rs. " << account->getBalance() << "\n";
            this->atmMachine->ejectCard();
            return;
        }

        // 2. Total ATM cash check
        if (atm->getCashAvailable() < amount) {
            cout << "[DECLINED] ATM does not have enough total cash to service request.\n";
            this->atmMachine->ejectCard();
            return;
        }

        // 3. Denomination availability check via Chain
        if (!this->chain->canDispense(atm, amount)) {
            cout << "[DECLINED] ATM cannot satisfy exact amount with current denominations.\n";
            this->atmMachine->ejectCard();
            return;
        }

        // 4. Execute physical dispense and balance updates
        cout << "\n--- DISPENSING CASH (Rs. " << amount << ") ---\n";
        this->chain->dispense(atm, amount);
        account->debit(amount);
        cout << "Withdrawal complete. Remaining Account Balance: Rs. " << account->getBalance() << "\n";

        // 5. Auto eject card & return to Idle
        this->atmMachine->ejectCard();
    }

    void ejectCard() override;

    ATMStatus getStatus() override {
        return ATMStatus::DISPENSE_CASH;
    }
};

// ==========================================
// 8. METHOD BODIES (Resolving Circular Dependencies)
// ==========================================
ATMMachine::ATMMachine(ATM* atm, ATMRepository* atmRepository) {
    this->atm = atm;
    this->atmRepository = atmRepository;
    this->currentCard = nullptr;
    this->state = new IdleState(this);
}

IdleState::IdleState(ATMMachine* atmMachine) {
    this->atmMachine = atmMachine;
}

void IdleState::insertCard(Card* card) {
    this->atmMachine->setCurrentCard(card);
    cout << "Card accepted (No: " << card->getCardNumber() << "). Please enter your PIN.\n";
    this->atmMachine->setState(new CardInsertedState(this->atmMachine));
}

void CardInsertedState::enterPin(string pin) {
    if (this->atmMachine->getCurrentCard()->getPin() == pin) {
        cout << "PIN Verified successfully.\n";
        this->atmMachine->setState(new AuthenticatedState(this->atmMachine));
    } else {
        cout << "[SECURITY ALERT] Invalid PIN entered. Ejecting card.\n";
        this->atmMachine->ejectCard();
    }
}

void CardInsertedState::ejectCard() {
    cout << "Ejecting card: " << this->atmMachine->getCurrentCard()->getCardNumber() << "\n";
    this->atmMachine->setCurrentCard(nullptr);
    this->atmMachine->setState(new IdleState(this->atmMachine));
}

void AuthenticatedState::selectOption(string option) {
    if (option == "WITHDRAW") {
        cout << "Option selected: WITHDRAW. Please enter withdrawal amount.\n";
        this->atmMachine->setState(new DispenseCashState(this->atmMachine));
    } else {
        cout << "Option not supported yet. Ejecting card.\n";
        this->atmMachine->ejectCard();
    }
}

void AuthenticatedState::ejectCard() {
    cout << "Ejecting card: " << this->atmMachine->getCurrentCard()->getCardNumber() << "\n";
    this->atmMachine->setCurrentCard(nullptr);
    this->atmMachine->setState(new IdleState(this->atmMachine));
}

void DispenseCashState::ejectCard() {
    cout << "Ejecting card: " << this->atmMachine->getCurrentCard()->getCardNumber() << "\n";
    this->atmMachine->setCurrentCard(nullptr);
    this->atmMachine->setState(new IdleState(this->atmMachine));
}

// ==========================================
// 9. DRIVER CODE
// ==========================================
int main() {
    // 1. Setup Data Models
    Account* myAccount = new Account("ACC_998877", 25000.0);
    Card* myCard = new Card("CARD_1234_5678", "4321", myAccount);

    // ATM inventory: 2x2000 (4000), 5x500 (2500), 10x100 (1000) = Total Rs. 7500
    ATM* atm1 = new ATM("ATM_DELHI_01", 2, 5, 10);

    // 2. Setup Repository
    ATMRepository* repository = new ATMRepository();
    repository->save(atm1);

    // 3. Initialize Controller
    ATMMachine* machine = new ATMMachine(atm1, repository);

    cout << "=== SCENARIO 1: WRONG PIN FLOW ===\n";
    machine->insertCard(myCard);
    machine->enterPin("0000"); // Fail check

    cout << "\n=== SCENARIO 2: SUCCESSFUL CASH WITHDRAWAL ===\n";
    machine->insertCard(myCard);
    machine->enterPin("4321"); // Pass check
    machine->selectOption("WITHDRAW");
    machine->dispenseCash(5700); // 2x2000 (4000) + 3x500 (1500) + 2x100 (200)

    cout << "\n=== SCENARIO 3: WITHDRAWAL EXCEEDING ATM CASH ===\n";
    machine->insertCard(myCard);
    machine->enterPin("4321");
    machine->selectOption("WITHDRAW");
    machine->dispenseCash(10000); // Exceeds remaining ATM vault limit

    return 0;
}