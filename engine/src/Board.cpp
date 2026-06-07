#include <iostream>
#include <cstring>

#include "Board.h"
#include "bitboard_operations.h"
#include "engine_exceptions.h"

using std::cout, std::string;

extern AttackTable ATTACKS;
extern TranspositionNode TRANSPOSITION_TABLE[];
extern U64 repetitions[];
extern int repetitionIndex;
extern U64 fileMasks[];
extern U64 isolatedPawnMasks[];
extern U64 whitePassedPawnMasks[];
extern U64 blackPassedPawnMasks[];
extern U64 PIECE_KEYS[12][64];
extern U64 ENPASSANT_KEYS[64];
extern U64 CASTLING_KEYS[16];
extern U64 SIDE_KEY;

void Board::resetBitboards()
{
    memset(bitboards, 0, sizeof(bitboards));
}

// Clear the occupancy arrays
void Board::resetOccupancies()
{
    memset(occupancies, 0, sizeof(occupancies));
}

// Populate occupancies from the board state
void Board::populateOccupancies()
{

    // Loop over white pieces
    for (int currentPiece = whitePawn; currentPiece <= whiteKing; currentPiece++)
    {
        // Perform a logical OR to add the bits on the bitboard of the current piece to the bitboard of occupancies
        occupancies[white] |= bitboards[currentPiece];
    }

    // Loop over black pieces
    for (int currentPiece = blackPawn; currentPiece <= blackKing; currentPiece++)
    {
        // Perform a logical OR to add the bits on the bitboard of the current piece to the bitboard of occupancies
        occupancies[black] |= bitboards[currentPiece];
    }

    // Perform a logical OR on the occupancies to get the combined occupancy
    occupancies[both] = occupancies[white] | occupancies[black];
}

