#include "loop_unroll.h"

int tmp_index = 0;

bool first_copy = true;
bool mod_before = false;

bool have_more_block = false;

HashSet* funcSet; //暂定同一个func只用一种方式

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

void connect_blocks(HashMap* block_map,Loop* loop,BasicBlock* block){
    HashMapFirst(block_map);
    for(Pair* p = HashMapNext(block_map); p!=NULL ;p = HashMapNext(block_map)){
        BasicBlock *b_loop = p->key;
        BasicBlock *b_new = p->value;
        b_new->true_block = NULL; b_new->false_block = NULL;
        if(block!=b_new)
            b_new->preBlocks = HashSetInit();

        if(b_loop == loop->head->true_block){
            if(!block){
                if(loop->tail->true_block)
                    bb_delete_one_prev(loop->tail->true_block,loop->tail);
                loop->tail->true_block = b_new;
                bb_add_prev(loop->tail,b_new);
            } else {
                block->true_block = b_new;
                if(block!=b_new)
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
    HashSetFirst(loopBody);
    for(BasicBlock *body = HashSetNext(loopBody); body != NULL; body = HashSetNext(loopBody)){
        InstNode *currNode = body->head_node;
        InstNode *bodyTail = body->tail_node;
        while(currNode != bodyTail){
            ir_cnt++;
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

InstNode *insert_ir_mod(Value* v_init, Value* v_end, Value* v_step, BasicBlock* preserve1,  BasicBlock *phi_block, BasicBlock* mod_block, InstNode* pos){
    //times = ((end - init) % step == 0)? (end - init) / step : (end - init) / step + 1;
    //给preserve block一个label
    Value *v_update_modifier = (Value*) malloc(sizeof (Value));
    value_init_int(v_update_modifier, update_modifier);

    Instruction *ins_label = ins_new_zero_operator(Label);
    InstNode *node_label = new_inst_node(ins_label);
    ins_label->Parent = preserve1;
    ins_insert_before(node_label, pos);
    preserve1->head_node = node_label;
    InstNode *node_sub = auto_binary_node_insert_after(Sub,v_end,v_init,tmp_index++,preserve1,node_label);
    InstNode *node_div = auto_binary_node_insert_after(Div,ins_get_dest(node_sub->inst),v_step,tmp_index++,preserve1,node_sub);

    InstNode *node_ret = auto_binary_node_insert_after(Mod, ins_get_dest(node_div->inst),v_update_modifier,tmp_index++,preserve1,node_div);

    InstNode *node_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_sub->inst),v_step,tmp_index++,preserve1,node_ret);

    preserve1->false_block = phi_block;
    preserve1->true_block = mod_block;
    bb_add_prev(preserve1,phi_block);    //TODO

    //icmp和br_i1
    Value *v_zero = (Value*) malloc(sizeof (Value));
    value_init_int(v_zero,0);
    InstNode *node_icmp = auto_binary_node_insert_after(NOTEQ, ins_get_dest(node_mod->inst),v_zero,tmp_index++,preserve1,node_mod);
    Instruction *ins_br_i1 = ins_new_unary_operator(br_i1, ins_get_dest(node_icmp->inst));
    ins_br_i1->Parent = preserve1;
    InstNode *node_br_i1 = new_inst_node(ins_br_i1);
    ins_insert_after(node_br_i1,node_icmp);
    preserve1->tail_node = node_br_i1;

    //填充mod块,需要做一个+1,br的话mod_block进来之前就已经有了
    Value *v_one = (Value*) malloc(sizeof (Value));
    value_init_int(v_one,1);
    InstNode *node_plus = auto_binary_node_insert_after(Add, ins_get_dest(node_ret->inst),v_one,tmp_index++,mod_block,mod_block->head_node);
    Instruction *ins_br_mod = ins_new_zero_operator(br);
    InstNode *node_br_mod = new_inst_node(ins_br_mod);
    ins_br_mod->Parent = mod_block;
    ins_insert_after(node_br_mod,node_plus);
    mod_block->tail_node = node_br_mod;

    //phi_block的label
    Instruction *ins_phi_label = ins_new_zero_operator(Label);
    InstNode *node_phi_label = new_inst_node(ins_phi_label);
    ins_insert_after(node_phi_label,node_br_mod);
    ins_phi_label->Parent = phi_block;
    phi_block->head_node = node_phi_label;

    Instruction *ins_phi = ins_new_zero_operator(Phi);
    ins_phi->Parent = phi_block;
    HashSet *set = HashSetInit();
    pair *pair1 = (pair*) malloc(sizeof (pair));
    pair *pair2 = (pair*) malloc(sizeof (pair));
    pair1->from =preserve1;
    pair1->define = ins_get_dest(node_ret->inst);
    pair2->from = mod_block;
    pair2->define = ins_get_dest(node_plus->inst);
    HashSetAdd(set,pair1);
    HashSetAdd(set,pair2);
    ins_get_value_with_name_and_index(ins_phi,tmp_index++)->pdata->pairSet = set;
    InstNode *node_phi = new_inst_node(ins_phi);
    ins_insert_after(node_phi,node_phi_label);

    //phi block的br, br到new_pre_block
    Instruction *ins_phi_br = ins_new_zero_operator(br);
    InstNode *node_phi_br = new_inst_node(ins_phi_br);
    ins_phi_br->Parent = phi_block;
    ins_insert_after(node_phi_br,node_phi);
    phi_block->tail_node = node_phi_br;
    return node_phi;
}

//非常数余数计算
//如果返回NULL代表确定的一次不走或wrong，返回int类型常数1表示一定走的这种
InstNode *get_mod(Value* v_init,Instruction *ins_modifier,Instruction *ins_end_cond,Value *v_step,Value *v_end,BasicBlock *mod_block,BasicBlock *new_pre_block,InstNode* pos,Loop* loop,BasicBlock** phi_block){
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
                case NOTEQ:
                    node_sub = auto_binary_node_insert_after(Sub,v_end,v_init,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_sub->inst),v_step,tmp_index++,mod_block,node_sub);
                    node_get_mod = auto_binary_node_insert_after(Mod,ins_get_dest(node_get_times->inst),v_update_modifier,tmp_index++,mod_block,node_get_times);
                    break;
                case LESSEQ: case GREATEQ:
                    node_sub = NULL;
                    node_sub = auto_binary_node_insert_after(Sub,v_end,v_init,tmp_index++,mod_block,pos);
                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_sub->inst),v_step,tmp_index++,mod_block,node_sub);
                    value_init_int(plus,1);
                    node_plus = auto_binary_node_insert_after(Add, ins_get_dest(node_get_times->inst),plus,tmp_index++,mod_block,node_get_times);
                    node_get_mod = auto_binary_node_insert_after(Mod, ins_get_dest(node_plus->inst),v_update_modifier,tmp_index++,mod_block,node_plus); break;

                case LESS: case GREAT:
                    //我哭死了，你还要分类讨论, 一个基本块该不会还放不下你吧....还真放不下
                    //来个新block,block在mod_block之前
                    have_more_block = true;
                    preserve1 = bb_create();
                    preserve1->Parent = mod_block->Parent;
                    (*phi_block)->Parent = mod_block->Parent;
                    adjust_blocks(mod_block,preserve1, loop);
                    adjust_blocks(new_pre_block,*phi_block,loop);
                    node_get_mod = insert_ir_mod(v_init,v_end,v_step,preserve1,*phi_block,mod_block,pos);
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
                    have_more_block = true;
                    preserve1 = bb_create();
                    preserve1->Parent = mod_block->Parent;
                    (*phi_block)->Parent = mod_block->Parent;
                    adjust_blocks(mod_block,preserve1, loop);
                    adjust_blocks(new_pre_block,*phi_block,loop);
                    node_get_mod = insert_ir_mod(v_end,v_init,v_step,preserve1,*phi_block,mod_block,pos);
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

bool have_phi_from_block(HashSet *curr_pair_set,BasicBlock* block,HashMap* block_map){
    HashSetFirst(curr_pair_set);
    for(pair* phiInfo = HashSetNext(curr_pair_set); phiInfo!=NULL; phiInfo = HashSetNext(curr_pair_set)){
        if(HashMapGet(block_map,block) == phiInfo->from)
            return true;
    }
    return false;
}
void update_replace_value_with_block(HashMap* map,Value* v_before,Value* v_replace,BasicBlock* block)
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
                pp->from = block;
                HashSetAdd(pairSet,pp);
            }
        }
    }
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

