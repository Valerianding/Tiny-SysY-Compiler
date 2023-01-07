#include "bblock.h"
#include "stdio.h"
void bblock_init(BasicBlock* this, Function* func) {
    //对头上的两个后继节点另外开辟use
    uint8_t *storage = (uint8_t *) malloc(sizeof(Use) * 2);
    Use *temp1 = (Use *) storage;
    temp1->Parent = &this->user;

    Use *temp2 = (Use *) (storage) + 1;
    temp2->Parent = &this->user;
    this->user.use_list = (Use *) storage;
    value_init(&this->user.value);
    //this->Parent = func;

    this->head_node = NULL;
    this->tail_node = NULL;
}


//// 将instruction放在instnode里面
InstNode* new_inst_node(Instruction* inst){
    InstNode* n = malloc(sizeof(InstNode));
    n->inst = inst;
    // 初始化结点
    sc_list_init(&(n->list));
    return n;
}

InstNode* bblock_get_inst_back(BasicBlock* this){
    return this->tail_node;
}

InstNode* bblock_get_inst_head(BasicBlock* this){
    return this->head_node;
}

Function *bblock_get_parent(BasicBlock* this){
    return this->Parent;
}

void moveBefore(BasicBlock *this,BasicBlock *MovePos){
//    Use *temp = this->user.use_list;
//    if(temp->Val == NULL){
//        use_set_value(temp,&MovePos->user.value);
//    }else{
//        temp = temp + 1;
//        if(temp->Val == NULL) {
//            use_set_value(temp,&MovePos->user.value);
//        }else{
//            printf("already have two kids!\n");
//        }
//    }
}

void moveAfter(BasicBlock *this,BasicBlock *MovePos){
//    Use *temp = MovePos->user.use_list;
//    if(temp->Val == NULL){
//        use_set_value(temp,&this->user.value);
//    }else{
//        temp = temp + 1;
//        if(temp->Val == NULL){
//            use_set_value(temp,&this->user.value);
//        }else{
//            printf("already have two kids!\n");
//        }
//    }
//    if(this->next == )
}

InstNode *get_prev_inst(InstNode *this){
    struct sc_list *list = this->list.prev;
    if(list == NULL) return NULL;
    InstNode *temp = sc_list_entry(list,InstNode,list);
    return temp;
}

InstNode *get_next_inst(InstNode *this){
    struct sc_list *list = this->list.next;
    if(list == NULL) return NULL;
    InstNode *temp = sc_list_entry(list,InstNode,list);
    return temp;
}

//lsy
InstNode *get_last_inst(InstNode *this)
{
    struct sc_list *list=&this->list;
    while(list->next!=NULL)
        list=list->next;
    InstNode *temp = sc_list_entry(list,InstNode,list);
    return temp;
}

InstNode *search_inst_node(InstNode *head,int id){
    while(head != NULL){
        if(head->inst->i == id){
            return head;
        }
        else{
            //printf("now id %d\n",head->inst->i);
            head = get_next_inst(head);
        }
    }
}

//1.ALLBegin跳过
//2.开始语句是 FuncBegin 或者 Label
//3.结束语句就是br br_i1 ret
void bblock_divide(InstNode *head){
    InstNode *cur = head;
    cur = get_next_inst(cur); //跳过第一个ALLBegin
    //第一次全部打点
    //printf("first while\n");
    while(cur != NULL){
        if(cur->inst->Opcode == FunBegin || cur->inst->Opcode == Label){
            InstNode *cur_prev = get_prev_inst(cur);
            if(cur_prev != NULL){
                cur_prev->inst->user.value.is_out = true;
            }
            cur->inst->user.value.is_in = true;
        }
        if(cur->inst->Opcode == Return || cur->inst->Opcode == br || cur->inst->Opcode == br_i1){
            cur->inst->user.value.is_out = true;
            InstNode *cur_next = get_next_inst(cur);
            if(cur_next != NULL){
                cur_next->inst->user.value.is_in = true;
            }
        }
        cur = get_next_inst(cur);
    }
    cur = head;
    cur = get_next_inst(cur);
    InstNode *this_in;
    //printf("second while\n");
    //第二次直接套壳
    while(cur != NULL){
        if(cur->inst->user.value.is_in == true){
            this_in = cur;
        }
        if(cur->inst->user.value.is_out == true){
            BasicBlock *this = (BasicBlock*)malloc(sizeof(BasicBlock));
            this->head_node = this_in;
            this->tail_node = cur;
            bblock_init(this,NULL);
            bb_set_block(this,this_in,cur);
        }
        cur = get_next_inst(cur);
    }
}

void bb_set_block(BasicBlock *this,InstNode *head,InstNode *tail){
    for(;head != tail;head = get_next_inst(head)){
        ins_set_parent(head->inst,this);
    }
    ins_set_parent(tail->inst,this);
}

void ins_node_add(InstNode *head,InstNode *this){
    sc_list_add_tail(&head->list,&this->list);
}

BasicBlock *get_next_block(BasicBlock *this){

}

BasicBlock *get_prev_block(BasicBlock *this){

}