// Find the heuristic value of the position
int Board::staticEvaluate()
{
    // Initialise the variables
    int score = 0, scoreOpening = 0, scoreEndgame = 0;
    int squareIndex = 0, doubledPawns = 0, gamePhase = 0;

    // Obtain game score
    int gameScore = calculateGameScore();

    // If the game score is higher than the opening bound
    if (gameScore > OPENING_SCORE)
    {
        // Set the game phase to the opening
        gamePhase = opening;
        // If the game score is lower than the opening bound
    }
    else if (gamePhase < ENDGAME_SCORE)
    {
        // Set the game phase to the endgame
        gamePhase = endgame;
        // Otherwise
    }
    else
    {
        // Set the game phase to the middlegame
        gameScore = middlegame;
    }

    // Loop over all of the pieces
    for (int currentPiece = whitePawn; currentPiece <= blackKing; currentPiece++)
    {

        // Fetch the piece bitboard
        U64 currentPieceBitboard = bitboards[currentPiece];

        // While there are bits on the current bitboard
        while (currentPieceBitboard)
        {

            // Add the material scores
            scoreOpening += MATERIAL_SCORE[opening][currentPiece];
            scoreEndgame += MATERIAL_SCORE[endgame][currentPiece];

            // Record the position of the piece
            squareIndex = getLS1BIndex(currentPieceBitboard);

            // Remove the bit from the current piece bitboard
            popBit(currentPieceBitboard, squareIndex);

            switch (currentPiece)
            {

            // If the piece is a white pawn
            case (whitePawn):

                // Add the positional scores
                scoreOpening += POSITIONAL_SCORE[opening][pawn][squareIndex];
                scoreEndgame += POSITIONAL_SCORE[endgame][pawn][squareIndex];

                // Count the number of doubled pawns
                doubledPawns = getPopulationCount(bitboards[whitePawn] & fileMasks[squareIndex % 8]) - 1;

                // If doubled pawns were found
                if (doubledPawns > 0)
                {

                    // Apply the doubled pawn penalty
                    scoreOpening += doubledPawns * DOUBLED_PENALTY_OPENING;
                    scoreEndgame += doubledPawns * DOUBLED_PENALTY_ENDGAME;
                }

                // If the isolated pawns were found
                if (!(bitboards[whitePawn] & isolatedPawnMasks[squareIndex % 8]))
                {

                    // Apply the isolated pawn penalty
                    scoreOpening += ISOLATED_PENALTY_OPENING;
                    scoreEndgame += ISOLATED_PENALTY_ENDGAME;
                }

                // If a passed pawn was found
                if (!(bitboards[blackPawn] & whitePassedPawnMasks[squareIndex]))
                {

                    // Add the passed pawn score
                    scoreOpening += PP_SCORE[RANKS[squareIndex]];
                    scoreEndgame += PP_SCORE[RANKS[squareIndex]];
                }

                break;

            // If the curent piece is a white knight
            case (whiteKnight):

                // Add the positional score
                scoreOpening += POSITIONAL_SCORE[opening][knight][squareIndex];
                scoreEndgame += POSITIONAL_SCORE[endgame][knight][squareIndex];

                break;

            // If the current piece is a white bishop
            case (whiteBishop):

                // Add the positional scores
                scoreOpening += POSITIONAL_SCORE[opening][bishop][squareIndex];
                scoreEndgame += POSITIONAL_SCORE[endgame][bishop][squareIndex];

                // Apply piece mobility calculations
                scoreOpening += (getPopulationCount(ATTACKS.getBishopAttacks(squareIndex, occupancies[both])) - BISHOP_VALUE) * BISHOP_MOB_OPENING;
                scoreEndgame += (getPopulationCount(ATTACKS.getBishopAttacks(squareIndex, occupancies[both])) - BISHOP_VALUE) * BISHOP_MOB_ENDGAME;

                break;

            // If the current piece is a white bishop
            case (whiteRook):

                // Add the positional scores
                scoreOpening += POSITIONAL_SCORE[opening][rook][squareIndex];
                scoreEndgame += POSITIONAL_SCORE[endgame][rook][squareIndex];

                // If the rook occupies a file with only enemy pawns
                if (!(bitboards[whitePawn] & fileMasks[squareIndex % 8]))
                {
                    // Add the semi-open file score
                    score += SEMI_OPEN_FILE_SCORE;
                }

                // If the rook occupies a file without any pawns
                if (!((bitboards[whitePawn] | bitboards[blackPawn]) & fileMasks[squareIndex % 8]))
                {
                    // Add the open file score
                    score += FULL_OPEN_FILE_SCORE;
                }

                break;

            // If the current piece is a white queen
            case (whiteQueen):

                // Add the positional score
                scoreOpening += POSITIONAL_SCORE[opening][queen][squareIndex];
                scoreEndgame += POSITIONAL_SCORE[endgame][queen][squareIndex];

                // Apply piece mobility calculations
                scoreOpening += (getPopulationCount(ATTACKS.getQueenAttacks(squareIndex, occupancies[both])) - QUEEN_VALUE) * QUEEN_MOB_OPENING;
                scoreOpening += (getPopulationCount(ATTACKS.getQueenAttacks(squareIndex, occupancies[both])) - QUEEN_VALUE) * QUEEN_MOB_ENDGAME;

                break;

            // If the current piece is a white king
            case (whiteKing):

                // Add the positional score
                scoreOpening += POSITIONAL_SCORE[opening][king][squareIndex];
                scoreEndgame += POSITIONAL_SCORE[endgame][king][squareIndex];

                // If the king is on the file with only enemy pawns
                if (!(bitboards[whitePawn] & fileMasks[squareIndex % 8]))
                {
                    // Deduct the semi-open file score
                    score -= SEMI_OPEN_FILE_SCORE;
                }

                // If the king is on the file with no pawns
                if (!((bitboards[whitePawn] | bitboards[blackPawn]) & fileMasks[squareIndex % 8]))
                {
                    // Score
                    score -= FULL_OPEN_FILE_SCORE;
                }

                // Add the king safety coefficient
                scoreOpening += getPopulationCount(ATTACKS.getKingAttacks(squareIndex) & bitboards[whitePawn]) * KING_SAFETY_COEFFICIENT;
                scoreEndgame += getPopulationCount(ATTACKS.getKingAttacks(squareIndex) & bitboards[whitePawn]) * KING_SAFETY_COEFFICIENT;

                break;

            // Same working principle for black pieces
            case (blackPawn):

                scoreOpening -= POSITIONAL_SCORE[opening][pawn][OPPOSITE_SIDE[squareIndex]];
                scoreEndgame -= POSITIONAL_SCORE[endgame][pawn][OPPOSITE_SIDE[squareIndex]];

                doubledPawns = getPopulationCount(bitboards[blackPawn] & fileMasks[squareIndex % 8]) - 1;

                if (doubledPawns > 0)
                {

                    scoreOpening -= doubledPawns * DOUBLED_PENALTY_OPENING;
                    scoreEndgame -= doubledPawns * DOUBLED_PENALTY_ENDGAME;
                }

                if (!(bitboards[blackPawn] & isolatedPawnMasks[squareIndex % 8]))
                {

                    scoreOpening -= ISOLATED_PENALTY_OPENING;
                    scoreEndgame -= ISOLATED_PENALTY_ENDGAME;
                }

                if (!(bitboards[whitePawn] & blackPassedPawnMasks[squareIndex]))
                {

                    scoreOpening -= PP_SCORE[RANKS[OPPOSITE_SIDE[squareIndex]]];
                    scoreEndgame -= PP_SCORE[RANKS[OPPOSITE_SIDE[squareIndex]]];
                }

                break;

            case (blackKnight):

                scoreOpening -= POSITIONAL_SCORE[opening][knight][OPPOSITE_SIDE[squareIndex]];
                scoreEndgame -= POSITIONAL_SCORE[endgame][knight][OPPOSITE_SIDE[squareIndex]];

                break;

            case (blackBishop):

                scoreOpening -= POSITIONAL_SCORE[opening][bishop][OPPOSITE_SIDE[squareIndex]];
                scoreEndgame -= POSITIONAL_SCORE[endgame][bishop][OPPOSITE_SIDE[squareIndex]];

                scoreOpening -= (getPopulationCount(ATTACKS.getBishopAttacks(squareIndex, occupancies[both])) - BISHOP_VALUE) * BISHOP_MOB_OPENING;
                scoreEndgame -= (getPopulationCount(ATTACKS.getBishopAttacks(squareIndex, occupancies[both])) - BISHOP_VALUE) * BISHOP_MOB_ENDGAME;

                break;

            case (blackRook):

                scoreOpening -= POSITIONAL_SCORE[opening][rook][OPPOSITE_SIDE[squareIndex]];
                scoreEndgame -= POSITIONAL_SCORE[endgame][rook][OPPOSITE_SIDE[squareIndex]];

                if (!(bitboards[blackPawn] & fileMasks[squareIndex % 8]))
                {
                    score -= SEMI_OPEN_FILE_SCORE;
                }

                if (!((bitboards[whitePawn] | bitboards[blackPawn]) & fileMasks[squareIndex % 8]))
                {
                    score -= FULL_OPEN_FILE_SCORE;
                }

                break;

            case (blackQueen):

                scoreOpening -= POSITIONAL_SCORE[opening][queen][OPPOSITE_SIDE[squareIndex]];
                scoreEndgame -= POSITIONAL_SCORE[endgame][queen][OPPOSITE_SIDE[squareIndex]];

                scoreOpening -= (getPopulationCount(ATTACKS.getQueenAttacks(squareIndex, occupancies[both])) - QUEEN_VALUE) * QUEEN_MOB_OPENING;
                scoreOpening -= (getPopulationCount(ATTACKS.getQueenAttacks(squareIndex, occupancies[both])) - QUEEN_VALUE) * QUEEN_MOB_ENDGAME;

                break;

            case (blackKing):

                scoreOpening -= POSITIONAL_SCORE[opening][king][OPPOSITE_SIDE[squareIndex]];
                scoreEndgame -= POSITIONAL_SCORE[endgame][king][OPPOSITE_SIDE[squareIndex]];

                if (!(bitboards[whitePawn] & fileMasks[squareIndex % 8]))
                {
                    score += SEMI_OPEN_FILE_SCORE;
                }

                if (!((bitboards[whitePawn] | bitboards[blackPawn]) & fileMasks[squareIndex % 8]))
                {
                    score += FULL_OPEN_FILE_SCORE;
                }

                scoreOpening -= getPopulationCount(ATTACKS.getKingAttacks(squareIndex) & bitboards[blackPawn]) * KING_SAFETY_COEFFICIENT;
                scoreEndgame -= getPopulationCount(ATTACKS.getKingAttacks(squareIndex) & bitboards[blackPawn]) * KING_SAFETY_COEFFICIENT;

                break;
            }
        }
    }

    // Interpolate the scores
    if (gamePhase == middlegame)
    {
        score = (scoreOpening * gameScore + scoreEndgame * (OPENING_SCORE - gameScore)) / OPENING_SCORE;
    }
    else if (gamePhase == opening)
    {
        score = scoreOpening;
    }
    else if (gamePhase == endgame)
    {
        score = scoreEndgame;
    }

    // In negamax the score is evaluated relative to the side
    return (sideToMove == white) ? score : -score;
}















