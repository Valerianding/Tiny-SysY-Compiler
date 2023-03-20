//
// Created by Valerian on 2023/3/19.
//

#include "livenessanalysis.h"

void HashSetCopy(HashSet *dest, HashSet *src){
    HashSetFirst(src);
    for(Value *liveVariable = HashSetNext(src); liveVariable != NULL; liveVariable = HashSetNext(src)){
        HashSetAdd(dest,liveVariable);
    }
}

// 注意判断是否是立即数
void caculateLiveness(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *exit = currentFunction->tail;

    InstNode *currNode = exit->tail_node;

    clear_visited_flag(entry->head_node);

    bool changed = true;
    while(changed){
        changed = false;
        switch (currNode->inst->Opcode) {
            case Return:{
                Value *variable = ins_get_lhs(currNode->inst);
                // 还没有考虑数组
                if(!isImm(variable) && !isArray(variable)){
                    HashSetAdd(currNode->inst->in,variable);
                }
                break;
            }
            case Phi:{

                break;
            }
            case br:{

            }
            case br_i1:{

            }
            default:{
                //不然就是普通的情况
                InstNode *next = get_next_inst(currNode);

                //如果当前指令的下一条指令不是跳转语句的话那么就可以直接将下一条的in 拷贝到现在的out

                //现在的out 拷贝到现在的in然后再根据use def来解决
                if(next->inst->Opcode != br && next->inst->Opcode != br_i1){

                }

                Value *lhs = ins_get_lhs(currNode->inst);
                Value *rhs = ins_get_rhs(currNode->inst);

                if(!isImm(lhs) && !isArray(lhs)){
                    HashSetAdd(currNode->inst->in,lhs);
                }
                if(!isImm(rhs) && !isArray(rhs)){
                    HashSetAdd(currNode->inst->in,lhs);
                }
            }
        }
        currNode = get_prev_inst(currNode);
    }
}