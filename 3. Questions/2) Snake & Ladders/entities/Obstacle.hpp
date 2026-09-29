#pragma once
#include "../enums/ObstacleType.hpp"

class Obstacle{
protected:
    int src;
    int dest;

public:
    Obstacle(int src,int dest){
        this->src = src;
        this->dest = dest;
    }
    virtual ~Obstacle() = default;

    virtual ObstacleType getObstacleType() const = 0;

    int movePlayer() const{
        return dest;
    }
};