#ifndef GLOBALS_H
#define GLOBALS_H

#include "typedef.h"
#include "AttackTable.h"
#include "TranspositionNode.h"
#include "const.h"

extern AttackTable ATTACKS;
extern TranspositionNode TRANSPOSITION_TABLE[NUM_TT_ENTRIES];

extern U64 repetitions[4096];
extern int repetitionIndex;

extern U64 fileMasks[8];
extern U64 rankMasks[8];
extern U64 isolatedPawnMasks[8];
extern U64 whitePassedPawnMasks[64];
extern U64 blackPassedPawnMasks[64];

extern U64 PIECE_KEYS[12][64];
extern U64 ENPASSANT_KEYS[64];
extern U64 CASTLING_KEYS[16];
extern U64 SIDE_KEY;

void generateKeys();
void generateEvaluationMasks();

#endif
