#include <iostream>
using namespace std;

//enums
enum class ObstacleType{
    SNAKE, LADDER
};

//entities- snake,ladder,board,player,dice, obstacles
class Player{
private:
    string name;
    int pos;
public:
    Player(string name,int pos){
        this->name = name;
        this->pos = pos;
    }
    int getPos(){ 
        return pos; 
    }
};

class Dice{
public:
    int noOfDice;
    Dice(int noOfDice){
        this->noOfDice = noOfDice;
    }
    int roll(){
        return (rand()%6)+1;
    }
};

class Obstacle{
protected:
    int src,dest;
public:
    Obstacle(int src,int dest){
        this->src=src;
        this->dest=dest;
    }
    virtual ~Obstacle() = default;

    virtual ObstacleType getType() = 0;
    int getsrc(){
        return src;
    }
    int getdest(){
        return dest;
    }
};

class Snake:public Obstacle{
public:
    Snake(int head,int tail) : Obstacle(head,tail){}
    ObstacleType getType(){
        return ObstacleType::SNAKE;
    }
};
class Ladder:public Obstacle{
public:
    Ladder(int start,int end) : Obstacle(end,start){}
    ObstacleType getType(){
        return ObstacleType::LADDER;
    }
};

class Board{ //1D
private:
    int size;
    unordered_map<int,Obstacle*> mpp;
public:
    Board(int size){
        this->size = size;
    }
    void addobstacle(Obstacle* x){
        mpp[x->getsrc()] = x;
    }
    int getNewPost(Player* player,int roll){
        int target = player->getPos() + roll;
        
        if(mpp.find(tar)!=mpp.end()){
            Obstacle* obs = mpp[tar];
            string type = (obs->getType() == ObstacleType::SNAKE) ? "bitten by Snake" : "took a Ladder";

            cout << type << obs->getsrc() << obs->getdest() << endl;
        }
        return target;
    }
};

//factory- obstaclefactory
class ObstacleFactory{
public:
    static Obstacle* func(ObstacleType type,int src,int dest){
        switch(type){
            case ObstacleType::SNAKE: return new Snake(src,dest);
            case ObstacleType::LADDER: return new Ladder(src,dest);
            default: break;
        }
    }
};

//manager- game
class Game{
private:
    Board* board;
    Dice* dice;
    queue<Player*> players;
public:
    Game(int boardsize,int noOfDiceses){
        board = new Board(boardsize);
        dice = new Dice(noOfDiceses);
    }
    void addPlayer(string name){
        players.push(new Player(name));
    }
    void addObstacle(ObstacleType type,int src,int dest){
        board->addobstacle(ObstacleFactory::func(type,src,dest));
    }

    void start(){
        while(!players.empty()){
            Player* p = players.front();
            players.pop();

            int roll = dice->roll();
            int newPos = board->getNewPosition(p,roll);
            
            if(newPos != p->getPosition()) {
                cout << p->getName() << " rolled " << roll << ", moved from " << p->getPosition() << " to " << newPos << "\n";
                p->setPosition(newPos);
            }

            if(newPos == board->getSize()) {
                cout << "🏆 " << p->getName() << " WINS!\n";
            } 
            else {
                players.push(p);
            }
        } 
    }
};


int main(){
    Game game(100,2);

    game.addObstacle(ObstacleType::SNAKE, 99, 10);
    game.addObstacle(ObstacleType::SNAKE, 50, 5);
    game.addObstacle(ObstacleType::LADDER, 14, 48);
    game.addObstacle(ObstacleType::LADDER, 42, 88);

    game.addPlayer("Alice");
    game.addPlayer("Bob");

    game.start();

    return 0;
}