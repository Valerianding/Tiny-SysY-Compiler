#include "remove_call.h"
int call_self;
int call_else;
int phi_num;
int have_imm;
extern Symtab * this;
void remake_func(Function * function)
{
    //先删去当前所有ir,除了funcend和funbegin
    InstNode *cur_node = get_next_inst(function->entry->head_node);
    while (cur_node!=function->tail->tail_node){
        InstNode *now = cur_node;
        cur_node = get_next_inst(cur_node);
        deleteIns(now);
    }

    // 拿到参数
    Value *v_func = symtab_lookup_withmap(this,function->name,&this->value_maps->next->map);
    Value *v_param0 = (Value*) malloc(sizeof (Value));
    value_init(v_param0);
    v_param0->VTy->ID = v_func->pdata->symtab_func_pdata.param_type_lists[0].ID;
    v_param0->name=(char *) malloc(3);
    strcpy(v_param0->name,"%0");
    Value *v_param1 = (Value*) malloc(sizeof (Value));
    value_init(v_param1);
    v_param1->VTy->ID = v_func->pdata->symtab_func_pdata.param_type_lists[1].ID;
    v_param1->name=(char *) malloc(3);
    strcpy(v_param1->name,"%1");

    //需要的常数value先构造出来
    Value *v_0 = (Value*) malloc(sizeof (Value));
    value_init_int(v_0,0);
    Value *v_2 = (Value*) malloc(sizeof (Value));
    value_init_int(v_2,2);

    int t_index = 3;
    BasicBlock *new_block = bb_create();
    //mod
    Instruction *ins_mod = ins_new_binary_operator(Mod,v_param1,v_2);
    ins_mod->Parent = new_block;
    InstNode *node_mod = new_inst_node(ins_mod);
    node_mod->inst->user.value.VTy->ID = Var_INT;
    new_block->head_node = function->entry->head_node;
    ins_insert_after(node_mod,function->entry->head_node);
    function->entry->head_node->inst->Parent = new_block;

    //icmp
    Instruction *ins_icmp = ins_new_binary_operator(NOTEQ, ins_get_value_with_name_and_index(ins_mod,++t_index),v_0);
    ins_icmp->user.value.VTy->ID = Var_INT;
    ins_icmp->Parent = new_block;
    InstNode *node_icmp = new_inst_node(ins_icmp);
    ins_insert_after(node_icmp,node_mod);
    //br
    Instruction *ins_br_i1 = ins_new_unary_operator(br_i1, ins_get_value_with_name_and_index(ins_icmp,++t_index));
    ins_br_i1->Parent = new_block;
    InstNode *node_br_i1= new_inst_node(ins_br_i1);
    new_block->tail_node = node_br_i1;
    ins_insert_after(node_br_i1,node_icmp);

    BasicBlock *new_bb2 = bb_create();
    BasicBlock * new_bb3 = bb_create();
    bb_add_prev(new_block,new_bb2);
    bb_add_prev(new_block,new_bb3);
    bb_add_prev(new_bb2,new_bb3);
    new_block->true_block = new_bb2;
    new_bb2->true_block = new_bb3;
    new_block->false_block = new_bb3;
    new_bb2->Parent = function;
    new_bb3->Parent = function;
    new_block->Parent = function;

    //label5
    Instruction *ins_l5 = ins_new_zero_operator(Label);
    ins_l5->Parent =new_bb2;
    ins_l5->user.value.pdata->instruction_pdata.true_goto_location = ++t_index;
    InstNode *node_l5 = new_inst_node(ins_l5);
    new_bb2->head_node = node_l5;
    ins_insert_after(node_l5,node_br_i1);
    //br
    Instruction *ins_br = ins_new_zero_operator(br);
    ins_br->Parent = new_bb2;
    InstNode *node_br = new_inst_node(ins_br);
    ins_insert_after(node_br,node_l5);
    new_bb2->tail_node = node_br;

    //label6
    Instruction *ins_l6 = ins_new_zero_operator(Label);
    ins_l6->Parent = new_bb3;
    ins_l6->user.value.pdata->instruction_pdata.true_goto_location = ++t_index;
    InstNode *node_l6 = new_inst_node(ins_l6);
    new_bb3->head_node = node_l6;
    ins_insert_after(node_l6,node_br);
    //phi
    Instruction *ins_phi = ins_new_zero_operator(Phi);
    ins_phi->Parent = new_bb3;
    Value *v_phi = ins_get_value_with_name_and_index(ins_phi,++t_index);
    v_phi->IsPhi = true;
    v_phi->VTy->ID = v_func->pdata->symtab_func_pdata.return_type.ID;
    v_phi->pdata->pairSet = HashSetInit();
    pair *p1 = (pair*) malloc(sizeof (pair));
    Value *v_zero = (Value*) malloc(sizeof (Value));
    value_init_int(v_zero,0);
    p1->define = v_zero;
    p1->from = new_bb2;
    pair *p2 = (pair*) malloc(sizeof (pair));
    p2->define = v_param0;
    p2->from = new_block;
    HashSetAdd(v_phi->pdata->pairSet,p1);
    HashSetAdd(v_phi->pdata->pairSet,p2);
    InstNode *node_phi = new_inst_node(ins_phi);
    ins_insert_after(node_phi,node_l6);

    //return
    Instruction *ins_ret = ins_new_unary_operator(Return,v_phi);
    ins_ret->Parent = new_bb3;
    InstNode *node_ret = new_inst_node(ins_ret);
    new_bb3->tail_node = node_ret;
    ins_insert_after(node_ret,node_phi);
}