void adjust_init_phi(Loop *loop,HashMap *v_match_phiMap){
    HashMapFirst(v_match_phiMap);
    for(Pair *p = HashMapNext(v_match_phiMap); p != NULL; p = HashMapNext(v_match_phiMap)){
        Instruction *init_phi = (Instruction*)p->key;
        Instruction *pre_phi = (Instruction*)p->value;

        HashSet *pairSet = ins_get_dest(init_phi)->pdata->pairSet;
        HashSetFirst(pairSet);
        for(pair* pp= HashSetNext(pairSet);pp!=NULL;pp= HashSetNext(pairSet))
        {
            //更新初值的到达块为new_pre_block
            if(!HashSetFind(loop->loopBody, pp->from)){
                pp->from = pre_phi->Parent;
                pp->define = (Value*)p->value;
            }
        }
    }
}

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

//将phi的信息补充完整
void reduce_phi_(HashMap* phi_map,HashMap* other_value_map,HashMap* block_map,HashMap* v_new_valueMap,Loop* loop){
    HashMapFirst(phi_map);
    for(Pair* p = HashMapNext(phi_map); p!=NULL; p = HashMapNext(phi_map)){
        Value *v_key = p->key;
        Value *v_value = p->value;

        HashSetFirst(v_key->pdata->pairSet);
        for(pair *pp = HashSetNext(v_key->pdata->pairSet); pp!=NULL ;pp = HashSetNext(v_key->pdata->pairSet)){
            //块a对块b具有支配关系的才新加，因为别的之前的都加过了
            if(!have_phi_from_block(v_value->pdata->pairSet,pp->from,block_map)){
                pair* pair1 = (pair*) malloc(sizeof (pair));
                pair1->from = HashMapGet(block_map,pp->from);

                Value *num = (Value*) malloc(sizeof (Value));
                if(pp->define == NULL)
                    num = NULL;
                else if(pp->define->VTy->ID == Int){
                    value_init_int(num,pp->define->pdata->var_pdata.iVal);
                }else if(pp->define->VTy->ID == Float){
                    value_init_float(num,pp->define->pdata->var_pdata.fVal);
                }else if(HashMapContain(other_value_map,pp->define)){
                    num = HashMapGet(other_value_map,pp->define);
                } else {
                    //在v_new_valueMap中
                    num = get_replace_value(HashMapGet(v_new_valueMap,pp->define),loop->tail);
                }

                pair1->define = num;
                HashSetAdd(v_value->pdata->pairSet,pair1);
            }
        }
    }
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

//mod_flag表示是不是mod的地方需要复制，如果是mod那边需要，就必须给插入位置block
BasicBlock *copy_one_time(Loop* loop, bool mod_flag,BasicBlock* block,HashMap* v_new_valueMap,HashMap* other_new_valueMap,bool last,HashMap* phi_map){
    if(first_copy && !mod_flag){
        Instruction *ins_tmp= ins_new_zero_operator(tmp);
        InstNode *node_tmp = new_inst_node(ins_tmp);
        ins_insert_before(node_tmp,loop->tail->tail_node);
    }

    BasicBlock *curr_block = NULL,*prev_block = NULL;
    HashMap *block_map = HashMapInit();

    //不能遍历loop body的set, set出来的顺序不对!!!
    InstNode *currNode = loop->head->true_block->head_node;
    InstNode *Tail = get_prev_inst(loop->tail->tail_node);
    while(currNode != Tail && currNode->inst->Opcode != tmp) {
        BasicBlock *body = currNode->inst->Parent;

        //exit和head块不展开
        if(body!=loop->head && body!=loop->exit_block){
            if((!hasNoDestOperator(currNode) || currNode->inst->Opcode == Store ||
                currNode->inst->Opcode == GIVE_PARAM) && currNode->inst->Opcode != Phi){
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
                if(!v_l)
                    v_l = lhs;

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
                    if(!v_r)
                        v_r = rhs;
                    copy_ins= ins_new_binary_operator(currNode->inst->Opcode,v_l,v_r);
                }
                else
                    copy_ins= ins_new_unary_operator(currNode->inst->Opcode,v_l);

                //TODO 其实还没考虑多基本块
                InstNode *node = new_inst_node(copy_ins);

//                if(mod_flag){
//                    node->inst->Parent = block;
//                    ins_insert_before(node,block->tail_node);
//                }
                if(HashSetSize(loop->loopBody) <= 2){
                    node->inst->Parent=loop->tail;
                    ins_insert_before(node,loop->tail->tail_node);
                } else
                {
                    node->inst->Parent = curr_block;
                    ins_insert_before(node,curr_block->tail_node);
                }

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
                if(currNode->inst->Opcode != Store && currNode->inst->Opcode != GIVE_PARAM && has_dest) {
                    new_dest->alias = v_dest->alias;
                    new_dest->pdata->var_pdata.iVal = v_dest->pdata->var_pdata.iVal;
                    new_dest->pdata->var_pdata.is_offset = v_dest->pdata->var_pdata.is_offset;
                    new_dest->VTy = v_dest->VTy;

                    if(HashMapContain(other_new_valueMap,v_dest) && !mod_before)
                        update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                    else
                        update_replace_value(v_new_valueMap,v_dest,new_dest);
                    HashMapPut(other_new_valueMap,v_dest ,new_dest);
                }
            } else if(currNode->inst->Opcode == Phi){
                Instruction *ins_phi = ins_new_zero_operator(Phi);
                InstNode *node_phi = new_inst_node(ins_phi);
                Value *v_phi = ins_get_value_with_name_and_index(ins_phi,tmp_index++);
                //v_phi->pdata = ins_get_dest(currNode->inst)->pdata;
                v_phi->alias = ins_get_dest(currNode->inst)->alias;
                v_phi->IsPhi = true;
                v_phi->VTy = currNode->inst->user.value.VTy;
                v_phi->pdata->pairSet = HashSetInit();
                ins_phi->Parent = curr_block;
                ins_insert_before(node_phi,curr_block->tail_node);

                //将能加上的pair加上
                bool need_phi_reduce = false;
                HashSetFirst(currNode->inst->user.value.pdata->pairSet);
                for(pair *pp = HashSetNext(currNode->inst->user.value.pdata->pairSet); pp!=NULL ;pp = HashSetNext(currNode->inst->user.value.pdata->pairSet)){
                    if(HashMapContain(block_map,pp->from)){
                        //加入目前已存在的
                        pair* pair1 = (pair*) malloc(sizeof (pair));
                        pair1->from = HashMapGet(block_map,pp->from);

                        Value *num = (Value*) malloc(sizeof (Value));
                        if(pp->define == NULL)
                            num = NULL;
                        else if(pp->define->VTy->ID == Int){
                            value_init_int(num,pp->define->pdata->var_pdata.iVal);
                        }else if(pp->define->VTy->ID == Float){
                            value_init_float(num,pp->define->pdata->var_pdata.fVal);
                        }else if(HashMapContain(other_new_valueMap,pp->define)){
                            num = HashMapGet(other_new_valueMap,pp->define);
                        } else {
                            //在v_new_valueMap中
                            num = get_replace_value(HashMapGet(v_new_valueMap,pp->define),loop->tail);
                        }

                        pair1->define = num;
                        HashSetAdd(v_phi->pdata->pairSet,pair1);
                    } else
                        need_phi_reduce = true;
                }

                //------------------
                if(need_phi_reduce)
                    HashMapPut(phi_map,&currNode->inst->user.value,v_phi);

                if (HashMapContain(other_new_valueMap, ins_get_dest(currNode->inst)) && !mod_before)
                    update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap, ins_get_dest(currNode->inst)), v_phi);
                else
                    update_replace_value(v_new_valueMap, ins_get_dest(currNode->inst), v_phi);

                HashMapPut(other_new_valueMap, ins_get_dest(currNode->inst), v_phi);

            }else if (HashSetSize(loop->loopBody) > 2 && currNode->inst->Opcode == Label) {

                //建新块+label+br
                BasicBlock *more_block = bb_create();
                //先随便加个true_block关系
                HashMapPut(block_map, currNode->inst->Parent, more_block);
                more_block->Parent = currNode->inst->Parent->Parent;
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

            currNode = get_next_inst(currNode);
        }
    }

    first_copy = false;

    if(HashSetSize(loop->loopBody) > 2){
        connect_blocks(block_map,loop,block);
        reduce_phi_(phi_map,other_new_valueMap,block_map,v_new_valueMap,loop);
        HashMapClean(phi_map);
    }

    if(last && curr_block!=NULL){
        BasicBlock *prev_tail = loop->tail;
        curr_block->true_block = loop->head;
        bb_add_prev(curr_block,loop->head);
        HashMapFirst(block_map);
        for(Pair* p = HashMapNext(block_map); p!=NULL; p= HashMapNext(block_map)){
            BasicBlock *b_new = p->value;
            if(p->key == loop->tail)
                loop->tail = b_new;
            HashSetAdd(loop->loopBody, b_new);
            Loop *trans = loop->parent;
            while (trans){
                HashSetAdd(trans->loopBody, b_new);
                trans = trans->parent;
            }
        }

        update_head_phi(prev_tail,loop->head,loop->tail);
    } else {
        HashMapFirst(block_map);
        for(Pair* p = HashMapNext(block_map); p!=NULL; p= HashMapNext(block_map)){
            BasicBlock *b_new = p->value;
            HashSetAdd(loop->loopBody, b_new);
            Loop *trans = loop->parent;
            while (trans){
                HashSetAdd(trans->loopBody, b_new);
                trans = trans->parent;
            }
        }
    }
    return curr_block;
}

