//
// Created by ljf on 2023/7/26.
//
#include "live_interval.h"
PriorityQueue *pqueue;
HashMap *hashmap;
BasicBlock *block;
InstNode *ins_end;
InstNode *ins_head;
int CompareNumerics(const void* lhs, const void* rhs){
    value_live_range * x1=(value_live_range *)lhs;
    value_live_range * x2=(value_live_range *)rhs;
    if(x1->start==x2->start){
        return 0;
    }
    return x1->start>=x2->start ? 1 : (-1);

}

void init_PriorityQueue(PriorityQueue*queue){
    queue=PriorityQueueInit();
    queue->set_compare(queue,CompareNumerics);
}

void init_HashMap(HashMap*hashMap){
    hashMap=HashMapInit();
}

//每个function建立一张live_interval,会从vector数组中，取出每个block
PriorityQueue *build_live_interval(Vector* vector){
    init_PriorityQueue(pqueue);
    init_HashMap(hashmap);
    void *elem;
    VectorFirst(vector,false);
    while (VectorNext(vector,&elem)){
        block=(BasicBlock*)elem;
        analyze_block();
    }
    Pair *ptr_pair;
    HashMapFirst(hashmap);
    while ((ptr_pair= HashMapNext(hashmap))!=NULL){
        Value *key=(Value*)ptr_pair->key;
        live_range *range=(live_range*)ptr_pair->value;
        value_live_range *vlr=(value_live_range*) malloc(sizeof(value_live_range));
        vlr->value=key;
        vlr->start=range->start;
        vlr->end=range->end;
        PriorityQueuePush(pqueue,vlr);
    }
    return pqueue;
}

void analyze_block(){
    ins_end=block->tail_node;
    ins_head=block->head_node;
    InstNode *ins=ins_end;
    while (ins!=block->head_node){
        analyze_ins(ins);
        ins= get_prev_inst(ins);
    }
    if(ins==block->head_node){ //是该block的第一条ir,一般来说每个block的第一条语句会是一个label,label的话就没有必要给其分配寄存器了

    }
}

bool value_is_in_liveout(Value*tvalue){
    if(HashSetFind(block->out,tvalue)==true){
        return true;
    }else{
        return false;
    }
}

void handle_def(Value*dvalue,int ins_id){
    int flag=0;
    if(value_is_in_liveout(dvalue)){
        live_range *range= HashMapGet(hashmap,dvalue);
        if(range==NULL){
            flag=1;
            range=(live_range*) malloc(sizeof(live_range));
            range->start=ins_head->inst->i;
            range->end=ins_end->inst->i;
        }
        range->start=ins_id;
        if(flag==1){
            HashMapPut(hashmap,dvalue,range);
        }
    }
}

void handle_use(Value*uvalue,int ins_id){
    int flag=0;
    if(value_is_in_liveout(uvalue)){
        live_range *range= HashMapGet(hashmap,uvalue);
        if(range==NULL){
            flag=1;
            range=(live_range*) malloc(sizeof(live_range));
            range->start=ins_head->inst->i;
            range->end=ins_end->inst->i;
        }else{
            range->start=ins_head->inst->i;
        }
        if(flag==1){
            HashMapPut(hashmap,uvalue,range);
        }
    }
}

void analyze_ins(InstNode *ins){
    Value *value0,*value1,*value2;
    switch (ins->inst->Opcode) {
        case Add:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2=user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
            break;
        case Sub:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2=user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
            break;
        case Mul:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2=user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
            break;
        case Div:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2=user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
            break;
        case Mod:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
            break;
        case Call:
            if(!returnValueNotUsed(ins)){
                value0=&ins->inst->user.value;
                handle_def(value0,ins->inst->i);
            }
            break;
        case FunBegin:
            break;
        case Return:
            value1= user_get_operand_use(&ins->inst->user,0)->Val;
            handle_use(value1,ins->inst->i);
            break;
        case Store:
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
            break;
        case Load:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            break;
        case Alloca:
            value0 = &ins->inst->user.value;
            break;
        case GIVE_PARAM:
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_use(value1,ins->inst->i);
            break;
        case ALLBEGIN:
            break;
        case LESS:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
//            value0可能不需要分配
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_def(value2,ins->inst->i);
            break;
        case GREAT:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_def(value2,ins->inst->i);
            break;
        case LESSEQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_def(value2,ins->inst->i);
            break;
        case GREATEQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_def(value2,ins->inst->i);
            break;
        case EQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_def(value2,ins->inst->i);
            break;
        case NOTEQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_def(value2,ins->inst->i);
            break;
        case br_i1:
            break;
        case br:
            break;
        case Label:
            break;
        case XOR:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            break;
        case zext:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            break;
        case bitcast:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            break;
        case GEP:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_def(value2,ins->inst->i);
            break;
        case MEMCPY:
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            break;
        case MEMSET:
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            break;
        case zeroinitializer:
            break;
        case GLOBAL_VAR:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            break;
        case FunEnd:
            break;
        case CopyOperation:
            value0=ins->inst->user.value.alias;//这里是需要进到alias里面的
            value1= user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            break;
        case fptosi:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            break;
        case sitofp:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            break;
        case SysYMemset:
            break;
        default:
            break;
    }
}

void print_live_interval(){
    void *elem;
    value_live_range *cur;
    while (PriorityQueueSize(pqueue)!=0){
        PriorityQueueTop(pqueue,&elem);
        PriorityQueuePop(pqueue);
        cur=(value_live_range*)elem;
        assert(cur->value->name!=NULL);
        printf("%s\t\t:",cur->value->name);
        for (int i=0;i<cur->start;i++){
            printf(" ");
        }
        printf("|");
        for(int i=cur->start;i!=cur->end;i++){
            printf("-");
        }
        printf("\n");
    }
}