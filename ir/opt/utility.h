//
// Created by Valerian on 2023/3/19.
//

#ifndef C22V1_UTILITY_H
#define C22V1_UTILITY_H
#include "function.h"
void correctType(Function *currentFunction);
bool isValidOperator(InstNode *insNode);
void clear_visited_flag(BasicBlock *block);
#endif //C22V1_UTILITY_H