//TODO 目前还是基本块内, 完全一样地复制
//返回一个map,表明每个value的更新情况
BasicBlock *copy_for_mod(Loop* loop, BasicBlock* block, HashMap* v_new_valueMap,HashMap* other_new_valueMap,bool mod,HashMap* v_new_phiMap,HashMap* v_match_phi_map,bool last,HashMap* phi_map){
    HashMap *block_map = HashMapInit();

    if(first_copy){
        Instruction *ins_tmp= ins_new_zero_operator(tmp);
        InstNode *node_tmp = new_inst_node(ins_tmp);
        ins_insert_before(node_tmp,loop->tail->tail_node);
    }

    BasicBlock *curr_block = block;
    BasicBlock *prev_block = block;

    //不能遍历loop body的set, set出来的顺序不对!!!
    InstNode *currNode = loop->head->true_block->head_node;
    InstNode *Tail = get_prev_inst(loop->tail->tail_node);
    while(currNode != Tail && currNode->inst->Opcode != tmp) {
        BasicBlock *body = currNode->inst->Parent;

        //exit和head块不展开
        if(body!=loop->head && body!=loop->exit_block){
            if((!hasNoDestOperator(currNode) || currNode->inst->Opcode == Store ||
                currNode->inst->Opcode == GIVE_PARAM) && currNode->inst->Opcode != Phi){
                Value *lhs= ins_get_lhs(currNode->inst);
                Value *rhs= ins_get_rhs(currNode->inst);
                Value *v_r=NULL;
                Value *v_l=NULL;
                Instruction *copy_ins=NULL;
                //如果v_new_valueMap中有，则要替换成最后使用值，而不能用原值
                if(v_new_phiMap == NULL && HashMapContain(v_new_valueMap,lhs))
                {
                    HashSet *set_replace=HashMapGet(v_new_valueMap,lhs);
                    v_l=get_replace_value_first(set_replace,loop);
                }
                else if(HashMapContain(v_match_phi_map,lhs))
                    v_l = HashMapGet(v_match_phi_map,lhs);
                else if(v_new_phiMap && HashMapContain(v_new_phiMap,lhs)){
                    HashSet *set_replace=HashMapGet(v_new_phiMap,lhs);
                    v_l=get_replace_value_first(set_replace,loop);
                }
                    //变化过的中间变量
                    //比如%6= add nsw i32 %2,1，复制第一次后变为%8 = add nsw i32 %6,1
                    //做一个%6-----%8                                                       //CHECK: 这个地方应该是不用考虑参数的吧
                else if(HashMapContain(other_new_valueMap,lhs))
                    v_l= HashMapGet(other_new_valueMap,lhs);

                if(v_l == NULL)
                    v_l = lhs;

                //如果有第二个操作数
                if(rhs!=NULL)
                {
                    if(v_new_phiMap == NULL && HashMapContain(v_new_valueMap,rhs))
                    {
                        HashSet *set_replace=HashMapGet(v_new_valueMap,rhs);
                        v_r=get_replace_value_first(set_replace,loop);
                    }
                    else if(HashMapContain(v_match_phi_map,rhs))
                        v_r = HashMapGet(v_match_phi_map,rhs);
                    else if(v_new_phiMap && HashMapContain(v_new_phiMap,rhs)){
                        HashSet *set_replace=HashMapGet(v_new_phiMap,rhs);
                        v_r=get_replace_value_first(set_replace,loop);
                    }
                        //变化过的中间变量
                    else if(HashMapContain(other_new_valueMap,rhs))
                        v_r= HashMapGet(other_new_valueMap,rhs);

                    if(v_r == NULL)
                        v_r = rhs;

                    copy_ins= ins_new_binary_operator(currNode->inst->Opcode,v_l,v_r);
                }
                else
                    copy_ins= ins_new_unary_operator(currNode->inst->Opcode,v_l);

                InstNode *node = new_inst_node(copy_ins);

                node->inst->Parent = curr_block;
                ins_insert_before(node,curr_block->tail_node);

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
                //TODO 其他v_new_phiMap的使用
                if(currNode->inst->Opcode != Store && currNode->inst->Opcode != GIVE_PARAM && has_dest){
                    new_dest->alias = v_dest->alias;
                    new_dest->pdata->var_pdata.iVal = v_dest->pdata->var_pdata.iVal;
                    new_dest->pdata->var_pdata.is_offset = v_dest->pdata->var_pdata.is_offset;
                    new_dest->VTy = v_dest->VTy;
                    if(v_new_phiMap == NULL){
                        if(HashMapContain(other_new_valueMap,v_dest)){
                            update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                        }
                        else{
                            update_replace_value_mod(v_new_valueMap,v_dest,new_dest,loop);
                        }
                    } else {
                        if(HashMapContain(other_new_valueMap,v_dest)){
                            update_replace_value(v_new_phiMap, HashMapGet(other_new_valueMap,v_dest),new_dest);
                        }
                        else{
                            update_replace_value_with_block(v_new_phiMap,v_dest,new_dest,block);
                        }
                    }
                }

                HashMapPut(other_new_valueMap,v_dest ,new_dest);
            }  else if(currNode->inst->Opcode == Phi){
                Instruction *ins_phi = ins_new_zero_operator(Phi);
                InstNode *node_phi = new_inst_node(ins_phi);
                Value *v_phi = ins_get_value_with_name_and_index(ins_phi,tmp_index++);
                //v_phi->pdata = ins_get_dest(currNode->inst)->pdata;
                v_phi->alias = ins_get_dest(currNode->inst)->alias;
                v_phi->IsPhi = true;
                v_phi->VTy = currNode->inst->user.value.VTy;
                v_phi->pdata->pairSet = HashSetInit();
                ins_phi->Parent = curr_block;
                ins_insert_before(node_phi,curr_block->tail_node);

                //将能加上的pair加上
                bool need_phi_reduce = false;
                HashSetFirst(currNode->inst->user.value.pdata->pairSet);
                for(pair *pp = HashSetNext(currNode->inst->user.value.pdata->pairSet); pp!=NULL ;pp = HashSetNext(currNode->inst->user.value.pdata->pairSet)){
                    if(HashMapContain(block_map,pp->from)){
                        //加入目前已存在的
                        pair* pair1 = (pair*) malloc(sizeof (pair));
                        pair1->from = HashMapGet(block_map,pp->from);

                        Value *num = (Value*) malloc(sizeof (Value));
                        if(pp->define == NULL)
                            num = NULL;
                        else if(pp->define->VTy->ID == Int){
                            value_init_int(num,pp->define->pdata->var_pdata.iVal);
                        }else if(pp->define->VTy->ID == Float){
                            value_init_float(num,pp->define->pdata->var_pdata.fVal);
                        }else if(HashMapContain(other_new_valueMap,pp->define)){
                            num = HashMapGet(other_new_valueMap,pp->define);
                        } else if(HashMapContain(v_match_phi_map,pp->define)){
                            num = HashMapGet(v_match_phi_map,pp->define);
                        } else if(v_new_phiMap!=NULL){          //TODO 不知道为什么要加这个
                            num = get_replace_value_first(HashMapGet(v_new_phiMap,pp->define),loop);
                        } else {
                            //在v_new_valueMap中
                            num = get_replace_value(HashMapGet(v_new_valueMap,pp->define),loop->tail);
                        }

                        pair1->define = num;
                        HashSetAdd(v_phi->pdata->pairSet,pair1);
                    } else
                        need_phi_reduce = true;
                }

                //------------------
                if(need_phi_reduce)
                    HashMapPut(phi_map,&currNode->inst->user.value,v_phi);

                if(v_new_phiMap == NULL){
                    if(HashMapContain(other_new_valueMap,ins_get_dest(currNode->inst))){
                        update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap, ins_get_dest(currNode->inst)), v_phi);
                    }
                    else{
                        update_replace_value_mod(v_new_valueMap,ins_get_dest(currNode->inst), v_phi,loop);
                    }
                } else {
                    if(HashMapContain(other_new_valueMap,ins_get_dest(currNode->inst))){
                        update_replace_value(v_new_phiMap, HashMapGet(other_new_valueMap, ins_get_dest(currNode->inst)), v_phi);
                    }
                    else{
                        update_replace_value_with_block(v_new_phiMap,ins_get_dest(currNode->inst), v_phi,block);
                    }
                }

                HashMapPut(other_new_valueMap, ins_get_dest(currNode->inst), v_phi);

            } else if (HashSetSize(loop->loopBody) > 2 && currNode->inst->Opcode == Label) {

                if(!mod){
                    //建新块+label+br
                    BasicBlock *more_block = bb_create();
                    //先随便加个true_block关系
                    HashMapPut(block_map, currNode->inst->Parent, more_block);
                    more_block->Parent = currNode->inst->Parent->Parent;
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
                } else{
                    mod = 0;
                    HashMapPut(block_map, currNode->inst->Parent, block);
                }

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

            currNode = get_next_inst(currNode);
        }
    }

    if(HashSetSize(loop->loopBody) > 2){
        connect_blocks(block_map,loop,block);
        reduce_phi_(phi_map,other_new_valueMap,block_map,v_new_valueMap,loop);
        HashMapClean(phi_map);
    }

    //多次复制感觉没做完
    if(HashSetSize(loop->loopBody) > 2){
//        curr_block->true_block = loop->head;
//        bb_add_prev(curr_block,loop->head);
        HashMapFirst(block_map);
        for(Pair* p = HashMapNext(block_map); p!=NULL; p= HashMapNext(block_map)){
            BasicBlock *b_new = p->value;
            HashSetAdd(loop->loopBody, b_new);
            Loop *trans = loop->parent;
            while (trans){
                HashSetAdd(trans->loopBody, b_new);
                trans = trans->parent;
            }
        }
    }
    return curr_block;
}

