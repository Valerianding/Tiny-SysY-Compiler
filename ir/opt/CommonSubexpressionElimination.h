//
// Created by Valerian on 2023/4/16.
//

#ifndef C22V1_COMMONSUBEXPRESSIONELIMINATION_H
#define C22V1_COMMONSUBEXPRESSIONELIMINATION_H
#include "function.h"
#include "utility.h"
typedef struct Subexpression{
    Value *lhs;
    Value *rhs;
    Opcode op;
}Subexpression;

Subexpression *createSubExpression(Value *lhs, Value *rhs, Opcode op);
bool commonSubexpressionElimination(Function *currentFunction);
bool commonSubexpression(BasicBlock *block);
bool isSimpleOperator(InstNode *instNode);
#endif //C22V1_COMMONSUBEXPRESSIONELIMINATION_H
