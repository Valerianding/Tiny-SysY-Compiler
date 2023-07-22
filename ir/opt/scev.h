//
// Created by Valerian on 2023/7/18.
//

#ifndef C22V1_SCEV_H
#define C22V1_SCEV_H
#include "utility.h"
#include "loopinfo.h"
#include "function.h"

typedef struct AddRecExpr{
    Value *initValue;
    Opcode op; //must be add
    Value *modify;
}AddRecExpr;

typedef struct SCEVExpr{
    struct SCEVExpr *left;
    Opcode op; // + / *
    struct SCEVExpr *right;
}SCEVExpr;
#endif //C22V1_SCEV_H
