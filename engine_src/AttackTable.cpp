    #include "AttackTable.h"
    #include "bitboard_operations.h"
    #include "engine_exceptions.h"
    #include "magic_numbers.h"
    #include "masks.h"
    #include "const.h"

    extern "C" {

        enum {white, black, both};

        //Initialise magic numbers
        void AttackTable::initialiseMagicNumbers(){

            //Loop over the squares 
            for(int squareIndex = 0; squareIndex < 64; squareIndex++){

                //Initialise magic numbers for bishops
                bishopMagics[squareIndex] = findMagicNumber(squareIndex, BISHOP_RELEVANT_BITS[squareIndex], true);

                //Initialise magic numbers for rooks
                rookMagics[squareIndex] = findMagicNumber(squareIndex, ROOK_RELEVANT_BITS[squareIndex], false);

            }    

        }

        //Initialise leaping piece attack tables
        void AttackTable::initialiseLeapingPieceTables(){

            //Loop over the squares 
            for(int squareIndex = 0; squareIndex < 64; squareIndex++){

                //Initialise leaping piece atttacks//
                pawnAttacks[white][squareIndex] = maskPawnAttacks(squareIndex, white);
                pawnAttacks[black][squareIndex] = maskPawnAttacks(squareIndex, black);
                knightAttacks[squareIndex] = maskKnightAttacks(squareIndex);
                kingAttacks[squareIndex] = maskKingAttacks(squareIndex);

            }

        }

        //Initialise sliding piece attack tables
        void AttackTable::initialiseSlidingPieceTables(bool fBishop){

            //Check if the magic numbers are initialised
            if(!bishopMagics[0] || !rookMagics[0]){
                throw MagicNumberNotInitialisedException();
            }

            //Loop over the squares
            for(int squareIndex = 0; squareIndex < 64; squareIndex++){

                //Fill the arrays of masks to fetch attacks later
                bishopMasks[squareIndex] = maskBishopAttacks(squareIndex);
                rookMasks[squareIndex] = maskRookAttacks(squareIndex);

                //Assign the attack mask
                U64 attackMask = fBishop ? bishopMasks[squareIndex] : rookMasks[squareIndex];

                int relevantBits = getPopulationCount(attackMask);
                int maxOccupancyIndex = 1 << relevantBits;

                //Loop over the occupancy indicies
                for(int occupancyIndex = 0; occupancyIndex < maxOccupancyIndex; occupancyIndex++){

                    //Get an occupancy bitboard from the current occupancy index
                    U64 occupancy = getOccupancyFromIndex(occupancyIndex, relevantBits, attackMask);
                        
                    if(fBishop){

                        //Generate a magicIndex
                        int magicIndex = (occupancy * bishopMagics[squareIndex]) >> (64 - BISHOP_RELEVANT_BITS[squareIndex]);

                        //Fill the bishop attacks array
                        bishopAttacks[squareIndex][magicIndex] = generateBishopAttacks(squareIndex, occupancy);

                    }else{

                        //Generate a magicIndex
                        int magicIndex = (occupancy * rookMagics[squareIndex]) >> (64 - ROOK_RELEVANT_BITS[squareIndex]);

                        //Fill the rook attacks array
                        rookAttacks[squareIndex][magicIndex] = generateRookAttacks(squareIndex, occupancy);

                    }

                }

            }

        }

        //Get pawn attacks
        const U64 AttackTable::getPawnAttacks(int sideToMove, int squareIndex){
            //Fetch the attacks
            return pawnAttacks[black][squareIndex];
        }

        //Get knight attacks
        const U64 AttackTable::getKnightAttacks(int squareIndex){
            //Fetch the attacks
            return knightAttacks[squareIndex];
        }

        //Get king attacks
        const U64 AttackTable::getKingAttacks(int squareIndex){
            //Fetch the attacks
            return kingAttacks[squareIndex];
        }

        //Get bishop attacks
        const U64 AttackTable::getBishopAttacks(int squareIndex, U64 occupancy){

            //Convert the occupancy into the index of the attack table 
            occupancy &= bishopMasks[squareIndex]; 
            occupancy *= bishopMagics[squareIndex];
            occupancy >>= 64 - BISHOP_RELEVANT_BITS[squareIndex];

            //Fetch the attacks
            return bishopAttacks[squareIndex][occupancy];
        }

        //Get rook attacks
        const U64 AttackTable::getRookAttacks(int squareIndex, U64 occupancy){
            
            //Convert the occupancy into the index of the attack table 
            occupancy &= rookMasks[squareIndex]; 
            occupancy *= rookMagics[squareIndex];
            occupancy >>= 64 - ROOK_RELEVANT_BITS[squareIndex];

            //Fetch the attacks
            return rookAttacks[squareIndex][occupancy];
        }

        //Get queen attacks
        const U64 AttackTable::getQueenAttacks(int squareIndex, U64 occupancy){
            //Logical AND on the bishop and rook attacks
            return getBishopAttacks(squareIndex, occupancy) | getRookAttacks(squareIndex, occupancy);
        }  

    }
