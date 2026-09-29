#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <queue>
#include <iomanip>

#include "Cell.hpp"
#include "Player.hpp"
#include "Obstacle.hpp"
#include "../enums/ObstacleType.hpp"
using namespace std;

class Board{
private:
    int size;
    int sideLength;
    vector<vector<Cell*>> grid;

public:
    Board(int size){
        this->size = size;
        this->sideLength = sqrt(size);
        this->grid.resize(sideLength,vector<Cell*>(sideLength,nullptr));

        int position = 1;
        bool leftToRight = true;

        for(int i=sideLength-1;i>=0;i--){
            if(leftToRight){
                for(int j=0;j<sideLength;j++){
                    grid[i][j] = new Cell(position++);
                }
            } 
            else{
                for(int j=sideLength-1;j>=0;j--){
                    grid[i][j] = new Cell(position++);
                }
            }
            leftToRight = !leftToRight;
        }
    }

    int getRow(int position){
        int row =(position - 1) / sideLength;
        return sideLength - 1 - row;
    }

    int getCol(int position){
        int row = getRow(position);
        int col = (position-1) % sideLength;
        return (row%2 == 0) ? sideLength-1-col : col;
    }

    Cell* getCell(int position){
        return grid[getRow(position)][getCol(position)];
    }

    bool addObstacle(Obstacle* obstacle){
        Cell* srcCell = getCell(obstacle->getSrc());
        Cell* destCell = getCell(obstacle->getDest());

        if(srcCell->hasObstacle() || destCell->hasObstacle()){
            return false;
        }

        srcCell->setObstacle(obstacle);
        return true;
    }

    int getNewPosition(Player* player,int offset){
        int newPosition = player->getPosition() + offset;

        if(newPosition > size){
            cout << "You are going out of the board! Better luck next time!" << endl;
            return player->getPosition();
        }

        Cell* cell = grid[getRow(newPosition)][getCol(newPosition)];
        int finalPosition = cell->getFinalPosition();

        if(finalPosition < newPosition){
            cout << "Oops! Snake has bitten " << player->getName() << endl;
        } 
        else if(finalPosition > newPosition){
            cout << "Congratulations! " << player->getName() << " moved up through a ladder\n";
        } 
        else{
            cout << player->getName() << " moved from " << player->getPosition() << " to " << newPosition << endl;
        }

        return finalPosition;
    }

    void printBoard(queue<Player*> players){
        cout << "\nCurrent Board State:\n";

        for(int i=0;i<sideLength;i++){
            for(int j=0;j<sideLength;j++){
                int position = grid[i][j]->getPosition();
                
                string cellContent = to_string(position);

                if(grid[i][j]->hasObstacle()){
                    Obstacle* obstacle = grid[i][j]->getObstacle();
                    if(obstacle->getObstacleType() == ObstacleType::SNAKE){
                        cellContent = "🐍" + to_string(obstacle->getDest());
                    }
                    else{
                        cellContent = "🪜" + to_string(obstacle->getDest());
                    }
                }

                queue<Player*> tempQueue = players;
                while(!tempQueue.empty()){
                    Player* player = tempQueue.front();
                    tempQueue.pop();

                    if(player->getPosition() == position){
                        cellContent = player->getName();
                    }
                }

                cout << left << setw(8) << cellContent;
            }
            cout << endl;
        }
        cout << endl;
    }
};