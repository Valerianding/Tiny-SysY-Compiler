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
    pqueue=PriorityQueueInit();
    pqueue->set_compare(pqueue,CompareNumerics);
    hashmap=HashMapInit();
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
//        printf("curr at %d opcode %d\n",ins->inst->i,ins->inst->Opcode);
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
//    printf("handle_def %d\n",ins_id);
    assert(dvalue!=NULL);
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
//    printf("handle_use %d\n",ins_id);
    assert(uvalue!=NULL);
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
//            printf("add\n");
            break;
        case Sub:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2=user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("sub\n");
            break;
        case Mul:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2=user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("mul\n");
            break;
        case Div:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2=user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("div\n");
            break;
        case Mod:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("mod\n");
            break;
        case Call:
//            printf("call %d\n",ins->inst->i);
            if(!returnValueNotUsed(ins)){
                value0=&ins->inst->user.value;
                handle_def(value0,ins->inst->i);
            }
//            printf("call\n");
            break;
//        case FunBegin:
//            break;
        case Return:
            value1= user_get_operand_use(&ins->inst->user,0)->Val;
            handle_use(value1,ins->inst->i);
//            printf("return\n");
            break;
        case Store:
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("store\n");
            break;
        case Load:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
//            printf("load\n");
            break;
//        case Alloca:
//            value0 = &ins->inst->user.value;
//            break;
        case GIVE_PARAM:
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_use(value1,ins->inst->i);
//            printf("give_param\n");
            break;
//        case ALLBEGIN:
//            break;
        case LESS:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
//            value0可能不需要分配
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("less\n");
            break;
        case GREAT:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("great\n");
            break;
        case LESSEQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("lesseq\n");
            break;
        case GREATEQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("greateq\n");
            break;
        case EQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("eq\n");
            break;
        case NOTEQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("noteq\n");
            break;
//        case br_i1:
//            break;
//        case br:
//            break;
//        case Label:
//            break;
        case XOR:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
//            printf("xor\n");
            break;
//        case zext:
//            value0=&ins->inst->user.value;
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            break;
//        case bitcast:
//            value0=&ins->inst->user.value;
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            break;
        case GEP:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
            handle_use(value2,ins->inst->i);
//            printf("gep\n");
            break;
//        case MEMCPY:
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            value2= user_get_operand_use(&ins->inst->user,1)->Val;
//            break;
//        case MEMSET:
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            value2= user_get_operand_use(&ins->inst->user,1)->Val;
//            break;
//        case zeroinitializer:
//            break;
//        case GLOBAL_VAR:
//            value0=&ins->inst->user.value;
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            value2= user_get_operand_use(&ins->inst->user,1)->Val;
//            break;
//        case FunEnd:
//            break;
        case CopyOperation:
            value0=ins->inst->user.value.alias;//这里是需要进到alias里面的
            value1= user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
//            printf("copy\n");
            break;
        case fptosi:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
//            printf("fptosi\n");
            break;
        case sitofp:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def(value0,ins->inst->i);
            handle_use(value1,ins->inst->i);
//            printf("sitofp\n");
            break;
//        case SysYMemset:
//            break;
        default:
            break;
    }
}

void print_live_interval(){
    void *elem;
    value_live_range *cur;
    printf("/********************live interval*************************/\n");
    while (PriorityQueueSize(pqueue)!=0){
        PriorityQueueTop(pqueue,&elem);
        PriorityQueuePop(pqueue);
//        printf("%d\n",PriorityQueueSize(pqueue));
        cur=(value_live_range*)elem;
        printf("%s start %d    end %d\n",cur->value->name,cur->start,cur->end);
//        assert(cur->value->name!=NULL);
//        printf("%s\t\t:",cur->value->name);
//        for (int i=0;i<cur->start;i++){
//            printf(" ");
//        }
//        printf("|");
//        for(int i=cur->start;i!=cur->end;i++){
//            printf("-");
//        }
    }
    printf("/********************live interval*************************/\n\n");
}