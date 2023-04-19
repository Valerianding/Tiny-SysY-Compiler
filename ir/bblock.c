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

// 将instruction放在instnode里面
InstNode* new_inst_node(Instruction* inst){
    InstNode* n = malloc(sizeof(InstNode));
    assert(n != NULL && " out of memory");
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

HashSet *get_prev_block(BasicBlock *this){
    HashSet *prevBlocks = this->preBlocks;
    return prevBlocks;
}

void bb_add_prev(BasicBlock *prev,BasicBlock *pos){
    HashSet *posPrevBlocks = pos->preBlocks;
    HashSetAdd(posPrevBlocks,prev);
}

void print_one_ins_info(InstNode *insNode){
    printf("%d : opcode:", insNode->inst->i);
    print_ins_opcode(insNode->inst);
    if(insNode->inst->Opcode == Br_i1){
        printf("%d %d", insNode->inst->user.value.pdata->instruction_pdata.true_goto_location, insNode->inst->user.value.pdata->instruction_pdata.false_goto_location);
    }else if(insNode->inst->Opcode == Br){
        printf("%d", insNode->inst->user.value.pdata->instruction_pdata.true_goto_location);
    }else if(insNode->inst->Opcode == Label){
        printf("%d", insNode->inst->user.value.pdata->instruction_pdata.true_goto_location);
    }else if(insNode->inst->Opcode == Alloca){
        if(insNode->inst->user.value.name != NULL){
            printf(" user.value.name : %s", insNode->inst->user.value.name);
        }
        printf("ins type : ");
        Value *insValue = ins_get_dest(insNode->inst);
        typePrinter(insValue->VTy);
    }else if(insNode->inst->Opcode == Load){
        if(insNode->inst->user.value.name != NULL){
            printf(" name : %s", insNode->inst->user.value.name);
        }
    }else if(insNode->inst->Opcode == Store){
        if(insNode->inst->user.value.name != NULL){
            printf("name : %s", insNode->inst->user.value.name);
        }
        printf(" oprand name : %s", insNode->inst->user.use_list[1].Val->name);
    }

    Value *lhs = NULL;
    Value *rhs = NULL;
    if(insNode->inst->user.value.NumUserOperands == 1){
        lhs = ins_get_lhs(insNode->inst);
    }
    if(insNode->inst->user.value.NumUserOperands == 2){
        lhs = ins_get_lhs(insNode->inst);
        rhs = ins_get_rhs(insNode->inst);
    }
    printf(" number of operands : %d ", insNode->inst->user.value.NumUserOperands);


    if(insNode->inst->Parent != NULL){
        printf(" parent: b%d", insNode->inst->Parent->id);
    }else{
        printf(" parent:NULL");
    }
    if(insNode->inst->user.value.use_list == NULL){
        printf(" used by: NULL");
    }else{
        printf(" used by: ");
        Use *temp = insNode->inst->user.value.use_list;
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
    else if(prev != NULL && next == NULL){
        prev->list.next = NULL;
    }
    // 要么就都不为空
    else{
        next->list.prev = &(prev->list);
        prev->list.next = &(next->list);
    }
    //free(this);
}

void print_block_info(BasicBlock *this){
    InstNode *currNode = this->head_node;
    while(currNode != NULL) {
        printf("currNode id is %d\n",currNode->inst->i);
        BasicBlock *block = currNode->inst->Parent;
        if(block->visited == false){
            block->visited = true;
            printf("block : b%d ",block->id);
            //打印后继
            if(block->head_node){
                printf("entry : id : %d ",block->head_node->inst->i);
            }
            if(block->tail_node){
                printf("tail : id : %d ",block->tail_node->inst->i);
            }
            //打印前驱
            printf("prev: ");
            if(HashSetSize(block->preBlocks) != 0){
                HashSetFirst(block->preBlocks);
                for(BasicBlock *prevBlock = HashSetNext(block->preBlocks); prevBlock != NULL; prevBlock = HashSetNext(block->preBlocks)){
                    printf(" b%d", prevBlock->id);
                }
            }else{
                printf("NULL ");
            }
            if(block->true_block){
                printf("true_block : %d ",block->true_block->id);
            }
            if(block->false_block){
                printf("false_block : %d ",block->false_block->id);
            }
            if(block->Parent){
                printf("parent : %p",block->Parent);
            }else{
                printf("parent : NULL");
            }
            printf("\n");
        }
        currNode = get_next_inst(currNode);
    }
}