#include "inliner.h"

extern InstNode * one_param[];   //存放单次正确位置的参数
extern InstNode* params[];      //存放所有参数
extern Symtab *this;

//根据名字拿到它的index
int get_name_index(Value* v)
{
    return (int)strtol(v->name+1,NULL,10);
}

//NULL就是库函数
InstNode *find_func_begin(struct _InstNode* instruction_node,char* func_name)
{
    instruction_node= get_next_inst(instruction_node);

    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN)
    {
        Instruction *instruction=instruction_node->inst;
        if(instruction->Opcode==FunBegin && strcmp(instruction->user.use_list->Val->name,func_name)==0)
            return instruction_node;

        instruction_node= get_next_inst(instruction_node);
    }
    return NULL;
}

//TODO 目前内联条件(可能会再修改)
//1. 最多4个基本块
//2. 不递归
//3. 变量数最大为20
void label_func_inline(struct _InstNode* instNode_list)
{
    InstNode *temp = get_next_inst(instNode_list);
    //找到第一个function的
    while(temp->inst->Parent->Parent == NULL){
        temp = get_next_inst(temp);
    }
    BasicBlock *block = temp->inst->Parent;

    //遍历函数
    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        //1.数变量数量
        //2.判断基本块与递归call
        BasicBlock *entry = currentFunction->entry;
        BasicBlock *end = currentFunction->tail;

        InstNode *currNode = entry->head_node;

        Value *funcValue = currNode->inst->user.use_list->Val;
        //先默认都可内联
        if(strcmp(funcValue->name,"main")!=0)
            funcValue->pdata->symtab_func_pdata.flag_inline=1;
        //跳过第一条FunBegin
        currNode = get_next_inst(currNode);
        int max_num=0;       //不管有参没参，变量数都等于max_num数
        int block_num = 1;
        while (currNode != get_next_inst(end->tail_node)) {
            //最后的一定出现在左边
            if(currNode->inst->user.value.name!=NULL){
                max_num= get_name_index(&currNode->inst->user.value);
                if(max_num>20)
                {
                    funcValue->pdata->symtab_func_pdata.flag_inline=0;
                    break;
                }
            }
            //判断基本块数量
            if(currNode->inst->Opcode==Label){
                block_num++;
                if(block_num>4){
                    funcValue->pdata->symtab_func_pdata.flag_inline=0;
                    break;
                }
            }

            if(currNode->inst->Opcode==Call && currNode->inst->user.use_list->Val->name == funcValue->name){
                funcValue->pdata->symtab_func_pdata.flag_inline=0;
                break;
            }

            currNode = get_next_inst(currNode);
        }
    }
}

//将phi的信息补充完整
void reduce_phi(HashMap* phi_map,HashMap* alias_map,HashMap* block_map){
    HashMapFirst(phi_map);
    for(Pair* p = HashMapNext(phi_map); p!=NULL; p = HashMapNext(phi_map)){
        Value *v_key = p->key;
        Value *v_value = p->value;

        HashSet* set = HashSetInit();
        HashSetFirst(v_key->pdata->pairSet);
        for(pair *pp = HashSetNext(v_key->pdata->pairSet); pp!=NULL ;pp = HashSetNext(v_key->pdata->pairSet)){
            pair* pair1 = (pair*) malloc(sizeof (pair));
            pair1->from = HashMapGet(block_map,pp->from);

            Value *num = (Value*) malloc(sizeof (Value));
            if(pp->define->VTy->ID == Int){
                value_init_int(num,pp->define->pdata->var_pdata.iVal);
            }else if(pp->define->VTy->ID == Float){
                value_init_float(num,pp->define->pdata->var_pdata.fVal);
            }else{
                num = HashMapGet(alias_map,pp->define);
            }
            pair1->define = num;
            HashSetAdd(set,pair1);
        }
        v_value->pdata->pairSet = set;
    }
}

void connect_caller_block(HashMap* block_map, HashSet* callee_block_set, BasicBlock* caller_cur_block, Function* callee_func, BasicBlock* last_cur_new_block){
    //callee的最后一个copy block后继为caller_cur_block的后继,必须先做，因为caller_cur_block的后继会改变
    last_cur_new_block->true_block = caller_cur_block->true_block;
    last_cur_new_block->false_block = caller_cur_block->false_block;

    HashSetFirst(callee_block_set);
    for(BasicBlock* block = HashSetNext(callee_block_set); block!=NULL; block = HashSetNext(callee_block_set)){
        BasicBlock *alias_block = HashMapGet(block_map,block);
        if(block != callee_func->tail) {
            if(block->true_block)
                alias_block->true_block = HashMapGet(block_map,block->true_block);
            if(block->false_block)
                alias_block->false_block = HashMapGet(block_map,block->false_block);
        }
    }
}

