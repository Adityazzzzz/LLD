#pragma once
#include "Obstacle.hpp"

class Snake:public Obstacle{
public:
    Snake(int src,int dest) : Obstacle(src,dest){
        if(dest >= src){
            throw std::invalid_argument("Error");
        }
    }
    
}