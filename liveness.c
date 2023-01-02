//
// Created by Valerian on 2022/12/2.
//

#include "liveness.h"

void init_VarSpace(VarSpace *this){
    this->isLive = false;
    this->order = 0;
    this->place = 0;
}

/*
 * 1.如果哈希表为空 初始化
 * 2.因为保留的是in
 * 3.最后一条语句使用的value是活跃的
 */
void ll_analysis(BasicBlock *this) {
    InstNode *head = this->head_node;
    InstNode *tail = this->tail_node;
    InstNode *cur_node = tail;
    /* 从最后一个node开始分析 */
    for (; cur_node != NULL && cur_node->inst->i >= head->inst->i; cur_node = get_prev_inst(cur_node)){
        printf("cur_node id %d\n",cur_node->inst->i);
        int flag = 1;
        if(cur_node == tail){
            //如果是最后一个语句就不需要拷贝这个过程
            flag = 0;
        }
        if (NULL == cur_node->inst->value_VarSpace) {
            cur_node->inst->value_VarSpace = HashMapInit();
        }
        if(flag == 1){
            copy_liveness(cur_node, get_next_inst(cur_node));
        }
        if(flag == 0){
            //如果是最后的话，可能会需要从block最后的地方取出来

        }
        //1. 先分析一下 instruction左边value*是需要
        // 用到了多少个value
        for(unsigned int i = 0; i < cur_node->inst->user.value.NumUserOperands; i++){
            Value *now = cur_node->inst->user.use_list[i].Val;
            printf("for now!\n");
            //从前有value 直接修改  没有需要new一个varspace
            if(HashMapContain(cur_node->inst->value_VarSpace,(Value*)now)){
                VarSpace *var_space = (VarSpace*)HashMapGet(cur_node->inst->value_VarSpace,(Value*)now);
                var_space->isLive = true;
            }else{
                VarSpace *temp = (VarSpace*)malloc(sizeof(VarSpace));
                init_VarSpace(temp);
                temp->isLive = true;
                HashMapPut(cur_node->inst->value_VarSpace,now,temp);
            }
        }
        printf("dest now\n");
        //判断dest 除了最后一个语句的dest不用加入
        if(flag == 1){
            if(HashMapContain(cur_node->inst->value_VarSpace,(Value*)cur_node->inst->dest)){
                printf("dest exsit!\n");
                VarSpace *var_space = (VarSpace *)HashMapGet(cur_node->inst->value_VarSpace,(Value*)cur_node->inst->dest);
                var_space->isLive = false;
            }else{
                VarSpace *temp = (VarSpace*)malloc(sizeof(VarSpace));
                temp->isLive = false;
                HashMapPut(cur_node->inst->value_VarSpace,cur_node->inst->dest,temp);
            }
        }
        printf("for end\n");
    }

    //分析一个块分析到了头
    if(cur_node == head && cur_node != NULL){
        if(cur_node->inst->Parent->in == NULL){
            cur_node->inst->Parent->in = HashMapInit();
            for(Pair *pair = HashMapNext();pair != NULL;pair = HashMapNext(cur_node->inst->value_VarSpace)){
            }
        }
    }
}

/*将下一个instruction的in简单的赋值给上一个instruction的in之后再回到原函数进行分析*/
void copy_liveness(InstNode *this,InstNode *next){
    HashMapFirst(next->inst->value_VarSpace);
    for(Pair *pair = HashMapNext(next->inst->value_VarSpace); pair != NULL; pair = HashMapNext(next->inst->value_VarSpace)){
        VarSpace *temp = (VarSpace*)malloc(sizeof(VarSpace));
        VarSpace *pair_value = (VarSpace*)pair->value;
        temp->isLive = pair_value->isLive;
        temp->place = pair_value->place;
        temp->order = pair_value->place;
        Value *pair_key = (Value*)pair->key;
        HashMapPut(this->inst->value_VarSpace,pair_key,temp);
    }
}

void do_ll_analysis(InstNode *all_end){
    BasicBlock *this = all_end->inst->Parent;
    do{
        ll_analysis(this);
        if(this->tail_node->inst->Opcode == Goto || this->tail_node->inst->Opcode == IF_Goto){

        }
        this = get_prev_block(this);
    }while(this != NULL);
}

void printliveness(BasicBlock *this){
    InstNode *head = this->head_node;
    InstNode *tail = this->tail_node;
    for(;tail != NULL && tail->inst->i >= head->inst->i; tail = get_prev_inst(tail)){
        printf("instnode id : %d\n",tail->inst->i);
        HashMapFirst(tail->inst->value_VarSpace);
        for(Pair *value_varspace = HashMapNext(tail->inst->value_VarSpace); value_varspace != NULL; value_varspace = HashMapNext(tail->inst->value_VarSpace)){
            Value *value = (Value*)value_varspace->key;
            VarSpace *var_space = (VarSpace*)value_varspace->value;
            printf("    value : %s islive: %d\n",value->name,var_space->isLive);
        }
    }
}