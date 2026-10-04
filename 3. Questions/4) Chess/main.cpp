#include <iostream>
using namespace std;

enum class Color { WHITE, BLACK };
enum class PieceType { KING, KNIGHT };

class Board;
class Piece;

class Spectator {
public:
    virtual ~Spectator() = default;
    virtual void update(string moveInfo) = 0;
};



class ConsoleSpectator : public Spectator {
private:
    string name;
public:
    ConsoleSpectator(string name) { this->name = name; }
    void update(string moveInfo) override {
        cout << "[Spectator " << name << "] Broadcast received: " << moveInfo << "\n";
    }
};




class Cell {
private:
    int row, col;
    Piece* piece;
public:
    Cell(int r, int c) {
        this->row = r;
        this->col = c;
        this->piece = nullptr;
    }
    int getRow() { return row; }
    int getCol() { return col; }
    Piece* getPiece() { return piece; }
    void setPiece(Piece* p) { this->piece = p; }
};

class Move {
private:
    Cell* startCell;
    Cell* endCell;
    Piece* pieceMoved;
    Piece* pieceKilled;
public:
    Move(Cell* start, Cell* end, Piece* moved, Piece* killed) {
        this->startCell = start;
        this->endCell = end;
        this->pieceMoved = moved;
        this->pieceKilled = killed;
    }
    Cell* getStartCell() { return startCell; }
    Cell* getEndCell() { return endCell; }
    Piece* getPieceMoved() { return pieceMoved; }
    Piece* getPieceKilled() { return pieceKilled; }
};

// ==========================================
// 4. STRATEGY PATTERN (Movement Math)
// ==========================================
class MoveStrategy {
public:
    virtual ~MoveStrategy() = default;
    virtual bool isValidMove(Cell* src, Cell* dest, Board* board) = 0;
};

class KingMoveStrategy : public MoveStrategy {
public:
    bool isValidMove(Cell* src, Cell* dest, Board* board) override {
        int dx = abs(src->getRow() - dest->getRow());
        int dy = abs(src->getCol() - dest->getCol());
        return (dx <= 1 && dy <= 1); // Moves 1 step in any direction
    }
};

class LShapedStrategy : public MoveStrategy {
public:
    bool isValidMove(Cell* src, Cell* dest, Board* board) override {
        int dx = abs(src->getRow() - dest->getRow());
        int dy = abs(src->getCol() - dest->getCol());
        return (dx * dy == 2); // 2x1 or 1x2 L-shape
    }
};

// ==========================================
// 5. PIECE ENTITY & CHILDREN
// ==========================================
class Piece {
protected:
    Color color;
    bool isKilled;
    vector<MoveStrategy*> strategies;
public:
    Piece(Color color) {
        this->color = color;
        this->isKilled = false;
    }
    virtual ~Piece() = default;
    virtual PieceType getType() = 0;

    void addStrategy(MoveStrategy* strategy) {
        this->strategies.push_back(strategy);
    }

    bool canMove(Cell* src, Cell* dest, Board* board) {
        for (auto strategy : strategies) {
            if (strategy->isValidMove(src, dest, board)) {
                return true;
            }
        }
        return false;
    }

    Color getColor() { return color; }
    bool getIsKilled() { return isKilled; }
    void setIsKilled(bool status) { this->isKilled = status; }
};

class King : public Piece {
public:
    King(Color color) : Piece(color) {}
    PieceType getType() override { return PieceType::KING; }
};

class Knight : public Piece {
public:
    Knight(Color color) : Piece(color) {}
    PieceType getType() override { return PieceType::KNIGHT; }
};

// ==========================================
// 6. FACTORY PATTERN
// ==========================================
class PieceFactory {
public:
    static Piece* createPiece(PieceType type, Color color) {
        Piece* piece = nullptr;
        switch (type) {
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

// ==========================================
// 7. BOARD
// ==========================================
class Board {
private:
    vector<vector<Cell*>> grid;
public:
    Board() {
        grid.resize(8, vector<Cell*>(8, nullptr));
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                grid[i][j] = new Cell(i, j);
            }
        }
    }
    Cell* getCell(int r, int c) {
        if (r < 0 || r > 7 || c < 0 || c > 7) return nullptr;
        return grid[r][c];
    }
};

// ==========================================
// 8. MANAGER (Chess Controller)
// ==========================================
class Chess {
private:
    Board* board;
    stack<Move*> moveHistory;
    vector<Spectator*> spectators;

