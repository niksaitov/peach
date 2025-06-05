#include <iostream>
#include <cstdlib>
#include "random.h"


//Create a seed for the random numbers using the current time
void seedRandom(){
    srand(time(NULL));
}

//Get a random bitboard 
U64 getRandom(){

    //Declare 4 64-bit integers
    U64 r1, r2, r3, r4;

    //Generate random numbers and take the first 16 bits
    r1 = (U64)(rand()) & 0xFFFF;
    r2 = (U64)(rand()) & 0xFFFF;
    r3 = (U64)(rand()) & 0xFFFF;
    r4 = (U64)(rand()) & 0xFFFF;    

    //Shift the numbers to create one 64-bit integer
    return r1 | (r2 << 16) | (r3 << 32) | (r4 << 48);
}

//Ger a random bitboard with a few bits set
U64 getRandomFewBits(){
    return getRandom() & getRandom() & getRandom();
}