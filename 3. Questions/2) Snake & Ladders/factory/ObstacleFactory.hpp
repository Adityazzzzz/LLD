#pragma once
#include <stdexcept>
#include "../enums/ObstacleType.hpp"
#include "../model/Obstacle.hpp"
#include "../model/Snake.hpp"
#include "../model/Ladder.hpp"
using namespace std;

class ObstacleFactory{
public:
    static Obstacle* createObstacle(ObstacleType type,int up,int down){
        switch(type){
            case ObstacleType::SNAKE:
                return new Snake(up,down);
            case ObstacleType::LADDER:
                return new Ladder(up,down);
            default:
                throw invalid_argument("Invalid obstacle type");
        }
    }
};