void func_inline(struct _InstNode* instruction_node)
{
    //先跑一遍，看是否符合内联条件
    label_func_inline(instruction_node);

    InstNode *start=instruction_node;
    instruction_node= get_next_inst(instruction_node);
    InstNode *begin_func=NULL;

    int give_count = 0;
    //Value*——>Value*
    HashMap *left_alias_map = HashMapInit();     //代替原来alias的形式

    //callee中基本块-------->caller中基本块
    HashMap* block_map = HashMapInit();
    //phi map,因为phi的信息可能要遍历到后面才能找全
    HashMap *phi_map = HashMapInit();
    //callee中的block set
    HashSet* block_set = HashSetInit();

    BasicBlock *cur_new_block = NULL;

    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        Instruction *instruction = instruction_node->inst;

        if(instruction->Opcode==Call)
        {
            int num=0;
            //遍历ir找到对应的函数ir,进行一波复制
            if((begin_func=find_func_begin(start,instruction->user.use_list->Val->name))!=NULL && begin_func->inst->user.use_list->Val->pdata->symtab_func_pdata.flag_inline==1)
            {
                Value *v_func=begin_func->inst->user.use_list->Val;
                //判断被内联的函数有无参数
                //有参
                if(begin_func->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num!=0)
                {
                    //记一下参数个数，比它小的都是参数，要区别处理(原value的alias为NULL)
                    num= v_func->pdata->symtab_func_pdata.param_num;

                    //在one_param里填满参数
                    get_param_list(v_func,&give_count);
                }

                //无参数直接上手
                //复制ir
                begin_func=get_next_inst(begin_func);     //函数中的第一条ir
                int index=1;
                int param_index=0;

                HashMapPut(block_map, begin_func->inst->Parent, instruction->Parent);
                HashSetAdd(block_set, begin_func->inst->Parent);
                while (begin_func->inst->Opcode!=Return)
                {
                    Instruction *ins_copy=NULL;
                    InstNode *node_copy=NULL;

                    //copy这条ir，把它加进来
                    Value *v,*vl,*vr;
                    v= ins_get_dest(begin_func->inst);
                    vl= ins_get_lhs(begin_func->inst);
                    vr= ins_get_rhs(begin_func->inst);
                    if(vl==NULL)    //0操作数，比如br
                        ins_copy= ins_new_zero_operator(begin_func->inst->Opcode);
                    else if(vr==NULL)   //1操作数
                    {
                        //是直接用的参数
                        if(begin_func->inst->user.use_list->Val->VTy->ID!=Int && begin_func->inst->user.use_list->Val->VTy->ID!=Float &&
                        begin_tmp(begin_func->inst->user.use_list->Val->name) && get_name_index(begin_func->inst->user.use_list->Val)<num)
                        {
                            param_index=get_name_index(begin_func->inst->user.use_list->Val);
                            ins_copy= ins_new_unary_operator(begin_func->inst->Opcode,one_param[param_index]->inst->user.use_list->Val);
                        }
                        //其他正常情况
                        else if(begin_func->inst->user.use_list->Val->VTy->ID!=Int && begin_func->inst->user.use_list->Val->VTy->ID!=Float && begin_tmp(begin_func->inst->user.use_list->Val->name))
                            ins_copy= ins_new_unary_operator(begin_func->inst->Opcode, HashMapGet(left_alias_map,begin_func->inst->user.use_list->Val));
                        else  //全局
                            ins_copy= ins_new_unary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val);
                    }
                    else   //2操作数
                    {
                        Value *v1=begin_func->inst->user.use_list->Val;
                        Value *v2=begin_func->inst->user.use_list[1].Val;
                        //getint(),@a
                        if(((!begin_tmp(begin_func->inst->user.use_list->Val->name) || begin_func->inst->user.use_list->Val->VTy->ID == Int || begin_func->inst->user.use_list->Val->VTy->ID==Float) && (!begin_tmp(begin_func->inst->user.use_list[1].Val->name) || begin_func->inst->user.use_list[1].Val->VTy->ID == Int || begin_func->inst->user.use_list[1].Val->VTy->ID==Float)) )
                            ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val,begin_func->inst->user.use_list[1].Val);
                        //%1,%2
                        else if((begin_func->inst->user.use_list->Val->VTy->ID!=Int && begin_func->inst->user.use_list->Val->VTy->ID!=Float && begin_tmp(begin_func->inst->user.use_list->Val->name))
                        && (begin_func->inst->user.use_list[1].Val->VTy->ID!=Int && begin_func->inst->user.use_list[1].Val->VTy->ID!=Float && begin_tmp(begin_func->inst->user.use_list[1].Val->name)))
                        {
                            if((param_index=get_name_index(begin_func->inst->user.use_list->Val))<num && get_name_index(begin_func->inst->user.use_list[1].Val)<num)
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,one_param[param_index]->inst->user.use_list->Val,one_param[get_name_index(begin_func->inst->user.use_list[1].Val)]->inst->user.use_list->Val);
                            else if((param_index=get_name_index(begin_func->inst->user.use_list->Val))<num)
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,one_param[param_index]->inst->user.use_list->Val,HashMapGet(left_alias_map,begin_func->inst->user.use_list[1].Val));
                            else if((param_index=get_name_index(begin_func->inst->user.use_list[1].Val))<num)
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode, HashMapGet(left_alias_map,begin_func->inst->user.use_list->Val),one_param[param_index]->inst->user.use_list->Val);
                            else
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,HashMapGet(left_alias_map,begin_func->inst->user.use_list->Val),HashMapGet(left_alias_map,begin_func->inst->user.use_list[1].Val));
                        }
                        //%1,1
                        else if(begin_func->inst->user.use_list->Val->VTy->ID!=Int && begin_func->inst->user.use_list->Val->VTy->ID!=Float && begin_tmp(begin_func->inst->user.use_list->Val->name))
                        {
                            if((param_index= get_name_index(begin_func->inst->user.use_list->Val))<num)
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,one_param[param_index]->inst->user.use_list->Val,begin_func->inst->user.use_list[1].Val);
                            else
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,HashMapGet(left_alias_map,begin_func->inst->user.use_list->Val),begin_func->inst->user.use_list[1].Val);
                        }
                        else
                        {
                            if((param_index= get_name_index(v2))<num)
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val,one_param[param_index]->inst->user.use_list->Val);
                            else
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val,HashMapGet(left_alias_map,begin_func->inst->user.use_list[1].Val));
                        }
                    }

                    node_copy = new_inst_node(ins_copy);

                    if(begin_func->inst->Opcode == Label){
                        ins_copy->user.value.pdata->instruction_pdata.true_goto_location = begin_func->inst->user.value.pdata->instruction_pdata.true_goto_location;
                        BasicBlock *new_block = bb_create();
                        new_block->head_node = node_copy;
                        new_block->Parent = instruction->Parent->Parent;
                        //TODO new_block->preBlocks
                        if(cur_new_block == NULL)
                            new_block->preBlocks = instruction->Parent->preBlocks;
//                        else
//                            bb_add_prev(cur_new_block,)
                        new_block->id = index;
                        index++;
                        //new_block->id = begin_func->inst->user.value.pdata->instruction_pdata.true_goto_location;
                        cur_new_block = new_block;
                        HashMapPut(block_map, begin_func->inst->Parent, new_block);
                        HashSetAdd(block_set, begin_func->inst->Parent);
                    }
                    else if(begin_func->inst->Opcode == br || begin_func->inst->Opcode == br_i1){
                        node_copy->inst->user.value.pdata = begin_func->inst->user.value.pdata;
                        if(cur_new_block == NULL)
                            instruction->Parent->tail_node = node_copy;
                        else
                            cur_new_block->tail_node = node_copy;
                    }


                    if(begin_func->inst->Opcode == Phi){
                        Value *v_left_now=ins_get_value_with_name_and_index(ins_copy,index);
                        v_left_now->IsPhi = true;
                        v_left_now->VTy = begin_func->inst->user.value.VTy;
                        HashMapPut(phi_map, &begin_func->inst->user.value, &ins_copy->user.value);
                        HashMapPut(left_alias_map,&begin_func->inst->user.value,v_left_now);
                    }
                    else if(v->name!=NULL)  //左值有名字
                    {
                        Value *v_left_now=ins_get_value_with_name_and_index(ins_copy,index);
                        index++;
                        //左值的pdata可能需要copy
                        //v_left_now->pdata->var_pdata.iVal=begin_func->inst->user.value.pdata->var_pdata.iVal;
                        v_left_now->pdata=begin_func->inst->user.value.pdata;
                        v_left_now->alias=begin_func->inst->user.value.alias;     //数组的话，需要alias--->v_array才能打印
                        v_left_now->VTy = begin_func->inst->user.value.VTy;

                        //说明有左值
                        //建立原%1与现%1的alias关系
                        HashMapPut(left_alias_map,&begin_func->inst->user.value,v_left_now);
                    }

                    //设置所属基本块
                    if(cur_new_block == NULL)
                        ins_copy->Parent = instruction->Parent;
                    else
                        ins_copy->Parent = cur_new_block;

                    //alloca上提
                    if(ins_copy->Opcode==Alloca)
                    {
                        InstNode *instNode=instruction_node;
                        while(get_prev_inst(instNode)->inst->Opcode!=Alloca)
                            instNode= get_prev_inst(instNode);
                        //将alloca插在instNode前面
                        ins_insert_before(node_copy,instNode);
                    }
                    else
                        ins_insert_before(node_copy,instruction_node);

                    begin_func=get_next_inst(begin_func);
                }

                //如果caller的这个block是function的最后一个block,换成cur_new_block
                if(cur_new_block!=NULL && instruction->Parent->Parent->tail == instruction->Parent)
                    instruction->Parent->Parent->tail = cur_new_block;

                //将最后一个块中的后半段ir速速设parent
                //此时的cur_new_block必然是最后一个新block
                InstNode *reserve = get_next_inst(instruction_node);
                InstNode *prev = NULL;
                while(cur_new_block!=NULL && reserve!=NULL &&reserve->inst->Opcode!=Label){
                    reserve->inst->Parent = cur_new_block;
                    prev = reserve;
                    reserve = get_next_inst(reserve);
                }
                if(cur_new_block!=NULL)
                    cur_new_block->tail_node = prev;


                reduce_phi(phi_map,left_alias_map,block_map);
                if(cur_new_block!=NULL)
                    connect_caller_block(block_map,block_set,instruction->Parent,begin_func->inst->Parent->Parent,cur_new_block);

                //处理最后的一句ret和call
                //将ret的值替换call的左值
                //ret void的情况不用管
                if(ins_get_lhs(begin_func->inst)!=NULL)
                {
                    Value *v_call=&instruction->user.value;
                    Value *v_return=begin_func->inst->user.use_list->Val;

                    if(v_return->VTy->ID!=Int)
                        valueReplaceAll(v_call, HashMapGet(left_alias_map,v_return),instruction->Parent->Parent);
                    else if(begin_tmp(v_return->name) && ((param_index=get_name_index(v_return))<num))
                        valueReplaceAll(v_return,one_param[param_index]->inst->user.use_list->Val, instruction->Parent->Parent);
                    else
                        valueReplaceAll(v_call,v_return,instruction->Parent->Parent);
                }

                //one_param里面的ir(give_param)也可以从链中删掉了
                for(int i=0;i<v_func->pdata->symtab_func_pdata.param_num;i++){
                    deleteIns(one_param[i]);
                }

                //删掉当前这句call
                InstNode *pre=get_prev_inst(instruction_node);
                deleteIns(instruction_node);
                instruction_node=pre;

                //对map进行内容清0
                HashMapClean(left_alias_map);
                HashMapClean(block_map);
                HashSetClean(block_set);
                HashMapClean(phi_map);
            }
        }
        else if(instruction->Opcode==GIVE_PARAM)
        {
            //加到对应数组里
            params[give_count++]=instruction_node;
        }
        instruction_node= get_next_inst(instruction_node);
    }

    //将被内联的ir函数删除
    //修改这句call ir
    InstNode *tmp= get_next_inst(start);
    //找到第一个function的
    while(tmp->inst->Parent->Parent == NULL){
        tmp = get_next_inst(tmp);
    }

    BasicBlock *block_ = tmp->inst->Parent;

    for(Function *currentFunction = block_->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        BasicBlock *entry = currentFunction->entry;
        BasicBlock *end = currentFunction->tail;

        InstNode *currNode = entry->head_node;

        Value *funcValue = currNode->inst->user.use_list->Val;

        if(funcValue->pdata->symtab_func_pdata.flag_inline==1)
        {
            while (currNode != get_next_inst(end->tail_node)) {
                InstNode *next=get_next_inst(currNode);
                deleteIns(currNode);
                currNode=next;
            }
        }
        else
            renameVariables(currentFunction);
    }

    HashMapDeinit(left_alias_map);
    HashMapDeinit(block_map);
    HashMapDeinit(phi_map);
    HashSetDeinit(block_set);
}