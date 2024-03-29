//
// Created by Valerian on 2023/5/31.
//

#ifndef C22V1_SIDEEFFECT_H
#define C22V1_SIDEEFFECT_H

//简单的副作用分析
//current not used -> inline + mark + sweep already perform well
//target at median -> meaning less calculation!
#include "function.h"
#include "utility.h"
typedef struct CallGraphNode{
    HashSet *parents; // CallGraphNode *
    Function *function; // make it easier to go through all the ins
    bool visited;
    HashSet *children; // CallGraphNode * -> can contain itself
}CallGraphNode;


void buildCallGraphNode(Function *currentFunction);
void sideEffectAnalysis(Function *currentFunction);
CallGraphNode* createCallGraph(Function *function);
#endif //C22V1_SIDEEFFECT_H
