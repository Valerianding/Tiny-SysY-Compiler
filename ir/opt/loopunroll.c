//
// 原来那个逻辑有点死，remake
// Created by 12167 on 2023/7/21.
//
#include "loopunroll.h"
int tmp_index = 0;

bool first_copy = true;
bool mod_before = false;

void adjust_phi_from(Loop *loop,BasicBlock *new_pre_block, HashMap *v_new_valueMap){
    HashMapFirst(v_new_valueMap);
    for(Pair *p = HashMapNext(v_new_valueMap); p != NULL; p = HashMapNext(v_new_valueMap)){
        HashSet *pairSet = p->value;
        HashSetFirst(pairSet);
        for(pair* pp= HashSetNext(pairSet);pp!=NULL;pp= HashSetNext(pairSet))
        {
            //更新初值的到达块为new_pre_block
            if(!HashSetFind(loop->loopBody, pp->from)){
                pp->from = new_pre_block;
            }
        }
    }
}

void adjust_blocks(BasicBlock* head, BasicBlock* new_pre_block,Loop* loop){
    //将Head的前驱对应的后继全部指向new_pre_block
    HashSet *preBlocks = head->preBlocks;
    HashSetFirst(preBlocks);
    for(BasicBlock *block = HashSetNext(preBlocks); block != NULL ;block = HashSetNext(preBlocks)){
        if(!HashSetFind(loop->loopBody,block)) {
            if(block->true_block && block->true_block == head){
                block->true_block = new_pre_block;
                bb_delete_one_prev(head,block);
            }
            else if(block->false_block && block->false_block == head){
                block->false_block = new_pre_block;
                bb_delete_one_prev(head,block);
            }
            bb_add_prev(block, new_pre_block);
        }
    }
    new_pre_block->true_block = head;
    bb_add_prev(new_pre_block,head);
}

Value *get_replace_value(HashSet* set,BasicBlock* block)
{
    HashSetFirst(set);
    for(pair *p = HashSetNext(set); p != NULL; p = HashSetNext(set)){
        if(block==p->from)
            return p->define;
    }
    return NULL;
}


//更新的是初值，不知道具体block,但是block肯定不在循环内
Value *get_replace_value_first(HashSet* set,Loop* loop){

    HashSetFirst(set);
    for(pair *p = HashSetNext(set); p != NULL; p = HashSetNext(set)){
        if(!HashSetFind(loop->loopBody, p->from))
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
            if(v_before == pp->define)
            {
                HashSetRemove(pairSet,pp);
                pp->define=v_replace;
                HashSetAdd(pairSet,pp);
            }
        }
    }
}

void update_replace_value_mod(HashMap* map,Value* v_before,Value* v_replace, Loop* loop)
{
    HashMapFirst(map);
    bool flag = false;
    Value *v_init = NULL;

    for(Pair *p = HashMapNext(map); p != NULL; p = HashMapNext(map)){
        HashSet *pairSet = p->value;
        HashSetFirst(pairSet);
        for(pair* pp= HashSetNext(pairSet);pp!=NULL;pp= HashSetNext(pairSet))
        {
            if(v_before == pp->define){
                flag = true;
                //找到正确的pairSet了
                break;
            }
        }
        if(flag)
        {
            v_init = get_replace_value_first(pairSet,loop);
            HashSetFirst(pairSet);
            for(pair* pp= HashSetNext(pairSet);pp!=NULL;pp= HashSetNext(pairSet))
            {
                if(v_init == pp->define){
                    HashSetRemove(pairSet,pp);
                    pp->define=v_replace;
                    HashSetAdd(pairSet,pp);
                    return;
                }
            }
        }
    }
}


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


