#ifndef TRANSPOSITIONNODE_H
#define TRANSPOSITIONNODE_H

#include "typedef.h"

struct TranspositionNode
{
    U64 hashKey = 0;
    int depth = 0;
    int flag = 0;
    int score = 0;
};

#endif