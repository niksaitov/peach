/*
void generatePawnMoves(MoveList& output, int color){

       int indexShift = (color == white) ? -8 : +8;
       int opposite = color ^ 1;

       int pawn = (color == white) ? whitePawn : blackPawn;
       int knight = (color == white) ? whiteKnight : blackKnight;
       int queen = (color == white) ? whiteQueen : blackQueen;

       int promotionLeftBound = (color == white) ? a7 : a2;
       int doublePushLeftBound = (color == white) ? a2 : a7;

       int startSquareIndex, targetSquareIndex;
       U64 pawnBitboard = bitboards[pawn], pawnAttacks;

       while(pawnBitboard){
              
              startSquareIndex = getLS1BIndex(pawnBitboard);
              targetSquareIndex = startSquareIndex + indexShift;
              popBit(pawnBitboard, startSquareIndex);

              if(targetSquareIndex >= a8 && targetSquareIndex <= h1 && !getBit(occupancies[both], targetSquareIndex)){

              if(startSquareIndex >= promotionLeftBound && startSquareIndex <= promotionLeftBound + 7){

                     for(int promotedPiece = knight; promotedPiece <= queen; promotedPiece++){
                     output.appendMove(startSquareIndex, targetSquareIndex, pawn, promotedPiece, 0, 0, 0, 0);
                     }

              }else{
                     
                     if(startSquareIndex >= doublePushLeftBound && startSquareIndex <= doublePushLeftBound + 7 && !getBit(occupancies[both], targetSquareIndex) && !getBit(occupancies[both], targetSquareIndex + indexShift)){
                     output.appendMove(startSquareIndex, targetSquareIndex + indexShift, pawn, 0, 0, 1, 0, 0);
                     }

                     output.appendMove(startSquareIndex, targetSquareIndex, pawn, 0, 0, 0, 0, 0);

              }

              }

              pawnAttacks = ATTACKS.getPawnAttacks(color, startSquareIndex) & occupancies[opposite];

              while(pawnAttacks){

              targetSquareIndex = getLS1BIndex(pawnAttacks);
              popBit(pawnAttacks, targetSquareIndex);

              if(startSquareIndex >= promotionLeftBound && startSquareIndex <= promotionLeftBound + 7){

                     for(int promotedPiece = knight; promotedPiece <= queen; promotedPiece++){
                     output.appendMove(startSquareIndex, targetSquareIndex, pawn, promotedPiece, 1, 0, 0, 0);
                     }

              }else{
                     output.appendMove(startSquareIndex, targetSquareIndex, pawn, 0, 1, 0, 0, 0);
              }

              }

              if(enPassantSquareIndex != NO_SQUARE_INDEX){

              U64 enPassantAttacks = ATTACKS.getPawnAttacks(color, startSquareIndex) & (1ULL << enPassantSquareIndex);

              if(enPassantAttacks){

                     int enPassantTarget = getLS1BIndex(enPassantAttacks);
                     output.appendMove(startSquareIndex, enPassantTarget, pawn, 0, 1, 0, 1, 0);

              }
              }
       }
       }

       void generatePieceMoves(MoveList& output, int piece, std::function<U64(int)> getAttacks){

       int startSquareIndex, targetSquareIndex;
       U64 pieceBitboard = bitboards[piece], pieceAttacks;

       while(pieceBitboard){

              startSquareIndex = getLS1BIndex(pieceBitboard);
              popBit(pieceBitboard, startSquareIndex);

              pieceAttacks = getAttacks(startSquareIndex) & ((sideToMove == white) ? ~occupancies[white] : ~occupancies[black]);

              while(pieceAttacks){

              targetSquareIndex = getLS1BIndex(pieceAttacks);

              if(!getBit(((sideToMove == white) ? occupancies[black] : occupancies[white]), targetSquareIndex)){
                     output.appendMove(startSquareIndex, targetSquareIndex, piece, 0, 0, 0, 0, 0);
              }else{
                     output.appendMove(startSquareIndex, targetSquareIndex, piece, 0, 1, 0, 0, 0);
              }
              popBit(pieceAttacks, targetSquareIndex);
              }
       }
       }

       MoveList generateMoves(){

       MoveList output; 

       if(sideToMove == white){

              generatePawnMoves(output, white);
              
              if(canCastle & K){
              if(!getBit(occupancies[both], f1) && !getBit(occupancies[both], g1) && !isSquareAttacked(e1, black) && !isSquareAttacked(f1, black)){
                     output.appendMove(e1, g1, whiteKing, 0, 0, 0, 0, 1);
              }
              }

              if(canCastle & Q){
              if(!getBit(occupancies[both], d1) && !getBit(occupancies[both], c1) && !getBit(occupancies[both], b1) 
              && !isSquareAttacked(e1, black) && !isSquareAttacked(d1, black)){
                     output.appendMove(e1, c1, whiteKing, 0, 0, 0, 0, 1);
              } 
              }

              generatePieceMoves(output, whiteKnight, [&](int startSquareIndex) {return ATTACKS.getKnightAttacks(startSquareIndex);});
              generatePieceMoves(output, whiteBishop, [&](int startSquareIndex) {return ATTACKS.getBishopAttacks(startSquareIndex, occupancies[both]);});
              generatePieceMoves(output, whiteRook, [&](int startSquareIndex) {return ATTACKS.getRookAttacks(startSquareIndex, occupancies[both]);});
              generatePieceMoves(output, whiteQueen, [&](int startSquareIndex) {return ATTACKS.getQueenAttacks(startSquareIndex, occupancies[both]);});
              generatePieceMoves(output, whiteKing, [&](int startSquareIndex) {return ATTACKS.getKingAttacks(startSquareIndex);});
       
       }else if (sideToMove == black){

              generatePawnMoves(output, black);

              if(canCastle & k){
                     if(!getBit(occupancies[both], f8) && !getBit(occupancies[both], g8) && !isSquareAttacked(e8, white) && !isSquareAttacked(f8, white)){
                     output.appendMove(e8, g8, blackKing, 0, 0, 0, 0, 1);
                     }
              }

              if(canCastle & q){
              if(!getBit(occupancies[both], d8) && !getBit(occupancies[both], c8) && !getBit(occupancies[both], b8) && !isSquareAttacked(e8, white) && !isSquareAttacked(d8, white)){
                     output.appendMove(e8, c8, blackKing, 0, 0, 0, 0, 1);
              } 
              }

              generatePieceMoves(output, blackKnight, [&](int startSquareIndex) {return ATTACKS.getKnightAttacks(startSquareIndex);});
              generatePieceMoves(output, blackBishop, [&](int startSquareIndex) {return ATTACKS.getBishopAttacks(startSquareIndex, occupancies[both]);});
              generatePieceMoves(output, blackRook, [&](int startSquareIndex) {return ATTACKS.getRookAttacks(startSquareIndex, occupancies[both]);});
              generatePieceMoves(output, blackQueen, [&](int startSquareIndex) {return ATTACKS.getQueenAttacks(startSquareIndex, occupancies[both]);});
              generatePieceMoves(output, blackKing, [&](int startSquareIndex) {return ATTACKS.getKingAttacks(startSquareIndex);});
       }
       return output;
       }
*/