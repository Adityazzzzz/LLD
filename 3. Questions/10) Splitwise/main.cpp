#include<bits/stdc++.h>
using namespace std;

enum class SplitType{ EQUAL,PERCENTAGE };

class User{
public:
    string id;
    string name;
    User(string id,string name) : id(id),name(name){}
};

class Split{
public:
    User* user;
    double amount;
    double percentage;

    Split(User* user) : user(user),amount(0.0),percentage(0.0){}
};

class Expense{
public:
    string description;
    double amount;
    User* paidBy;
    vector<Split*> splits;
    SplitType splitType;

    Expense(string desc,double amt,User* paidBy,vector<Split*> splits,SplitType type) : description(desc),amount(amt),paidBy(paidBy),splits(splits),splitType(type){}
};

class BalanceSheet{
public:
    unordered_map<string,unordered_map<string,double>> balances;
    unordered_map<string,double> totalPaid;
    unordered_map<string,double> totalExpense;

    void updateLedger(string paidBy,string paidTo,double amount){
        if(paidBy == paidTo) return;
        balances[paidTo][paidBy] += amount;
        balances[paidBy][paidTo] -= amount;
    }
};

class Group{
public:
    string id;
    string name;
    vector<User*> members;
    vector<Expense*> expenses;
    BalanceSheet* balanceSheet;

    Group(string id,string name) : id(id),name(name){
        balanceSheet = new BalanceSheet();
    }
    void addMember(User* user){ members.push_back(user); }
    void addExpense(Expense* expense){ expenses.push_back(expense); }
};

class GroupRepo{
private:
    unordered_map<string,Group*> groups;
public:
    void save(Group* group){ 
        groups[group->id] = group; 
    }
    Group* findById(string id){
        if(groups.find(id) != groups.end()) return groups[id];
        return nullptr;
    }
};

class SplitStrategy{
public:
    virtual ~SplitStrategy() = default;
    virtual bool split(double totalAmount,vector<Split*>& splits) = 0;
};

class EqualSplitStrategy:public SplitStrategy{
public:
    bool split(double totalAmount,vector<Split*>& splits) override{
        if(splits.empty()) return false;
        double splitAmount = totalAmount / splits.size();
        for(Split* s:splits){
            s->amount = splitAmount;
        }
        return true;
    }
};

class PercentageSplitStrategy:public SplitStrategy{
public:
    bool split(double totalAmount,vector<Split*>& splits) override{
        double totalPercent = 0;
        for(Split* s:splits){
            totalPercent += s->percentage;
            s->amount =(totalAmount * s->percentage) / 100.0;
        }
        return abs(totalPercent - 100.0) < 0.001;
    }
};

class SplitStrategyFactory{
public:
    static SplitStrategy* getStrategy(SplitType type){
        switch(type){
            case SplitType::EQUAL: return new EqualSplitStrategy();
            case SplitType::PERCENTAGE: return new PercentageSplitStrategy();
            default: return nullptr;
        }
    }
};

class BalanceSheetService{
public:
    void updateBalances(Group* group,Expense* expense){
        BalanceSheet* sheet = group->balanceSheet;
        sheet->totalPaid[expense->paidBy->name] += expense->amount;

        for(Split* split:expense->splits){
            sheet->totalExpense[split->user->name] += split->amount;
            sheet->updateLedger(expense->paidBy->name,split->user->name,split->amount);
        }
    }

    void printGroupBalances(Group* group){
        cout << "\n--- STANDARD BALANCES: " << group->name << " ---\n";
        bool isSettled = true;
        for(auto const& [userId,userBalances]:group->balanceSheet->balances){
            for(auto const& [otherUser,amount]:userBalances){
                if(amount > 0.01){ 
                    cout << userId << " owes " << otherUser << ":$" << fixed << setprecision(2) << amount << "\n";
                    isSettled = false;
                }
            }
        }
        if(isSettled) cout << "All balances are settled!\n";
    }
};

