//
// Created by Valerian on 2023/3/19.
//

#include "livenessanalysis.h"

bool HashSetCopyValue(HashSet *dest, HashSet *src){
    bool changed = false;
    HashSetFirst(src);
    for(Value *liveVariable = HashSetNext(src); liveVariable != NULL; liveVariable = HashSetNext(src)){
        if(!HashSetFind(dest,liveVariable)){
            HashSetAdd(dest,liveVariable);
            changed = true;
        }
    }
    return changed;
}


// 现在的活跃变量分析是消除了SSA之后的
void calculateLiveness(Function *currentFunction) {
    BasicBlock *exit = currentFunction->tail;
    bool changed = true;
    while(changed){
        changed = analysisBlock(exit,NULL);
    }
}


//递归遍历
bool analysisBlock(BasicBlock *block, HashSet* nextLiveIn){
    //一顿分析
    bool changed = false;

    HashSet *liveIn = block->in;
    HashSet *liveOut = block->out;

    InstNode *currNode = block->tail_node;
    InstNode *blockHead = block->head_node;

    // 首先现在的OUT 加上 来的In 并判断有没有改变
    if(nextLiveIn != NULL){
        HashSetFirst(nextLiveIn);
        changed |= HashSetCopyValue(liveOut,nextLiveIn);
    }
    HashSet *tempSet = HashSetInit();

    // 现在的Live-out拷贝到一个temp里面
    HashSetCopyValue(tempSet,liveOut);

    //后续对tempSet进行操作
    //反正blockHead不是funBegin 就是 label
    while(currNode != blockHead){
        if(isValidOperator(currNode)){
            printf("current Opcode is %d\n",currNode->inst->Opcode);

            Value *def = NULL;
            Value *lhs = NULL;
            Value *rhs = NULL;

            //如果是return 语句的话就不需要左边这个
            if(currNode->inst->Opcode != Return && currNode->inst->Opcode != CopyOperation){
                def = ins_get_value(currNode->inst);
            }else if(currNode->inst->Opcode == CopyOperation){
                Value *insValue = ins_get_value(currNode->inst);
                def = insValue->alias;
            }

            //现在是否只有可能是
            if(currNode->inst->user.value.NumUserOperands == (unsigned int)1){
                lhs = ins_get_lhs(currNode->inst);
            }
            if(currNode->inst->user.value.NumUserOperands == (unsigned int)2){
                lhs = ins_get_lhs(currNode->inst);
                rhs = ins_get_rhs(currNode->inst);
            }

            // 不是立即数且不是数组的话我们可以加入分析
            if(def != NULL && !isImm(def) && !isArray(def)){
                if(HashSetFind(tempSet,def)){
                    HashSetRemove(tempSet,def);
                }
            }
            if(lhs != NULL && !isImm(lhs) && !isArray(lhs)){
                if(!HashSetFind(tempSet,lhs)){
                    HashSetAdd(tempSet,lhs);
                }
            }

            if(rhs != NULL && !isImm(rhs) && !isArray(rhs)){
                if(!HashSetFind(tempSet,rhs)){
                    HashSetAdd(tempSet,rhs);
                }
            }
        }
        currNode = get_prev_inst(currNode);
    }

    //OK 现在的tempSet就是我们的live-in了
    changed |= HashSetCopyValue(liveIn,tempSet);


    //递归
    HashSet *prevBlocks = block->preBlocks;
    HashSetFirst(prevBlocks);
    for(BasicBlock *prevBlock = HashSetNext(prevBlocks); prevBlock != NULL; prevBlock = HashSetNext(prevBlocks)){
        changed |= analysisBlock(prevBlock,liveIn);
    }

    // 释放内存
    HashSetDeinit(tempSet);
    return changed;
}

void printLiveness(BasicBlock *block){
    printf("b%d ", block->id);
    HashSetFirst(block->in);
    printf("live in: ");
    for(Value *liveInVariable = HashSetNext(block->in); liveInVariable != NULL; liveInVariable = HashSetNext(block->in)){
        if(liveInVariable->name != NULL){
            printf("%s ",liveInVariable->name);
        }else{
            printf("null ");
        }
    }
    printf("\n");

    printf("b%d ", block->id);
    HashSetFirst(block->out);
    printf("live out: ");
    for(Value *liveOutVariable = HashSetNext(block->out); liveOutVariable != NULL; liveOutVariable = HashSetNext(block->out)){
        if(liveOutVariable->name != NULL){
            printf("%s",liveOutVariable->name);
        }else{
            printf("null ");
        }
    }
    printf("\n");

    //递归调用printLiveness
    if(block->true_block != NULL) printLiveness(block->true_block);
    if(block->false_block != NULL) printLiveness(block->false_block);
}