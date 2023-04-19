//
// Created by Valerian on 2023/4/18.
//

#include "Loop.h"
void HashSetCopyBlock(HashSet *dest, HashSet *src){
    HashSetFirst(src);
    BasicBlock* block = NULL;
    for(block = HashSetNext(src); block != NULL; block = HashSetNext(src)){
        HashSetAdd(dest,block);
    }
    return;
}

void loop(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    // 寻找循环体结构
    HashSet *workList = HashSetInit();  // 先把所有的基本块放在这里面 bfs
    HashSet *allBlocks = HashSetInit();
    clear_visited_flag(entry);

    //bfs
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        //block 我们是一定没有visited
        block->visited = true;
        // 加入allBlocks
        HashSetAdd(allBlocks,block);
        //
        HashSet *dom = block->dom;
        if(block->true_block){
            if(block->true_block->visited){
                //回边
                BasicBlock *head = block->true_block;
                // 必然是回边 不是回边我吃屎
                assert(HashSetFind(dom,head));
                // 寻找这个循环的结构体

                // findbody();
            }else{
                HashSetAdd(workList,block->true_block);
            }
        }
        if(block->false_block){
            if(block->false_block->visited){

                BasicBlock *head = block->false_block;
                assert(HashSetFind(dom,head));
            }else{
                HashSetAdd(workList,block->false_block);
            }
        }
    }


    // 找到所有的回边之后
}

void findbody(BasicBlock *head,BasicBlock *tail){
    stack *workStack = stackInit();
    HashSet *loop = HashSetInit();
    // 将head 和 tail放进去
    HashSetAdd(loop, tail);
    HashSetAdd(loop,head);


    //现将tail 放进去的
    stackPush(workStack,tail);
    while(stackSize(workStack) != 0){
        BasicBlock *block = NULL;
        stackTop(workStack, (void *)&block);
        stackPop(workStack);
        assert(block != NULL);
        HashSetFirst(block->preBlocks);
        for(BasicBlock *preBlock = HashSetNext(block->preBlocks); preBlock != NULL; preBlock = HashSetNext(block->preBlocks)){
            if(!HashSetFind(loop,preBlock)){
                HashSetAdd(loop,preBlock);
                stackPush(workStack,preBlock);
            }
        }
    }
}


void loopVariant(Function *currentFunction, HashSet *loop, BasicBlock *head){
    //
    HashSet *def = HashSetInit();

    HashSetFirst(loop);
    for(BasicBlock *block = HashSetNext(loop); block != NULL; block = HashSetNext(loop)){
        InstNode *currNode = block->head_node;
        while(currNode != block->tail_node){
            if(isCalculationOperator(currNode)){
                Value *insValue = ins_get_dest(currNode->inst);
                HashSetAdd(def,insValue);
            }
            currNode = get_next_inst(currNode);
        }
    }

    HashSet *loopInvariantVariable = HashSetInit();
    //def 全部知道了那我们就开始迭代吧
    bool changed = true;
    while(changed){
        changed = false;
        HashSetFirst(loop);
        BasicBlock *block = NULL;
        for(block = HashSetNext(loop);block != NULL; block = HashSetNext(loop)){
            // 我们需要
            InstNode *currNode = block->head_node;
            while(currNode != block->tail_node){
                // TODO 解决所有Operator的情况
                if(isCalculationOperator(currNode)){
                    Value *lhs = ins_get_lhs(currNode->inst);
                    Value *rhs = ins_get_rhs(currNode->inst);
                    Value *dest = ins_get_dest(currNode->inst);

                    if(!HashSetFind(def,lhs) && !HashSetFind(def,rhs)){
                        changed = true;
                        HashSetAdd(loopInvariantVariable,dest);
                    }else if(!HashSetFind(def,lhs) && HashSetFind(loopInvariantVariable,rhs)){
                        changed = true;
                        HashSetAdd(loopInvariantVariable,dest);
                    }else if(HashSetFind(loopInvariantVariable,lhs) && !HashSetFind(def,rhs)){
                        changed = true;
                        HashSetAdd(loopInvariantVariable,dest);
                    }
                }
                currNode = get_next_inst(currNode);
            }
        }
    }


    //找到了所有loop  invariant


    // 先看看head有多少个前驱节点
    if(HashSetSize(head->preBlocks) > 1){
        // 需要新建基本块

        //将所有的前驱基本块连在这个新的基本块上
        BasicBlock *newBlock = bb_create();
    }
    //
    //销毁内存
    HashSetDeinit(loopInvariantVariable);
    HashSetDeinit(def);
}