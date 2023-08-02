////
//// Created by Valerian on 2023/5/14.
////
//
//#ifndef C22V1_INSCOMB_H
//#define C22V1_INSCOMB_H
//#include "function.h"
//#include "utility.h"
//#include "stdbool.h"
//bool InstCombine(Function *currentFunction);
//#endif //C22V1_INSCOMB_H


//
// Created by Valerian on 2023/5/14.
//
#ifndef C22V1_INSCOMB_H
#define C22V1_INSCOMB_H
#include <function.h>
#include <stdbool.h>
void instruction_combination(Function *currentFunction);
bool check(Instruction *instruction,int location);
int get_const_location(Instruction* instruction);
Value *get_value(Instruction *instruction);
Value *get_const(Instruction *instruction);
void combination(Instruction *instruction_A,Instruction * instruction_B);
#endif //C22V1_INSCOMB_H
