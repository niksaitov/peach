#ifndef POSITION_H
#define POSITION_H

#include <iostream>
#include <cstring>
#include <chrono>

#include "Board.h"
#include "move_encoding.h"
#include "bitboard_operations.h"
#include "const.h"
#include "enum.h"
#include "typedef.h"

using std::cout, std::string;

extern U64 repetitions[];
extern int repetitionIndex;
extern U64 ENPASSANT_KEYS[64];
extern U64 SIDE_KEY;

class Position {

    private:

        Board currentBoard;
        int killerMoves[2][MAX_SEARCH_DEPTH];
        int historyMoves[12][64];

        int pvTable[MAX_SEARCH_DEPTH][MAX_SEARCH_DEPTH];
        int pvLength[MAX_SEARCH_DEPTH];

        bool fPVScore = false;
        bool fPVFollow = true;

        int bestMove;
        int searchPly;

    public:

        Position(string fenString) {
            currentBoard = Board(fenString);
            searchPly = 0;
        }

        U64 perft(int depth) {

            U64 nodes = 0ULL;

            if (depth == 0) {
                return 1ULL;
            }

            MoveList moves = currentBoard.generateMoves();

            for (int moveIndex = 0; moveIndex < moves.getCount(); moveIndex++) {

                Board temporaryBoard = currentBoard;

                if (!currentBoard.makeMove(moves.getMoves()[moveIndex])) {
                    continue;
                }

                nodes += perft(depth - 1);

                currentBoard = temporaryBoard;
            }

            return nodes;
        }

        void perftDebugInfo(int depth) {

            cout << "\n    Performance test\n\n";

            U64 nodes = 0ULL;

            MoveList moves = currentBoard.generateMoves();

            auto start = std::chrono::high_resolution_clock::now();

            for (int moveIndex = 0; moveIndex < moves.getCount(); moveIndex++) {

                int currentMove = moves.getMoves()[moveIndex];

                Board temporaryBoard = currentBoard;

                if (!currentBoard.makeMove(currentMove)) {
                    continue;
                }

                U64 currentNodes = perft(depth - 1);

                nodes += currentNodes;

                currentBoard = temporaryBoard;

                cout << "Move: " << SQUARE_INDEX_TO_COORDINATES[getStartSquareIndex(currentMove)] << SQUARE_INDEX_TO_COORDINATES[getTargetSquareIndex(currentMove)];
                cout << ((getPromotedPiece(currentMove) != 0) ? PIECE_INDEX_TO_ASCII[getPromotedPiece(currentMove)] : ' ');
                cout << "\tnodes: " << currentNodes << '\n';
            }

            cout << "\nDepth: " << depth;
            cout << "\nTotal number of nodes: " << nodes;
            std::cout << "\nTest time: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds\n\n";
        }

        void updatePVScore(MoveList &moveList) {

            fPVFollow = false;

            for (int moveIndex = 0; moveIndex < moveList.getCount(); moveIndex++) {
                if (moveList.getMoves()[moveIndex] == pvTable[0][searchPly]) {
                    fPVScore = true;
                    fPVFollow = true;
                }
            }
        }

        int scoreMove(int move) {

            if (fPVScore && pvTable[0][searchPly] == move) {
                fPVScore = false;
                return 15000;
            }

            if (isCapture(move)) {

                int targetPiece = 0, startPiece = 0, endPiece = 0;

                if (currentBoard.getSideToMove() == white) {
                    startPiece = blackPawn;
                    endPiece = blackKing;
                } else if (currentBoard.getSideToMove() == black) {
                    startPiece = whitePawn;
                    endPiece = whiteKing;
                }

                for (int currentPiece = startPiece; currentPiece <= endPiece; currentPiece++) {
                    if (getBit(currentBoard.getBitboards()[currentPiece], getTargetSquareIndex(move))) {
                        targetPiece = currentPiece;
                        break;
                    }
                }

                return MVV_LVA[getPiece(move)][targetPiece] + 10000;

            } else if (killerMoves[0][searchPly] == move) {
                return 5000;
            } else if (killerMoves[1][searchPly] == move) {
                return 1000;
            } else if (historyMoves[getPiece(move)][searchPly] == move) {
                return 100;
            }

            return 0;
        }

