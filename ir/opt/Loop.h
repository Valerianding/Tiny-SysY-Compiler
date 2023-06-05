//
// Created by Valerian on 2023/4/18.
//

#ifndef C22V1_LOOP_H
#define C22V1_LOOP_H
#include "function.h"
#include "utility.h"
#include <stack.h>
typedef struct Loop{
    struct Loop *parent;
    BasicBlock *head;
    BasicBlock *tail; //back-edge's tail
    HashSet *exit;
    HashSet *loopBody;
    HashSet *child; // loop *

    Value *inductionVariable; //inductionVariable may be NULL also for the initValue and modifier
    Value *initValue; //induction variable's init value
    Value *modifier; //modifier is the dest of the instruction that change the inductionVariable
}Loop;


// 每次需要对循环做优化的时候就再计算
void loop(Function *currentFunction);
Loop *constructLoop(BasicBlock *head,BasicBlock *tail);
void findBody(Loop *loop);
void findExit(Loop *loop);
bool LICM_EACH(Loop *loop);
bool LICM(Function *currentFunction);
bool dfsLoopTree(Loop *loop);
void reconstructLoop(Loop *loop);
#endif //C22V1_LOOP_H
