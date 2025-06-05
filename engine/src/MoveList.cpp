#include "MoveList.h"
#include "move_encoding.h"


//Add move to the moves array at index specified by the count member variable
void MoveList::appendMove(int startSquareIndex, int targetSquareIndex, int piece, int promotedPiece, 
                                bool fCapture, bool fDoublePawnPush, bool fEnPassant, bool fCastling){
    
    //Encode a move into an integer
    moves[count] = createMove(startSquareIndex, targetSquareIndex, piece, promotedPiece,
                                fCapture, fDoublePawnPush, fEnPassant, fCastling);
    //Update the count
    count++;

}

//Get the array of moves
int* MoveList::getMoves(){
    return moves;
}

//Get the value of the count variable
const int MoveList::getCount(){
    return count;
}     