//一次循环展开
//mod_flag表示是不是mod的地方需要复制，如果是mod那边需要，就必须给插入位置block
void copy_one_time(Loop* loop, bool mod_flag,BasicBlock* block,HashMap* v_new_valueMap,HashMap* other_new_valueMap){
    HashSetFirst(loop->loopBody);
    for(BasicBlock *body = HashSetNext(loop->loopBody); body != NULL; body = HashSetNext(loop->loopBody)){
        if(first_copy && !mod_flag){
            Instruction *ins_tmp= ins_new_zero_operator(tmp);
            InstNode *node_tmp = new_inst_node(ins_tmp);
            ins_insert_before(node_tmp,body->tail_node);
        }

        //exit和head块不展开
        if(body!=loop->head && body!=loop->exit_block){
            InstNode *currNode = body->head_node;
            InstNode *bodyTail = get_prev_inst(body->tail_node);
            //留给添加新基本块做准备的
            BasicBlock *curr_block=NULL;

            while(currNode != bodyTail && currNode->inst->Opcode != tmp){
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
                        v_l=get_replace_value(set_replace,loop->tail);           //CHECK: 更新值一定在最后吗,应该是吧，phi的话
                    }
                    //变化过的中间变量
                    //比如%6= add nsw i32 %2,1，复制第一次后变为%8 = add nsw i32 %6,1
                    //做一个%6-----%8                                                       //CHECK: 这个地方应该是不用考虑参数的吧
                    else if(HashMapContain(other_new_valueMap,lhs))
                        v_l= HashMapGet(other_new_valueMap,lhs);
                    else   //是无所谓的，比如是常数
                        v_l=copy_value(lhs,tmp_index++);
//                    if(v_l->VTy->ID==Int || v_l->VTy->ID==Float)
//                        tmp_index--;

                    //如果有第二个操作数
                    if(rhs!=NULL)
                    {
                        if(HashMapContain(v_new_valueMap,rhs))
                        {
                            HashSet *set_replace=HashMapGet(v_new_valueMap,rhs);
                            v_r=get_replace_value(set_replace,body);
                        }
                            //变化过的中间变量
                        else if(HashMapContain(other_new_valueMap,rhs))
                            v_r= HashMapGet(other_new_valueMap,rhs);
                        else   //是无所谓的，比如是常数
                            v_r=copy_value(rhs,tmp_index);
//                        if(v_r->VTy->ID==Int || v_r->VTy->ID==Float)
//                            tmp_index--;
                        copy_ins= ins_new_binary_operator(currNode->inst->Opcode,v_l,v_r);
                    }
                    else
                        copy_ins= ins_new_unary_operator(currNode->inst->Opcode,v_l);

                    //TODO 其实还没考虑多基本块
                    InstNode *node = new_inst_node(copy_ins);
//                    if(curr_block==NULL)
//                        node->inst->Parent=loop->tail;
//                    else
//                        node->inst->Parent=curr_block;
                    if(mod_flag){
                        node->inst->Parent = block;
                        ins_insert_before(node,block->tail_node);
                    }
                    else{
                        node->inst->Parent=loop->tail;
                        ins_insert_before(node,body->tail_node);
                    }

                    //左值的对应关系也要存起来了
                    Value *new_dest= ins_get_value_with_name_and_index(copy_ins,tmp_index++);
                    Value *v_dest=ins_get_dest(currNode->inst);

                    //如果在另两个表中的对应value里有v_dest，要更新
                    //如果v_new_valueMap有了新对应关系,比如本来记录了%2( %3) = phi i32[0 , %0], [%6 , %5]
                    //然后other_new_valueMap中%6与它复制的下一条左值%8有了%6----%8对应，进行替换
                    if(HashMapContain(other_new_valueMap,v_dest) && !mod_before)
                        update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                    else
                        update_replace_value(v_new_valueMap,v_dest,new_dest);
                    HashMapPut(other_new_valueMap,v_dest ,new_dest);
                }

                currNode = get_next_inst(currNode);
            }
        }
    }
    first_copy = false;
}

