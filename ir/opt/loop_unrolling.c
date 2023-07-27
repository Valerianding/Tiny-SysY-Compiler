//
// 原来那个逻辑有点死，remake
// Created by 12167 on 2023/7/21.
//
#include "loop_unrolling.h"
int tmp_index = 0;

bool first_copy = true;
bool mod_before = false;

//head前面如果插入了新基本块，更新初值的到达块为新加入的new_pre_block
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

//在head块前插入new_pre_block块，调整块间信息
void adjust_blocks(BasicBlock* head, BasicBlock* new_pre_block,Loop* loop, bool in_loop){
    //将Head的前驱对应的后继全部指向new_pre_block
    HashSet *preBlocks = head->preBlocks;
    HashSetFirst(preBlocks);
    for(BasicBlock *block = HashSetNext(preBlocks); block != NULL ;block = HashSetNext(preBlocks)){
        if((!in_loop && !HashSetFind(loop->loopBody,block)) || in_loop) {
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

void connect_blocks(HashMap* block_map,Loop* loop,BasicBlock* block){
    HashMapFirst(block_map);
    for(Pair* p = HashMapNext(block_map); p!=NULL ;p = HashMapNext(block_map)){
        BasicBlock *b_loop = p->key;
        BasicBlock *b_new = p->value;
        b_new->true_block = NULL; b_new->false_block = NULL;
        b_new->preBlocks = HashSetInit();

        if(b_loop == loop->head->true_block){
            if(!block){
                loop->tail->true_block = b_new;
                bb_add_prev(loop->tail,b_new);
            } else {
                block->true_block = b_new;
                bb_add_prev(block,b_new);
            }
        }

        //如果是tail的话不用专门调了, 它的在最开始就出来了
        if(b_loop != loop->tail){
            if(b_loop->true_block){
                if(HashMapContain(block_map, b_loop->true_block)){
                    b_new->true_block = HashMapGet(block_map, b_loop->true_block);
                    bb_add_prev(b_new, HashMapGet(block_map, b_loop->true_block));
                }
                else {
                    b_new->true_block = b_loop->true_block;
                    bb_add_prev(b_new,b_loop->true_block);
                }
            }
            if(b_loop->false_block){
                if(HashMapContain(block_map, b_loop->false_block)){
                    b_new->false_block = HashMapGet(block_map, b_loop->false_block);
                    bb_add_prev(b_new, HashMapGet(block_map, b_loop->false_block));
                }
                else {
                    b_new->false_block = b_loop->false_block;
                    bb_add_prev(b_new,b_loop->false_block);
                }
            }
        }
    }
}

//在phi_set中拿到指定block为from的value
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
//在phi_set中拿到初值
Value *get_replace_value_first(HashSet* set,Loop* loop){

    HashSetFirst(set);
    for(pair *p = HashSetNext(set); p != NULL; p = HashSetNext(set)){
        if(!HashSetFind(loop->loopBody, p->from))
            return p->define;
    }
    return NULL;
}

//这种head后加基本块，更新的phi中的value是后面从tail过来的value
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

//在head前加基本块(余数模式),更新的phi中的Value是从初值过来的value
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

//每复制完一次，将跳转前的value和block生成一个pair,保存进对应的exit_phi_map
void update_exit_phi(BasicBlock *curr_block, HashMap *v_new_valueMap,HashMap *exit_phi_map, Loop* loop){
    HashMapFirst(exit_phi_map);
    for(Pair* p = HashMapNext(exit_phi_map); p!=NULL; p = HashMapNext(exit_phi_map)){
        HashMapFirst(v_new_valueMap);
        for(Pair* pp = HashMapNext(v_new_valueMap); pp!=NULL ; pp = HashMapNext(v_new_valueMap)){
            if(pp->key == p->key){        //找到同一个phi了
                HashSetFirst(pp->value);
                for(pair* pair_new = HashSetNext(pp->value); pair_new!=NULL; pair_new = HashSetNext(pp->value)){
                    if(HashSetFind(loop->loopBody, pair_new->from)){
                        pair *insert = (pair*) malloc(sizeof (pair));
                        insert->from = curr_block;
                        insert->define = pair_new->define;

                        HashSetAdd(p->value,insert);
                    }
                }
            }
        }
    }
}

//返回一个map,有所有对应信息，比如原来的%4对应Phi后的%27
//在exit_block中生成往后走的phi
HashMap *insert_exit_phi(BasicBlock *exit_block,HashMap *exit_phi_map, Loop* loop){
    HashMap *match_phi_map = HashMapInit();

    HashMapFirst(exit_phi_map);
    for(Pair* pair1 = HashMapNext(exit_phi_map); pair1!=NULL; pair1 = HashMapNext(exit_phi_map)){
        Instruction *ins_phi = ins_new_zero_operator(Phi);
        Value *v_phi = ins_get_value_with_name_and_index(ins_phi,tmp_index++);
        v_phi->pdata->pairSet = HashSetInit();
        ins_phi->Parent = exit_block;
        //第一个pair是从head过来的
        pair *pp = (pair*) malloc(sizeof (pair));
        Value *v_key = pair1->key;
        pp->define = v_key;
        pp->from = loop->head;
        HashSetAdd(v_phi->pdata->pairSet, pp);

        HashSetFirst(pair1->value);
        for(pair *p = HashSetNext(pair1->value); p!=NULL; p = HashSetNext(pair1->value)){
            HashSetAdd(v_phi->pdata->pairSet,p);
        }
        InstNode *node_phi = new_inst_node(ins_phi);
        ins_insert_after(node_phi,exit_block->head_node);
        HashMapPut(match_phi_map, v_key ,v_phi);
    }
    return match_phi_map;
}

//更新loop head的phi中从原先tail到达的，改为从更新后的tail到达的
void update_head_phi(BasicBlock *prev_tail,BasicBlock* head, BasicBlock* new_tail){
    InstNode *curr = get_next_inst(head->head_node);
    while(curr->inst->Opcode == Phi){
        HashSetFirst(curr->inst->user.value.pdata->pairSet);
        for(pair *p = HashSetNext(curr->inst->user.value.pdata->pairSet); p!=NULL; p = HashSetNext(curr->inst->user.value.pdata->pairSet)){
            if(p->from == prev_tail)
                p->from = new_tail;
        }
        curr = get_next_inst(curr);
    }
}

//一次循环展开, 是icmp的模式
//返回新建立的，下一次使用的基本块
BasicBlock *copy_one_time_icmp(Loop* loop, BasicBlock* block,HashMap* v_new_valueMap,HashMap* other_new_valueMap, Instruction* ins_end,bool last, HashMap* exit_phi_map){
    BasicBlock *curr_block=NULL;
    BasicBlock *tail_curr_block = NULL;
    BasicBlock *prev_block = NULL;

    if(first_copy){
        Instruction *ins_tmp= ins_new_zero_operator(tmp);
        InstNode *node_tmp = new_inst_node(ins_tmp);
        ins_insert_before(node_tmp,loop->tail->tail_node);
    }

    curr_block = bb_create();
    curr_block->Parent = loop->head->Parent;
    if(!block && first_copy){
        curr_block->true_block = loop->tail->true_block;
        curr_block->false_block = loop->exit_block;
        bb_add_prev(loop->tail,  curr_block);
        bb_delete_one_prev(loop->head,loop->tail);
        loop->tail->true_block = curr_block;
        loop->tail->false_block = loop->exit_block;

        bb_add_prev(loop->tail,loop->exit_block);
    } else
    {
        curr_block->true_block = block->true_block;
        bb_add_prev(block,  curr_block);
        block->true_block = curr_block;
        bb_delete_one_prev(loop->head,block);
        block->false_block = loop->exit_block;
        bb_add_prev(block,loop->exit_block);
    }

    Value *compare = loop->inductionVariable;

    if(HashMapContain(exit_phi_map, compare)){
        HashSet* pairSet = HashMapGet(exit_phi_map,compare);
        HashSetFirst(pairSet);
        for(pair* check = HashSetNext(pairSet); check!=NULL; check = HashSetNext(pairSet)){
            if(!first_copy){
                if(check->from == block){
                    compare = check->define;
                    break;
                }
            }
            else{
                if(check->from == loop->head->true_block){
                    compare = check->define;
                    break;
                }
            }
        }
    }
    Instruction *ins_icmp = NULL;
    if(ins_end->Opcode == LESS)
        ins_icmp = ins_new_binary_operator(LESS, compare, ins_get_rhs(ins_end));
    else if(ins_end->Opcode == LESSEQ)
        ins_icmp = ins_new_binary_operator(LESSEQ,compare, ins_get_rhs(ins_end));
    else if(ins_end->Opcode == GREAT)
        ins_icmp = ins_new_binary_operator(GREAT,compare, ins_get_rhs(ins_end));
    else if(ins_end->Opcode == GREATEQ)
        ins_icmp = ins_new_binary_operator(GREATEQ,compare, ins_get_rhs(ins_end));
    else if(ins_end->Opcode == EQ)
        ins_icmp = ins_new_binary_operator(EQ,compare, ins_get_rhs(ins_end));
    else
        ins_icmp = ins_new_binary_operator(NOTEQ,compare, ins_get_rhs(ins_end));
    ins_get_value_with_name_and_index(ins_icmp,tmp_index++);
    InstNode * node_icmp = new_inst_node(ins_icmp);

    tail_curr_block = curr_block;
    prev_block = curr_block;

    if(!block && first_copy){
        ins_icmp->Parent = loop->tail;
        ins_insert_before(node_icmp, loop->tail->tail_node);
    } else {
        ins_icmp->Parent = block;
        ins_insert_before(node_icmp, block->tail_node);
    }

    //br_i1
    Instruction *ins_br_i1 = ins_new_unary_operator(br_i1, ins_get_dest(ins_icmp));
    InstNode *node_br_i1 = new_inst_node(ins_br_i1);
    if(!block && first_copy){
        ins_br_i1->Parent = loop->tail;
        ins_insert_before(node_br_i1, loop->tail->tail_node);
        //删掉原来的tail
        deleteIns(loop->tail->tail_node);

        loop->tail->tail_node = node_br_i1;       //原来的tail是另一条br
    } else {
        ins_br_i1->Parent = block;
        ins_insert_before(node_br_i1, block->tail_node);
        deleteIns(block->tail_node);
        block->tail_node = node_br_i1;
    }

    //label
    Instruction *ins_label = ins_new_zero_operator(Label);
    InstNode *node_label = new_inst_node(ins_label);
    ins_label->Parent = curr_block;
    curr_block->head_node = node_label;
    ins_insert_after(node_label,node_br_i1);

    //new_block的tail
    Instruction *ins_tail = ins_new_zero_operator(br);
    InstNode *node_tail = new_inst_node(ins_tail);
    ins_tail->Parent = curr_block;
    curr_block->tail_node = node_tail;
    ins_insert_after(node_tail,node_label);

    //和内联一样，准备一个map保存原block与现在block的对应关系
    HashMap *block_map = HashMapInit();

    //不能遍历loop body的set, set出来的顺序不对!!!
    InstNode *currNode = loop->head->true_block->head_node;
    InstNode *Tail = get_prev_inst(loop->tail->tail_node);
    while(currNode != Tail && currNode->inst->Opcode != tmp){
        BasicBlock *body = currNode->inst->Parent;

        //exit和head块不展开
        if(body!=loop->head && body!=loop->exit_block) {
            if (!hasNoDestOperator(currNode) || currNode->inst->Opcode == Store ||
                currNode->inst->Opcode == GIVE_PARAM) {
                Value *lhs = ins_get_lhs(currNode->inst);
                Value *rhs = ins_get_rhs(currNode->inst);
                Value *v_r = NULL;
                Value *v_l = NULL;
                Instruction *copy_ins = NULL;
                //如果v_new_valueMap中有，则要替换成最后使用值，而不能用原值
                if (HashMapContain(v_new_valueMap, lhs)) {
                    HashSet *set_replace = HashMapGet(v_new_valueMap, lhs);
                    v_l = get_replace_value(set_replace, loop->tail);           //CHECK: 更新值一定在最后吗,应该是吧，phi的话
                }
                    //变化过的中间变量
                    //比如%6= add nsw i32 %2,1，复制第一次后变为%8 = add nsw i32 %6,1
                    //做一个%6-----%8
                else if (HashMapContain(other_new_valueMap, lhs))
                    v_l = HashMapGet(other_new_valueMap, lhs);
//                    else if(isGlobalType(lhs->VTy))
                if(v_l == NULL)
                    v_l = lhs;
//                    else   //是无所谓的，比如是常数
//                        v_l=copy_value(lhs,tmp_index++);

                //如果有第二个操作数
                if (rhs != NULL) {
                    if (HashMapContain(v_new_valueMap, rhs)) {
                        HashSet *set_replace = HashMapGet(v_new_valueMap, rhs);
                        v_r = get_replace_value(set_replace, body);
                    }
                        //变化过的中间变量
                    else if (HashMapContain(other_new_valueMap, rhs))
                        v_r = HashMapGet(other_new_valueMap, rhs);
                    if(v_r == NULL)
                        v_r = rhs;
                    copy_ins = ins_new_binary_operator(currNode->inst->Opcode, v_l, v_r);
                } else
                    copy_ins = ins_new_unary_operator(currNode->inst->Opcode, v_l);

                InstNode *node = new_inst_node(copy_ins);

                node->inst->Parent = curr_block;
                ins_insert_before(node, curr_block->tail_node);

                //左值的对应关系也要存起来了
                bool has_dest = true;
                if (currNode->inst->Opcode == Call) {
                    Value *v_func = currNode->inst->user.use_list->Val;
                    if (v_func->pdata->symtab_func_pdata.return_type.ID == VoidTyID)
                        has_dest = false;
                }

                Value *new_dest = NULL;
                if (currNode->inst->Opcode != Store && currNode->inst->Opcode != GIVE_PARAM && has_dest)
                    new_dest = ins_get_value_with_name_and_index(copy_ins, tmp_index++);
                Value *v_dest = ins_get_dest(currNode->inst);

                //如果在另两个表中的对应value里有v_dest，要更新
                //如果v_new_valueMap有了新对应关系,比如本来记录了%2( %3) = phi i32[0 , %0], [%6 , %5]
                //然后other_new_valueMap中%6与它复制的下一条左值%8有了%6----%8对应，进行替换
                if (currNode->inst->Opcode != Store && currNode->inst->Opcode != GIVE_PARAM && has_dest) {
                    new_dest->alias = v_dest->alias;
                    new_dest->pdata = v_dest->pdata;
                    if (HashMapContain(other_new_valueMap, v_dest) && !mod_before)
                        update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap, v_dest), new_dest);
                    else
                        update_replace_value(v_new_valueMap, v_dest, new_dest);

                    HashMapPut(other_new_valueMap, v_dest, new_dest);
               //     goto L;
                }
            } else if (currNode->inst->Opcode == Label && currNode->inst->Parent == loop->tail) {
                //多基本块，但不用建新块
                //TODO 不知道有没有false_block的情况
                prev_block = curr_block;
                curr_block = tail_curr_block;
                HashMapPut(block_map, loop->tail, curr_block);
            } else if (currNode->inst->Opcode == Label) {
                //多基本块的话，第一条就会走到这里

                //建新块+label+br
                BasicBlock *more_block = bb_create();
                //先随便加个true_block关系
               // more_block->true_block = curr_block;
                HashMapPut(block_map, currNode->inst->Parent, more_block);
                more_block->Parent = currNode->inst->Parent->Parent;
               // adjust_blocks(curr_block, more_block, loop, true);
                //建一条Label
                Instruction *inst_label = ins_new_zero_operator(Label);
                InstNode *no_label = new_inst_node(inst_label);
                more_block->head_node = no_label;
                inst_label->Parent = more_block;
                if(!block && first_copy && prev_block == curr_block)
                    ins_insert_after(no_label,loop->tail->tail_node);
                else if(prev_block == curr_block)
                    ins_insert_after(no_label,block->tail_node);
                else
                    ins_insert_after(no_label,curr_block->tail_node);

                //建一条br
                Instruction *inst_br = ins_new_zero_operator(br);
                inst_br->Parent = more_block;
                InstNode *no_br = new_inst_node(inst_br);

                ins_insert_after(no_br,no_label);

                more_block->tail_node = no_br;
                prev_block = curr_block;
                curr_block = more_block;
            } else if (currNode->inst->Opcode == br || currNode->inst->Opcode == br_i1) {
                Instruction *inst_br_i1 = NULL;
                if (currNode->inst->Opcode == br)
                    inst_br_i1 = ins_new_zero_operator(br);
                else
                    inst_br_i1 = ins_new_unary_operator(br_i1, ins_get_dest(get_prev_inst(curr_block->tail_node)->inst));
                inst_br_i1->Parent = curr_block;
                InstNode *no_br_i1 = new_inst_node(inst_br_i1);
                ins_insert_before(no_br_i1, curr_block->tail_node);
                //删掉原来的tail
                deleteIns(curr_block->tail_node);
                curr_block->tail_node = no_br_i1;
            }
        }
        currNode = get_next_inst(currNode);
    }


    //一次循环跑完，更新一个出去时phi的值, 内容从v_new_map中取
    if(!last)
        update_exit_phi(curr_block, v_new_valueMap,exit_phi_map,loop);

    first_copy = false;

    if(HashSetSize(loop->loopBody) > 2)
        connect_blocks(block_map,loop,block);

    //将新块都加入loop body, 改变loop tail, 并改变head phi中的from
    if(last){
        BasicBlock *prev_tail = loop->tail;

        HashMapFirst(block_map);
        for(Pair* p = HashMapNext(block_map); p!=NULL; p= HashMapNext(block_map)){
            BasicBlock *b_new = p->value;
            if(p->key == loop->tail)
                loop->tail = b_new;
            HashSetAdd(loop->loopBody, b_new);
        }

        //TODO 目前只打算修改head的，可能多基本块之后还要改别的
        update_head_phi(prev_tail,loop->head,loop->tail);
    }

    return curr_block;
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
                if(!hasNoDestOperator(currNode) || currNode->inst->Opcode == Store  || currNode->inst->Opcode == GIVE_PARAM){
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
//                    else if(isGlobalType(lhs->VTy))
                    else
                        v_l = lhs;
//                    else   //是无所谓的，比如是常数
//                        v_l=copy_value(lhs,tmp_index++);

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
//                        else if(isGlobalType(rhs->VTy))
                        else
                            v_r = rhs;
//                        else   //是无所谓的，比如是常数
//                            v_r=copy_value(rhs,tmp_index);
                        copy_ins= ins_new_binary_operator(currNode->inst->Opcode,v_l,v_r);
                    }
                    else
                        copy_ins= ins_new_unary_operator(currNode->inst->Opcode,v_l);

                    InstNode *node = new_inst_node(copy_ins);
                    if(mod_flag){
                        node->inst->Parent = block;
                        ins_insert_before(node,block->tail_node);
                    }
                    else{
                        node->inst->Parent=loop->tail;
                        ins_insert_before(node,body->tail_node);
                    }

                    //左值的对应关系也要存起来了
                    bool has_dest = true;
                    if(currNode->inst->Opcode == Call){
                        Value *v_func = currNode->inst->user.use_list->Val;
                        if(v_func->pdata->symtab_func_pdata.return_type.ID == VoidTyID)
                            has_dest = false;
                    }

                    Value *new_dest = NULL;
                    if(currNode->inst->Opcode != Store && currNode->inst->Opcode != GIVE_PARAM && has_dest)
                        new_dest= ins_get_value_with_name_and_index(copy_ins,tmp_index++);
                    Value *v_dest=ins_get_dest(currNode->inst);

                    //如果在另两个表中的对应value里有v_dest，要更新
                    //如果v_new_valueMap有了新对应关系,比如本来记录了%2( %3) = phi i32[0 , %0], [%6 , %5]
                    //然后other_new_valueMap中%6与它复制的下一条左值%8有了%6----%8对应，进行替换
                    if(currNode->inst->Opcode != Store && currNode->inst->Opcode != GIVE_PARAM && has_dest){
                        new_dest->alias = v_dest->alias;
                        new_dest->pdata = v_dest->pdata;
                        if(HashMapContain(other_new_valueMap,v_dest) && !mod_before)
                            update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                        else
                            update_replace_value(v_new_valueMap,v_dest,new_dest);
                        HashMapPut(other_new_valueMap,v_dest ,new_dest);
                    }
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
                if(!hasNoDestOperator(currNode) || currNode->inst->Opcode == Store || currNode->inst->Opcode == GIVE_PARAM){
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
//                        if(first_time || isGlobalType(lhs->VTy) || is)
                            v_l = lhs;
//                        else
//                            v_l=copy_value(lhs,tmp_index++);
                    }

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
//                            if(first_time || isGlobalType(rhs->VTy))
                                v_r = rhs;
//                            else
//                                v_r=copy_value(rhs,tmp_index++);
                        }
                        copy_ins= ins_new_binary_operator(currNode->inst->Opcode,v_l,v_r);
                    }
                    else
                        copy_ins= ins_new_unary_operator(currNode->inst->Opcode,v_l);

                    InstNode *node = new_inst_node(copy_ins);

                    node->inst->Parent = block;
                    ins_insert_before(node,block->tail_node);


                    //左值的对应关系也要存起来了
                    bool has_dest = true;
                    if(currNode->inst->Opcode == Call){
                        Value *v_func = currNode->inst->user.use_list->Val;
                        if(v_func->pdata->symtab_func_pdata.return_type.ID == VoidTyID)
                            has_dest = false;
                    }

                    Value *new_dest = NULL;
                    if(currNode->inst->Opcode != Store && currNode->inst->Opcode != GIVE_PARAM && has_dest)
                         new_dest= ins_get_value_with_name_and_index(copy_ins,tmp_index++);
                    Value *v_dest=ins_get_dest(currNode->inst);

                    //如果在另两个表中的对应value里有v_dest，要更新
                    //如果v_new_valueMap有了新对应关系,比如本来记录了%2( %3) = phi i32[0 , %0], [%6 , %5]
                    //然后other_new_valueMap中%6与它复制的下一条左值%8有了%6----%8对应，进行替换
                    if(currNode->inst->Opcode != Store && currNode->inst->Opcode != GIVE_PARAM && has_dest){
                        new_dest->alias = v_dest->alias;
                        new_dest->pdata = v_dest->pdata;
                        if(HashMapContain(other_new_valueMap,v_dest)){
                            update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                        }
                        else{
                            update_replace_value_mod(v_new_valueMap,v_dest,new_dest,loop);
                        }
                        HashMapPut(other_new_valueMap,v_dest ,new_dest);
                    }
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
            if(!hasNoDestOperator(currNode) || currNode->inst->Opcode == Store){
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

bool LOOP_UNROLL_EACH(Loop* loop)
{
    //TODO 目前不做基本块间
//    if(HashSetSize(loop->loopBody) > 2 || HashSetSize(loop->child) != 0 || !loop->hasDedicatedExit)
//        return false;

    if(!loop->hasDedicatedExit)
        return false;

    if(!loop->initValue || !loop->modifier || !loop->end_cond)
        return false;

    //全局为条件的也不做
    if(loop->conditionChangeWithinLoop)
        return false;

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
        return false;

    int cnt=cal_ir_cnt(loop->loopBody);
    //如果是常数，就能计算迭代次数，然后进行一个判断,TODO 非常数就不判断了吗
    Value *times = (Value*) malloc(sizeof (Value));
    if(check_idc(loop->initValue,v_step,v_end)){
        value_init_int(times, cal_times(loop->initValue->pdata->var_pdata.iVal,ins_modifier,ins_end_cond,v_step,v_end));
        //超出一定长度的循环，不进行展开
        if((long)times*cnt>loop_unroll_up_lines)
            return false;
    }

    //一个map用来保存header中变量与最新值的对应关系
    //value*----pair的hashset
    HashMap *v_new_valueMap=HashMapInit();

    //一个map初始值与上一个map相同，为了最后exit_block跳出的phi准备, 删去preHeader进来的
    HashMap* exit_phi_map = HashMapInit();

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
        HashSet *exit_phi_set = HashSetInit();
        HashMapPut(v_new_valueMap,variable,map_phi_set);
        HashMapPut(exit_phi_map,variable,exit_phi_set);

        HashSet *phiSet = head_currNode->inst->user.value.pdata->pairSet;
        HashSetFirst(phiSet);
        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
            //直接装进去
            HashSetAdd(map_phi_set,phiInfo);
            if(HashSetFind(loop->loopBody,phiInfo->from)){
                //不用一个地址，因为需要保留
                pair* restore_phiInfo = (pair*) malloc(sizeof (pair));
                restore_phiInfo->from = phiInfo->from;
                restore_phiInfo->define = phiInfo->define;
                HashSetAdd(exit_phi_set,restore_phiInfo);
            }
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
            adjust_blocks(loop->head,new_pre_block,loop,false);
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
        }
        //余数处理完毕，开始循环内容的复制
        for(int i = 0;i < update_modifier-1;i++){
            copy_one_time(loop, 0, NULL, v_new_valueMap,other_new_valueMap);
            mod_before = false;
        }
    }
    else {
        BasicBlock *cur_block = NULL;
        for(int i = 0;i < update_modifier-1;i++){
            if(i==0)
                cur_block = copy_one_time_icmp(loop,NULL, v_new_valueMap,other_new_valueMap,ins_end_cond,false,exit_phi_map);
            else if(i+1 == update_modifier-1)
                cur_block = copy_one_time_icmp(loop,cur_block, v_new_valueMap,other_new_valueMap,ins_end_cond,true,exit_phi_map);
            else
                cur_block = copy_one_time_icmp(loop,cur_block, v_new_valueMap,other_new_valueMap,ins_end_cond,false,exit_phi_map);
        }
        //给exit_block加上一些Phi
        HashMap *match_phi_map = insert_exit_phi(loop->exit_block,exit_phi_map,loop);
        //TODO 进行value的替换
        HashMapFirst(match_phi_map);
        for(Pair* p = HashMapNext(match_phi_map); p!=NULL; p = HashMapNext(match_phi_map)){
            specialValueReplace(p->key, p->value, loop->exit_block);
        }

        first_copy = true;
    }

    //遍历loop, 删去作挡板的tmp
    HashSetFirst(loop->loopBody);
    for(BasicBlock* body = HashSetNext(loop->loopBody);body!=NULL;body = HashSetNext(loop->loopBody)){
        InstNode *curr= body->head_node;
        InstNode *tail = body->tail_node;
        while (curr!=tail && curr!=NULL){
            if(curr->inst->Opcode == tmp){         //一个block应该只有一条挡板
                deleteIns(curr);
                break;
            }
            curr = get_next_inst(curr);
        }
    }

    first_copy = true;
    return true;
}

//进行dfs，从最里层一步步展开
bool dfsLoop(Loop *loop){
    bool effective = false;
    HashSetFirst(loop->child);
    for(Loop *childLoop = HashSetNext(loop->child); childLoop != NULL; childLoop = HashSetNext(loop->child)){
        /*内层循环先处理*/
        effective |= dfsLoop(childLoop);
    }
    first_copy = true;
    effective |= LOOP_UNROLL_EACH(loop);
    return effective;
}

void loop_unroll(Function *currentFunction)
{
    HashSetFirst(currentFunction->loops);
    //遍历每个loop
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        dfsLoop(root);
    }
}