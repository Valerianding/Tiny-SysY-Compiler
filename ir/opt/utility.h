//
// Created by Valerian on 2023/3/19.
//

#ifndef C22V1_UTILITY_H
#define C22V1_UTILITY_H
#include "function.h"
#include "loopinfo.h"
#include "time.h"
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
struct Loop;
bool isValidOperator(InstNode *insNode);
void clear_visited_flag(BasicBlock *block);
void correctType(Function *currentFunction);
float getOperandValue(Value *operand);
bool isCompareOperator(InstNode *insNode);
void renameVariables(Function *currentFunction);
void showInstructionInfo(InstNode *instruction_list);
void showBlockInfo(InstNode *instruction_list);
void HashSetClean(HashSet *set);
//not set function have bug!! only used for certain condition
BasicBlock *newBlock(HashSet *prevBlocks,BasicBlock *block);
bool hasNoDestOperator(InstNode *insNode);
InstNode *findNode(BasicBlock *block,Instruction *inst);
bool isCriticalOperator(InstNode *insNode);
bool isCalculationOperator(InstNode *instNode);
bool HashSetDifferent(HashSet *lhs,HashSet *rhs);
void calculateNonLocals(Function *currentFunction);
void valueReplaceAll(Value *oldValue, Value *newValue, Function *currentFunction);
bool isParam(Value *val, int paramNum);
void HashSetCopy(HashSet *dest, HashSet *src);
unsigned long int hash_values(Vector *valueVector);
bool isSySYFunction(Value *function);
bool isInputFunction(Value *function);
bool isOutputFunction(Value *function);
bool isTimeFunction(Value *function);
bool JudgeXor(InstNode *insList);
void combineZext(InstNode *insList);
bool isSame(Value *left, Value *right);
//true -> 会被其他用到
//false -> 不会被其他用到
bool JudgeIcmp(InstNode *icmp);
bool isSimpleOperator(InstNode *instNode);
bool isSameLargeType(Value *left, Value *right);
void valueReplaceWithout(Value *oldValue, Value *newValue, Value *cur, Function *currentFunction);
Function *ReconstructFunction(InstNode *inst_list);
bool isLoopInvariant(struct Loop *loop,Value *var);
bool returnValueNotUsed(InstNode *instNode);
bool specialValueReplace(Value *old, Value *new, BasicBlock *pos);
bool isDominated(BasicBlock *block, BasicBlock *target);
void RPOCfg(Function *currentFunction);
void topCfg(Function *currentFunction);
void cleanBlock(BasicBlock *block);
bool containFloat(InstNode *node);
bool usedInPhi(Value *value, Function *function);
bool isRegionalConstant(Value *value,struct Loop *loop);
#endif //C22V1_UTILITY_H
