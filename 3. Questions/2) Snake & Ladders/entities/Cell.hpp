#pragma once
#include "Obstacle.hpp"

class Cell{
private:
    int position;
    Obstacle* obstacle;

public:
    Cell(int position){
        this->position = position;
        this->obstacle = nullptr;
    }

// getter-----------------
    int getPosition(){
        return position;
    }
    Obstacle* getObstacle(){
        return obstacle;
    }
// setter --------------------
    void setObstacle(Obstacle* obstacle){
        this->obstacle = obstacle;
    }


    bool hasObstacle(){
        return obstacle != nullptr;
    }
    int getFinalPosition(){
        return hasObstacle() ? obstacle->movePlayer() : position;
    }
};