bool Board::isSquareAttacked(int squareIndex, int sideToMove)
{

    // Return true if the square is attacked by pawns
    if (sideToMove == white && ATTACKS.getPawnAttacks(black, squareIndex) & bitboards[whitePawn])
    {
        return true;
    }

    // Return true if the square is attacked by pawns
    if (sideToMove == black && ATTACKS.getPawnAttacks(white, squareIndex) & bitboards[blackPawn])
    {
        return true;
    }

    // Return true if the square is attacked by the knight
    if (ATTACKS.getKnightAttacks(squareIndex) & ((sideToMove == white) ? bitboards[whiteKnight] : bitboards[blackKnight]))
    {
        return true;
    }

    // Return true if the square is attacked by the bishop
    if (ATTACKS.getBishopAttacks(squareIndex, occupancies[both]) & ((sideToMove == white) ? bitboards[whiteBishop] : bitboards[blackBishop]))
    {
        return true;
    }

    // Return true if the square is attacked by the rook
    if (ATTACKS.getRookAttacks(squareIndex, occupancies[both]) & ((sideToMove == white) ? bitboards[whiteRook] : bitboards[blackRook]))
    {
        return true;
    }

    // Return true if the square is attacked by the queen
    if (ATTACKS.getQueenAttacks(squareIndex, occupancies[both]) & ((sideToMove == white) ? bitboards[whiteQueen] : bitboards[blackQueen]))
    {
        return true;
    }

    // Return true if the square is attacked by the king
    if (ATTACKS.getKingAttacks(squareIndex) & ((sideToMove == white) ? bitboards[whiteKing] : bitboards[blackKing]))
    {
        return true;
    }

    return false;
}

void Board::generateHash()
{

    // Check if the hash keys are initialised
    if (!PIECE_KEYS[0][0] || !CASTLING_KEYS[0] || !ENPASSANT_KEYS[0] || !SIDE_KEY)
    {
        throw HashKeysNotInitialisedException();
    }

    // Loop over the pieces
    for (int currentPiece = whitePawn; currentPiece <= blackKing; currentPiece++)
    {

        // Copy the current piece bitboard
        U64 currentBiboard = bitboards[currentPiece];

        while (currentBiboard)
        {

            // Get the square index
            int squareIndex = getLS1BIndex(currentBiboard);
            // Add the value to the hash key
            hashKey ^= PIECE_KEYS[currentPiece][squareIndex];
            // Remove the bit
            popBit(currentBiboard, squareIndex);
        }
    }

    // If en passant is possible
    if (enPassantSquareIndex != NO_SQUARE_INDEX)
    {
        // Add the en passant value to the hash key
        hashKey ^= ENPASSANT_KEYS[enPassantSquareIndex];
    }

    // Add the castle state value to the hash key
    hashKey ^= CASTLING_KEYS[canCastle];

    // If the side to move is not 0 (not white)
    if (sideToMove)
    {
        // Add the side value to the hash key
        hashKey ^= SIDE_KEY;
    }
}

