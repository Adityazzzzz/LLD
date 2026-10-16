#include<bits/stdc++.h>
using namespace std;

enum class SplitType{ EQUAL,PERCENTAGE };

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