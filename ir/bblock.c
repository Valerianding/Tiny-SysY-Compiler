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
    return NULL;
}

InstNode *search_ins_label(InstNode *head,int label_id){
    while(head != NULL){
        if(head->inst->Opcode == Label && head->inst->user.value.pdata->instruction_pdata.true_goto_location == label_id){
            return head;
        }
        head = get_next_inst(head);
    }
    return nullptr;
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

BlockNode *get_next_block(BlockNode *this){
    if(this->list.next == NULL) return nullptr;
    BlockNode *next = sc_list_entry(this->list.next,BlockNode,list);
    return next;
}

BasicBlock *blocklist_pop(BlockList list){
    if(list == nullptr){
        return nullptr;
    }else{
        struct sc_list *temp = &list->list;
        while(temp->next != nullptr){
            temp = temp->next;
        }
        BlockNode *tail = sc_list_entry(temp,BlockNode,list);
        return tail->block;
    }
}

HashSet *get_prev_block(BasicBlock *this){
    HashSet *prevBlocks = HashSetInit();
    if(this->prev_blocks == NULL) return NULL;
    struct sc_list *temp = &this->prev_blocks->list;
    while(temp != NULL){
        BlockNode *blockNode = sc_list_entry(temp,BlockNode,list);
        BasicBlock *block = blockNode->block;
        HashSetAdd(prevBlocks,block);
        temp = temp->next;
    }
    return prevBlocks;
}

void bb_add_prev(BasicBlock *prev,BasicBlock *pos){
    BlockNode *blockNode = (BlockNode*)malloc(sizeof(BlockNode));
    memset(blockNode,0,sizeof(BlockNode));
    sc_list_init(&blockNode->list);
    blockNode->block = prev;
    if(pos->prev_blocks == nullptr) {
        pos->prev_blocks = blockNode;
    }else{
        sc_list_add_tail(&pos->prev_blocks->list,&blockNode->list);
    }
}

void clear_visited_flag(InstNode *head) {
    InstNode *temp = head;
    while(temp != NULL){
        BasicBlock *parent = temp->inst->Parent;
        parent->visited = 0;
        temp = get_next_inst(temp);
    }
}

void blocklist_add(BlockList list,BasicBlock *block){
    BlockNode *blocknode = (BlockNode*)malloc(sizeof(BlockNode));
    memset(blocknode,0,sizeof(BlockNode));
    blocknode->block = block;
    if(list == nullptr){
        list = blocknode;
    }else{
        sc_list_add_tail(&list->list,&blocknode->list);
    }
}

void print_one_ins_info(InstNode *instruction_list){
    printf("%d : opcode:",instruction_list->inst->i);
    print_ins_opcode(instruction_list->inst);
    if(instruction_list->inst->Opcode == br_i1){
        printf("%d %d",instruction_list->inst->user.value.pdata->instruction_pdata.true_goto_location,instruction_list->inst->user.value.pdata->instruction_pdata.false_goto_location);
    }else if(instruction_list->inst->Opcode == br){
        printf("%d",instruction_list->inst->user.value.pdata->instruction_pdata.true_goto_location);
    }else if(instruction_list->inst->Opcode == Label){
        printf("%d",instruction_list->inst->user.value.pdata->instruction_pdata.true_goto_location);
    }else if(instruction_list->inst->Opcode == Alloca){
        if(instruction_list->inst->user.value.name != NULL){
            printf(" user.value.name : %s",instruction_list->inst->user.value.name);
        }
    }else if(instruction_list->inst->Opcode == Load){
        if(instruction_list->inst->user.value.name != NULL){
            printf(" name : %s",instruction_list->inst->user.value.name);
        }
    }else if(instruction_list->inst->Opcode == Store){
        if(instruction_list->inst->user.value.name != NULL){
            printf("name : %s",instruction_list->inst->user.value.name);
        }
        printf(" oprand name : %s",instruction_list->inst->user.use_list[1].Val->name);
    }
    if(instruction_list->inst->Parent != NULL){
        printf(" parent: b%d",instruction_list->inst->Parent->id);
    }else{
        printf(" parent:NULL");
    }
    if(instruction_list->inst->user.value.use_list == NULL){
        printf(" used by: NULL");
    }else{
        //TODO 打印user而不是use_list
        printf(" used by: ");
        Use *temp = instruction_list->inst->user.value.use_list;
        while(temp != NULL){
            User *parent = temp->Parent;
            Instruction *ins = (Instruction*)parent;
            printf("%d ",ins->i);
            temp = temp->Next;
        }
    }
    printf("\n");
}

BlockNode *get_next_prevblock(BlockNode *this){
    if(this->list.next == NULL) return NULL;
    struct sc_list *list= this->list.next;
    BlockNode *next = sc_list_entry(list,BlockNode,list);
    return next;
}

void ins_insert_after(InstNode *this,InstNode *pos){
    //printf("in ins_insert_after\n");
    struct sc_list* posList = &pos->list;
    struct sc_list* nextList = pos->list.next;
    posList->next = &this->list;
    this->list.prev = posList;

    this->list.next = nextList;
    if(nextList!=NULL)
        nextList->prev = &this->list;
    //printf("after ins_insert_after\n");
}

void print_block_info(BasicBlock *this){
    printf("block : b%d ",this->id);
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
            printf("%d : b%d ",i++,temp->block->id);
            temp = get_next_prevblock(temp);
        }
    }else{
        printf("NULL ");
    }
    if(this->true_block){
        printf("true_block : %d ",this->true_block->id);
    }
    if(this->false_block){
        printf("false_block : %d ",this->false_block->id);
    }
    if(this->Parent){
        printf("parent : %p",this->Parent);
    }else{
        printf("parent : NULL");
    }
    printf("\n");
}