// Print the state of the bitboard
void Board::printState()
{

    cout << '\n';

    // Loop over the ranks
    for (int rank = 0; rank < 8; rank++)
    {

        // Loop over the files
        for (int file = 0; file < 8; file++)
        {

            // Least significant file (LSF) mapping
            int squareIndex = rank * 8 + file;

            // Print the ranks
            if (!file)
            {
                cout << 8 - rank << "  ";
            }

            int piece = -1;

            // Loop over the pieces
            for (int currentPiece = whitePawn; currentPiece <= blackKing; currentPiece++)
            {

                // If a piece has been found
                if (getBit(bitboards[currentPiece], squareIndex))
                {
                    // Record the piece
                    piece = currentPiece;
                }
            }

            // Convert the piece into an ASCII character
            cout << ((piece == -1) ? '.' : PIECE_INDEX_TO_ASCII[piece]) << ' ';
        }

        cout << '\n';
    }

    // Print the files
    cout << '\n'
         << "   a b c d e f g h " << "\n\n";

    // Special position details
    cout << "Turn: " << ((sideToMove != NO_SIDE_TO_MOVE) ? ((!sideToMove) ? "white" : "black") : "not specified") << '\n';
    cout << "Can castle: " << ((canCastle & K) ? 'K' : '-') << ((canCastle & Q) ? 'Q' : '-') << ((canCastle & k) ? 'k' : '-') << ((canCastle & q) ? 'q' : '-') << '\n';
    cout << "EnPassant square: " << ((enPassantSquareIndex != NO_SQUARE_INDEX) ? SQUARE_INDEX_TO_COORDINATES[enPassantSquareIndex] : "no square") << '\n';
    cout << "Hash: " << hashKey << "\n";
};