        void merge(int *moveArray, int leftIndex, int middleIndex, int rightIndex) {

            int leftArraySize = middleIndex - leftIndex + 1;
            int rightArraySize = rightIndex - middleIndex;

            int leftArray[128], rightArray[128];

            for (int i = 0; i < leftArraySize; i++) {
                leftArray[i] = moveArray[leftIndex + i];
            }

            for (int j = 0; j < rightArraySize; j++) {
                rightArray[j] = moveArray[middleIndex + j + 1];
            }

            int i = 0, j = 0, k = leftIndex;

            while (i < leftArraySize && j < rightArraySize) {
                if (scoreMove(leftArray[i]) > scoreMove(rightArray[j])) {
                    moveArray[k] = leftArray[i];
                    i++;
                } else {
                    moveArray[k] = rightArray[j];
                    j++;
                }
                k++;
            }

            while (i < leftArraySize) {
                moveArray[k] = leftArray[i];
                i++; k++;
            }

            while (j < rightArraySize) {
                moveArray[k] = rightArray[j];
                j++; k++;
            }
        }

        void mergeSort(int *moveArray, int leftIndex, int rightIndex) {

            if (leftIndex < rightIndex) {
                int middleIndex = leftIndex + (rightIndex - leftIndex) / 2;
                mergeSort(moveArray, leftIndex, middleIndex);
                mergeSort(moveArray, middleIndex + 1, rightIndex);
                merge(moveArray, leftIndex, middleIndex, rightIndex);
            }
        }

        void sortMoves(MoveList &moveList) {
            mergeSort(moveList.getMoves(), 0, moveList.getCount() - 1);
        }

        int quiescence(int alpha, int beta) {

            int evaluation = currentBoard.staticEvaluate();

            if (evaluation >= beta) {
                return beta;
            }

            if (evaluation > alpha) {
                alpha = evaluation;
            }

            MoveList moves = currentBoard.generateMoves();
            sortMoves(moves);

            for (int moveIndex = 0; moveIndex < moves.getCount(); moveIndex++) {

                int currentMove = moves.getMoves()[moveIndex];

                if (isCapture(currentMove)) {

                    Board temporaryBoard = currentBoard;

                    repetitions[repetitionIndex] = currentBoard.getHashKey();
                    repetitionIndex++;
                    searchPly++;

                    if (!currentBoard.makeMove(moves.getMoves()[moveIndex])) {
                        repetitionIndex--;
                        searchPly--;
                        continue;
                    }

                    int score = -quiescence(-beta, -alpha);

                    repetitionIndex--;
                    searchPly--;

                    currentBoard = temporaryBoard;

                    if (score >= beta) {
                        return beta;
                    }

                    if (score > alpha) {
                        alpha = score;
                    }
                }
            }

            return alpha;
        }

        bool isRepetition() {

            for (int i = 0; i < repetitionIndex; i++) {
                if (repetitions[i] == (U64)currentBoard.getHashKey()) {
                    return true;
                }
            }
            return false;
        }

