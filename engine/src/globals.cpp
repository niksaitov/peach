#include "globals.h"
#include "masks.h"
#include "random.h"
#include "enum.h"

AttackTable ATTACKS;
TranspositionNode TRANSPOSITION_TABLE[NUM_TT_ENTRIES];

U64 repetitions[4096];
int repetitionIndex = 0;

U64 fileMasks[8];
U64 rankMasks[8];
U64 isolatedPawnMasks[8];
U64 whitePassedPawnMasks[64];
U64 blackPassedPawnMasks[64];

U64 PIECE_KEYS[12][64] = {0};
U64 ENPASSANT_KEYS[64] = {0};
U64 CASTLING_KEYS[16] = {0};
U64 SIDE_KEY = 0;

void generateEvaluationMasks()
{
    for (int rank = 0; rank < 8; rank++)
    {
        rankMasks[rank] |= generateMask(-1, rank);
    }

    for (int file = 0; file < 8; file++)
    {
        fileMasks[file] |= generateMask(file, -1);
        isolatedPawnMasks[file] |= generateMask(file - 1, -1);
        isolatedPawnMasks[file] |= generateMask(file + 1, -1);
    }

    for (int squareIndex = 0; squareIndex < 64; squareIndex++)
    {
        int file = squareIndex % 8;
        int rank = squareIndex / 8;

        whitePassedPawnMasks[squareIndex] = isolatedPawnMasks[file] | generateMask(file, -1);
        blackPassedPawnMasks[squareIndex] = whitePassedPawnMasks[squareIndex];

        for (int i = 0; i < (8 - rank); i++)
            whitePassedPawnMasks[squareIndex] &= ~rankMasks[7 - i];

        for (int i = 0; i < rank + 1; i++)
            blackPassedPawnMasks[squareIndex] &= ~rankMasks[i];
    }
}

void generateKeys()
{
    for (int currentPiece = whitePawn; currentPiece <= blackKing; currentPiece++)
    {
        for (int sq = 0; sq < 64; sq++)
        {
            PIECE_KEYS[currentPiece][sq] = getRandom();
            ENPASSANT_KEYS[sq] = getRandom();
        }
    }

    for (int castlingIndex = 0; castlingIndex < 16; castlingIndex++)
        CASTLING_KEYS[castlingIndex] = getRandom();

    SIDE_KEY = getRandom();
}