//TODO 目前还是基本块内, 完全一样地复制
//返回一个map,表明每个value的更新情况
void copy_for_mod(Loop* loop, BasicBlock* block, HashMap* v_new_valueMap,HashMap* other_new_valueMap,bool first_time){
    HashSetFirst(loop->loopBody);
    for(BasicBlock *body = HashSetNext(loop->loopBody); body != NULL; body = HashSetNext(loop->loopBody)){

        //exit和head块不展开
        if(body!=loop->head && body!=loop->exit_block){
            InstNode *currNode = body->head_node;
            InstNode *bodyTail = body->tail_node;
            //留给添加新基本块做准备的
            BasicBlock *curr_block=NULL;

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
                        v_l=get_replace_value_first(set_replace,loop);
                    }
                        //变化过的中间变量
                        //比如%6= add nsw i32 %2,1，复制第一次后变为%8 = add nsw i32 %6,1
                        //做一个%6-----%8                                                       //CHECK: 这个地方应该是不用考虑参数的吧
                    else if(HashMapContain(other_new_valueMap,lhs))
                        v_l= HashMapGet(other_new_valueMap,lhs);
                    else {
                        if(first_time)
                            v_l = lhs;
                        else
                            v_l=copy_value(lhs,tmp_index++);
                    }
//                    if(v_l->VTy->ID==Int || v_l->VTy->ID==Float)
//                        tmp_index--;

                    //如果有第二个操作数
                    if(rhs!=NULL)
                    {
                        if(HashMapContain(v_new_valueMap,rhs))
                        {
                            HashSet *set_replace=HashMapGet(v_new_valueMap,rhs);
                            v_r=get_replace_value_first(set_replace,loop);
                        }
                            //变化过的中间变量
                        else if(HashMapContain(other_new_valueMap,rhs))
                            v_r= HashMapGet(other_new_valueMap,rhs);
                        else {
                            if(first_time)
                                v_r = rhs;
                            else
                                v_r=copy_value(rhs,tmp_index++);
                        }
//                        if(v_r->VTy->ID==Int || v_r->VTy->ID==Float)
//                            tmp_index--;
                        copy_ins= ins_new_binary_operator(currNode->inst->Opcode,v_l,v_r);
                    }
                    else
                        copy_ins= ins_new_unary_operator(currNode->inst->Opcode,v_l);

                    //TODO 其实还没考虑多基本块
                    InstNode *node = new_inst_node(copy_ins);
//                    if(curr_block==NULL)
//                        node->inst->Parent=loop->tail;
//                    else
//                        node->inst->Parent=curr_block;

                     node->inst->Parent = block;
                     ins_insert_before(node,block->tail_node);            //TODO 插不插tmp


                    //左值的对应关系也要存起来了
                    Value *new_dest= ins_get_value_with_name_and_index(copy_ins,tmp_index++);
                    Value *v_dest=ins_get_dest(currNode->inst);

                    //如果在另两个表中的对应value里有v_dest，要更新
                    //如果v_new_valueMap有了新对应关系,比如本来记录了%2( %3) = phi i32[0 , %0], [%6 , %5]
                    //然后other_new_valueMap中%6与它复制的下一条左值%8有了%6----%8对应，进行替换
                    if(HashMapContain(other_new_valueMap,v_dest)){
                        update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                    }
                    else{
                        update_replace_value_mod(v_new_valueMap,v_dest,new_dest,loop);
                    }
                    HashMapPut(other_new_valueMap,v_dest ,new_dest);
                }

                currNode = get_next_inst(currNode);
            }
        }
    }
}