        int negamax(int alpha, int beta, int depth) {

            int score;

            bool isPV = beta - alpha > 1;

            pvLength[searchPly] = searchPly;

            if (searchPly && isRepetition()) {
                return DRAW_SCORE;
            }

            if (!isPV && (score = currentBoard.readHashEntry(alpha, beta, depth, searchPly)) != fHASH_NOT_FOUND) {
                return score;
            }

            if (depth == 0) {
                return quiescence(alpha, beta);
            }

            if (searchPly > MAX_SEARCH_DEPTH - 1) {
                return currentBoard.staticEvaluate();
            }

            bool inCheck = currentBoard.isKingInCheck();

            if (inCheck) {
                depth++;
            }

            if (depth >= REDUCTION_LIMIT && !inCheck && searchPly) {

                Board nullMoveTemporaryBoard = currentBoard;

                repetitions[repetitionIndex] = currentBoard.getHashKey();
                repetitionIndex++;
                searchPly++;

                if (currentBoard.getEnPassantSquareIndex() != NO_SQUARE_INDEX) {
                    currentBoard.updateHashKey(ENPASSANT_KEYS[currentBoard.getEnPassantSquareIndex()]);
                }

                currentBoard.resetEnPassantSquareIndex();
                currentBoard.switchSideToMove();
                currentBoard.updateHashKey(SIDE_KEY);

                score = -negamax(-beta, -beta + 1, depth - REDUCTION_LIMIT);

                repetitionIndex--;
                searchPly--;

                currentBoard = nullMoveTemporaryBoard;

                if (score >= beta) {
                    return beta;
                }
            }

            int legalMoves = 0;

            MoveList moves = currentBoard.generateMoves();

            if (fPVFollow) {
                updatePVScore(moves);
            }

            sortMoves(moves);

            int movesSearched = 0;

            for (int moveIndex = 0; moveIndex < moves.getCount(); moveIndex++) {

                Board temporaryBoard = currentBoard;

                int currentMove = moves.getMoves()[moveIndex];

                repetitions[repetitionIndex] = currentBoard.getHashKey();
                repetitionIndex++;
                searchPly++;

                if (!currentBoard.makeMove(currentMove)) {
                    repetitionIndex--;
                    searchPly--;
                    continue;
                }

                legalMoves++;

                if (movesSearched == 0) {
                    score = -negamax(-beta, -alpha, depth - 1);
                } else {

                    if (movesSearched >= FULL_DEPTH_MOVES &&
                        depth >= REDUCTION_LIMIT &&
                        inCheck == false &&
                        !isCapture(currentMove) &&
                        !getPromotedPiece(currentMove)) {
                        score = -negamax(-alpha - 1, -alpha, depth - 2);
                    } else {
                        score = alpha + 1;
                    }

                    if (score > alpha) {
                        score = -negamax(-alpha - 1, -alpha, depth - 1);

                        if (score > alpha && score < beta) {
                            score = -negamax(-beta, -alpha, depth - 1);
                        }
                    }
                }

                searchPly--;
                repetitionIndex--;
                movesSearched++;

                currentBoard = temporaryBoard;

                if (score >= beta) {

                    currentBoard.writeHashEntry(beta, depth, searchPly, fBETA_HASH);

                    if (!isCapture(currentMove)) {
                        killerMoves[1][searchPly] = killerMoves[0][searchPly];
                        killerMoves[0][searchPly] = currentMove;
                    }

                    return beta;
                }

                if (score > alpha) {

                    if (!isCapture(currentMove)) {
                        historyMoves[getPiece(currentMove)][getTargetSquareIndex(currentMove)] = currentMove;
                    }

                    alpha = score;

                    pvTable[searchPly][searchPly] = currentMove;

                    for (int nextPly = searchPly + 1; nextPly < pvLength[searchPly + 1]; nextPly++) {
                        pvTable[searchPly][nextPly] = pvTable[searchPly + 1][nextPly];
                    }

                    pvLength[searchPly] = pvLength[searchPly + 1];

                    if (!searchPly) {
                        bestMove = currentMove;
                    }
                }
            }

            if (!legalMoves) {
                if (inCheck) {
                    return -CHECKMATE_SCORE + searchPly;
                } else {
                    return DRAW_SCORE;
                }
            }

            currentBoard.writeHashEntry(beta, depth, searchPly, fBETA_HASH);
            return alpha;
        }

        void printPV() {
            for (int i = 0; i < pvLength[0]; i++) {
                printMove(pvTable[0][i]);
                cout << ' ';
            }
        }

        void resetSearchVariables() {
            bestMove = 0; searchPly = 0;
            memset(killerMoves, 0, sizeof(killerMoves));
            memset(historyMoves, 0, sizeof(historyMoves));
            memset(pvTable, 0, sizeof(pvTable));
            memset(pvLength, 0, sizeof(pvLength));
        }

        int getBestMove() {
            return bestMove;
        }

        Board getBoard() {
            return currentBoard;
        }
};

#endif