// Generate the list of all pseudo-legal moves in a position
MoveList Board::generateMoves()
{

    // Initialise the start and target square indicies
    int startSquareIndex, targetSquareIndex;
    // Initialise the bitboar of the current piece and the bitboards of its attacks
    U64 currentPieceBitboard, currentPieceAttacks;
    // Initialise the move list where all of the moves are added
    MoveList output;

    // Loop over the pieces
    for (int currentPiece = whitePawn; currentPiece <= blackKing; currentPiece++)
    {

        // Copy the bitboard of the current piece
        currentPieceBitboard = bitboards[currentPiece];

        // If it is white's turn
        if (sideToMove == white)
        {

            switch (currentPiece)
            {

            // If the current piece is a white pawn
            case whitePawn:

                // While there are bits on the current piece bitboard
                while (currentPieceBitboard)
                {

                    // Get the start square index
                    startSquareIndex = getLS1BIndex(currentPieceBitboard);

                    // Apply an offset (up one rank)
                    targetSquareIndex = startSquareIndex - 8;

                    // If the target square is in the board and empty
                    if (!(targetSquareIndex < a8) && !getBit(occupancies[both], targetSquareIndex))
                    {

                        // If start square is on the 7th rank
                        if (startSquareIndex >= a7 && startSquareIndex <= h7)
                        {

                            // Loop over the possible promotions
                            for (int promotedPiece = whiteKnight; promotedPiece <= whiteQueen; promotedPiece++)
                            {
                                // Add the promotion move to the move list
                                output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, promotedPiece, 0, 0, 0, 0);
                            }

                            // If the start square is not on the 7th rank
                        }
                        else
                        {

                            // Add the standard pawn to the move list
                            output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 0, 0, 0, 0);

                            // If the start square is on the 2nd rank
                            if ((startSquareIndex >= a2 && startSquareIndex <= h2) && !getBit(occupancies[both], targetSquareIndex - 8))
                            {
                                // Add the double pawn push to the move list
                                output.appendMove(startSquareIndex, targetSquareIndex - 8, currentPiece, 0, 0, 1, 0, 0);
                            }
                        }
                    }

                    // Get the attacks of the pawn
                    currentPieceAttacks = ATTACKS.getPawnAttacks(white, startSquareIndex) & occupancies[black];

                    // While there are bits on the attacks bitboard
                    while (currentPieceAttacks)
                    {

                        // Get the target square index
                        targetSquareIndex = getLS1BIndex(currentPieceAttacks);

                        // If the pawn is on the 7th rank,
                        if (startSquareIndex >= a7 && startSquareIndex <= h7)
                        {

                            // Loop over the possible promotions
                            for (int promotedPiece = whiteKnight; promotedPiece <= whiteQueen; promotedPiece++)
                            {
                                // Add the promotion move to the move list
                                output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, promotedPiece, 1, 0, 0, 0);
                            }

                            // If the pawn is not on the 7th rank
                        }
                        else
                        {
                            // Add the standard pawn capture to the move list
                            output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 1, 0, 0, 0);
                        }

                        // Remove the LS1B from the attacks biboard
                        popBit(currentPieceAttacks, targetSquareIndex);
                    }

                    // If en passant is possible
                    if (enPassantSquareIndex != NO_SQUARE_INDEX)
                    {

                        // Get the square index of the possible en passant capture
                        U64 enPassantAttacks = ATTACKS.getPawnAttacks(white, startSquareIndex) & (1ULL << enPassantSquareIndex);

                        // If the square was found
                        if (enPassantAttacks)
                        {

                            // Initialise the target square
                            int enPassantTarget = getLS1BIndex(enPassantAttacks);

                            // Add the en passant capture to the move list
                            output.appendMove(startSquareIndex, enPassantTarget, currentPiece, 0, 1, 0, 1, 0);
                        }
                    }

                    // Remove a bit from the current piece bitboard
                    popBit(currentPieceBitboard, startSquareIndex);
                }

                break;

            // If the current piece is a white king
            case whiteKing:

                // If kingside castling is avaliable
                if (canCastle & K)
                {
                    // If the squares between the king and the rook are niether occupied and nor attacked
                    if (!getBit(occupancies[both], f1) && !getBit(occupancies[both], g1) && !isSquareAttacked(e1, black) && !isSquareAttacked(f1, black))
                    {
                        // Add the kingside castling to the move list
                        output.appendMove(e1, g1, currentPiece, 0, 0, 0, 0, 1);
                    }
                }

                // If queenside castling is avaliable
                if (canCastle & Q)
                {
                    // If the squares between the king and the rook are niether occupied nor attacked
                    if (!getBit(occupancies[both], d1) && !getBit(occupancies[both], c1) && !getBit(occupancies[both], b1) && !isSquareAttacked(e1, black) && !isSquareAttacked(d1, black))
                    {
                        // Add the queenside casting to the move list
                        output.appendMove(e1, c1, currentPiece, 0, 0, 0, 0, 1);
                    }
                }

                break;
            }

            // If it is black's turn
        }
        else if (sideToMove == black)
        {

            switch (currentPiece)
            {

            // If the current piece is a black pawn
            case blackPawn:

                // While there are bits on the current piece bitboard
                while (currentPieceBitboard)
                {

                    // Get the start square index
                    startSquareIndex = getLS1BIndex(currentPieceBitboard);

                    // Apply an offset (down one rank)
                    targetSquareIndex = startSquareIndex + 8;

                    // If the target square is in the board and empty
                    if (!(targetSquareIndex > h1) && !getBit(occupancies[both], targetSquareIndex))
                    {

                        // If the start square is on the 2nd rank
                        if (startSquareIndex >= a2 && startSquareIndex <= h2)
                        {

                            // Loop over the possible promotions
                            for (int promotedPiece = blackKnight; promotedPiece <= blackQueen; promotedPiece++)
                            {
                                // Add the promotion move to the move list
                                output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, promotedPiece, 0, 0, 0, 0);
                            }

                            // If the start square is not on the 2nd rank
                        }
                        else
                        {

                            // Add the standard pawn to the move list
                            output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 0, 0, 0, 0);

                            // If start square is on the 7th rank
                            if ((startSquareIndex >= a7 && startSquareIndex <= h7) && !getBit(occupancies[both], targetSquareIndex + 8))
                            {
                                // Add the double pawn push to the move list
                                output.appendMove(startSquareIndex, targetSquareIndex + 8, currentPiece, 0, 0, 1, 0, 0);
                            }
                        }
                    }

                    // Get the attacks of the pawn
                    currentPieceAttacks = ATTACKS.getPawnAttacks(black, startSquareIndex) & occupancies[white];

                    // While there are bits on the attacks bitboard
                    while (currentPieceAttacks)
                    {

                        // Get the target square index
                        targetSquareIndex = getLS1BIndex(currentPieceAttacks);

                        // If the start square is on the 2nd rank
                        if (startSquareIndex >= a2 && startSquareIndex <= h2)
                        {

                            // Loop over the possible promotions
                            for (int promotedPiece = blackKnight; promotedPiece <= blackQueen; promotedPiece++)
                            {
                                // Add the promotion move to the move list
                                output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, promotedPiece, 1, 0, 0, 0);
                            }

                            // If the start square is not on the 2nd rank
                        }
                        else
                        {
                            // Add the promotion move to the move list
                            output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 1, 0, 0, 0);
                        }

                        // Remove the LS1B from the attacks biboard
                        popBit(currentPieceAttacks, targetSquareIndex);
                    }

                    // If en passant is possible
                    if (enPassantSquareIndex != NO_SQUARE_INDEX)
                    {

                        // Get the square index of the possible en passant capture
                        U64 enPassantAttacks = ATTACKS.getPawnAttacks(black, startSquareIndex) & (1ULL << enPassantSquareIndex);

                        // If the square was found
                        if (enPassantAttacks)
                        {

                            // Initialise the target square
                            int enPassantTarget = getLS1BIndex(enPassantAttacks);

                            // Add the en passant capture to the move list
                            output.appendMove(startSquareIndex, enPassantTarget, currentPiece, 0, 1, 0, 1, 0);
                        }
                    }

                    // Remove a bit from the current piece bitboard
                    popBit(currentPieceBitboard, startSquareIndex);
                }

                break;

            // If the current piece is a black king
            case blackKing:

                // If kingside castling is avaliable
                if (canCastle & k)
                {
                    // If the squares between the king and the rook are niether occupied nor attacked
                    if (!getBit(occupancies[both], f8) && !getBit(occupancies[both], g8) && !isSquareAttacked(e8, white) && !isSquareAttacked(f8, white))
                    {
                        // Add the kingside castling to the move list
                        output.appendMove(e8, g8, currentPiece, 0, 0, 0, 0, 1);
                    }
                }

                // If queenside castling is avaliable
                if (canCastle & q)
                {
                    // If the squares between the king and the rook are niether occupied nor attacked
                    if (!getBit(occupancies[both], d8) && !getBit(occupancies[both], c8) && !getBit(occupancies[both], b8) && !isSquareAttacked(e8, white) && !isSquareAttacked(d8, white))
                    {
                        // Add the queenside casting to the move list
                        output.appendMove(e8, c8, currentPiece, 0, 0, 0, 0, 1);
                    }
                }

                break;
            }
        }

        // Fetch the color of the knight
        if ((sideToMove == white) ? currentPiece == whiteKnight : currentPiece == blackKnight)
        {

            // While there are bits on the current piece bitboard
            while (currentPieceBitboard)
            {

                // Get the start square index
                startSquareIndex = getLS1BIndex(currentPieceBitboard);

                // Get the attacks of the knight
                currentPieceAttacks = ATTACKS.getKnightAttacks(startSquareIndex) & ((sideToMove == white) ? ~occupancies[white] : ~occupancies[black]);

                while (currentPieceAttacks)
                {

                    // Get the target square index
                    targetSquareIndex = getLS1BIndex(currentPieceAttacks);

                    // If the target square is not occupied
                    if (!getBit(((sideToMove == white) ? occupancies[black] : occupancies[white]), targetSquareIndex))
                    {
                        // Add the standard knight move to the move list
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 0, 0, 0, 0);
                        // If the target square index is occupied
                    }
                    else
                    {
                        // Add the standard knight capture to the move list
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 1, 0, 0, 0);
                    }

                    // Remove the LS1B from the attacks biboard
                    popBit(currentPieceAttacks, targetSquareIndex);
                }

                // Remove a bit from the current piece bitboard
                popBit(currentPieceBitboard, startSquareIndex);
            }
        }

        // Bishop moves (same approach as the knights)
        if ((sideToMove == white) ? currentPiece == whiteBishop : currentPiece == blackBishop)
        {

            while (currentPieceBitboard)
            {

                startSquareIndex = getLS1BIndex(currentPieceBitboard);
                currentPieceAttacks = ATTACKS.getBishopAttacks(startSquareIndex, occupancies[both]) & ((sideToMove == white) ? ~occupancies[white] : ~occupancies[black]);

                while (currentPieceAttacks)
                {

                    targetSquareIndex = getLS1BIndex(currentPieceAttacks);

                    // Quiet
                    if (!getBit(((sideToMove == white) ? occupancies[black] : occupancies[white]), targetSquareIndex))
                    {
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 0, 0, 0, 0);
                        // Captures
                    }
                    else
                    {
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 1, 0, 0, 0);
                    }

                    popBit(currentPieceAttacks, targetSquareIndex);
                }

                popBit(currentPieceBitboard, startSquareIndex);
            }
        }

        // Rook moves (same approach as the knights)
        if ((sideToMove == white) ? currentPiece == whiteRook : currentPiece == blackRook)
        {

            while (currentPieceBitboard)
            {

                startSquareIndex = getLS1BIndex(currentPieceBitboard);
                currentPieceAttacks = ATTACKS.getRookAttacks(startSquareIndex, occupancies[both]) & ((sideToMove == white) ? ~occupancies[white] : ~occupancies[black]);

                while (currentPieceAttacks)
                {

                    targetSquareIndex = getLS1BIndex(currentPieceAttacks);

                    // Quiet
                    if (!getBit(((sideToMove == white) ? occupancies[black] : occupancies[white]), targetSquareIndex))
                    {
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 0, 0, 0, 0);
                        // Captures
                    }
                    else
                    {
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 1, 0, 0, 0);
                    }

                    popBit(currentPieceAttacks, targetSquareIndex);
                }

                popBit(currentPieceBitboard, startSquareIndex);
            }
        }

        // Queen moves (same approach as the knights)
        if ((sideToMove == white) ? currentPiece == whiteQueen : currentPiece == blackQueen)
        {

            while (currentPieceBitboard)
            {

                startSquareIndex = getLS1BIndex(currentPieceBitboard);
                currentPieceAttacks = ATTACKS.getQueenAttacks(startSquareIndex, occupancies[both]) & ((sideToMove == white) ? ~occupancies[white] : ~occupancies[black]);

                while (currentPieceAttacks)
                {

                    targetSquareIndex = getLS1BIndex(currentPieceAttacks);

                    // Quiet
                    if (!getBit(((sideToMove == white) ? occupancies[black] : occupancies[white]), targetSquareIndex))
                    {
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 0, 0, 0, 0);
                        // Captures
                    }
                    else
                    {
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 1, 0, 0, 0);
                    }

                    popBit(currentPieceAttacks, targetSquareIndex);
                }

                popBit(currentPieceBitboard, startSquareIndex);
            }
        }

        // King moves (same approach as the knights)
        if ((sideToMove == white) ? currentPiece == whiteKing : currentPiece == blackKing)
        {

            while (currentPieceBitboard)
            {

                startSquareIndex = getLS1BIndex(currentPieceBitboard);
                currentPieceAttacks = ATTACKS.getKingAttacks(startSquareIndex) & ((sideToMove == white) ? ~occupancies[white] : ~occupancies[black]);

                while (currentPieceAttacks)
                {

                    targetSquareIndex = getLS1BIndex(currentPieceAttacks);

                    // Quiet
                    if (!getBit(((sideToMove == white) ? occupancies[black] : occupancies[white]), targetSquareIndex))
                    {
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 0, 0, 0, 0);
                        // Captures
                    }
                    else
                    {
                        output.appendMove(startSquareIndex, targetSquareIndex, currentPiece, 0, 1, 0, 0, 0);
                    }

                    popBit(currentPieceAttacks, targetSquareIndex);
                }

                popBit(currentPieceBitboard, startSquareIndex);
            }
        }
    }

    // Return the move list
    return output;
}

