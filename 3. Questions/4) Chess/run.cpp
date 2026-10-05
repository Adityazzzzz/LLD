#include <iostream>
using namespace std;

enum class Color{WHITE,BLACK};
enum class PieceType{KING,KNIGHT};
enum class PlayerType{ HUMAN,COMPUTER };

class Board;
class Piece;

class Player{
protected:
    string name;
    Color color;
public:
    Player(string name,Color color){
        this->name = name;
        this->color = color;
    }
    virtual ~Player() = default;
    virtual PlayerType getType() = 0;
    
    string getName(){ return name; }
    Color getColor(){ return color; }
};

class HumanPlayer:public Player{
public:
    HumanPlayer(string name,Color color):Player(name,color){}
    PlayerType getType() override{ return PlayerType::HUMAN; }
};
class ComputerPlayer:public Player{
public:
    ComputerPlayer(string name,Color color):Player(name,color){}
    PlayerType getType() override{ return PlayerType::COMPUTER; }
};

class PlayerFactory{
public:
    static Player* createPlayer(PlayerType type,string name,Color color){
        switch(type){
            case PlayerType::HUMAN:
                return new HumanPlayer(name,color);
            case PlayerType::COMPUTER:
                return new ComputerPlayer(name,color);
            default:
                return nullptr;
        }
    }
};

class Cell{
private:
    int row;
    int col;
    Piece* piece;
public:
    Cell(int row,int col){
        this->row=row;
        this->col=col;
    }
    int getRow(){ return row; }
    int getCol(){ return col; }

    Piece* getPiece(){ return piece; }
    void setPiece(Piece* p){ this->piece = p; }
};

class MoveStrategy{
public:
    virtual ~MoveStrategy() = default;
    virtual isValidMove(Cell* src,Cell* dest,Board* board) = 0;
};

class KingStrategy:public MoveStrategy{
public:
    bool isValidMove(Cell* src,Cell* dest,Board* board) override{
        int dx = abs(src->getRow() - dest->getRow());
        int dy = abs(src->getCol() - dest->getCol());
        return (dx <= 1 && dy <= 1);
    }
};
class LShapedStrategy:public MoveStrategy{
public:
    bool isValidMove(Cell* src,Cell* dest,Board* board) override{
        int dx = abs(src->getRow() - dest->getRow());
        int dy = abs(src->getCol() - dest->getCol());
        return (dx * dy == 2); 
    }
};

class Piece{
protected:
    Color color;
    bool isKilled;
    vector<MoveStrategy*>moves;
public:
    Piece(Color color){
        this->color = color;
        this->isKilled = false;
    }
    virtual ~Piece() = default;
    virtual PieceType getType() = 0;

    Color getColor(){ return color; }
    bool getIsKilled(){ return isKilled; }
    void setIskilled(bool status){ this->isKilled = status; }

    void addStrategy(MoveStrategy* strategy){
        this->moves.push_back(strategy);
    }
    bool canMove(Cell* src,Cell* dest,Board* board){
        for(auto it:moves){
            if(it->isValidMove(src,dest,board)){
                return true;
            }
        }
        return false;
    }
};

class King:public Piece{
public:
    King(Color color):Piece(color){}
    PieceType getType() override{ return PieceType::KING; }
};
class Knight:public Piece{
public:
    Knight(Color color):Piece(color){}
    PieceType getType() override{ return PieceType::KNIGHT; }
};

class PieceFactory{
public:
    static Piece* createPiece(PieceType type,Color color){
        Piece* piece = nullptr;
        switch(type){
            case PieceType::KING:
                piece = new King(color);
                piece->addStrategy(new KingMoveStrategy());
                break;
            case PieceType::KNIGHT:
                piece = new Knight(color);
                piece->addStrategy(new LShapedStrategy());
                break;
        }
        return piece;
    }
};

class Move*{
private:
    Cell* startCell;
    Cell* endCell;
    Piece* pieceMoved;
    Piece* pieceKilled;
public:
    Move(Cell* startCell,Cell* endCell,Piece* pieceMoved,Piece* pieceKilled){
        this->startCell = startCell;
        this->endCell = endCell;
        this->pieceMoved = pieceMoved;
        this->pieceKilled = pieceKilled;
    }
    Cell* getStartCell(){ return startCell; }
    Cell* getEndCell(){ return endCell; }
    Piece* getPieceMoved(){ return pieceMoved; }
    Piece* getPieceKilled(){ return pieceKilled; }
};

class Board{
private:
    // int size;
    vector<vector<Cell*>> grid;
public:
    Board(){
        grid.resize(8,vector<Cell*>(8,nullptr));
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                grid[i][j] = new Cell(i,j);
            }
        }
    }
    Cell* cell(int r,int c){
        if(r<0 || r>8-1 || c<0 || c>8-1) return nullptr;
        return grid[r][c];
    }
};

