#include <cstring>
#include "magic_numbers.h"
#include "bitboard_operations.h"
#include "engine_exceptions.h"
#include "masks.h"
#include "random.h"
#include "const.h"

extern "C" {

    //Dynamically generate bishop attack mask for a given occupancy and the square index
    U64 generateBishopAttacks(int squareIndex, const U64& occupancy){

        U64 attacksBitboard = 0ULL;
        
        int rank, file;

        //Get the file and the rank from the square index
        int targetRank = squareIndex / 8;
        int targetFile = squareIndex % 8;

        //Cast an attack ray in the positive file and positive rank direction
        for(rank = targetRank + 1, file  = targetFile + 1; rank < 8 && file < 8; rank++, file++){

            //Set the bit on the attack bitboard
            setBit(attacksBitboard, rank * 8 + file);

            //Break out the loop if the piece is found at the current square index
            if(getBit(occupancy, rank * 8 + file)){
                break;
            }

        }

        //Cast an attack ray in the positive file and negative rank direction
        for(rank = targetRank - 1, file = targetFile + 1; rank >= 0 && file < 8; rank--, file++){

            //Set the bit on the attack bitboard
            setBit(attacksBitboard, rank * 8 + file);

            //Break out the loop if the piece is found at the current square index
            if(getBit(occupancy, rank * 8 + file)){
                break;
            }

        }

        //Cast an attack ray in the negative file and positive rank direction
        for(rank = targetRank + 1, file = targetFile - 1; rank < 8 && file >= 0; rank++, file--){
            
            //Set the bit on the attack bitboard
            setBit(attacksBitboard, rank * 8 + file);

            //Break out the loop if the piece is found at the current square index
            if(getBit(occupancy, rank * 8 + file)){
                break;
            }

        }

        //Cast an attack ray in the negative file and negative rank direction
        for(rank = targetRank - 1, file = targetFile - 1; rank >= 0 && file >= 0; rank--, file--){

            //Set the bit on the attack bitboard
            setBit(attacksBitboard, rank * 8 + file);

            //Break out the loop if the piece is found at the current square index
            if(getBit(occupancy, rank * 8 + file)){
                break;
            }

        }

        return attacksBitboard;

    }

    //Dynamically generate rook attack mask for a given occupancy and the square index
    U64 generateRookAttacks(int squareIndex, const U64& occupancy){

        U64 attacksBitboard = 0ULL;

        int rank, file;

        //Get the file and the rank from the square index
        int targetRank = squareIndex / 8;
        int targetFile = squareIndex % 8;

        //Cast an attack ray in the positive rank direction
        for(rank = targetRank + 1; rank < 8; rank++){

            //Set the bit on the attack bitboard
            setBit(attacksBitboard, rank * 8 + targetFile);
            
            //Break out the loop if the piece is found at the current square index
            if(getBit(occupancy, rank * 8 + targetFile)){
                break;
            }

        }

        //Cast an attack ray in the negative rank direction
        for(rank = targetRank - 1; rank >= 0; rank--){

            //Set the bit on the attack bitboard
            setBit(attacksBitboard, rank * 8 + targetFile);

            //Break out the loop if the piece is found at the current square index
            if(getBit(occupancy, rank * 8 + targetFile)){
                break;
            }

        }

        //Cast an attack ray in the positive file direction
        for(file = targetFile + 1; file < 8; file++){

            //Set the bit on the attack bitboard
            setBit(attacksBitboard, targetRank * 8 + file);

            //Break out the loop if the piece is found at the current square index
            if(getBit(occupancy, targetRank * 8 + file)){
                break;
            }
        }

        //Cast an attack ray in the negative file direction
        for(file = targetFile - 1; file >= 0; file--){

            //Set the bit on the attack bitboard
            setBit(attacksBitboard, targetRank * 8 + file);

            //Break out the loop if the piece is found at the current square index
            if(getBit(occupancy, targetRank * 8 + file)){
                break;
            }
        }

        return attacksBitboard;

    }

    //Generate the occupancy bitboards from the given occupancy index
    U64 getOccupancyFromIndex(int occupancyIndex, int relevantBits, U64 attackMask){

        U64 bitboard = 0ULL;

        //Loop over the relevant bits of the attack mask
        for (int currentBit = 0; currentBit < relevantBits; currentBit++){
            
            //Get the LS1B index
            int squareIndex = getLS1BIndex(attackMask);

            //Pop the LS1B from the bitboard
            popBit(attackMask, squareIndex);

            //If the occupancy index includes the current bit
            if(occupancyIndex & (1 << currentBit)){
                //Set the bit on the occupancy bitboard
                setBit(bitboard, squareIndex);
            }

        }

        return bitboard;

    }
    
    //Find a magic number for the given sliding piece and a given 
    U64 findMagicNumber(int squareIndex, int relevantBits, bool fBishop){

        //Initialise attack and occupancy arrays; 4096 is used as log2(4096) is 12, which is the highest number of relevant bits
        U64 occupancies[4096], attacks[4096], usedAttacks[4096];

        //Assign the attack mask
        U64 attackMask = fBishop ? maskBishopAttacks(squareIndex) : maskRookAttacks(squareIndex);

        /*
        Maximum occupancy index depends on the position on the piece, so if it only controls 3 squares, 
        there is no need to loop over large indicies, 
        as the logical AND from getOccupancyFromIdex (occupancyIndex & (1 << currentBit)) will never return 1
        */
        int maxOccupancyIndex = 1 << relevantBits;

        for(int index = 0; index < maxOccupancyIndex; index++){

            //Fill the occupancy arrays
            occupancies[index] = getOccupancyFromIndex(index, relevantBits, attackMask);

            //Fill the attack arrays with dynamic attack masks, passing occupancies at the same index as one of the arguments
            attacks[index] = fBishop ? generateBishopAttacks(squareIndex, occupancies[index]) : generateRookAttacks(squareIndex, occupancies[index]);

        }

        //Seed the pseudo-random number generator
        seedRandom();

        //Trial and error method needs a lot of repetitions, values from 10^7 to 10^9 should be used
        for(int i = 0; i < 100000000; i++){

            //Get a random bitboard with a few bits set
            U64 magicNumber  = getRandomFewBits();

            //Skip the magic number if the magic index will not be dense enough
            if(getPopulationCount((attackMask * magicNumber) & 0xFF00000000000000) < 6){
                continue;
            }

            //Reset the array of used attacks
            memset(usedAttacks, 0, sizeof(usedAttacks));

            int index;
            bool fFail;

            for(index = 0, fFail = false; !fFail && index < maxOccupancyIndex; index++){

                //Generate the magic number and destroy the garbage bits
                int magicIndex = (int)((occupancies[index] * magicNumber) >> (64 - relevantBits));

                //If the magicIndex has no mapping
                if(usedAttacks[magicIndex] == 0ULL){
                    //Save the attack bitboard
                    usedAttacks[magicIndex] = attacks[index];
                //If the magicIndex does not map the occupancy to the attacks
                }else if(usedAttacks[magicIndex] != attacks[index]){
                    fFail = true;
                }

            }

            //If magicIdex works correctly
            if(!fFail){
                //Return the magic number
                return magicNumber;
            }

        }

        //Throw an exception if the magic number was not found
        throw CannotFindMagicNumberException();

        //Return an empty bitboard
        return 0ULL;

    }

}