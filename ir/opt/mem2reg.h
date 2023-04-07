//
// Created by Valerian on 2023/1/15.
//

#ifndef C22V1_MEM2REG_H
#define C22V1_MEM2REG_H
#include "bblock.h"
#include "function.h"
#include "dominance.h"
#include "stack.h"
#include "livenessanalysis.h"
typedef struct CopyPair{
    Value *src;
    Value *dest;
}CopyPair;

extern HashMap *GlobalIncomingVal;
void mem2reg(Function *currentFunction);
void insert_phi(BasicBlock *block,Value *val);
void insertPhiInfo(InstNode *ins,pair *phiInfo);
void insertCopies(BasicBlock *block,Value *dest,Value *src);
void insertPhiCopies(DomTreeNode *node, HashMap *varStack);
InstNode *newCopyOperation(Value *src);
InstNode* new_phi(Value *val);
void dfsTravelDomTree(DomTreeNode *node,HashMap *IncomingVals);
void deleteLoadStore(Function *currentFunction);
void renameVariabels(Function *currentFunction);
void outOfSSA(Function *currentFunction);
void SSADeconstruction(Function *currentFunction);
void calculateNonLocals(Function *currentFunction);
bool correctPhiNode(Function *currentFunction);
CopyPair *createCopyPair(Value *src, Value *dest);
#endif //C22V1_MEM2REG_H
