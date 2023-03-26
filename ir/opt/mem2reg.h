//
// Created by Valerian on 2023/1/15.
//

#ifndef C22V1_MEM2REG_H
#define C22V1_MEM2REG_H
#include "bblock.h"
#include "function.h"
#include "dominance.h"
#include "stack.h"
extern HashMap *GlobalIncomingVal;
void mem2reg(Function *currentFunction);
void insert_phi(BasicBlock *block,Value *val);
void insertPhiInfo(InstNode *ins,pair *phiInfo);
void dfsTravelDomTree(DomTreeNode *node,HashMap *IncomingVals);
void deleteLoadStore(Function *currentFunction);
void renameVariabels(Function *currentFunction);
InstNode *newCopyOperation(Value *dest, Value *src);
void outOfSSA(Function *currentFunction);
InstNode* new_phi(Value *val);
#endif //C22V1_MEM2REG_H