// Determine if the king is in the check
bool Board::isKingInCheck()
{
    // Return true if the square, at which the king is attacked by the opposite color
    return isSquareAttacked((sideToMove == white) ? getLS1BIndex(bitboards[whiteKing]) : getLS1BIndex(bitboards[blackKing]), sideToMove ^ 1);
}

// Pass a turn to the opposite color
void Board::switchSideToMove()
{
    sideToMove ^= 1;
}

// Write a hash entry into the transposition table
void Board::writeHashEntry(int score, int depth, int searchPly, int flag)
{

    // Locate the transposition node and get the reference to it
    TranspositionNode *pHashEntry = &TRANSPOSITION_TABLE[hashKey % NUM_TT_ENTRIES];

    // Adjust the score if the node is a checkmating one
    if (score < -CHECKMATE_BOUND)
    {
        score -= searchPly;
    }
    else if (score > CHECKMATE_BOUND)
    {
        score += searchPly;
    }

    // Write data into the transposition node
    pHashEntry->hashKey = hashKey;
    pHashEntry->score = score;
    pHashEntry->depth = depth;
    pHashEntry->flag = flag;
}

// Read the hash entry from the transposition table
int Board::readHashEntry(int alpha, int beta, int depth, int searchPly)
{

    // Locate the transposition node and get the reference to it
    TranspositionNode *pHashEntry = &TRANSPOSITION_TABLE[hashKey % NUM_TT_ENTRIES];

    // Check if the value stored in the transposition table can be used
    if (pHashEntry->hashKey == hashKey && pHashEntry->depth >= depth && searchPly)
    {

        // Get the score
        int score = pHashEntry->score;

        // Adjust the score if the node is a checkmating one
        if (score < -CHECKMATE_BOUND)
        {
            score += searchPly;
        }
        else if (score > CHECKMATE_BOUND)
        {
            score -= searchPly;
        }

        // Retrieve the value based on the flags provided
        if (pHashEntry->flag == fPV_HASH)
        {
            return score;
        }

        // Retrieve the value based on the flags provided
        if (pHashEntry->flag == fALPHA_HASH && score <= alpha)
        {
            return alpha;
        }

        // Retrieve the value based on the flags provided
        if (pHashEntry->flag == fBETA_HASH && score >= beta)
        {
            return beta;
        }
    }

    // Return the placeholder value
    return fHASH_NOT_FOUND;
}

