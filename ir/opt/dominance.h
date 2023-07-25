//
// Created by Valerian on 2023/1/18.
//

#ifndef C22V1_DOMINANCE_H
#define C22V1_DOMINANCE_H
#include "hash.h"
#include "hash_set.h"
#include "hash_map.h"
#include "utility.h"
#include "function.h"
#include "assert.h"
extern bool Optimize;

struct _DomNode{
    BasicBlock *parent;
    BasicBlock *block;
    HashSet *children;  //保存DomNode*
    int flag;
    int depth; // dom depth

    int loopNest; // nest depth
};

struct _PostDomNode{
    BasicBlock *parent;
    BasicBlock *block;
    HashSet *children; // 保存PostDomNode *类型
    int flag;
};

PostDomNode *createPostDomNode(BasicBlock *block, BasicBlock *ipDom);
void calculate_dominance(Function *currentFunction);
void calculate_dominance_frontier(Function *currentFunction);
void calculate_iDominator(Function *currentFunction);
void calculate_DomTree(Function *currentFunction);
void DomTreeAddChild(DomTreeNode *parent, DomTreeNode *child);
void DomTreePrinter(DomTreeNode *root);
void cleanDominance(Function *currentFunction);
void cleanPostDominance(Function *currentFunction);
void dominanceAnalysis(Function *currentFunction);
void postDominanceAnalysis(Function *currentFunction);
void calculatePostDominance(Function *currentFunction);
void removeUnreachable(Function *currentFunction);
#endif //C22V1_DOMINANCE_H
