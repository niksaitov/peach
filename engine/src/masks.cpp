#include "../include/masks.h"
#include "../include/bitboard_operations.h"
#include "../include/const.h"


enum {white, black, both};


//Mask the pawn attacks for a given square and color
U64 maskPawnAttacks(int squareIndex, int colour){

    U64 pawnsBitboard = 0ULL, attacksBitboard = 0ULL;
    setBit(pawnsBitboard, squareIndex);

    switch (colour)
    {

    //If the pawn is white
    case white:

        //If the pawn is not located on the h (rightmost) file 
        if(pawnsBitboard & NOT_H_FILE){
            //Shift the bit on the pawn bitboard up one row and one square to the right, then add that bit to the attacks bitboard
            attacksBitboard |= (pawnsBitboard >> 7);
        }

        //If the pawn is not located on the a (leftmost) file
        if(pawnsBitboard & NOT_A_FILE){
            //Shift the bit on the pawn bitboard up one row and to one square the left, then add that bit to the attacks bitboard
            attacksBitboard |= (pawnsBitboard >> 9);
        }

        break;

    //If the pawn is black
    case black:

        //If the pawn is not located on the h (rightmost) file 
        if(pawnsBitboard & NOT_H_FILE){
            //Shift the bit on the pawn bitboard down one row and one square to the left, then add that bit to the attacks bitboard
            attacksBitboard |= (pawnsBitboard << 9);
        }

        //If the pawn is not located on the a (leftmost) file
        if(pawnsBitboard & NOT_A_FILE){
            //Shift the bit on the pawn bitboard down one row and one square to the right, then add that bit to the attacks bitboard
            attacksBitboard |= (pawnsBitboard << 7);
        }

        break;

    }

    return attacksBitboard;

}

//Mask the knight attacks for a given square
U64 maskKnightAttacks(int squareIndex){

    U64 knightsBitboard = 0ULL, attacksBitboard = 0ULL;
    setBit(knightsBitboard, squareIndex);

    //If the knight is not located on the h (rightmost) file 
    if(knightsBitboard & NOT_H_FILE){

        //Shift the bit on the knight bitboard up two rows and one square to the right, then add that bit to the attacks bitboard
        attacksBitboard |= (knightsBitboard >> 15);

        //Shift the bit on the knight bitboard down two rows and one square to the right, then add that bit to the attacks bitboard
        attacksBitboard |= (knightsBitboard << 17);

    }

    //If the knight is not located on the a (leftmost) file
    if(knightsBitboard & NOT_A_FILE){

        //Shift the bit on the knight bitboard up two rows and one square to the left, then add that bit to the attacks bitboard
        attacksBitboard |= (knightsBitboard >> 17);
        
        //Shift the bit on the knight bitboard down two rows and one square to the left, then add that bit to the attacks bitboard
        attacksBitboard |= (knightsBitboard << 15);

    }

    //If the knight is not located on the h or g (two rightmost) files
    if(knightsBitboard & NOT_HG_FILE){

        //Shift the bit on the knight bitboard up one row and two squares to the right, then add that bit to the attacks bitboard
        attacksBitboard |= (knightsBitboard >> 6);

        //Shift the bit on the knight bitboard down one row and two squares to the right, then add that bit to the attacks bitboard
        attacksBitboard |= (knightsBitboard << 10);

    }

    //If the knight is not located on the a or b (two leftmost) files
    if(knightsBitboard & NOT_AB_FILE){

        //Shift the bit on the knight bitboard up one row and two squares to the left, then add that bit to the attacks bitboard
        attacksBitboard |= (knightsBitboard >> 10);

        //Shift the bit on the knight bitboard down one row and two squares to the left, then add that bit to the attacks bitboard
        attacksBitboard |= (knightsBitboard << 6);
    }

    return attacksBitboard;

}