//一次循环展开, 是icmp的模式
//返回新建立的，下一次使用的基本块
BasicBlock *copy_one_time_icmp(Loop* loop, BasicBlock* block,HashMap* v_new_valueMap,HashMap* other_new_valueMap, Instruction* ins_end,bool last, HashMap* exit_phi_map,HashMap* phi_map, HashMap* block_map){
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
                //TODO 感觉有点奇怪
                if(check->from == loop->head->true_block || check->from == loop->tail){
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
    HashMapClean(block_map);

    //不能遍历loop body的set, set出来的顺序不对!!!
    InstNode *currNode = loop->head->true_block->head_node;
    InstNode *Tail = get_prev_inst(loop->tail->tail_node);
    while(currNode != Tail && currNode->inst->Opcode != tmp){
        BasicBlock *body = currNode->inst->Parent;

        //exit和head块不展开
        if(body!=loop->head && body!=loop->exit_block) {
            if ((!hasNoDestOperator(currNode) || currNode->inst->Opcode == Store ||
                 currNode->inst->Opcode == GIVE_PARAM) && currNode->inst->Opcode != Phi) {
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
                        v_r = get_replace_value(set_replace, loop->tail);
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
                    new_dest->pdata->var_pdata.iVal = v_dest->pdata->var_pdata.iVal;
                    new_dest->pdata->var_pdata.is_offset = v_dest->pdata->var_pdata.is_offset;
                    new_dest->VTy = v_dest->VTy;
                    if (HashMapContain(other_new_valueMap, v_dest) && !mod_before)
                        update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap, v_dest), new_dest);
                    else
                        update_replace_value(v_new_valueMap, v_dest, new_dest);

                    HashMapPut(other_new_valueMap, v_dest, new_dest);
                }
            } else if (currNode->inst->Opcode == Label && currNode->inst->Parent == loop->tail) {
                //多基本块，但不用建新块
                //TODO 不知道有没有false_block的情况
                prev_block = curr_block;
                curr_block = tail_curr_block;
                HashMapPut(block_map, loop->tail, curr_block);
            } else if(currNode->inst->Opcode == Phi){
                Instruction *ins_phi = ins_new_zero_operator(Phi);
                InstNode *node_phi = new_inst_node(ins_phi);
                Value *v_phi = ins_get_value_with_name_and_index(ins_phi,tmp_index++);
                //v_phi->pdata = ins_get_dest(currNode->inst)->pdata;
                v_phi->alias = ins_get_dest(currNode->inst)->alias;
                v_phi->IsPhi = true;
                v_phi->VTy = currNode->inst->user.value.VTy;
                v_phi->pdata->pairSet = HashSetInit();
                ins_phi->Parent = curr_block;
                ins_insert_before(node_phi,curr_block->tail_node);

                //将能加上的pair加上
                bool need_phi_reduce = false;
                HashSetFirst(currNode->inst->user.value.pdata->pairSet);
                for(pair *pp = HashSetNext(currNode->inst->user.value.pdata->pairSet); pp!=NULL ;pp = HashSetNext(currNode->inst->user.value.pdata->pairSet)){
                    if(HashMapContain(block_map,pp->from)){
                        //加入目前已存在的
                        pair* pair1 = (pair*) malloc(sizeof (pair));
                        pair1->from = HashMapGet(block_map,pp->from);

                        Value *num = (Value*) malloc(sizeof (Value));
                        if(pp->define == NULL)
                            num = NULL;
                        else if(pp->define->VTy->ID == Int){
                            value_init_int(num,pp->define->pdata->var_pdata.iVal);
                        }else if(pp->define->VTy->ID == Float){
                            value_init_float(num,pp->define->pdata->var_pdata.fVal);
                        }else if(HashMapContain(other_new_valueMap,pp->define)){
                            num = HashMapGet(other_new_valueMap,pp->define);
                        } else {
                            //在v_new_valueMap中
                            num = get_replace_value(HashMapGet(v_new_valueMap,pp->define),loop->tail);
                        }

                        pair1->define = num;
                        HashSetAdd(v_phi->pdata->pairSet,pair1);
                    } else
                        need_phi_reduce = true;
                }

                //------------------
                if(need_phi_reduce)
                    HashMapPut(phi_map,&currNode->inst->user.value,v_phi);

                if (HashMapContain(other_new_valueMap, ins_get_dest(currNode->inst)) && !mod_before)
                    update_replace_value(v_new_valueMap, HashMapGet(other_new_valueMap, ins_get_dest(currNode->inst)), v_phi);
                else
                    update_replace_value(v_new_valueMap, ins_get_dest(currNode->inst), v_phi);

                HashMapPut(other_new_valueMap, ins_get_dest(currNode->inst), v_phi);

            } else if (currNode->inst->Opcode == Label) {
                //多基本块的话，第一条就会走到这里

                //建新块+label+br
                BasicBlock *more_block = bb_create();
                //先随便加个true_block关系
                HashMapPut(block_map, currNode->inst->Parent, more_block);
                more_block->Parent = currNode->inst->Parent->Parent;
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

    if(HashSetSize(loop->loopBody) > 2){
        connect_blocks(block_map,loop,block);
        reduce_phi_(phi_map,other_new_valueMap,block_map,v_new_valueMap,loop);
        HashMapClean(phi_map);
    }

    //将新块都加入loop body, 改变loop tail, 并改变head phi中的from
    if(last){
        BasicBlock *prev_tail = loop->tail;
        tail_curr_block->true_block = loop->head;
        bb_add_prev(tail_curr_block,loop->head);

        HashMapFirst(block_map);
        for(Pair* p = HashMapNext(block_map); p!=NULL; p= HashMapNext(block_map)){
            BasicBlock *b_new = p->value;
            if(p->key == loop->tail)
                loop->tail = b_new;
            HashSetAdd(loop->loopBody, b_new);
            Loop *trans = loop->parent;
            while (trans){
                HashSetAdd(trans->loopBody, b_new);
                trans = trans->parent;
            }
        }

        update_head_phi(prev_tail,loop->head,loop->tail);
    } else {
        HashMapFirst(block_map);
        for(Pair* p = HashMapNext(block_map); p!=NULL; p= HashMapNext(block_map)){
            BasicBlock *b_new = p->value;
            HashSetAdd(loop->loopBody, b_new);
            Loop *trans = loop->parent;
            while (trans){
                HashSetAdd(trans->loopBody, b_new);
                trans = trans->parent;
            }
        }
    }

    return curr_block;
}

//余数模块
bool handle_mod(Loop* loop,Value* v_step, Value* v_end, Value* times,Instruction* ins_end_cond, Instruction *ins_modifier, HashMap* v_new_valueMap, HashMap* other_new_valueMap, HashMap* phi_map, HashMap* v_new_phiMap, HashMap* v_match_phiMap){
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
            copy_for_mod(loop, new_pre_block, v_new_valueMap, other_new_valueMap, 1,NULL,HashMapInit(),false,phi_map);
            //2. 将块在新block中再复制mod_num - 1次
            //3. 置换更新原block中的元素，更新map
            BasicBlock *curr_block = NULL;
            if(HashSetSize(loop->loopBody) <= 2){
                for(int i=0; i < mod_num - 1 ;i++){
                    copy_for_mod(loop, new_pre_block, v_new_valueMap, other_new_valueMap ,0,NULL,HashMapInit(),false,phi_map);
                }
            } else{
                for(int i=0; i < mod_num - 1 ;i++){
                    if(i == 0 && i!=mod_num-2)
                        curr_block = copy_for_mod(loop, new_pre_block, v_new_valueMap, other_new_valueMap ,0,NULL,HashMapInit(),false,phi_map);
                    else if(i == 0 && i == mod_num-2)
                        curr_block = copy_for_mod(loop, new_pre_block, v_new_valueMap, other_new_valueMap ,0,NULL,HashMapInit(),true,phi_map);
                    else if(i == mod_num-2)
                        curr_block = copy_for_mod(loop, curr_block, v_new_valueMap, other_new_valueMap ,0,NULL,HashMapInit(),true,phi_map);
                    else
                        curr_block = copy_for_mod(loop, curr_block, v_new_valueMap, other_new_valueMap ,0,NULL,HashMapInit(),false,phi_map);
                }
                curr_block->true_block = loop->head;
                bb_add_prev(curr_block,loop->head);
                bb_delete_one_prev(loop->head,new_pre_block);
            }

            //调整一下head phi中的from
            if(HashSetSize(loop->loopBody) <= 2)
                adjust_phi_from(loop,new_pre_block,v_new_valueMap);
            else
                adjust_phi_from(loop,curr_block,v_new_valueMap);
        }
    } else {
        //* preserv1      : 要分类讨论的余数计算
        //mod_block       : 算余数
        //* phi_block 要分类讨论的余数计算的phi块
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

        BasicBlock *phi_block = bb_create();
        InstNode *node_mod = get_mod(loop->initValue,ins_modifier,ins_end_cond,v_step,v_end,mod_block, new_pre_block,node_label, loop,&phi_block);
        if(!have_more_block)
            phi_block = mod_block;
        //br到下一个block循环mod次
        InstNode *node_br = NULL;
        if(mod_block->tail_node!=NULL)              //有preserve
            node_br = mod_block->true_block->tail_node;
        else {           //无preserve
            Instruction *ins_br = ins_new_zero_operator(br);
            ins_br->Parent = mod_block;
            node_br = new_inst_node(ins_br);
            ins_insert_after(node_br,node_mod);
            mod_block->tail_node = node_br;
        }

        ins_insert_after(node_label2,node_br);          //new_pre_block的label


        //包装一个phi，初始值为0，每次加1,就专职用来计数这个补充余数的小循环
        Value *v_zero=(Value*) malloc(sizeof (Value));
        value_init_int(v_zero,0);
        Instruction *ins_phi = ins_new_zero_operator(Phi);
        ins_phi->Parent = new_pre_block;
        HashSet * set = HashSetInit();
        pair *pair1 = (pair*) malloc(sizeof (pair));

        if(node_mod->inst->Opcode == Phi)
            pair1->from = mod_block->true_block;
        else
            pair1->from = mod_block;
        pair1->define = v_zero;
        HashSetAdd(set,pair1);
        ins_get_value_with_name_and_index(ins_phi,tmp_index++)->pdata->pairSet = set;
        InstNode *node_phi = new_inst_node(ins_phi);

        ins_insert_after(node_phi,node_label2);

        //把head中的phi都装上
        InstNode *phi_ = get_next_inst(loop->head->head_node);
        while(phi_->inst->Opcode == Phi){
            Instruction *phi1  = ins_new_zero_operator(Phi);
            ins_get_value_with_name_and_index(phi1,tmp_index++);
            phi1->Parent = new_pre_block;
            phi1->user.value.pdata->pairSet = HashSetInit();
            InstNode *no_phi = new_inst_node(phi1);
            HashSetFirst(phi_->inst->user.value.pdata->pairSet);
            for(pair* phiInfo = HashSetNext(phi_->inst->user.value.pdata->pairSet); phiInfo!=NULL; phiInfo = HashSetNext(phi_->inst->user.value.pdata->pairSet)){
                pair *new_pair = (pair*) malloc(sizeof (pair));
                if(!HashSetFind(loop->loopBody,phiInfo->from)){
                    new_pair->define = phiInfo->define;
                    new_pair->from = phi_block;
                } else {
                    new_pair->define = phiInfo->define;
                    new_pair->from = phiInfo->from;
                }
                HashSetAdd( phi1->user.value.pdata->pairSet,new_pair);
            }
            HashMapPut(v_new_phiMap,&phi_->inst->user.value,phi1->user.value.pdata->pairSet);
            HashMapPut(v_match_phiMap,&phi_->inst->user.value, ins_get_dest(phi1));
            ins_insert_before(no_phi,node_phi);
            //更新v_new_valueMap
            //update_replace_value_mod(v_new_valueMap,&phi_->inst->user.value, ins_get_dest(phi1),loop);
            phi_ = get_next_inst(phi_);
        }

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

        new_pre_block->true_block = loop_pre_block;
        new_pre_block->false_block = loop->head;
        loop_pre_block->Parent = new_pre_block->Parent;

        if(HashSetSize(loop->loopBody) <= 2){
            bb_add_prev(loop_pre_block,new_pre_block);
            loop_pre_block->true_block = new_pre_block;
        }

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
        BasicBlock *ret = copy_for_mod(loop, loop_pre_block, v_new_valueMap, other_new_valueMap, 1,v_new_phiMap,v_match_phiMap,false,phi_map);

        // 一次add +1,然后存进phi_set
        Value *v_one=(Value*) malloc(sizeof (Value));
        value_init_int(v_one,1);
        InstNode *node_plus = auto_binary_node_insert_before(Add, ins_get_dest(node_phi->inst),v_one,tmp_index++,loop_pre_block,loop_pre_block->tail_node);

        pair *pair2 = (pair*) malloc(sizeof (pair));
        pair2->define = ins_get_dest(node_plus->inst);
        pair2->from =loop_pre_block;
        HashSetAdd(set,pair2);

        if(HashSetSize(loop->loopBody) > 2){
            ret->true_block = new_pre_block;
            bb_add_prev(ret,new_pre_block);
            update_head_phi(new_pre_block->true_block,new_pre_block,ret);
        }

        //调整一下head phi中的from
        adjust_init_phi(loop,v_match_phiMap);

        //放这里才行是因为connect_block被清除了一次
        bb_add_prev(new_pre_block, loop_pre_block);
    }


    //余数处理完毕，开始循环内容的复制
    BasicBlock *curr_block = NULL;
    if(mod_before){
        if(HashSetSize(loop->loopBody) <= 2){
            for(int i = 0;i < update_modifier-1;i++){
                if(i == 0){
                    copy_one_time(loop, 1, NULL, v_new_valueMap,other_new_valueMap,false,phi_map);
                    mod_before = false;
                } else if(i == update_modifier-2)
                    copy_one_time(loop, 1, NULL, v_new_valueMap,other_new_valueMap,true,phi_map);
                else
                    copy_one_time(loop, 1, NULL, v_new_valueMap,other_new_valueMap,false,phi_map);
            }
        } else {
            for(int i = 0;i < update_modifier-1;i++){
                if(i == 0){
                    curr_block = copy_one_time(loop, 1, NULL, v_new_valueMap,other_new_valueMap,false,phi_map);
                    mod_before = false;
                }
                else if(i == update_modifier-2)
                    curr_block = copy_one_time(loop, 1, curr_block, v_new_valueMap,other_new_valueMap,true,phi_map);
                else
                    curr_block = copy_one_time(loop, 1, curr_block, v_new_valueMap,other_new_valueMap,false,phi_map);
            }
        }

    } else {
        if(HashSetSize(loop->loopBody) <= 2){
            for(int i = 0;i < update_modifier-1;i++){
                if(i == 0){
                    copy_one_time(loop, 0, NULL, v_new_valueMap,other_new_valueMap,false,phi_map);
                    mod_before = false;
                } else if(i == update_modifier-2)
                    copy_one_time(loop, 0, NULL, v_new_valueMap,other_new_valueMap,true,phi_map);
                else
                    copy_one_time(loop, 0, NULL, v_new_valueMap,other_new_valueMap,false,phi_map);
            }
        } else {
            for(int i = 0;i < update_modifier-1;i++){
                if(i == 0){
                    curr_block = copy_one_time(loop, 0, NULL, v_new_valueMap,other_new_valueMap,false,phi_map);
                    mod_before = false;
                }
                else if(i == update_modifier-2)
                    curr_block = copy_one_time(loop, 0, curr_block, v_new_valueMap,other_new_valueMap,true,phi_map);
                else
                    curr_block = copy_one_time(loop, 0, curr_block, v_new_valueMap,other_new_valueMap,false,phi_map);
            }
        }
    }
    return true;
}