bool is_icmp_ir(InstNode* instNode){
    Opcode op = instNode->inst->Opcode;
    if(op == LESS || op == LESSEQ || op == GREATEQ || op == GREAT || op == EQ || op ==NOTEQ)
        return true;
    return false;
}


bool is_change_edge(InstNode* node,Value* v_edge){
    if(node->inst->Opcode == Add || node->inst->Opcode == Sub || node->inst->Opcode == Mul || node->inst->Opcode == Div){
        if(v_edge == ins_get_lhs(node->inst))
            return true;
        if(v_edge == ins_get_rhs(node->inst))
            return true;
        return false;
    }
    return false;
}

int get_init_num(InstNode* change_ir,Value* v_edge,Value* v_const){
    //1.拿到退出num,v_const是结果
    //TODO 先只做个sub看看情况
    if(change_ir->inst->Opcode == Sub && ins_get_lhs(change_ir->inst) == v_edge){
        int init = v_const->pdata->var_pdata.iVal - ins_get_rhs(change_ir->inst)->pdata->var_pdata.iVal ;
        return init;
    }
}

int get_next_int(InstNode* change_ir,Value* v_edge,int now_int){
    if(change_ir->inst->Opcode == Sub && ins_get_lhs(change_ir->inst) == v_edge){
        int init = now_int+ ins_get_rhs(change_ir->inst)->pdata->var_pdata.iVal;
        return init;
    }
}

bool check_be_while(Value* v_const,InstNode* change_ir,BasicBlock* block,InstNode* end_node,Value* v_edge,Function* function){
    //判断是否能循环上的队列
    Queue *results = QueueInit();
    //先将常数值(结果)和它对应的num入队列
    //1.拿到退出num,v_const是结果
    int init = get_init_num(change_ir,v_edge,v_const);
    r_node *rNode = (r_node*) malloc(sizeof(r_node));
    rNode->num = init;rNode->result = v_const->pdata->var_pdata.iVal;
    QueuePush(results,rNode);

    //遍历block,除去give param和change_ir 计算一次改变后的value
    int new_int = get_next_int(change_ir,v_edge,init);
    InstNode *cur_node = block->head_node;
    while (cur_node != block->tail_node){
        if(cur_node!=change_ir && cur_node->inst->Opcode!=GIVE_PARAM){
            if(cur_node->inst->Opcode == Call){
                //拿出上一次放进去的值
                r_node *n = (r_node*) malloc(sizeof (r_node));
                QueueBack(results,(void**)&n);
                int res = n->result;
                Value *v_res = (Value*) malloc(sizeof (Value));
                value_init_int(v_res,res);
                //TODO 常量传播， instcomb
            }
        }

        cur_node = get_next_inst(cur_node);
    }
}

