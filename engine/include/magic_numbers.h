#ifndef MAGICS_NUMBERS_H
#define MAGICS_NUMBERS_H

using U64 = unsigned long long;

//Dynamically generate bishop attack mask for a given occupancy and the square index
U64 generateBishopAttacks(int squareIndex, const U64& occupancy);

//Dynamically generate rook attack mask for a given occupancy and the square index
U64 generateRookAttacks(int squareIndex, const U64& occupancy);

//Generate the occupancy bitboards from the given occupancy index
U64 getOccupancyFromIndex(int occupancyIndex, int relevantBits, U64 attackMask);

//Find a magic number for the given sliding piece and a given 
U64 findMagicNumber(int squareIndex, int relevantBits, bool fBishop);

#endif
