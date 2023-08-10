//
// Created by Valerian on 2023/4/18.
//

#ifndef C22V1_LOOPINFO_H
#define C22V1_LOOPINFO_H
#include "function.h"
#include "utility.h"
#include <stack.h>
typedef struct Loop{
    struct Loop *parent;

    HashSet *child; // loop *

    //entry block (head of back edge)
    BasicBlock *head;
    //tail block (tail of back edge)
    BasicBlock *tail;
    //preHeader of the loop(predecessor is the only guard, successors is the loop entry)
    BasicBlock *preHeader;
    //guard of the loop(successor is the preHeader)
    BasicBlock *guard;
    // exiting Block(it is actually called exiting Block)
    HashSet *exitingBlock;
    //all blocks of loop containing entry and tail
    HashSet *loopBody;
    //inductionVariable may be NULL also for the initValue and modifier
    Value *inductionVariable;
    //induction variable's init value
    Value *initValue;
    //modifier is the dest of the instruction that change the inductionVariable
    //for example %3 if %3 = %ind_var + c
    Value *modifier;
    //the end condition of loop
    //the dest of a icmp
    Value *end_cond;

    //block to jump when leaving loop entry
    BasicBlock *body_block;
    //block to jump to when the loop exits (it looks like to be the entry's false block ??)
    BasicBlock *exit_block;

    HashSet *latches; //current not used
    bool containMultiBackEdge;

    //often, if the loop has not dedicated exit it means it has "break"
    bool hasDedicatedExit;

    //it is true if the condition changes during the iteration of execution
    bool conditionChangeWithinLoop;

    HashMap *inductionVariables; // Vale * -> BIVExpression*
}Loop;


// 每次需要对循环做优化的时候就再计算
void loopAnalysis(Function *currentFunction);
Loop *constructLoop(BasicBlock *head,BasicBlock *tail);
void findBody(Loop *loop);
void findExit(Loop *loop);
void findInductionVariable(Loop *loop);
#endif //C22V1_LOOPINFO_H
