/*
    I am writing inline/header-only classes to save time and simplify compilation for this 45-minute window,but in production,I would separate the implementations into .cpp files
*/
#include <iostream>
#include <string>
#include "service/Game.hpp"
#include "model/Player.hpp"
using namespace std;

int main(){
    int boardSize,noOfSnakes,noOfLadders,noOfPlayers,noOfDice;

    cout << "Enter the board size : ";
    cin >> boardSize;

    cout << "Enter the number of snakes : ";
    cin >> noOfSnakes;

    cout << "Enter the number of ladders : ";
    cin >> noOfLadders;

    cout << "Enter the number of players : ";
    cin >> noOfPlayers;

    cout << "Enter the number of Dice : ";
    cin >> noOfDice;

    Game game(boardSize,noOfLadders,noOfSnakes,noOfDice);

    for(int i=0;i<noOfPlayers;i++){
        cout << "Enter the name of player " << (i+1) << " : ";
        string playerName;
        cin >> playerName;
        
        Player* player = new Player(playerName);
        game.addPlayer(player);
    }
    game.startGame();
    return 0;
}