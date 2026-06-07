#ifndef RANDOM_H
#define RANDOM_H

#include "typedef.h"

// Create a seed for the random numbers using the current time
inline void seedRandom()
{
    srand(static_cast<unsigned int>(time(nullptr)));
}

// Get a random bitboard
inline U64 getRandom()
{
    // Declare 4 64-bit integers
    U64 r1, r2, r3, r4;

    // Generate random numbers and take the first 16 bits
    r1 = (U64)(rand()) & 0xFFFF;
    r2 = (U64)(rand()) & 0xFFFF;
    r3 = (U64)(rand()) & 0xFFFF;
    r4 = (U64)(rand()) & 0xFFFF;

    // Shift the numbers to create one 64-bit integer
    return r1 | (r2 << 16) | (r3 << 32) | (r4 << 48);
}

// Get a random bitboard with a few bits set
inline U64 getRandomFewBits()
{
    return getRandom() & getRandom() & getRandom();
}

#endif