//Mask the bishop attacks for a given square
U64 maskBishopAttacks(int squareIndex){

    U64 attacksBitboard = 0ULL;

    int rank, file;

    //Get the file and the rank from the square index
    int targetRank = squareIndex / 8;
    int targetFile = squareIndex % 8;

    //Cast an attack ray in the positive file and positive rank direction
    for(rank = targetRank + 1, file  = targetFile + 1; rank < 7 && file < 7; rank++, file++){
        //Set the bit on the attack bitboard
        setBit(attacksBitboard, rank * 8 + file);
    }

    //Cast an attack ray in the positive file and negative rank direction
    for(rank = targetRank - 1, file = targetFile + 1; rank > 0 && file < 7; rank--, file++){
        //Set the bit on the attack bitboard
        setBit(attacksBitboard, rank * 8 + file);
    }

    //Cast an attack ray in the negative file and positive rank direction
    for(rank = targetRank + 1, file = targetFile - 1; rank < 7 && file > 0; rank++, file--){
        //Set the bit on the attack bitboard
        setBit(attacksBitboard, rank * 8 + file);
    }

    //Cast an attack ray in the negative file and negative rank direction
    for(rank = targetRank - 1, file = targetFile - 1; rank > 0 && file > 0; rank--, file--){
        //Set the bit on the attack bitboard
        setBit(attacksBitboard, rank * 8 + file);
    }

    return attacksBitboard;

}

//Mask the rook attacks for a given square
U64 maskRookAttacks(int squareIndex){

    U64 attacksBitboard = 0ULL;

    int rank, file;

    //Get the file and the rank from the square index
    int targetRank = squareIndex / 8;
    int targetFile = squareIndex % 8;

    //Cast an attack ray in the positive rank direction
    for(rank = targetRank + 1; rank < 7; rank++){
        //Set the bit on the attack bitboard
        setBit(attacksBitboard, rank * 8 + targetFile);
    }

    //Cast an attack ray in the negative rank direction
    for(rank = targetRank - 1; rank > 0; rank--){
        //Set the bit on the attack bitboard
        setBit(attacksBitboard, rank * 8 + targetFile);
    }

    //Cast an attack ray in the positive file direction
    for(file = targetFile + 1; file < 7; file++){
        //Set the bit on the attack bitboard
        setBit(attacksBitboard, targetRank * 8 + file);
    }

    //Cast an attack ray in the negative file direction
    for(file = targetFile - 1; file > 0; file--){
        //Set the bit on the attack bitboard
        setBit(attacksBitboard, targetRank * 8 + file);
    }

    return attacksBitboard;

}

//Mask the king attacks for a given square
U64 maskKingAttacks(int squareIndex){

    U64 kingBitboard = 0ULL, attacksBitboard = 0ULL;
    setBit(kingBitboard, squareIndex);

    //If moving the king up one row keeps it on the board 
    if(kingBitboard >> 8){
        //Shift the bit on the king bitboard up one row, then add that bit to the attacks bitboard
        attacksBitboard |= kingBitboard >> 8;
    }

    //If moving the king down one row keeps it on the board 
    if(kingBitboard << 8){
        //Shift the bit on the king bitboard up one row, then add that that bit to the attacks bitboard
        attacksBitboard |= kingBitboard << 8;
    }
    
    //If the knight is not located on the h (rightmost) file 
    if(kingBitboard & NOT_H_FILE){

        //Shift the bit on the  king bitboard up one row and one square to the right, then add that bit to the attacks bitboard
        attacksBitboard |= kingBitboard >> 7;

        //Shift the bit on the  king bitboard down one row and one square to the right, then add that bit to the attacks bitboard
        attacksBitboard |= kingBitboard << 9;

        //Shift the bit on the  king bitboard one square to the right, then add that bit to the attacks bitboard
        attacksBitboard |= kingBitboard << 1;
    }

    //If the knight is not located on the a (leftmost) file 
    if(kingBitboard & NOT_A_FILE){

        //Shift the bit on the  king bitboard up one row and one square to the left, then add that bit to the attacks bitboard
        attacksBitboard |= kingBitboard >> 9;

        //Shift the bit on the  king bitboard down one row and one square to the left, then add that bit to the attacks bitboard
        attacksBitboard |= kingBitboard << 7;

        //Shift the bit on the  king bitboard one square to the left, then add that bit to the attacks bitboard
        attacksBitboard |= kingBitboard >> 1;
    }

    return attacksBitboard;

}

//Generate a custom file-rank mask
U64 generateMask(int fileNumber, int rankNumber){

    //Initialise an emty mask
    U64 mask = 0ULL;

    //Loop over ranks
    for(int rank = 0; rank < 8; rank++){

        //Loop over files
        for(int file = 0; file < 8; file++){

            //Calculate a square index
            int squareIndex = rank * 8 + file;

            //If the given file has been reached
            if(fileNumber != -1 && file == fileNumber){
                //Set a bit on the current square index
                setBit(mask, squareIndex);
            }

            //If the given rank has been reached
            if(rankNumber != -1 && rank == rankNumber){
                //Set a bit on the current square index
                setBit(mask, squareIndex);
            }

        }

    }

    return mask;

}