int Board::makeMove(int move)
{

    U64 tempBitboards[12], tempOccupancies[3];
    U64 tempHash = hashKey;
    int tempEnPassantSquareIndex = enPassantSquareIndex, tempCanCastle = canCastle;

    memcpy(tempBitboards, bitboards, sizeof(tempBitboards));
    memcpy(tempOccupancies, occupancies, sizeof(tempOccupancies));

    int piece = getPiece(move);
    int startSquareIndex = getStartSquareIndex(move);
    int targetSquareIndex = getTargetSquareIndex(move);
    int promotedPiece = getPromotedPiece(move);

    popBit(bitboards[piece], startSquareIndex);
    setBit(bitboards[piece], targetSquareIndex);

    hashKey ^= PIECE_KEYS[piece][startSquareIndex];
    hashKey ^= PIECE_KEYS[piece][targetSquareIndex];

    if (isCapture(move))
    {

        int startPiece = 0, endPiece = 0;

        if (sideToMove == white)
        {

            startPiece = blackPawn;
            endPiece = blackKing;
        }
        else if (sideToMove == black)
        {

            startPiece = whitePawn;
            endPiece = whiteKing;
        }

        for (int currentPiece = startPiece; currentPiece <= endPiece; currentPiece++)
        {

            if (getBit(bitboards[currentPiece], targetSquareIndex))
            {

                popBit(bitboards[currentPiece], targetSquareIndex);
                hashKey ^= PIECE_KEYS[currentPiece][targetSquareIndex];
                break;
            }
        }
    }

    if (promotedPiece)
    {

        if (sideToMove == white)
        {

            popBit(bitboards[whitePawn], targetSquareIndex);
            hashKey ^= PIECE_KEYS[whitePawn][targetSquareIndex];
        }
        else if (sideToMove == black)
        {

            popBit(bitboards[blackPawn], targetSquareIndex);
            hashKey ^= PIECE_KEYS[blackPawn][targetSquareIndex];
        }

        setBit(bitboards[promotedPiece], targetSquareIndex);
        hashKey ^= PIECE_KEYS[promotedPiece][targetSquareIndex];
    }

    if (isEnPassant(move))
    {

        if (sideToMove == white)
        {

            popBit(bitboards[blackPawn], targetSquareIndex + 8);
            hashKey ^= PIECE_KEYS[blackPawn][targetSquareIndex + 8];
        }
        else if (sideToMove == black)
        {

            popBit(bitboards[whitePawn], targetSquareIndex - 8);
            hashKey ^= PIECE_KEYS[whitePawn][targetSquareIndex - 8];
        }
    }

    if (enPassantSquareIndex != NO_SQUARE_INDEX)
    {
        hashKey ^= ENPASSANT_KEYS[enPassantSquareIndex];
    }

    enPassantSquareIndex = NO_SQUARE_INDEX;

    if (isDoublePawnPush(move))
    {

        if (sideToMove == white)
        {
            enPassantSquareIndex = targetSquareIndex + 8;
        }
        else if (sideToMove == black)
        {
            enPassantSquareIndex = targetSquareIndex - 8;
        }

        hashKey ^= ENPASSANT_KEYS[enPassantSquareIndex];
    }

    if (isCastling(move))
    {

        switch (targetSquareIndex)
        {

        case (g1):

            popBit(bitboards[whiteRook], h1);
            setBit(bitboards[whiteRook], f1);

            hashKey ^= PIECE_KEYS[whiteRook][h1];
            hashKey ^= PIECE_KEYS[whiteRook][f1];

            break;

        case (c1):

            popBit(bitboards[whiteRook], a1);
            setBit(bitboards[whiteRook], d1);

            hashKey ^= PIECE_KEYS[whiteRook][a1];
            hashKey ^= PIECE_KEYS[whiteRook][d1];

            break;

        case (g8):

            popBit(bitboards[blackRook], h8);
            setBit(bitboards[blackRook], f8);

            hashKey ^= PIECE_KEYS[blackRook][h8];
            hashKey ^= PIECE_KEYS[blackRook][f8];

            break;

        case (c8):

            popBit(bitboards[blackRook], a8);
            setBit(bitboards[blackRook], d8);

            hashKey ^= PIECE_KEYS[blackRook][a8];
            hashKey ^= PIECE_KEYS[blackRook][d8];

            break;
        }
    }

    hashKey ^= CASTLING_KEYS[canCastle];

    canCastle &= CASTLE_STATE[startSquareIndex];
    canCastle &= CASTLE_STATE[targetSquareIndex];

    hashKey ^= CASTLING_KEYS[canCastle];

    resetOccupancies();
    populateOccupancies();

    if (isKingInCheck())
    {

        memcpy(bitboards, tempBitboards, sizeof(tempBitboards));
        memcpy(occupancies, tempOccupancies, sizeof(tempOccupancies));
        enPassantSquareIndex = tempEnPassantSquareIndex;
        canCastle = tempCanCastle;
        hashKey = tempHash;
        return 0;
    }

    switchSideToMove();
    hashKey ^= SIDE_KEY;

    return 1;
}

