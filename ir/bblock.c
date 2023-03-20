#include "bblock.h"
#include "stdio.h"
BasicBlock *bb_create(){
    BasicBlock *this = (BasicBlock*)malloc(sizeof(BasicBlock));
    memset(this,0,sizeof(BasicBlock));
    this->preBlocks = HashSetInit();
    this->in = HashSetInit();
    this->out = HashSetInit();
    return this;
}

//// 将instruction放在instnode里面
InstNode* new_inst_node(Instruction* inst){
    InstNode* n = malloc(sizeof(InstNode));
    n->inst = inst;
    // 初始化结点
    sc_list_init(&(n->list));
    return n;
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


InstNode *get_last_inst(InstNode *this){
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
    // 需要满足在一个function里面
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


HashSet *get_prev_block(BasicBlock *this){
    HashSet *prevBlocks = this->preBlocks;
    return prevBlocks;
}

void bb_add_prev(BasicBlock *prev,BasicBlock *pos){
    HashSet *posPrevBlocks = pos->preBlocks;
    printf("bb_add_prev!\n");
    HashSetAdd(posPrevBlocks,prev);
    printf("after bb_add_prev\n");
}

void clear_visited_flag(InstNode *head) {
    InstNode *temp = head;
    while(temp != NULL){
        BasicBlock *parent = temp->inst->Parent;
        parent->visited = 0;
        temp = get_next_inst(temp);
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
        printf(" alloca type : ");
        Value *lhs = ins_get_lhs(instruction_list->inst);
        if(lhs != NULL && isArray(lhs)){
            printf("array type");
        }else if(lhs != NULL && isVar(lhs)){
            printf("var type");
        }else if(lhs != NULL){
            printf("not any type");
        }else{
            printf("null!");
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
    }else if(instruction_list->inst->Opcode == Call){

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

void ins_insert_before(InstNode *this, InstNode *pos){
    //获得前一个节点
    InstNode *prevNode = get_prev_inst(pos);
    if(prevNode != nullptr){
        this->list.prev = &prevNode->list;
        prevNode->list.next = &this->list;
    }
    this->list.next = &pos->list;
    pos->list.prev = &this->list;
}

InstNode *ins_get_funcHead(InstNode *this){
    if(this->inst->Opcode == FunBegin) return this;
    InstNode *prev = get_prev_inst(this);
    while(prev != nullptr){
        if(prev->inst->Opcode == FunBegin) return prev;
        prev = get_prev_inst(prev);
    }
    return nullptr;
}

void delete_inst(InstNode *this){
    InstNode *prev = get_prev_inst(this);
    InstNode *next = get_next_inst(this);
    if(prev == NULL && next != NULL){
        next->list.prev = NULL;
    }
    if(prev != NULL && next == NULL){
        prev->list.next = NULL;
    }
    // 要么就都不为空
    next->list.prev = &(prev->list);
    prev->list.next = &(next->list);
    free(this);
}

void print_block_info(BasicBlock *this){
    printf("block : b%d ",this->id);
    //打印后继
    if(this->head_node){
        printf("entry : id : %d ",this->head_node->inst->i);
    }
    if(this->tail_node){
        printf("tail : id : %d ",this->tail_node->inst->i);
    }
    //打印前驱
    printf("prev: ");
    if(HashSetSize(this->preBlocks) != 0){
        HashSetFirst(this->preBlocks);
        for(BasicBlock *prevBlock = HashSetNext(this->preBlocks); prevBlock != NULL; prevBlock = HashSetNext(this->preBlocks)){
            printf(" b%d", prevBlock->id);
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