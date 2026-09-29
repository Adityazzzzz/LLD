#pragma once
#include "Obstacle.hpp"

class Ladder : public Obstacle{
public:
    Ladder(int top,int bottom) : Obstacle(bottom,top){
        //blank rhega
    }

    ObstacleType getObstacleType() const override{
        return ObstacleType::LADDER;
    }
};