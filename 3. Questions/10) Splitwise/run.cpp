#include<bits/stdc++.h>
using namespace std;

//enum
enum class SplitType{ EQUAL,PERCENTAGE };

//entities
class User{
public:
    string id;
    string name;
    User(string id,string name){
        this->id = id;
        this->name = name;
    }
};

class Split{
public:
    User* user;
    double amount;
    double percentage;

    Split(User* user){
        this->user = user;
        this->amount = 0.0;
        this->percentage = 0.0;
    }
};

class Expense{
public:
    string description;
    double amount;
    User* paidBy;
    vector<Split*> splits;
    SplitType splitType;

    Expense(string desc,double amt,User* paidBy,vector<Split*> splits,SplitType type){
        this->description = desc;
        this->amount = amt;
        this->paidBy = paidBy;
        this->splits = splits;
        this->splitType = type;
    }
};

class BalanceSheet{
public:
    // balances[UserA][UserB] = X means UserA owes UserB amount X
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

    Group(string id,string name){
        balanceSheet = new BalanceSheet();
        this->id = id;
        this->name = name;
    }
    void addMember(User* user){ 
        members.push_back(user); 
    }
    void addExpense(Expense* expense){ 
        expenses.push_back(expense);
    }
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

//strategy
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

//services
class BalanceSheetService{
public:
    void updateBalaces(Group* group,Expense* expense){
        BalanceSheet* sheet = group->balanceSheet;
        sheet->totalPaid[expense->paidBy->name] += expense->amount;

        for(Split* it:expense->splits){
            sheet->totalExpense[it->user->name] += it->amount;
            sheet->updateLedger(expense->paidBy->name,it->user->name,it->amount);
        }
    }

    void printGroupBalances(Group* group){
        cout<<group->name<<endl;

        bool isSettled = true;
        for(auto const& [userId,userBalances] : group->balanceSheet->balances){
            for(auto const& [otherUser,amount] : userBalances){
                if(amount > 0.01){ 
                    isSettled = false;
                }
            }
        }
        if(isSettled) cout << "All balances are settled!\n";
    }
};