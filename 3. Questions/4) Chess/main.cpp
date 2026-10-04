#include <iostream>
using namespace std;

//enum
enum class Color{
    WHITE, BLACK
};
enum class PieceType{
    KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

class MoveStrategy{
public:
    virtual ~MoveStrategy() = default;
    virtual bool isValidMove() = 0;
    virtual Move* getMove() = 0
}
// 8 type of moves
class ...

class Piece{
protected:
    Color color;
    bool isKilled;
public:
    Piece(Color color){
        this->color = color;
        this->isKilled = false;
    }
    virtual ~Piece() = default;
    virtual PieceType getType() = 0;

    Color getcolor(){ return color; }
    bool getIsKilled(){ 
        return isKilled;
    }
    void setIsKilled(bool status){ 
        this->isKilled = status; 
    }
}

class King:public Piece{
public:
    King(Color color) : Piece(color){}
    PieceType getType(){
        return PieceType::KING;
    }
}
// similarly for QUEEN, ROOK, BISHOP, KNIGHT, PAWN

class PieceFactory{
public:
    static Piece* createPiece(PieceType type,Color color){
        switch(type){
            case PieceType::KING: return new King(color);
            case PieceType::QUEEN: return new Queen(color);
            case PieceType::ROOK: return new Rook(color);
            case PieceType::BISHOP: return new Bishop(color);
            case PieceType::KNIGHT: return new Knight(color);
            case PieceType::PAWN: return new Pawn(color);
            default: return nullptr;
        }
    }
};