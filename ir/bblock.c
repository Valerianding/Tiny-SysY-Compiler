#include "bblock.h"
#include "stdio.h"
BasicBlock *bb_create(){
    BasicBlock *this = (BasicBlock*)malloc(sizeof(BasicBlock));
    memset(this,0,sizeof(BasicBlock));
    return this;
}

void bblock_init(BasicBlock* this) {
    memset(this,0,sizeof(BasicBlock));
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

InstNode *search_ins_id(InstNode *head,int id){
    while(head != NULL){
        if(head->inst->i == id){
            return head;
        }
        head = get_next_inst(head);
    }
}

InstNode *search_ins_label(InstNode *head,int label_id){
    while(head != NULL){
        if(head->inst->Opcode == Label && head->inst->user.value.pdata->instruction_pdata.true_goto_location == label_id){
            return head;
        }
        head = get_next_inst(head);
    }
}

void bb_set_block(BasicBlock *this,InstNode *head,InstNode *tail){
    this->head_node = head;
    this->tail_node = tail;
    for(;head != tail;head = get_next_inst(head)){
        ins_set_parent(head->inst,this);
    }
    ins_set_parent(tail->inst,this);
}

void ins_node_add(InstNode *head,InstNode *this){
    sc_list_add_tail(&head->list,&this->list);
}

BasicBlock *get_next_block(BasicBlock *this){
    return this->true_block;
}

BlockList get_prev_block(BasicBlock *this){
    return this->prev_blocks;
}

void bb_add_prev(BasicBlock *prev,BasicBlock *pos){
    BlockNode *blockNode = (BlockNode*)malloc(sizeof(BlockNode));
    sc_list_init(&blockNode->list);
    blockNode->block = prev;

    if(pos->prev_blocks == nullptr) {
        pos->prev_blocks = blockNode;
    }else{
        sc_list_add_tail(&pos->prev_blocks->list,&blockNode->list);
    }
}