bool check_call_func(Function* function){
    //临界value
    Value *v_edge = NULL;
    Value *v_const = NULL;
    InstNode *icmp_node =NULL;

    //直达最后一个基本块找phi
    BasicBlock *end = function->tail;
    InstNode *phi_node = end->head_node;
    HashSetFirst(phi_node->inst->user.value.pdata->pairSet);
    //找到是常数的情况，往前驱块找变化条件
    for(pair* pp = HashSetNext(phi_node->inst->user.value.pdata->pairSet); pp!=NULL; pp = HashSetNext(phi_node->inst->user.value.pdata->pairSet)){
        if(isImm(pp->define)){
            v_const = pp->define;
            //找前驱块
            HashSet* preBlocks = pp->from->preBlocks;
            assert(HashSetSize(preBlocks)==1);
            HashSetFirst(preBlocks);
            BasicBlock *prev = HashSetNext(preBlocks);
            //找到一条icmp
            InstNode *end_node = prev->tail_node;
            while(end_node != prev->head_node){
                if(is_icmp_ir(end_node)){
                    icmp_node = end_node;
                    //TODO 可能有点草率
                    if(!isImm(ins_get_lhs(end_node->inst))){
                        v_edge = ins_get_lhs(end_node->inst);
                    }
                    else{
                        v_edge = ins_get_rhs(end_node->inst);
                    }
                    break;
                }
                end_node = get_prev_inst(end_node);
            }
            break;
        }
    }
    for(pair* pp = HashSetNext(phi_node->inst->user.value.pdata->pairSet); pp!=NULL; pp = HashSetNext(phi_node->inst->user.value.pdata->pairSet)){
        if(!isImm(pp->define)){
            //是另一个需要check的基本块
            //找到对边界条件进行修改的ir
            InstNode *change_ir = pp->from->head_node;
            //TODO 先草率地默认就在give param前面
            while(!is_change_edge(change_ir,v_edge) && change_ir->inst->Opcode !=GIVE_PARAM){
                change_ir = get_next_inst(change_ir);
            }
            if(change_ir->inst->Opcode ==GIVE_PARAM)
                return false;

            //计算是否能循环

            break;
        }
    }
}

int issimple(Function * tempFunction)
{
    call_self=0;
    call_else=0;
    phi_num=0;
    BasicBlock *entry = tempFunction->entry;
    BasicBlock *end = tempFunction->tail;

    InstNode *currNode = entry->head_node;
    InstNode *endNode = get_next_inst(end->tail_node);
    currNode = get_next_inst(currNode);  // skip for FuncBegin
    while(currNode != endNode){
        switch (currNode->inst->Opcode)
        {
            case Call:
            {
                if(strcmp(currNode->inst->user.use_list->Val->name,tempFunction->name)==0)
                {
                    call_self=1;
                }
                else
                {
                    call_else=1;
                }
                break;

            }
            case Phi:
            {
                int i=0;
                have_imm=0;
                HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
                HashSetFirst(phiSet);
                unsigned int size=HashSetSize(phiSet);
                phi_num=size;
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    BasicBlock *from = phiInfo->from;
                    Value *incomingVal = phiInfo->define;
                    if(i + 1 == size)      //最后一次
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                            have_imm++;
                        }else if(incomingVal != NULL){
                            
                        }else{
                            
                        }
                    }
                    else
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                           have_imm++;
                        }else if(incomingVal != NULL){
                           
                        }else{
                            
                        }
                    }
                    i++;
                }
                break;
            }
        }
        currNode = get_next_inst(currNode);
    }
    if(call_self==1&&call_else==0&&phi_num==2&&have_imm==1)
    {
        tempFunction->issimplerecursive=1;
        remake_func(tempFunction);
        return 1;
    }
    else
    {
        tempFunction->issimplerecursive=0;
        return 0;
    }
    // printf("func:%s is simple :%d\n",tempFunction->name,tempFunction->issimplerecursive);
    return 0;
}

// void remake_func(Function * tempFunction)
// {

// }