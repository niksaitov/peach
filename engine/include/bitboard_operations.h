#ifndef BITBOARD_OPERATIONS_H
#define BITBOARD_OPERATIONS_H

#include <iostream>
#include <exception>

using U64 = unsigned long long;


//Set a bit at the given square index on the given bitboard
inline void setBit(U64& bitboard, int squareIndex){
    bitboard |= (1ULL << squareIndex);
}

//Pop the bit at the given square index on the given bitboard
inline void popBit(U64& bitboard, int squareIndex){
    bitboard & (1ULL << squareIndex) ? bitboard ^= (1ULL << squareIndex) : 0;
}

//Get a bit at the given square index on the given bitboard
inline int getBit(U64 bitboard, int squareIndex){
    return (bitboard & (1ULL << squareIndex))? 1 : 0;   
}

//Get the cardinality of the given bitboard
inline int getPopulationCount(U64 bitboard){

    int populationCount = 0;
    
    //While there are bits on the bitboard
    while(bitboard){

        //Increment the populationCount variable
        populationCount++;
        //Remove the LS1B of the bitboard
        bitboard &= bitboard - 1;

    }

    return populationCount;

}

//Get the LS1B if of the given bitboard
inline int getLS1BIndex(U64 bitboard){
    
    //If a non-empty bitboard is passed
    if(bitboard){
        //Bit manipulation to get the population count the trailing bits
        return getPopulationCount((bitboard & -bitboard) - 1);
    //If an empty bitboard is passed
    }else{
        throw std::invalid_argument("Invalid bitboard: empty bitboard");
    }

}

//Print the given bitboard 
inline void printBitboard(const U64& bitboard){

    std::cout << '\n' << "Visual representation: " << "\n\n";
    
    for(int rank = 0; rank < 8; rank++){

        for(int file = 0; file < 8; file++){

            //Least significant file (LSF) mapping
            int squareIndex = rank * 8 + file;

            //Print the ranks
            if(!file){
                std::cout << 8 - rank << "  ";
            }

            std::cout << (getBit(bitboard, squareIndex)? 1 : 0) << ' ';

        }
        std::cout << '\n';
    }

    //Print the files
    std::cout << '\n' << "   a b c d e f g h " << '\n';
    //Display the decimal equivalent of a bitboard
    std::cout << '\n' << "Decimal representation: " << bitboard << '\n';

}


#endif