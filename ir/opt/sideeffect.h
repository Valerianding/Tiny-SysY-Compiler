//
// Created by Valerian on 2023/5/31.
//

#ifndef C22V1_SIDEEFFECT_H
#define C22V1_SIDEEFFECT_H

//简单的副作用分析

#include "function.h"
#include "utility.h"
typedef struct CallGraphNode{
    HashSet *parents; // CallGraphNode *
    Value *function;
    bool visited;
    HashSet *children; // CallGraphNode * -> can contain itself
}CallGraphNode;


void buildCallGraphNode(Function *currentFunction);
void sideEffect(Function *currentFunction);
#endif //C22V1_SIDEEFFECT_H
