#include <iostream>
#include <cstring>
#include <chrono>
#include <vector>

#include "typedef.h"
#include "const.h"
#include "enum.h"
#include "AttackTable.h"
#include "TranspositionNode.h"
#include "masks.h"
#include "random.h"
#include "Position.h"
#include "move_encoding.h"

using std::cout, std::string;

// Declare global class instances
AttackTable ATTACKS;
TranspositionNode TRANSPOSITION_TABLE[NUM_TT_ENTRIES];

// Declare global variables tracking the repetitions
U64 repetitions[4096];
int repetitionIndex = 0;

// Declare the evaluation masks
U64 fileMasks[8];
U64 rankMasks[8];
U64 isolatedPawnMasks[8];
U64 whitePassedPawnMasks[64];
U64 blackPassedPawnMasks[64];

// Fill the evaluation masks
void generateEvaluationMasks()
{

    // Loop over the ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // Fill the rank masks
        rankMasks[rank] |= generateMask(-1, rank);
    }

    // Loop over the files
    for (int file = 0; file < 8; file++)
    {

        // Fill the file masks
        fileMasks[file] |= generateMask(file, -1);

        // Fill the isolatet pawn masks
        isolatedPawnMasks[file] |= generateMask(file - 1, -1);
        isolatedPawnMasks[file] |= generateMask(file + 1, -1);
    }

    // Loop over the squares
    for (int squareIndex = 0; squareIndex < 64; squareIndex++)
    {

        // Get the file and rank
        int file = squareIndex % 8;
        int rank = squareIndex / 8;

        // Fill the passed pawn masks
        whitePassedPawnMasks[squareIndex] = isolatedPawnMasks[file] | generateMask(file, -1);
        blackPassedPawnMasks[squareIndex] = whitePassedPawnMasks[squareIndex];

        // Remove the ranks that the pawn has already advanced past
        for (int i = 0; i < (8 - rank); i++)
        {
            whitePassedPawnMasks[squareIndex] &= ~rankMasks[7 - i];
        }

        // Remove the ranks that the pawn has already advanced past
        for (int i = 0; i < rank + 1; i++)
        {
            blackPassedPawnMasks[squareIndex] &= ~rankMasks[i];
        }
    }
}

// Declare the arrays of hashing keys
U64 PIECE_KEYS[12][64] = {0};
U64 ENPASSANT_KEYS[64] = {0};
U64 CASTLING_KEYS[16] = {0};
U64 SIDE_KEY = 0;

// Fill the hashing keys arrays
void generateKeys()
{
    // Loop over the pieces
    for (int currentPiece = whitePawn; currentPiece <= blackKing; currentPiece++)
    {

        // Loop over the squares
        for (int currentSquareIdex = 0; currentSquareIdex < 64; currentSquareIdex++)
        {
            // Assign a random 64-bit integer as the hash keys
            PIECE_KEYS[currentPiece][currentSquareIdex] = getRandom();
            ENPASSANT_KEYS[currentSquareIdex] = getRandom();
        }
    }

    // Loop over the castling rights indicies
    for (int castlingIndex = 0; castlingIndex < 16; castlingIndex++)
    {
        // Assign a random 64-bit integer as the hash key
        CASTLING_KEYS[castlingIndex] = getRandom();
    }

    // Assign a random 64-bit integer as the hash key
    SIDE_KEY = getRandom();
}

void search(string fenString, int depth)
{

    generateKeys();
    generateEvaluationMasks();

    Position position(fenString);
    position.getBoard().printState();
    position.resetSearchVariables();

    int alpha = -INF, beta = INF;

    for (int currentDepth = 1; currentDepth <= depth; currentDepth++)
    {
        int score = position.negamax(alpha, beta, currentDepth);

        if ((score <= alpha) || (score >= beta))
        {
            alpha = -INF;
            beta = INF;
            continue;
        }

        alpha = score + ASPIRATION_WINDOW;
        beta = score - ASPIRATION_WINDOW;

        cout << "\n\nEvaluation: " << score;
        cout << "\nPrincipled variation: ";
        position.printPV();
    }

    cout << "\n\nBest Move: ";
    printMove(position.getBestMove());
}

int main()
{
    search(START_POSITION_FEN, 10);
    cout << "\n";

    return 0;
}
