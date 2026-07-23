#include <iostream>
using namespace std;

/*
    Construct a complex object step by step

  Burger
    ↓
  Cheese
    ↓
Extra Patty
    ↓
  Coke
    ↓
  Fries

*/

class Burger{
public:
    void setBread(){
        cout<<"Bread Added";
    }
    void setCheese(){
        cout<<"Cheese Added";
    }
    void setPatty(){
        cout<<"Patty Added";
    }
};

class BurgerBuilder{
    Burger burger;
public:
    BurgerBuilder& addBread(){
        burger.setBread();
        return *this;
    }
    BurgerBuilder& addCheese(){
        burger.setCheese();
        return *this;
    }
    BurgerBuilder& addPatty(){
        burger.setPatty();
        return *this;
    }
    Burger build(){
        return burger;
    }
};

int main(){
    Burger burger = BurgerBuilder()
                        .addBread()
                        .addCheese()
                        .addPatty()
                        .build();
}