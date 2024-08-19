#ifndef MOVE_ENCODING_H
#define MOVE_ENCODING_H

#include <iostream>
#include "move_encoding.h"
#include "const.h"

extern "C" {

    //Encode a move using custom bit offsets 
    inline int createMove(int startSquareIndex, int targetSquareIndex, int piece, int promotedPiece, bool fCapture, bool fDoublePawnPush, bool fEnPassant, bool fCastling){
        return startSquareIndex | (targetSquareIndex << 6) | (piece << 12) | (promotedPiece << 16) | (fCapture << 20) | (fDoublePawnPush << 21) | (fEnPassant << 22) | (fCastling << 23);
    }

    //Get the start square index
    inline int getStartSquareIndex(int move){
        return move & 0x3f;
    }

    //Get the target square index
    inline int getTargetSquareIndex(int move){
        return (move & 0xfc0) >> 6;
    }

    //Get the piece
    inline int getPiece(int move){
        return (move & 0xf000) >> 12;
    }

    //Get the promoted piece
    inline int getPromotedPiece(int move){
        return (move & 0xf0000) >> 16;
    }

    //Check if the move is a capture
    inline bool isCapture(int move){
        return move & 0x100000;
    }

    //Check if the move is a double pawn push
    inline bool isDoublePawnPush(int move){
        return move & 0x200000;
    }

    //Check if the move is an enpassant 
    inline bool isEnPassant(int move){
        return move & 0x400000;
    }

    //Check the move if the move is a castling 
    inline bool isCastling(int move){
        return move & 0x800000;
    }

    //Print the move 
    inline void printMove(int move){
        
        //Print the start square and the target square coordinates
        std::cout << ' ' << SQUARE_INDEX_TO_COORDINATES[getStartSquareIndex(move)] << SQUARE_INDEX_TO_COORDINATES[getTargetSquareIndex(move)];

        //If a piece was promoted
        if(getPromotedPiece(move)){ 

            //Print the promoted piece
            std::cout << PIECE_INDEX_TO_ASCII[getPromotedPiece(move)];
        }

    }
    
}

#endif

