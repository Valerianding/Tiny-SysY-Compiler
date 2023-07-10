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
//1. 无跳转(单个基本块)
//2. 不call非库函数
//3. 变量数最大为6(私心多给了1个)
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
        //2.判断有无br或call
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
        while (currNode != get_next_inst(end->tail_node)) {
            //最后的一定出现在左边
            if(currNode->inst->user.value.name!=NULL)
            {
                max_num= get_name_index(&currNode->inst->user.value);
                if(max_num>6)
                {
                    funcValue->pdata->symtab_func_pdata.flag_inline=0;
                    break;
                }
            }

            if(currNode->inst->Opcode==Call || currNode->inst->Opcode==br || currNode->inst->Opcode==br_i1 || currNode->inst->Opcode==br_i1_true)
            {
                if(currNode->inst->Opcode==Call)
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,currNode->inst->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        funcValue->pdata->symtab_func_pdata.flag_inline=0;
                        break;
                    }
                }
                else
                {
                    funcValue->pdata->symtab_func_pdata.flag_inline=0;
                    break;
                }
            }

            currNode = get_next_inst(currNode);
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
                        if(begin_tmp(begin_func->inst->user.use_list->Val->name) &&
                                get_name_index(begin_func->inst->user.use_list->Val)<num)
                        {
                            param_index=get_name_index(begin_func->inst->user.use_list->Val);
                            ins_copy= ins_new_unary_operator(begin_func->inst->Opcode,one_param[param_index]->inst->user.use_list->Val);
                        }
                        //其他正常情况
                        else if(begin_tmp(begin_func->inst->user.use_list->Val->name))
                            ins_copy= ins_new_unary_operator(begin_func->inst->Opcode, HashMapGet(left_alias_map,begin_func->inst->user.use_list->Val));
                        else  //全局
                            ins_copy= ins_new_unary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val);
                    }
                    else   //2操作数
                    {
                        //getint(),@a
                        if(!(begin_tmp(begin_func->inst->user.use_list->Val->name)) && !(begin_tmp(begin_func->inst->user.use_list[1].Val->name)) )
                            ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val,begin_func->inst->user.use_list[1].Val);
                        //%1,%2
                        else if(begin_tmp(begin_func->inst->user.use_list->Val->name) && begin_tmp(begin_func->inst->user.use_list[1].Val->name))
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
                        else if(begin_tmp(begin_func->inst->user.use_list->Val->name))
                        {
                            if((param_index= get_name_index(begin_func->inst->user.use_list->Val))<num)
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,one_param[param_index]->inst->user.use_list->Val,begin_func->inst->user.use_list[1].Val);
                            else
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,HashMapGet(left_alias_map,begin_func->inst->user.use_list->Val),begin_func->inst->user.use_list[1].Val);
                        }
                        else
                        {
                            if((param_index= get_name_index(begin_func->inst->user.use_list[1].Val))<num)
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val,one_param[param_index]->inst->user.use_list->Val);
                            else
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val,HashMapGet(left_alias_map,begin_func->inst->user.use_list[1].Val));
                        }
                    }

                    if(v->name!=NULL)  //左值有名字
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
                    ins_copy->Parent = instruction->Parent;
                    node_copy = new_inst_node(ins_copy);
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
                for(int i=0;i<v_func->pdata->symtab_func_pdata.param_num;i++)
                {
                    deleteIns(one_param[i]);
                }

                //删掉当前这句call
                InstNode *pre=get_prev_inst(instruction_node);
                deleteIns(instruction_node);
                instruction_node=pre;
            }
            //对map进行内容清0
            HashMapClean(left_alias_map);
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
}