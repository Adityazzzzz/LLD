#pragma once
#include <iostream>
#include <queue>
#include <cstdlib>
#include "../enums/ObstacleType.hpp"
#include "../factory/ObstacleFactory.hpp"
#include "../entities/Board.hpp"
#include "../entities/Dice.hpp"
#include "../entities/Player.hpp"
#include "../entities/Obstacle.hpp"
using namespace std;

class Game{
private:
    int noOfSnakes;
    int noOfLadders;
    Board* board;
    Dice* dice;
    queue<Player*> players;

public:
    Game(int size,int noOfLadders,int noOfSnakes,int noOfDice){
        this->noOfSnakes = noOfSnakes;
        this->noOfLadders = noOfLadders;
        this->board = new Board(size);
        this->dice = new Dice(noOfDice);

        initBoardObstacles();
    }
    ~Game(){
        delete board;
        delete dice;
    }

private:
    void initBoardObstacles(){
        generateObstacles(noOfSnakes,ObstacleType::SNAKE);
        generateObstacles(noOfLadders,ObstacleType::LADDER);
    }

    void generateObstacles(int count,ObstacleType type){
        int size = board->getSize();

        while(count > 0){
            int up = (rand() %(size - 1)) + 2;
            int down = (rand() %(up - 1)) + 1;

            Obstacle* obstacle = ObstacleFactory::createObstacle(type,up,down);
            if(board->addObstacle(obstacle)){
                count--;
            }
        }
    }

public:
    void addPlayer(Player* player){
        players.push(player);
    }

    void startGame(){
        board->printBoard(players);

        while(players.size() > 1){
            Player* currPlayer = players.front();
            players.pop();

            cout << "-----------------------------------\n";
            int diceRoll = dice->roll();
            cout << currPlayer->getName() << " rolled " << diceRoll << "\n";

            int newPosition = board->getNewPosition(currPlayer,diceRoll);

            if(newPosition == currPlayer->getPosition()){
                players.push(currPlayer);
                continue;
            }

            currPlayer->setPosition(newPosition);

            if(newPosition == board->getSize()){
                cout << currPlayer->getName() << " has won the game!\n";
            } 
            else{
                players.push(currPlayer);
            }

            board->printBoard(players);
        }
    }
};