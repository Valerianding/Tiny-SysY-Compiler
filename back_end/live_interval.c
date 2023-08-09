//
// Created by ljf on 2023/7/26.
//
#include "live_interval.h"


int vfpFlag=0; //启用浮点寄存器分配

HashMap *hashmap;
BasicBlock *block;
InstNode *ins_end;
InstNode *ins_head;

int vfp_flag;//计算Var_Float的活跃变量表时需要置1，否则置0

//每个function建立一张live_interval,会从vector数组中，取出每个block
PriorityQueue *build_live_interval(Vector* vector,PriorityQueue*pqueue){
    vfp_flag=0;
    assert(vector!=NULL);
    assert(pqueue!=NULL);
    hashmap=HashMapInit();
    void *elem;
    VectorFirst(vector,false);
    while (VectorNext(vector,&elem)){
        block=(BasicBlock*)elem;
        analyze_block();
    }
    Pair *ptr_pair;
    HashMapFirst(hashmap);
    int k=0;
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
PriorityQueue *build_vfp_live_interval(Vector* vector,PriorityQueue*pqueue){
    vfp_flag=1;
    assert(vector!=NULL);
    assert(pqueue!=NULL);
    hashmap=HashMapInit();
    void *elem;
    VectorFirst(vector,false);
    while (VectorNext(vector,&elem)){
        block=(BasicBlock*)elem;
        analyze_block();
    }
    Pair *ptr_pair;
    HashMapFirst(hashmap);
    int k=0;
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


//void init_PriorityQueue(PriorityQueue*queue){
//    queue=PriorityQueueInit();
//    queue->set_compare(queue,CompareNumerics);
//}
//
//void init_HashMap(HashMap*hashMap){
//    hashMap=HashMapInit();
//}

void analyze_block(){
    ins_end=block->tail_node;
    ins_head=block->head_node;
    InstNode *ins=ins_end;

//    printf("block%d:\n",block->id);
//    变量in基本块的LiveOut集合中
    void *elem;
    Value *v;
    HashSetFirst(block->out);
    while ((elem= HashSetNext(block->out))!=NULL){
        v=(Value*)elem;
        live_range *range= HashMapGet(hashmap,v);
        if(vfp_flag==1 && vfpFlag==1){ //只计算浮点数
            if(isLocalVarFloatType(v->VTy)){
                if(range==NULL){ //添加新range
                    range=(live_range*) malloc(sizeof(live_range));
                    memset(range,0, sizeof(live_range));
                    range->start=ins_head->inst->i;
                    range->end=ins_end->inst->i;
                    HashMapPut(hashmap,(Value*) elem, range);
//            printf("new %s %d %d\n",v->name,range->start,range->end);
                }else{ //延长range
                    range->start=MIN(range->start,ins_head->inst->i);
                    range->end=MAX(range->end,ins_end->inst->i);
//            printf("extend %s %d %d\n",v->name,range->start,range->end);
                }
            }
        } else{
            if(vfpFlag==0){ //都算
                if(range==NULL){ //添加新range
                    range=(live_range*) malloc(sizeof(live_range));
                    memset(range,0, sizeof(live_range));
                    range->start=ins_head->inst->i;
                    range->end=ins_end->inst->i;
                    HashMapPut(hashmap,(Value*) elem, range);
//            printf("new %s %d %d\n",v->name,range->start,range->end);
                }else{ //延长range
                    range->start=MIN(range->start,ins_head->inst->i);
                    range->end=MAX(range->end,ins_end->inst->i);
//            printf("extend %s %d %d\n",v->name,range->start,range->end);
                }
            } else if(vfpFlag==1 && vfp_flag==0 ){ //只计算非浮点数
                if(!isLocalVarFloatType(v->VTy)){
                    if(range==NULL){ //添加新range
                        range=(live_range*) malloc(sizeof(live_range));
                        memset(range,0, sizeof(live_range));
                        range->start=ins_head->inst->i;
                        range->end=ins_end->inst->i;
                        HashMapPut(hashmap,(Value*) elem, range);
//            printf("new %s %d %d\n",v->name,range->start,range->end);
                    }else{ //延长range
                        range->start=MIN(range->start,ins_head->inst->i);
                        range->end=MAX(range->end,ins_end->inst->i);
//            printf("extend %s %d %d\n",v->name,range->start,range->end);
                    }
                }
            }

        }



    }
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
bool value_is_in_livein(Value*tvalue){
    if(HashSetFind(block->in,tvalue)==true){
        return true;
    }else{
        return false;
    }
}
void handle_def(Value*dvalue,int ins_id){
//    printf("handle_def %d\n",ins_id);
    assert(dvalue!=NULL);
    if(isImmIntType(dvalue->VTy) || isImmFloatType(dvalue->VTy)){
        return;
    }
    if(isGlobalArrayFloatType(dvalue->VTy)|| isGlobalArrayIntType(dvalue->VTy) || isGlobalVarFloatType(dvalue->VTy) ||
       isGlobalVarIntType(dvalue->VTy)){
        return;
    }
    if(isLocalArrayFloatType(dvalue->VTy) || isLocalArrayIntType(dvalue->VTy)){
        return;
    }
    if(vfpFlag==1 && vfp_flag==1){ //只计算浮点数，非浮点数直接返回
        if(!isLocalVarFloatType(dvalue->VTy)){
            return;
        }
    }
    if(vfpFlag==1 && vfp_flag==0){ //只计算非浮点数，浮点数直接返回
        if(isLocalVarFloatType(dvalue->VTy)){
            return;
        }
    }
//    vfpFlag==0,计算所有变量
    live_range *range= HashMapGet(hashmap,dvalue);
    if(range!=NULL){
        if(range->start==ins_head->inst->i){
            range->start=ins_id;
            range->firstisdef=1;
            range->lastisuse=0;
        }
    }
}

void handle_use(Value*uvalue,int ins_id){
//    printf("handle_use %d\n",ins_id);
    assert(uvalue!=NULL);
    if(isImmIntType(uvalue->VTy) || isImmFloatType(uvalue->VTy)){
        return;
    }
    if(isGlobalArrayFloatType(uvalue->VTy)|| isGlobalArrayIntType(uvalue->VTy) || isGlobalVarFloatType(uvalue->VTy) ||
            isGlobalVarIntType(uvalue->VTy)){
        return;
    }
    if(isLocalArrayFloatType(uvalue->VTy) || isLocalArrayIntType(uvalue->VTy)){
        return;
    }
    if(vfpFlag==1 && vfp_flag==1){ //只计算浮点数，非浮点数直接返回
        if(!isLocalVarFloatType(uvalue->VTy)){
            return;
        }
    }
    if(vfpFlag==1 && vfp_flag==0){ //只计算非浮点数，浮点数直接返回
        if(isLocalVarFloatType(uvalue->VTy)){
            return;
        }
    }
//    vfpFlag==0,计算所有变量
    live_range *range= HashMapGet(hashmap,uvalue);
    if(range==NULL){
        range=(live_range*) malloc(sizeof(live_range));
        memset(range,0,sizeof(live_range));
        range->start=ins_head->inst->i;
        range->firstisdef=0;
        range->end=ins_id;
        range->lastisuse=1;
        HashMapPut(hashmap,uvalue,range);
    }else{
        if(range->start>ins_head->inst->i)  range->firstisdef=0;
        range->start=MIN(range->start,ins_head->inst->i);
        if(range->end<ins_id) range->lastisuse=1;
        range->end=MAX(range->end,ins_id);
    }
}

void analyze_ins(InstNode *ins){
    int opNum;
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
//                printf("%s call def\n",value0->name);
                handle_def(value0,ins->inst->i);
            }
//            printf("call\n");
            break;
//        case FunBegin:
//            break;
        case Return:
            opNum=ins->inst->user.value.NumUserOperands;
            if(opNum!=0){
                value1= user_get_operand_use(&ins->inst->user,0)->Val;
                handle_use(value1,ins->inst->i);
            }
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
        case br_i1:
//            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val; //真值
            handle_use(value1,ins->inst->i);
            break;
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

void print_live_interval(PriorityQueue*pqueue){
    HashSet *tmp;
    tmp=HashSetInit();
    void *elem;
    value_live_range *cur;
    printf("/********************live interval*************************/\n");
    while (PriorityQueueSize(pqueue)!=0){
        PriorityQueueTop(pqueue,&elem);
        PriorityQueuePop(pqueue);
//        printf("%d\n",PriorityQueueSize(pqueue));
        cur=(value_live_range*)elem;
        printf("%s start %d    end %d\n",cur->value->name,cur->start,cur->end);
        HashSetAdd(tmp,cur);
    }
    printf("/********************live interval*************************/\n\n");

    HashSetFirst(tmp);
    while ((elem= HashSetNext(tmp))!=NULL){
        PriorityQueuePush(pqueue,elem);
    }
}