//检查init、modifier、end是否都是int常数
bool check_idc(Value* initValue,Value* step,Value* end)
{
    if(initValue->VTy->ID==Int && end->VTy->ID==Int && step->VTy->ID==Int)
        return true;
    return false;
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

//常数情况下计算迭代次数
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

//
InstNode *insert_ir_mod(Value* v_init, Value* v_end, Value* v_step, BasicBlock* preserve1,  BasicBlock* mod_block,BasicBlock *new_pre_block, InstNode* pos){
    //times = ((end - init) % step == 0)? (end - init) / step : (end - init) / step + 1;
    //给preserve block一个label
    Instruction *ins_label = ins_new_zero_operator(Label);
    InstNode *node_label = new_inst_node(ins_label);
    ins_label->Parent = preserve1;
    ins_insert_after(node_label, pos);
    preserve1->head_node = node_label;
    InstNode *node_sub = auto_binary_node_insert_after(Sub,v_end,v_init,tmp_index++,preserve1,node_label);
    InstNode *node_div = auto_binary_node_insert_after(Div,ins_get_dest(node_sub->inst),v_step,tmp_index++,preserve1,node_sub);
    InstNode *node_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_div->inst),v_step,tmp_index++,preserve1,node_div);

    preserve1->false_block = new_pre_block;
    preserve1->true_block = mod_block;
    bb_add_prev(preserve1,new_pre_block);

    //icmp和br_i1
    Value *v_zero = (Value*) malloc(sizeof (Value));
    value_init_int(v_zero,0);
    InstNode *node_icmp = auto_binary_node_insert_after(EQ, ins_get_dest(node_mod->inst),v_zero,tmp_index++,preserve1,node_mod);
    Instruction *ins_br_i1 = ins_new_unary_operator(br_i1, ins_get_dest(node_icmp->inst));
    ins_br_i1->Parent = preserve1;
    InstNode *node_br_i1 = new_inst_node(ins_br_i1);
    ins_insert_after(node_br_i1,node_icmp);
    preserve1->tail_node = node_br_i1;

    //填充mod块,需要做一个+1,br的话mod_block进来之前就已经有了
    Value *v_one = (Value*) malloc(sizeof (Value));
    value_init_int(v_one,1);
    InstNode *node_plus = auto_binary_node_insert_after(Add, ins_get_dest(node_div->inst),v_one,tmp_index++,mod_block,mod_block->head_node);

    //new_pre_block生成一条phi,node_mod是一条phi
    Instruction *ins_phi = ins_new_zero_operator(Phi);
    HashSet *set = HashSetInit();
    pair *pair1 = (pair*) malloc(sizeof (pair));
    pair *pair2 = (pair*) malloc(sizeof (pair));
    pair1->from =preserve1;
    pair1->define = ins_get_dest(node_div->inst);
    pair2->from = mod_block;
    pair2->define = ins_get_dest(node_plus->inst);
    HashSetAdd(set,pair1);
    HashSetAdd(set,pair2);
    ins_get_value_with_name_and_index(ins_phi,tmp_index++)->pdata->pairSet = set;
    InstNode *node_phi = new_inst_node(ins_phi);
    ins_insert_after(node_phi,new_pre_block->head_node);
    return node_phi;
}

//如果返回NULL代表确定的一次不走或wrong，返回int类型常数1表示一定走的这种
//TODO 插入位置得改
InstNode *get_mod(Value* v_init,Instruction *ins_modifier,Instruction *ins_end_cond,Value *v_step,Value *v_end,BasicBlock *mod_block,BasicBlock *new_pre_block,InstNode* pos,Loop* loop){
    Value *v_mod = (Value*) malloc(sizeof(Value));
    Value *v_update_modifier = (Value*) malloc(sizeof(Value));
    value_init_int(v_update_modifier,update_modifier);
    //1. 计算times   2.times % update_modifier
    //value_init(v_mod);
    InstNode *node_sub = NULL,*node_get_times = NULL,*node_get_mod = NULL,*node_plus = NULL;
    Value *plus = (Value*) malloc(sizeof(Value));
    InstNode *node_div = NULL;
    BasicBlock *preserve1 = NULL;
    switch (ins_modifier->Opcode) {
        case Add:
            switch (ins_end_cond->Opcode) {

                case EQ:
                    value_init_int(v_mod,1);
                    node_get_mod = (v_init == v_end) ? node_get_mod : NULL;
                    break;
                case NOTEQ:  //TODO 我目前就默认没有wrong的情况了
                    node_sub = auto_binary_node_insert_after(Sub,v_end,v_init,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_sub->inst),v_step,tmp_index++,mod_block,node_sub);
                    node_get_mod = auto_binary_node_insert_after(Mod,ins_get_dest(node_get_times->inst),v_update_modifier,tmp_index++,mod_block,node_get_times);
                    break;
                case LESSEQ: case GREATEQ:
                    node_sub = auto_binary_node_insert_after(Sub,v_end,v_init,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_sub->inst),v_step,tmp_index++,mod_block,node_sub);
                    value_init_int(plus,1);
                    node_plus = auto_binary_node_insert_after(Add, ins_get_dest(node_get_times->inst),plus,tmp_index++,mod_block,node_get_times);
                    node_get_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_plus->inst),v_update_modifier,tmp_index++,mod_block,node_plus); break;
                case LESS: case GREAT:
                    //我哭死了，你还要分类讨论, 一个基本块该不会还放不下你吧....还真放不下
                    //来个新block,block在mod_block之前
                    preserve1 = bb_create();
                    preserve1->Parent = mod_block->Parent;
                    adjust_blocks(mod_block,preserve1, loop);
                    node_get_mod = insert_ir_mod(v_init,v_end,v_step,preserve1,mod_block,new_pre_block,pos);
                    break;
            }
            break;
        case Sub:
            switch (ins_end_cond->Opcode) {
                case EQ: value_init_int(v_mod,1);
                    node_get_mod = (v_init == v_end) ? node_get_mod : NULL;
                    break;
                case NOTEQ:
                    node_sub = auto_binary_node_insert_after(Sub,v_init,v_end,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_sub->inst),v_step,tmp_index++,mod_block,node_sub);
                    node_get_mod = auto_binary_node_insert_after(Mod,ins_get_dest(node_get_times->inst),v_update_modifier,tmp_index++,mod_block,node_get_times);
                    break;
                case LESSEQ: case GREATEQ:
                    node_sub = auto_binary_node_insert_after(Sub,v_init,v_end,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_sub->inst),v_step,tmp_index++,mod_block,node_sub);
                    value_init_int(plus,1);
                    node_plus = auto_binary_node_insert_after(Add, ins_get_dest(node_get_times->inst),plus,tmp_index++,mod_block,node_get_times);
                    node_get_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_plus->inst),v_update_modifier,tmp_index++,mod_block,node_plus); break;
                case LESS: case GREAT:
                    preserve1 = bb_create();
                    preserve1->Parent = mod_block->Parent;
                    adjust_blocks(mod_block,preserve1,loop);
                    node_get_mod = insert_ir_mod(v_end,v_init,v_step,preserve1,mod_block,new_pre_block,pos);
                    break;
            }
            break;
        case Mul:
            //TODO 不知道要不要注意到小数点
            //val = log((double)end / (double)init) / log((double)step);
            //tag = init * pow(step, val) == end;
            switch (ins_end_cond->Opcode) {
                case EQ: value_init_int(v_mod,1);
                    node_get_mod = (v_init == v_end) ? node_get_mod : NULL;
                    break;
                case NOTEQ:
                    node_div = auto_binary_node_insert_after(Div,v_end,v_init,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_div->inst),v_step,tmp_index++,mod_block,node_div);
                    node_get_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_get_times->inst),v_update_modifier,tmp_index++,mod_block,node_get_times); break;
                case LESSEQ: case GREATEQ:
                    node_div = auto_binary_node_insert_after(Div,v_end,v_init,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_div->inst),v_step,tmp_index++,mod_block,node_div);
                    value_init_int(plus,1);
                    node_plus = auto_binary_node_insert_after(Add, ins_get_dest(node_get_times->inst),plus,tmp_index++,mod_block,node_get_times);
                    node_get_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_plus->inst),v_update_modifier,tmp_index++,mod_block,node_plus); break;
                case LESS: case GREAT:
                    //TODO 先不处理了
                    //times = tag ? (int) val : (int) val + 1
                    break;
            }
            break;
        case Div:
//            val = log((double)init / (double)end) / log((double)step);
//            tag = end * pow(step, val) == init;
            switch (ins_end_cond->Opcode) {
                case EQ: value_init_int(v_mod,1);
                    node_get_mod = (v_init == v_end) ? node_get_mod : NULL;
                    break;
                case NOTEQ:
                    node_div = auto_binary_node_insert_after(Div,v_init,v_end,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_div->inst),v_step,tmp_index++,mod_block,node_div);
                    node_get_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_get_times->inst),v_update_modifier,tmp_index++,mod_block,node_get_times); break;
                case LESSEQ: case GREATEQ:
                    node_div = auto_binary_node_insert_after(Div,v_init,v_end,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_div->inst),v_step,tmp_index++,mod_block,node_div);
                    value_init_int(plus,1);
                    node_plus = auto_binary_node_insert_after(Add, ins_get_dest(node_get_times->inst),plus,tmp_index++,mod_block,node_get_times);
                    node_get_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_plus->inst),v_update_modifier,tmp_index++,mod_block,node_plus); break;
                case LESS: case GREAT:
                    //TODO 先不处理了
                    //times = tag ? (int) val : (int) val + 1
                    break;
            }
            break;
    }
    return node_get_mod;
}

void LOOP_UNROLL_EACH(Loop* loop)
{
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

    //步长为0就return
    if(v_step->VTy->ID == Int && v_step->pdata->var_pdata.iVal==0)
        return;

    int cnt=cal_ir_cnt(loop->loopBody);
    //如果是常数，就能计算迭代次数，然后进行一个判断,TODO 非常数就不判断了吗
    Value *times = (Value*) malloc(sizeof (Value));
    if(check_idc(loop->initValue,v_step,v_end)){
        value_init_int(times, cal_times(loop->initValue->pdata->var_pdata.iVal,ins_modifier,ins_end_cond,v_step,v_end));
        //超出一定长度的循环，不进行展开
        if((long)times*cnt>loop_unroll_up_lines)
            return;
    }

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

    //循环展开
    ///思路:
    //1. 如果是常数: 可以直接计算出有无余数, 无余数的话不管；
    // 有余数或不是常数：在head之前建一个新block,将head的preBlocks的后继都指向新Block，在这个block中计算余数(非常数,记得判断一下一次循环都走不了的情况)并跑完余数的次数(可能不止花费一个基本块)
    //2. 归纳变量从无余数的起点开始跑

    //1. 如果是常数,可以直接算出有没有余数
    //TODO 新块的head,tail
    BasicBlock *new_pre_block = NULL;
    BasicBlock *mod_block = NULL;
    if(check_idc(loop->initValue,v_step,v_end)){
        int mod_num = times->pdata->var_pdata.iVal % update_modifier;       //余数是迭代次数对update_modifier取余
        printf("mod num : %d\n",mod_num);
        if(mod_num != 0){               //无余数的情况不处理
            mod_before = true;
            //来新block
            new_pre_block = bb_create();
            new_pre_block->Parent = ins_end_cond->Parent->Parent;

            InstNode *pos = loop->head->head_node;
            Instruction *label = ins_new_zero_operator(Label);
            label->Parent = new_pre_block;
            InstNode *node_label = new_inst_node(label);
            ins_insert_before(node_label,pos);
            new_pre_block->head_node = node_label;

            //来一条br直接跳转到head
            Instruction * ins_br = ins_new_zero_operator(br);
            ins_br->Parent = new_pre_block;
            InstNode *node_br = new_inst_node(ins_br);
            ins_insert_before(node_br,pos);
            new_pre_block->tail_node = node_br;

            //调整下前驱后继块
            adjust_blocks(loop->head,new_pre_block,loop);
            //将要copy的内容copy mod_num次
            //1. 将块完全一样地复制到新block中
            copy_for_mod(loop, new_pre_block, v_new_valueMap, other_new_valueMap, 1);
            //2. 将块在新block中再复制mod_num - 1次
            //3. 置换更新原block中的元素，更新map
            for(int i=0; i < mod_num - 1 ;i++){
                copy_for_mod(loop, new_pre_block, v_new_valueMap, other_new_valueMap ,0);
            }

            //调整一下head phi中的from
            adjust_phi_from(loop,new_pre_block,v_new_valueMap);

            //更新归纳变量
            //loop->initValue->pdata->var_pdata.iVal += mod_num;
        }
    } else {
        //* preserv1      : 要分类讨论的余数计算
        //mod_block       : 算余数
        //new_pre_block   : 循环变量余数的phi
        //loop_pre_block  : 装循环体

        mod_before = true;
        //用ir算余数
        //肯定有新block
        //icmp 0,%v_mod,mod的block在new_pre_block之前
        mod_block = bb_create();
        new_pre_block = bb_create();
        new_pre_block->Parent = ins_end_cond->Parent->Parent;
        mod_block->Parent = ins_end_cond->Parent->Parent;
        adjust_blocks(loop->head,new_pre_block,loop);
        adjust_blocks(new_pre_block,mod_block,loop);

        //给mod_block建一条head
        Instruction *label = ins_new_zero_operator(Label);
        InstNode *node_label = new_inst_node(label);
        label->Parent = mod_block;
        InstNode *pos = loop->head->head_node;
        ins_insert_before(node_label,pos);
        mod_block->head_node = node_label;         //然后算余数的ir都在这条ir后面

        //给new_pre_block建一条head　　．先弄这个，因为后面get_mod要用
        Instruction *label2 = ins_new_zero_operator(Label);
        label2->Parent = new_pre_block;
        InstNode *node_label2 = new_inst_node(label2);
        new_pre_block->head_node = node_label2;

        InstNode *node_mod = get_mod(loop->initValue,ins_modifier,ins_end_cond,v_step,v_end,mod_block, new_pre_block,node_label, loop);
        //br到下一个block循环mod次
        Instruction *ins_br = ins_new_zero_operator(br);
        ins_br->Parent = mod_block;
        InstNode *node_br = new_inst_node(ins_br);
        ins_insert_after(node_br,node_mod);
        mod_block->tail_node = node_br;

        ins_insert_after(node_label2,node_br);

        //包装一个phi，初始值为0，每次加1,就专职用来计数这个补充余数的小循环
        Value *v_zero=(Value*) malloc(sizeof (Value));
        value_init_int(v_zero,0);
        Instruction *ins_phi = ins_new_zero_operator(Phi);
        ins_phi->Parent = new_pre_block;
        HashSet * set = HashSetInit();
        pair *pair1 = (pair*) malloc(sizeof (pair));
        pair1->from = mod_block;
        pair1->define = v_zero;
        HashSetAdd(set,pair1);
        ins_get_value_with_name_and_index(ins_phi,tmp_index++)->pdata->pairSet = set;
        InstNode *node_phi = new_inst_node(ins_phi);
        ins_insert_after(node_phi,node_label2);
        //生成一条icmp,在new_pre_block中
        InstNode *node_icmp = auto_binary_node_insert_after(LESS, ins_get_dest(ins_phi), ins_get_dest(node_mod->inst),tmp_index++,new_pre_block,node_phi);
        //生成br_i1
        Instruction *ins_bri1 = ins_new_unary_operator(br_i1, ins_get_dest(node_icmp->inst));
        ins_bri1->Parent = new_pre_block;
        InstNode *node_br_i1 = new_inst_node(ins_bri1);
        ins_insert_after(node_br_i1,node_icmp);
        new_pre_block->tail_node = node_br_i1;

        //基本块内循环 还要再来个block来装循环体           //TODO 目前还是只考虑了单基本块
        BasicBlock *loop_pre_block = bb_create();
        bb_add_prev(new_pre_block, loop_pre_block);
        new_pre_block->true_block = loop_pre_block;
        new_pre_block->false_block = loop->head;
        loop_pre_block->Parent = new_pre_block->Parent;
        loop_pre_block->true_block = new_pre_block;

        //label
        Instruction *label3 = ins_new_zero_operator(Label);
        label3->Parent = loop_pre_block;
        InstNode *node_label3 = new_inst_node(label3);
        ins_insert_after(node_label3,node_br_i1);
        loop_pre_block->head_node = node_label3;
        //br
        Instruction *ins_br2 = ins_new_zero_operator(br);
        ins_br2->Parent = loop_pre_block;
        InstNode *node_br2 = new_inst_node(ins_br2);
        ins_insert_after(node_br2,node_label3);
        loop_pre_block->tail_node = node_br2;

        //调用copy内容的函数 一次
        copy_for_mod(loop, loop_pre_block, v_new_valueMap, other_new_valueMap, 1);
        // 一次add +1,然后存进phi_set
        Value *v_one=(Value*) malloc(sizeof (Value));
        value_init_int(v_one,1);
        InstNode *node_plus = auto_binary_node_insert_before(Add, ins_get_dest(node_phi->inst),v_one,tmp_index++,loop_pre_block,loop_pre_block->tail_node);

        pair *pair2 = (pair*) malloc(sizeof (pair));
        pair2->define = ins_get_dest(node_plus->inst);
        pair2->from =loop_pre_block;
        HashSetAdd(set,pair2);

        //调整一下head phi中的from
        adjust_phi_from(loop,new_pre_block,v_new_valueMap);

    }

    //余数处理完毕，开始循环内容的复制
    //TODO 要保存最初的ir,或者说给block一个alias是自己的副本，最后释放
    for(int i = 0;i < update_modifier-1;i++){
        copy_one_time(loop, 0, NULL, v_new_valueMap,other_new_valueMap);
        mod_before = false;
    }
    //遍历loop, 删去作挡板的tmp
    HashSetFirst(loop->loopBody);
    for(BasicBlock* body = HashSetNext(loop->loopBody);body!=NULL;body = HashSetNext(loop->loopBody)){
        InstNode *curr= body->head_node;
        InstNode *tail = body->tail_node;
        while (curr!=tail){
            if(curr->inst->Opcode == tmp){         //一个block应该只有一条挡板
                deleteIns(curr);
                break;
            }
            curr = get_next_inst(curr);
        }
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