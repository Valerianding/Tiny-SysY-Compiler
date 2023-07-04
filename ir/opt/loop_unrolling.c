#include "loop_unrolling.h"
#include "travel.h"

/**
 * 目前只考虑init、modifier、end均为常数
 * 不循环嵌套
 * 基本块内
 */
 //TODO 不能加新基本块的话，基本块内也没得做，余数问题无法解决

#define update_modifier 4
int loop_unroll_up_lines = 3000;

//复制一个信息都相同的value副本
Value *copy_value(Value *v_source,int index)
{
    Value *v_new=(Value*) malloc(sizeof (Value));
    value_init(v_new);
    v_new->pdata=v_source->pdata;
    v_new->VTy->ID=v_source->VTy->ID;
    v_new->HasHungOffUses=v_source->HasHungOffUses;
    v_new->IsPhi=v_source->IsPhi;
    v_new->Useless=v_source->Useless;
    if(v_source->VTy->ID==Int || v_source->VTy->ID==Float)
        return v_new;
    //一个新name
    return ins_get_new_value(v_new,index);
}

//检查init、modifier、end是否都是int常数
bool check_idc(Value* initValue,Value* step,Value* end)
{
    if(initValue->VTy->ID==Int && end->VTy->ID==Int && step->VTy->ID==Int)
        return true;
    return false;
}

//粗略计算迭代次数
int cal_times(int init,Instruction *ins_modifier,Instruction *ins_end_cond,Value* v_step,Value* v_end)
{
    int times=0;
    int step=v_step->pdata->var_pdata.iVal;
    int end=v_end->pdata->var_pdata.iVal;
    double val;bool tag;
    switch (ins_modifier->Opcode) {
        case Add:
            switch (ins_end_cond->Opcode) {
                case EQ: times = (init == end) ? 1 : 0; break;
                case NOTEQ: times = ((end - init) % step) == 0 ? (end - init / step) : -1; break;
                case LESSEQ: case GREATEQ:
                    times = (end - init) / step + 1; break;
                case LESS: case GREAT:
                    times = ((end - init) % step == 0)? (end - init) / step : (end - init) / step + 1; break;
            }
            break;
        case Sub:
            switch (ins_end_cond->Opcode) {
                case EQ: times = (init == end) ? 1 : 0; break;
                case NOTEQ: times = ((init - end) % step) == 0 ? (init - end / step) : -1; break;
                case LESSEQ: case GREATEQ:
                    times = (init - end) / step + 1; break;
                case LESS: case GREAT:
                    times = ((init - end) % step == 0)? (init - end) / step : (init - end) / step + 1; break;
            }
            break;
        case Mul:
            val = log((double)end / (double)init) / log((double)step);
            tag = init * pow(step, val) == end;
            switch (ins_end_cond->Opcode) {
                case EQ: times = (init == end) ? 1 : 0; break;
                case NOTEQ: times = tag ? (int)val : -1; break;
                case LESSEQ: case GREATEQ:
                    times = (int)val + 1; break;
                case LESS: case GREAT:
                    times = tag ? (int) val : (int) val + 1; break;
            }
            break;
        case Div:
            val = log((double)init / (double)end) / log((double)step);
            tag = end * pow(step, val) == init;
            switch (ins_end_cond->Opcode) {
                case EQ: times = (init == end) ? 1 : 0; break;
                case NOTEQ: times = tag ? (int)val : -1; break;
                case LESSEQ: case GREATEQ:
                    times = (int)val + 1; break;
                case LESS: case GREAT:
                    times = tag ? (int) val : (int) val + 1; break;
            }
            break;
    }
    return times;
}

//计算一次迭代的ir总数
int cal_ir_cnt(HashSet *loopBody)
{
    int ir_cnt=0;
    for(BasicBlock *body = HashSetNext(loopBody); body != NULL; body = HashSetNext(loopBody)){
        InstNode *currNode = body->head_node;
        InstNode *bodyTail = body->tail_node;
        while(currNode != bodyTail){
            if(!hasNoDestOperator(currNode)){
                ir_cnt++;
            }
            currNode = get_next_inst(currNode);
        }
    }
    return ir_cnt;
}