    void notifySpectators(string msg) {
        for (auto spectator : spectators) {
            spectator->update(msg);
        }
    }

public:
    Chess() {
        this->board = new Board();
    }

    void addSpectator(Spectator* s) {
        this->spectators.push_back(s);
    }

    // Skips standard 32-piece loop to cleanly demo the 2 MVP pieces
    void setupMVPBoard() {
        board->getCell(0, 4)->setPiece(PieceFactory::createPiece(PieceType::KING, Color::WHITE));
        board->getCell(7, 4)->setPiece(PieceFactory::createPiece(PieceType::KING, Color::BLACK));
        board->getCell(0, 1)->setPiece(PieceFactory::createPiece(PieceType::KNIGHT, Color::WHITE));
        cout << "Board initialized with Kings and a Knight.\n";
    }

    bool makeMove(int startRow, int startCol, int endRow, int endCol) {
        Cell* src = board->getCell(startRow, startCol);
        Cell* dest = board->getCell(endRow, endCol);

        if (!src || !dest || !src->getPiece()) {
            cout << "Invalid selection.\n";
            return false;
        }

        Piece* pieceToMove = src->getPiece();
        Piece* destPiece = dest->getPiece();

        // 1. Basic checks (Can't kill your own team)
        if (destPiece != nullptr && destPiece->getColor() == pieceToMove->getColor()) {
            cout << "Cannot attack your own piece.\n";
            return false;
        }

        // 2. Delegate to Strategy Pattern for Math
        if (!pieceToMove->canMove(src, dest, board)) {
            cout << "Invalid move for this piece type.\n";
            return false;
        }

        // 3. Execute Move Command
        if (destPiece != nullptr) {
            destPiece->setIsKilled(true);
        }
        
        dest->setPiece(pieceToMove);
        src->setPiece(nullptr);

        // 4. Push to Undo Stack
        Move* currentMove = new Move(src, dest, pieceToMove, destPiece);
        moveHistory.push(currentMove);

        // 5. Notify Observers
        string moveInfo = "Piece moved to (" + to_string(endRow) + "," + to_string(endCol) + ")";
        if (destPiece) moveInfo += " - CAPTURE!";
        notifySpectators(moveInfo);

        return true;
    }

    void undoMove() {
        if (moveHistory.empty()) {
            cout << "No moves to undo.\n";
            return;
        }

        Move* lastMove = moveHistory.top();
        moveHistory.pop();

        Cell* src = lastMove->getStartCell();
        Cell* dest = lastMove->getEndCell();
        Piece* movedPiece = lastMove->getPieceMoved();
        Piece* killedPiece = lastMove->getPieceKilled();

        // Revert board state
        src->setPiece(movedPiece);
        dest->setPiece(killedPiece);

        if (killedPiece != nullptr) {
            killedPiece->setIsKilled(false);
        }

        notifySpectators("Move undone. Piece returned to (" + to_string(src->getRow()) + "," + to_string(src->getCol()) + ")");
    }
};

// ==========================================
// 9. DRIVER
// ==========================================
int main() {
    Chess game;
    
    // Wire up Observer
    Spectator* fan = new ConsoleSpectator("Alice");
    game.addSpectator(fan);

    // Factory creates the pieces internally
    game.setupMVPBoard();

    cout << "\n--- TURN 1: Valid Knight L-Shape ---\n";
    game.makeMove(0, 1, 2, 2); 

    cout << "\n--- TURN 2: Invalid King Move (Too far) ---\n";
    game.makeMove(0, 4, 3, 4); 

    cout << "\n--- TURN 3: Undo Last Valid Move ---\n";
    game.undoMove();

    return 0;
}