class DebtSimplificationService{
public:
    void simplifyDebts(Group* group){
        cout << "\n--- SIMPLIFIED DEBTS: " << group->name << " ---\n";
        unordered_map<string,double> netBalances;

        //Calculate net balances(Positive = gets money back,Negative = owes money)
        for(auto const& [debtor,userBalances]:group->balanceSheet->balances){
            for(auto const& [creditor,amount]:userBalances){
                if(amount > 0.01){
                    netBalances[debtor] -= amount;
                    netBalances[creditor] += amount;
                }
            }
        }

        //Separate into Debtors and Creditors
        vector<pair<string,double>> debtors,creditors;
        for(auto const& [user,balance]:netBalances){
            if(balance < -0.01) debtors.push_back({user,-balance});
            else if(balance > 0.01) creditors.push_back({user,balance});
        }

        // 3. Greedy Settlement Algorithm
        int i = 0,j = 0;
        bool hasDebts = false;
        while(i < debtors.size() && j < creditors.size()){
            hasDebts = true;
            double settleAmount = min(debtors[i].second,creditors[j].second);
            
            cout << debtors[i].first << " pays " << creditors[j].first << ":$" << fixed << setprecision(2) << settleAmount << "\n";
            
            debtors[i].second -= settleAmount;
            creditors[j].second -= settleAmount;

            if(debtors[i].second < 0.01) i++;
            if(creditors[j].second < 0.01) j++;
        }

        if(!hasDebts) cout << "No debts to simplify.\n";
    }
};

class ExpenseService{
private:
    BalanceSheetService* balanceSheetService;
public:
    ExpenseService(BalanceSheetService* bsService) : balanceSheetService(bsService){}

    void addExpense(Group* group,string desc,double amount,User* paidBy,vector<Split*> splits,SplitType type){
        SplitStrategy* strategy = SplitStrategyFactory::getStrategy(type);
        
        if(strategy == nullptr || !strategy->split(amount,splits)){
            cout << "[ERROR] Invalid split math for: " << desc << "\n";
            delete strategy;
            return;
        }

        Expense* expense = new Expense(desc,amount,paidBy,splits,type);
        group->addExpense(expense);
        balanceSheetService->updateBalances(group,expense);
        
        cout << "[SUCCESS] Added Expense: " << desc << "($" << amount << ") paid by " << paidBy->name << "\n";
        delete strategy;
    }
};

class GroupService{
private:
    GroupRepo* repo;
    ExpenseService* expenseService;
    DebtSimplificationService* simplifier;
    BalanceSheetService* balanceSheetService;

public:
    GroupService(GroupRepo* repo,ExpenseService* eService,DebtSimplificationService* dService,BalanceSheetService* bsService) : repo(repo),expenseService(eService),simplifier(dService),balanceSheetService(bsService){}

    Group* createGroup(string id,string name){
        Group* group = new Group(id,name);
        repo->save(group);
        return group;
    }

    void addMember(string groupId,User* user){
        Group* group = repo->findById(groupId);
        if(group) group->addMember(user);
    }

    void addExpense(string groupId,string desc,double amount,User* paidBy,vector<Split*> splits,SplitType type){
        Group* group = repo->findById(groupId);
        if(group) expenseService->addExpense(group,desc,amount,paidBy,splits,type);
    }

    void printGroupBalances(string groupId){
        Group* group = repo->findById(groupId);
        if(group) balanceSheetService->printGroupBalances(group);
    }

    void simplifyDebts(string groupId){
        Group* group = repo->findById(groupId);
        if(group) simplifier->simplifyDebts(group);
    }
};

int main(){
    //Initialize Services & Repo
    GroupRepo* repo = new GroupRepo();
    BalanceSheetService* bsService = new BalanceSheetService();
    ExpenseService* expService = new ExpenseService(bsService);
    DebtSimplificationService* debtService = new DebtSimplificationService();
    GroupService* groupService = new GroupService(repo,expService,debtService,bsService);

    //Create Users
    User* A = new User("U1","Alice");
    User* B = new User("U2","Bob");
    User* C = new User("U3","Charlie");

    //Create Group
    groupService->createGroup("G1","Roommates");
    groupService->addMember("G1",A);
    groupService->addMember("G1",B);
    groupService->addMember("G1",C);

    //Add Complex Expenses

    // A pays $300 for A,B,C(Equal) -> B owes A $100,C owes A $100
    vector<Split*> split1 ={ new Split(A),new Split(B),new Split(C) };
    groupService->addExpense("G1","Rent",300.0,A,split1,SplitType::EQUAL);

    // B pays $100 for B,C(Equal) -> C owes B $50
    vector<Split*> split2 ={ new Split(B),new Split(C) };
    groupService->addExpense("G1","Groceries",100.0,B,split2,SplitType::EQUAL);

    //View Standard vs Simplified Balances
    groupService->printGroupBalances("G1");
    // Standard: 
    // B owes A: $100
    // C owes A: $100
    // C owes B: $50
    
    // Simplification changes the graph so C pays A directly instead of passing through B.
    groupService->simplifyDebts("G1");

    return 0;
}