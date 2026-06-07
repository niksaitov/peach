#ifndef MOVELIST_H
#define MOVELIST_H

#include "move_encoding.h"

class MoveList
{

private:
    int moves[256];
    int count = 0;

public:
    MoveList() {};

    // Add move to the moves array at index specified by the count member variable
    inline void appendMove(int startSquareIndex, int targetSquareIndex, int piece, int promotedPiece,
                           bool fCapture, bool fDoublePawnPush, bool fEnPassant, bool fCastling)
    {
        moves[count++] = createMove(startSquareIndex, targetSquareIndex, piece, promotedPiece,
                                    fCapture, fDoublePawnPush, fEnPassant, fCastling);
    }

    // Get the array of moves
    inline int *getMoves() { return moves; };

    // Get the value of the count variable
    inline int getCount() const { return count; };
};

#endif