//在map里的代表只能用icmp版本
void check_mod_type(Loop* loop){
    Instruction *ins_end_cond=(Instruction*)loop->end_cond;
    Instruction *ins_modifier=(Instruction*)loop->modifier;
    if(!ins_modifier || !ins_end_cond)
        return;
    Value *v_step=NULL,*v_end=NULL;
    if(ins_get_lhs(ins_end_cond)==loop->inductionVariable)
        v_end= ins_get_rhs(ins_end_cond);
    else
        v_end= ins_get_lhs(ins_end_cond);
    if(loop->modifier->IsPhi)
        v_step = loop->modifier;
    else if(ins_get_lhs(ins_modifier)==loop->inductionVariable)
        v_step= ins_get_rhs(ins_modifier);
    else
        v_step= ins_get_lhs(ins_modifier);

    Instruction *end_before = (Instruction*)v_end;
    if(!((HashSetSize(loop->loopBody) <= 2 && loop->parent == NULL) &&                //单基本块无parent
       (ins_modifier->Opcode != Mul && ins_modifier->Opcode != Div) &&         //余数情况下*/为modifier的不考虑
       (ins_end_cond->Opcode != EQ && ins_end_cond->Opcode != NOTEQ) &&        //结束条件为==或者!=的不处理
       (!v_end->IsPhi) && (end_before == NULL || (end_before && end_before->Opcode!=Add && end_before->Opcode!=Sub && end_before->Opcode!=Div && end_before->Opcode!=Mul)) &&  //特殊end条件不考虑
       (!v_step->IsPhi)))
        HashSetAdd(funcSet,ins_modifier->Parent->Parent);
}

