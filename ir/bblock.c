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
    memset(blockNode,0,sizeof(BlockNode));
    sc_list_init(&blockNode->list);
    blockNode->block = prev;
    if(pos->prev_blocks == nullptr) {
        pos->prev_blocks = blockNode;
    }else{
        printf("prev list is not empty!\n");
        printf("what : %p\n",prev);
        printf("blocknode : %p\n",blockNode);
        sc_list_add_tail(&pos->prev_blocks->list,&blockNode->list);
    }
}

void print_all_info(InstNode *instruction_list){
    printf("opcode:");
    print_ins_opcode(instruction_list->inst);
    if(instruction_list->inst->Opcode == br_i1){
        printf("%d %d",instruction_list->inst->user.value.pdata->instruction_pdata.true_goto_location,instruction_list->inst->user.value.pdata->instruction_pdata.false_goto_location);
    }else if(instruction_list->inst->Opcode == br){
        printf("%d",instruction_list->inst->user.value.pdata->instruction_pdata.true_goto_location);
    }else if(instruction_list->inst->Opcode == Label){
        printf("%d",instruction_list->inst->user.value.pdata->instruction_pdata.true_goto_location);
    }
    if(instruction_list->inst->Parent != NULL){
        printf(" parent:%p",instruction_list->inst->Parent);
    }else{
        printf(" parent:NULL");
    }
    printf("\n");
}

BlockNode *get_next_prevblock(BlockNode *this){
    if(this->list.next == NULL) return NULL;
    struct sc_list *list= this->list.next;
    BlockNode *next = sc_list_entry(list,BlockNode,list);
    return next;
}

void print_block_info(BasicBlock *this){
    printf("block : %p ",this);
    //打印后继
    if(this->head_node){
        printf("head : id : %d ",this->head_node->inst->i);
    }
    if(this->tail_node){
        printf("tail : id : %d ",this->tail_node->inst->i);
    }
    //打印前驱
    printf("prev: ");
    if(this->prev_blocks){
        BlockNode *temp = this->prev_blocks;
        int i = 0;
        while(temp != NULL){
            printf("%d : %p ",i++,temp->block);
            temp = get_next_prevblock(temp);
        }
    }else{
        printf("NULL ");
    }
    if(this->true_block){
        printf("true_block : %p ",this->true_block);
    }
    if(this->false_block){
        printf("false_block : %p ",this->false_block);
    }
    if(this->Parent){
        printf("parent : %p",this->Parent);
    }else{
        printf("parent : NULL");
    }
    printf("\n");
}