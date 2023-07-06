
//
// Created by Valerian on 2023/5/19.
//

#ifndef C22V1_DVNT_H
#define C22V1_DVNT_H
#include "utility.h"
#include "bblock.h"
#include "dominance.h"
typedef struct HashExpression{
    Opcode op;
    unsigned int lhsValueNumber;
    unsigned int rhsValueNumber;
}HashExpression;
bool DVNT(Function *currentFunction);
bool DVNT_EACH(BasicBlock *block, HashMap *table,HashMap *var2num,Function *currentFunction);
#endif //C22V1_DVNT_H