bool LOOP_UNROLL_EACH(Loop* loop)
{

    //全都不能开的条件
    if(!loop->hasDedicatedExit)
        return false;

    if(!loop->initValue || !loop->modifier || !loop->end_cond)
        return false;

    if(loop->conditionChangeWithinLoop)
        return false;

    Instruction *ins_end_cond=(Instruction*)loop->end_cond;
    Instruction *ins_modifier=(Instruction*)loop->modifier;
    Value *v_step=NULL,*v_end=NULL;
    if(ins_get_lhs(ins_end_cond)==loop->inductionVariable)
        v_end= ins_get_rhs(ins_end_cond);
    else
        v_end= ins_get_lhs(ins_end_cond);
    if(loop->modifier->IsPhi)
        v_step = loop->modifier;
    else if(ins_get_lhs(ins_modifier)==loop->inductionVariable)
        v_step= ins_get_rhs(ins_modifier);
    else
        v_step= ins_get_lhs(ins_modifier);

    //步长为0就return
    if(v_step->VTy->ID == Int && v_step->pdata->var_pdata.iVal==0)
        return false;

    int cnt=cal_ir_cnt(loop->loopBody);
    //如果是常数，就能计算迭代次数，然后进行一个判断
    //非常数的话进行一个指令条数的判断
    Value *times = (Value*) malloc(sizeof (Value));
    if(check_idc(loop->initValue,v_step,v_end)){
        int t = cal_times(loop->initValue->pdata->var_pdata.iVal,ins_modifier,ins_end_cond,v_step,v_end);
        value_init_int(times, t);
        //超出一定长度的循环，不进行展开
        if(times->pdata->var_pdata.iVal*cnt>loop_unroll_up_lines)
            return false;
    } else {
        if(cnt > 80)          //130行，感觉不能再大了
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

    HashMap* phi_map = HashMapInit();

    HashMap * block_map = HashMapInit();

    //保存新增phi块中的变量与最新值的对应关系，初始值与v_new_valueMap相同
    HashMap *v_new_phiMap = HashMapInit();

    //保存v_new_valueMap的key与新快phi左值的对应关系
    HashMap *v_match_phiMap = HashMapInit();

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

    //进行展开方式的选择
    //1. 余数: 单基本块等一系列条件
    Instruction *end_before = (Instruction*)v_end;
    if(0 && (HashSetSize(loop->loopBody) <= 2 && loop->parent == NULL) &&                //单基本块无parent
            (ins_modifier->Opcode != Mul && ins_modifier->Opcode != Div) &&         //余数情况下*/为modifier的不考虑
            (ins_end_cond->Opcode != EQ && ins_end_cond->Opcode != NOTEQ) &&        //结束条件为==或者!=的不处理
            (!v_end->IsPhi) && (end_before == NULL || (end_before && end_before->Opcode!=Add && end_before->Opcode!=Sub && end_before->Opcode!=Div && end_before->Opcode!=Mul)) &&  //特殊end条件不考虑
            (!v_step->IsPhi) &&
            !HashSetFind(funcSet,ins_modifier->Parent->Parent))
    {
        bool ret = handle_mod(loop,v_step,v_end,times,ins_end_cond,ins_modifier,v_new_valueMap,other_new_valueMap,phi_map,v_new_phiMap,v_match_phiMap);
    }
    //2. icmp
    else {
        BasicBlock *cur_block = NULL;
        for(int i = 0;i < update_modifier-1;i++){
            if(i==0)
                cur_block = copy_one_time_icmp(loop,NULL, v_new_valueMap,other_new_valueMap,ins_end_cond,false,exit_phi_map,phi_map,block_map);
            else if(i+1 == update_modifier-1)
                cur_block = copy_one_time_icmp(loop,cur_block, v_new_valueMap,other_new_valueMap,ins_end_cond,true,exit_phi_map,phi_map,block_map);
            else
                cur_block = copy_one_time_icmp(loop,cur_block, v_new_valueMap,other_new_valueMap,ins_end_cond,false,exit_phi_map,phi_map,block_map);
        }
        //给exit_block加上一些Phi
        HashMap *match_phi_map = insert_exit_phi(loop->exit_block,exit_phi_map,loop);

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
    mod_before = false;
    have_more_block = false;
    effective |= LOOP_UNROLL_EACH(loop);
    return effective;
}

void loop_unroll(Function *currentFunction)
{
    funcSet = HashSetInit();
    HashSetFirst(currentFunction->loops);

    ///先遍历一遍func,一个函数内如果余数情况用在icmp版本之后会有问题(因为dom还未建立,而loop因为是set中拿出来，又可能是先做了下面的loop再做上面的loop)
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        check_mod_type(root);
    }

    HashSetFirst(currentFunction->loops);
    //遍历每个loop
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        dfsLoop(root);
    }
}