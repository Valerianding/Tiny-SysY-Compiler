//
// Created by Valerian on 2023/4/16.
//

#ifndef C22V1_COMMONSUBEXPRESSIONELIMINATION_H
#define C22V1_COMMONSUBEXPRESSIONELIMINATION_H
#include "function.h"
typedef struct Subexpression{
    Value *lhs;
    Value *rhs;
    Opcode op;
}Subexpression;


#endif //C22V1_COMMONSUBEXPRESSIONELIMINATION_H