void update_replace_value(HashMap* map,Value* v_before,Value* v_replace)
{
    HashMapFirst(map);

    for(Pair *p = HashMapNext(map); p != NULL; p = HashMapNext(map)){
        HashSet *pairSet = p->value;
        HashSetFirst(pairSet);
        for(pair* pp= HashSetNext(pairSet);pp!=NULL;pp= HashSetNext(pairSet))
        {
            if(pp->define->name!=NULL && strcmp(v_before->name,pp->define->name)==0)
            {
                HashSetRemove(pairSet,pp);
                pp->define=v_replace;
                HashSetAdd(pairSet,pp);
            }
        }
    }
}

Value *get_replace_value(HashSet* set,int block_id)
{
    HashSetFirst(set);
    for(pair *p = HashSetNext(set); p != NULL; p = HashSetNext(set)){
        if(block_id==p->from->id)
            return p->define;
    }
    return NULL;
}

void LOOP_UNROLL_EACH(Loop* loop)
{
    //TODO 暂时是嵌套不处理循环展开
//    if(loop->child!=NULL)
//        return;

   //TODO 目前不做基本块间
    if(HashSetSize(loop->loopBody) > 2)
        return;

    Instruction *ins_end_cond=(Instruction*)loop->end_cond;
    Instruction *ins_modifier=(Instruction*)loop->modifier;
    Value *v_step=NULL,*v_end=NULL;
    if(ins_get_lhs(ins_end_cond)==loop->inductionVariable)
        v_end= ins_get_rhs(ins_end_cond);
    else
        v_end= ins_get_lhs(ins_end_cond);
    if(ins_get_lhs(ins_modifier)==loop->inductionVariable)
        v_step= ins_get_rhs(ins_modifier);
    else
        v_step= ins_get_lhs(ins_modifier);

    //目前只处理常数情况下的
    if(!check_idc(loop->initValue,v_step,v_end))
        return;

    if (v_step->pdata->var_pdata.iVal==0) {
        return;
    }

    int times= cal_times(loop->initValue->pdata->var_pdata.iVal,ins_modifier,ins_end_cond,v_step,v_end);
    int cnt=cal_ir_cnt(loop->loopBody);

    //超出一定长度的循环，不进行展开
    if((long)times*cnt>loop_unroll_up_lines)
        return;

    //一个map用来保存header中变量与最新值的对应关系
    //value*----pair的hashset
    HashMap *v_new_valueMap=HashMapInit();

    //记录其他中间量与最新值的对应关系
    //value*-----value*
    HashMap *other_new_valueMap=HashMapInit();

    //扫head
    //head块保存一下初始中map中变量与最新更新值的对应关系
    //%4( %2) = phi i32[%14 , %13], [%2 , %0]
    //保存一个%4------(13:%14, 0:%2)
    InstNode *head_currNode = get_next_inst(loop->head->head_node);
    InstNode *head_bodyTail = loop->head->tail_node;

    //head块中phi一定在最前面
    while(head_currNode != head_bodyTail && head_currNode->inst->Opcode==Phi){
        Value *variable=&head_currNode->inst->user.value;
        HashSet *map_phi_set=HashSetInit();
        HashMapPut(v_new_valueMap,variable,map_phi_set);

        HashSet *phiSet = head_currNode->inst->user.value.pdata->pairSet;
        HashSetFirst(phiSet);
        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
            //直接装进去
            HashSetAdd(map_phi_set,phiInfo);
        }

        head_currNode = get_next_inst(head_currNode);
    }

    /*
     * 基本块间：
     * 第一个基本块从label后开始复制(后续块label也需复制)，还是用v_new_valueMap的信息，开始ir都插入在latch块中,后来在curr_block中
     * phi单独处理,更新到latch的phi时，记得用alias对应到Head块中的phi信息
     */
    //循环展开
    HashSetFirst(loop->loopBody);
    for(BasicBlock *body = HashSetNext(loop->loopBody); body != NULL; body = HashSetNext(loop->loopBody)){
        Instruction *ins_tmp= ins_new_zero_operator(tmp);
        InstNode *node_tmp = new_inst_node(ins_tmp);
        ins_insert_before(node_tmp,body->tail_node);

        //exit和head块不展开
        if(body->id!=loop->head->id && body->id!=loop->exit_block->id)
        {
            int new_index=0;
            InstNode *currNode = body->head_node;
            InstNode *bodyTail = get_prev_inst(body->tail_node);
            //留给添加新基本块做准备的
            BasicBlock *curr_block=NULL;
            //复制这么多份
            //TODO 余数余数，具体怎么做
            for(int i=0;i<update_modifier-1;i++,currNode=loop->tail->head_node)
            {
                while(currNode != bodyTail){
                    if(!hasNoDestOperator(currNode)){
                        Value *lhs= ins_get_lhs(currNode->inst);
                        Value *rhs= ins_get_rhs(currNode->inst);
                        Value *v_r=NULL;
                        Value *v_l=NULL;
                        Instruction *copy_ins=NULL;
                        //如果v_new_valueMap中有，则要替换成最后使用值，而不能用原值
                        if(HashMapContain(v_new_valueMap,lhs))
                        {
                            HashSet *set_replace=HashMapGet(v_new_valueMap,lhs);
                            //loop->tail的block id可能随着新基本块的添加发生改变
                            v_l=get_replace_value(set_replace,loop->tail->id);
                        }
                        //变化过的中间变量
                        //比如%6= add nsw i32 %2,1，复制第一次后变为%8 = add nsw i32 %6,1
                        //做一个%6-----%8
                        else if(HashMapContain(other_new_valueMap,lhs))
                            v_l= HashMapGet(other_new_valueMap,lhs);
                        else   //是无所谓的，比如是常数
                            v_l=copy_value(lhs,++new_index);
                        if(v_l->VTy->ID==Int || v_l->VTy->ID==Float)
                            new_index--;

                        //如果有第二个操作数
                        if(rhs!=NULL)
                        {
                            if(HashMapContain(v_new_valueMap,rhs))
                            {
                                HashSet *set_replace=HashMapGet(v_new_valueMap,rhs);
                                v_r=get_replace_value(set_replace,body->id);
                            }
                                //变化过的中间变量
                            else if(HashMapContain(other_new_valueMap,rhs))
                                v_r= HashMapGet(other_new_valueMap,rhs);
                            else   //是无所谓的，比如是常数
                                v_r=copy_value(rhs,++new_index);
                            if(v_r->VTy->ID==Int || v_r->VTy->ID==Float)
                                new_index--;
                            copy_ins= ins_new_binary_operator(currNode->inst->Opcode,v_l,v_r);
                        }
                        else
                            copy_ins= ins_new_unary_operator(currNode->inst->Opcode,v_l);

                        InstNode *node = new_inst_node(copy_ins);
                        if(curr_block==NULL)
                            node->inst->Parent=loop->tail;
                        else
                            node->inst->Parent=curr_block;
                        ins_insert_before(node,body->tail_node);

                        //左值的对应关系也要存起来了
                        Value *new_dest= ins_get_value_with_name_and_index_(copy_ins,++new_index);
                        Value *v_dest=ins_get_dest(currNode->inst);

                        //如果在另两个表中的对应value里有v_dest，要更新
                        //如果v_new_valueMap有了新对应关系,比如本来记录了%2( %3) = phi i32[0 , %0], [%6 , %5]
                        //然后other_new_valueMap中%6与它复制的下一条左值%8有了%6----%8对应，进行替换
                        if(HashMapContain(other_new_valueMap,v_dest))
                            update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                        else
                            update_replace_value(v_new_valueMap,v_dest,new_dest);
                        HashMapPut(other_new_valueMap,v_dest ,new_dest);
                    }
                    currNode = get_next_inst(currNode);
                }
            }
        }

        //删除tmp
        deleteIns(node_tmp);
    }
}

//进行dfs，从最里层一步步展开
void dfsLoop(Loop *loop){
    HashSetFirst(loop->child);
    for(Loop *childLoop = HashSetNext(loop->child); childLoop != NULL; childLoop = HashSetNext(loop->child)){
        /*内层循环先处理*/
        dfsLoop(childLoop);
    }
    LOOP_UNROLL_EACH(loop);
}

void loop_unroll(Function *currentFunction)
{
    HashSetFirst(currentFunction->loops);
    //遍历每个loop
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        dfsLoop(root);
    }
}