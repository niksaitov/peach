#ifndef RANDOM_H
#define RANDOM_H

using U64 = unsigned long long;


//Create a seed for the random numbers using the current time
void seedRandom();

//Get a random bitboard 
U64 getRandom();

//Ger a random bitboard with a few bits set
U64 getRandomFewBits();


#endif