#pragma once
#include "Obstacle.hpp"

class Snake : public Obstacle{
public:
    Snake(int head,int tail) : Obstacle(head,tail){
        //blank
    }

    ObstacleType getObstacleType() const override {
        return ObstacleType::SNAKE;
    }
};