class Spectator{
public:
    virtual ~Spectator() = default;
    virtual void update(string moveInfo) = 0;
};
class ConsoleSpectator:public Spectator{
private:
    string name;
public:
    ConsoleSpectator(string name){ 
        this->name = name; 
    }
    void update(string moveInfo) override{
        cout << "[Spectator " << name << "] Broadcast received: " << moveInfo << "\n";
    }
};

class Chess{
private:
    Board* board;
    queue<Player*> players;
    stack<Move*> moveHistory;
    vector<Spectator*> spectators;

    void notifySpectators(sring msg){
        for(auto it:spectators){
            it->update(msg);
        }
    }
public:
    Chess(){
        this->board = new board();
    }
    void addPlayer(Player* p){
        this->players.push(p);
    }
    void addSpectator(Spectator* s){
        this->spectators.push_back(s);
    }

    bool placePiece(int row,int col,Piece* piece){
        Cell* cell = board->getCell(row,col);
        if(cell != nullptr && piece != nullptr){
            cell->setPiece(piece);
            return true;
        }
        return false;
    }

    bool makeMove(int startRow,int startCol,int endRow,int endCol){
        if(players.empty()) return false;
        
        Player* currentPlayer = players.front();
        Cell* src = board->getCell(startRow,startCol);
        Cell* dest = board->getCell(endRow,endCol);

        if(!src || !dest || !src->getPiece()){
            cout << "Invalid selection.\n";
            return false;
        }

        Piece* pieceToMove = src->getPiece();
        
        if(pieceToMove->getColor() != currentPlayer->getColor()){
            cout << currentPlayer->getName() << " cannot move opponent's pieces!\n";
            return false;
        }

        Piece* destPiece = dest->getPiece();

        if(destPiece != nullptr && destPiece->getColor() == pieceToMove->getColor()){
            cout << "Cannot attack your own piece.\n";
            return false;
        }

        if(!pieceToMove->canMove(src,dest,board)){
            cout << "Invalid move math.\n";
            return false;
        }

        if(destPiece != nullptr){
            destPiece->setIsKilled(true);
        }
        
        dest->setPiece(pieceToMove);
        src->setPiece(nullptr);

        Move* currentMove = new Move(src,dest,pieceToMove,destPiece);
        moveHistory.push(currentMove);

        string moveInfo = currentPlayer->getName() + " moved to (" + to_string(endRow) + "," + to_string(endCol) + ")";
        if(destPiece) moveInfo += " - CAPTURE!";
        notifySpectators(moveInfo);

        players.pop();
        players.push(currentPlayer);

        return true;
    }

    void undoMove(){
        if(moveHistory.empty()){
            cout << "No moves to undo.\n";
            return;
        }

        Move* lastMove = moveHistory.top();
        moveHistory.pop();

        Cell* src = lastMove->getStartCell();
        Cell* dest = lastMove->getEndCell();
        Piece* movedPiece = lastMove->getPieceMoved();
        Piece* killedPiece = lastMove->getPieceKilled();

        src->setPiece(movedPiece);
        dest->setPiece(killedPiece);

        if(killedPiece != nullptr){
            killedPiece->setIsKilled(false);
        }

        // Return turn to the previous player
        Player* lastPlayer = players.back();
        queue<Player*> temp;
        temp.push(lastPlayer);
        while(players.front() != lastPlayer){
            temp.push(players.front());
            players.pop();
        }
        players = temp;

        notifySpectators("Move undone. Board reverted.");
    }
};

int main(){
    Chess game;
    
    Player* p1 = PlayerFactory::createPlayer(PlayerType::HUMAN,"Alice",Color::WHITE);
    Player* p2 = PlayerFactory::createPlayer(PlayerType::COMPUTER,"DeepBlue",Color::BLACK);
    game.addPlayer(p1);
    game.addPlayer(p2);

    Spectator* fan = new ConsoleSpectator("Charlie");
    game.addSpectator(fan);

    game.placePiece(0,1,PieceFactory::createPiece(PieceType::KNIGHT,Color::WHITE));
    game.placePiece(0,4,PieceFactory::createPiece(PieceType::KING,Color::WHITE));
    game.placePiece(7,4,PieceFactory::createPiece(PieceType::KING,Color::BLACK));
    cout << "Board dynamically initialized from main().\n";

    cout << "\n--- TURN 1 (Alice): Valid Knight Move ---\n";
    game.makeMove(0,1,2,2); 

    cout << "\n--- TURN 2 (DeepBlue): Invalid King Move ---\n";
    game.makeMove(7,4,5,4); 

    cout << "\n--- TURN 3: Undo Last Move ---\n";
    game.undoMove();

    return 0;
}