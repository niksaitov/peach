#ifndef MASKS_H
#define MASKS_H

using U64 = unsigned long long;

//Mask the pawn attacks for a given square and color
U64 maskPawnAttacks(int squareIndex, int sideToMove);

//Mask the knight attacks for a given square
U64 maskKnightAttacks(int squareIndex);

//Mask the bishop attacks for a given square
U64 maskBishopAttacks(int squareIndex);

//Mask the rook attacks for a given square
U64 maskRookAttacks(int squareIndex);

//Mask the king attacks for a given square
U64 maskKingAttacks(int squareIndex);

//Generate a custom file-rank mask
U64 generateMask(int file, int rank);

#endif