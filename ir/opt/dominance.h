//
// Created by Valerian on 2023/1/18.
//

#ifndef C22V1_DOMINANCE_H
#define C22V1_DOMINANCE_H
#include "hash.h"
#include "hash_set.h"
#include "hash_map.h"
#include "bblock.h"
#include "function.h"
#include "assert.h"
struct _DomNode{
    BasicBlock *parent;
    //DomTreeNode *parent;
    BasicBlock *block;
    HashSet *children;  //保存DomNode*
    int flag;
};


// BasicBlock -> idom -> DomTreeNode
// DomTreeNode -> BasicBlock ->
void calculate_dominance(Function *currentFunction);
void calculate_dominance_frontier(Function *currentFunction);
void calculate_iDominator(Function *currentFunction);
void calculate_DomTree(Function *currentFunction);
void DomTreeAddChild(DomTreeNode *parent, DomTreeNode *child);


void HashSetCopy(HashSet *dest,HashSet *src);
bool HashSetDifferent(HashSet *lhs,HashSet *rhs);
DomTreeNode *createDomTreeNode(BasicBlock *block,BasicBlock *parent);
#endif //C22V1_DOMINANCE_H
