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
#include <time.h>
typedef struct CopyPair{
    Value *src;
    Value *dest;
}CopyPair;

void mem2reg(Function *currentFunction);
void insert_phi(BasicBlock *block,Value *val);
void insertPhiInfo(InstNode *ins,pair *phiInfo);
void insertCopies(BasicBlock *block,Value *dest,Value *src);
InstNode *newCopyOperation(Value *src);
InstNode* new_phi(Value *val);
void dfsTravelDomTree(DomTreeNode *node,HashMap *IncomingVals);
void deleteLoadStore(Function *currentFunction);
void renameVariabels(Function *currentFunction);
void outOfSSA(Function *currentFunction);
void SSADeconstruction(Function *currentFunction);
void calculateNonLocals(Function *currentFunction);
void correctPhiNode(Function *currentFunction);
void sequentialCopy(Function *currentFunction);
void prunePhi(Function *currentFunction);
CopyPair *createCopyPair(Value *src, Value *dest);
void insertPhiCopies(DomTreeNode *node, HashMap *varStack);
void HashSetCopyPair(HashSet *dest, HashSet *src);
#endif //C22V1_MEM2REG_H