// Load the board from the FEN string
void Board::loadFenString(const string &fenString)
{

    // Reset the board state
    resetBitboards();
    resetOccupancies();

    // Initialise the square index and the en passant square
    int squareIndex = 0;
    enPassantSquareIndex = NO_SQUARE_INDEX;

    // Loop through the characters in the FEN string
    for (int index = 0; index < (int)fenString.length(); index++)
    {

        char symbol = fenString[index];

        // If the character is a letter, it represents a chess piece
        if (isalpha(symbol))
        {

            // If the board is parsed
            if (squareIndex < 64)
            {

                // Set the bit corresponding bitboard
                setBit(bitboards[PIECE_INDEX_TO_ASCII.find(symbol)], squareIndex);
                squareIndex++;

                // Parse other data (castling, en passant etc.)
            }
            else
            {

                switch (symbol)
                {
                // Get the side to move
                case 'w':
                    sideToMove = white;
                    break;
                case 'b':
                    sideToMove = black;
                    break;

                // Get the castling rights
                case 'K':
                    canCastle |= K;
                    break;
                case 'Q':
                    canCastle |= Q;
                    break;
                case 'k':
                    canCastle |= k;
                    break;
                case 'q':
                    canCastle |= q;
                    break;
                }

                // If an en passant square is available
                if (fenString[index - 1] == ' ' && isdigit(fenString[index + 1]))
                {

                    // Get the file and rank of the en passant square
                    int file = symbol - 'a';
                    int rank = 8 - (fenString[index + 1] - '0');
                    enPassantSquareIndex = rank * 8 + file;
                }
            }

            // If the character is a digit, it represents empty squares
        }
        else if (isdigit(symbol))
        {
            squareIndex += (symbol - '0');
        }
    }

    // Calculate the occupancies based on the updated bitboards
    populateOccupancies();
}

// Load a move string in FEN notation
void Board::loadMoveString(const string &moveString)
{

    // Get the start square index and the target square index from a move
    int startSquareIndex = (moveString[0] - 'a') + (8 - (moveString[1] - '0')) * 8;
    int targetSquareIndex = (moveString[2] - 'a') + (8 - (moveString[3] - '0')) * 8;

    // Generate all pseudo-legal moves in a position
    MoveList moves = generateMoves();

    // Loop over the moves
    for (int moveIndex = 0; moveIndex < moves.getCount(); moveIndex++)
    {

        int move = moves.getMoves()[moveIndex];

        // If a move with the given start square index and target square index was found
        if (startSquareIndex == getStartSquareIndex(move) && targetSquareIndex == getTargetSquareIndex(move))
        {

            // Discard the moves trying to avoid pawn promotion
            if ((moveString[3] == '8' && moveString[4] == 'P') || (moveString[3] == '1' && moveString[4] == 'p'))
            {
                return;
            }

            // Discard moves trying to promote to early
            if (moveString[4])
            {
                if ((PIECE_INDEX_TO_ASCII[getPromotedPiece(move)] != moveString[4]) || (PIECE_INDEX_TO_ASCII[getPromotedPiece(move) - 6] != moveString[4]))
                {
                    return;
                }
            }

            // Update the repetition table
            repetitions[repetitionIndex] = hashKey;
            repetitionIndex++;

            // Commit the move
            makeMove(move);
        }
    }
}

// Calculate the game score
int Board::calculateGameScore()
{

    // Initialise the scores
    int whiteScore = 0, blackScore = 0;

    // Loop over white pieces
    for (int currentPiece = whitePawn; currentPiece <= whiteQueen; currentPiece++)
    {

        // Sum up the material scores
        whiteScore += getPopulationCount(bitboards[currentPiece]) * MATERIAL_SCORE[opening][currentPiece];
    }

    // Loop over black pieces
    for (int currentPiece = blackPawn; currentPiece <= blackQueen; currentPiece++)
    {

        // Sum up the material scores
        blackScore += getPopulationCount(bitboards[currentPiece]) * MATERIAL_SCORE[opening][currentPiece];
    }

    // "-" used because the black score is negative
    // Return the combined material score
    return whiteScore - blackScore;
}

// Reset the en passant square index
void Board::resetEnPassantSquareIndex()
{
    enPassantSquareIndex = NO_SQUARE_INDEX;
}

void Board::updateHashKey(int value)
{
    hashKey ^= value;
}

// Get the current side to move
int Board::getSideToMove()
{
    return sideToMove;
}

// Get the en passant square index
int Board::getEnPassantSquareIndex()
{
    return enPassantSquareIndex;
}

// Get the hash key
int Board::getHashKey()
{
    return hashKey;
}

// Get the array of bitboards
U64 *Board::getBitboards()
{
    return bitboards;
}
;