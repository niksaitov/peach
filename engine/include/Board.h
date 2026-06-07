#ifndef BOARD_H
#define BOARD_H

#include <string>

#include "AttackTable.h"
#include "TranspositionNode.h"
#include "MoveList.h"
#include "typedef.h"
#include "const.h"
#include "enum.h"

class Board
{

private:
    // Arrays representing the board
    U64 bitboards[12];
    U64 occupancies[3];

    // State variables
    int sideToMove = NO_SIDE_TO_MOVE;
    int enPassantSquareIndex = NO_SQUARE_INDEX;
    int canCastle = 0;
    U64 hashKey;

    // Clear the board
    void resetBitboards();

    // Clear the occupancy arrays
    void resetOccupancies();

    // Populate occupancies from the board state
    void populateOccupancies();

    void generateHash();

    bool isSquareAttacked(int squareIndex, int sideToMove);

public:
    // Default constructor
    Board() {}

    // FEN Constructor
    Board(std::string fenString)
    {
        loadFenString(fenString);
        generateHash();
    }

    // Find the heuristic value of the position
    int staticEvaluate();

    // Print the state of the board
    void printState();

    // Generate the list of all pseudo-legal moves in a position
    MoveList generateMoves();

    int makeMove(int move);

    // Calculate the game score
    int calculateGameScore();

    // Load the board from the FEN string
    void loadFenString(const std::string &fenString);

    // Load a move string in FEN notation
    void loadMoveString(const std::string &moveString);

    // Write a hash entry into the transposition table
    void writeHashEntry(int score, int depth, int searchPly, int flag);

    // Read the hash entry from the transposition table
    int readHashEntry(int alpha, int beta, int depth, int searchPly);

    void updateHashKey(int value);

    // Pass a turn to the opposite color
    void switchSideToMove();

    bool isKingInCheck();

    // Reset the en passant square index
    void resetEnPassantSquareIndex();

    int getSideToMove();

    int getEnPassantSquareIndex();

    int getHashKey();

    U64 *getBitboards();
};

#endif
