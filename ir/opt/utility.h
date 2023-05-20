//
// Created by Valerian on 2023/3/19.
//

#ifndef C22V1_UTILITY_H
#define C22V1_UTILITY_H
#include "function.h"
bool isValidOperator(InstNode *insNode);
void clear_visited_flag(BasicBlock *block);
void correctType(Function *currentFunction);
float getOperandValue(Value *operand);
bool isCompareOperator(InstNode *insNode);
void renameVariabels(Function *currentFunction);
void showInstructionInfo(InstNode *instruction_list);
void showBlockInfo(InstNode *instruction_list);
void HashSetClean(HashSet *set);
BasicBlock *newBlock(HashSet *prevBlocks,BasicBlock *block);
bool hasNoDestOperator(InstNode *insNode);
InstNode *findNode(BasicBlock *block,Instruction *inst);
bool isCriticalOperator(InstNode *insNode);
bool isCalculationOperator(InstNode *instNode);
bool HashSetDifferent(HashSet *lhs,HashSet *rhs);
void calculateNonLocals(Function *currentFunction);
#endif //C22V1_UTILITY_H
