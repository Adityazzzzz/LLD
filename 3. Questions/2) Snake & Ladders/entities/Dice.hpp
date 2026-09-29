#pragma once
#include <cstdlib> // for rand()

class Dice{
private:
    int noOfDice;

public:
    Dice(int noOfDice){
        this->noOfDice = noOfDice;
    }
    int roll(){
        int sum = 0;
        for(int i=0;i<noOfDice;i++){
            sum += (std::rand() %6) + 1;
        }
        return sum;
    }
};