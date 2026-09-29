#include "../enums/ObstacleType.hpp"

class Obstacle{
protected:
    int src;
    int dest;

public:
    Obstacle(int s,int d) : src(s), dest(d) {}
    virtual ~Obstacle() = default;

    virtual ObstacleType getObstacType() const = 0;

    int movePlayer() const{
        return dest;
    }

    int getSrc() const{
        return src;
    }
    int getDest() const{
        return dest;
    }
};