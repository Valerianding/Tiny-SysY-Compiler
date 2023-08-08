////
//// Created by Valerian on 2023/5/14.
////
//
//#ifndef C22V1_INSCOMB_H
//#define C22V1_INSCOMB_H
//#include "function.h"
//#include "utility.h"
//#include "stdbool.h"

//#endif //C22V1_INSCOMB_H


//
// Created by Valerian on 2023/5/14.
//
#ifndef C22V1_INSCOMB_H
#define C22V1_INSCOMB_H
#include "function.h"
#include "stdbool.h"
#include "utility.h"
void instruction_combination(Function *currentFunction);
bool check(Instruction *instruction,int location);
int get_const_location(Instruction* instruction);
Value *get_value(Instruction *instruction,bool special);
Value *get_const(Instruction *instruction, bool special);
void combination(Instruction *instruction_A,Instruction * instruction_B);


//djf
bool checkType(Instruction *ins);
bool InstCombine(Function *currentFunction);
#endif //C22V1_INSCOMB_H
