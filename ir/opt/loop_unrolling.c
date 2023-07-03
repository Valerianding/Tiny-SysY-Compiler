#include "loop_unrolling.h"
#include "travel.h"

/**
 * 目前只考虑init、modifier、end均为常数
 * 不循环嵌套
 * 基本块内
 */

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
bool check_idc(Loop* loop)
{
    if(loop->initValue->VTy->ID==Int && loop->modifier->VTy->ID==Int && loop->end_cond->VTy->ID==Int)
        return true;
    return false;
}

//计算迭代次数
//TODO 假设step有正负,但还是需要*,+等符号信息
int cal_times(int init,int step,int end)
{
    return 1;
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

bool check_in_latch_phi(Value *v,Value *variable,HashMap* latch_phi_map,HashMap* head_map)
{
    //将对应Pair装载
    if(HashMapContain(latch_phi_map,v))
    {
        HashSet *pairSet= HashMapGet(latch_phi_map,v);
        HashSet *variable_pairSet= HashMapGet(head_map,variable);
        //遍历pairSet，进行一波装载
        HashSetFirst(pairSet);
        for(pair *p = HashSetNext(pairSet); p != NULL; p = HashSetNext(pairSet)){
            HashSetAdd(variable_pairSet,p);
        }
        return true;
    }
    return false;
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

void LOOP_UNROLL_EACH(Loop* loop)
{
//    if(!check_idc(loop))
//        return;

//    int init = loop->initValue->pdata->var_pdata.iVal;
//    int step = loop->modifier->pdata->var_pdata.iVal;
//    int end =0;           //拿不到end
//    if (step == 0) {
//        return;
//    }

//    int times= cal_times(init,step,end);
//    int cnt=cal_ir_cnt(loop->loopBody);

    //超出一定长度的循环，不进行展开
//    if((long)times*cnt>loop_unroll_up_lines)
//        return;

    //一个map用来保存变量与最新值的对应关系
    //value*----pair的hashset
    HashMap *v_new_valueMap=HashMapInit();

    //记录其他中间量与最新值的对应关系
    //value*-----value*
    HashMap *other_new_valueMap=HashMapInit();

    //先扫latch，如果有phi,保存一个Phi的对应关系
    //value*----pair的hashset
    HashMap *v_latch_phiMap=HashMapInit();
    InstNode *latch_curr_node=get_next_inst(loop->tail->head_node);       //第一条是Label，自动挪到第二条
    InstNode *latch_tail_node=loop->tail->tail_node;
    while (latch_curr_node!=latch_tail_node && latch_curr_node->inst->Opcode==Phi)
    {
        HashMapPut(v_latch_phiMap,ins_get_dest(latch_curr_node->inst),latch_curr_node->inst->user.value.pdata->pairSet);
        latch_curr_node= get_next_inst(latch_curr_node);
    }



    //再扫head
    //head块保存一下初始中map中变量与最新更新值的对应关系
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
            Value *incomingVal = phiInfo->define;
            if(!check_in_latch_phi(incomingVal,variable,v_latch_phiMap,v_new_valueMap))
            {
                //不需要装载，说明本身就是给pair,直接装进去
                HashSetAdd(map_phi_set,phiInfo);
            }
        }

        //Value *new_value
        head_currNode = get_next_inst(head_currNode);
    }

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
            //复制这么多份
            for(int i=0;i<update_modifier-1;i++,currNode=body->head_node)
            {
                while(currNode != bodyTail){
                    if(!hasNoDestOperator(currNode)){
                        Value *lhs= ins_get_lhs(currNode->inst);
                        Value *rhs= ins_get_rhs(currNode->inst);
                        Value *v_r=NULL;
                        Value *v_l=NULL;
                        Instruction *copy_ins=NULL;
                        //如果map中有，则要替换成最后使用值，而不能用原值
                        if(HashMapContain(v_new_valueMap,lhs))
                        {
                            HashSet *set_replace=HashMapGet(v_new_valueMap,lhs);
                            v_l=get_replace_value(set_replace,body->id);
                        }
                            //变化过的中间变量
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
                        node->inst->Parent=body;
                        ins_insert_before(node,body->tail_node);

                        //左值的对应关系也要存起来了
                        Value *new_dest= ins_get_value_with_name_and_index_(copy_ins,++new_index);
                        Value *v_dest=ins_get_dest(currNode->inst);

                        //如果在另两个表中的对应value里有v_dest，要更新
                        if(HashMapContain(other_new_valueMap,v_dest))
                        {
                            update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                            update_replace_value(v_latch_phiMap,HashMapGet(other_new_valueMap,v_dest),new_dest);
                        }
                        else
                        {
                            update_replace_value(v_new_valueMap,v_dest,new_dest);
                            update_replace_value(v_latch_phiMap,v_dest,new_dest);
                        }
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