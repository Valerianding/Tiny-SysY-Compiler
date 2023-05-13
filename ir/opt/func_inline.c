#include "func_inline.h"

extern InstNode * one_param[];   //存放单次正确位置的参数
extern InstNode* params[];      //存放所有参数

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

void func_inline(struct _InstNode* instruction_node)
{
    InstNode *start=instruction_node;
    instruction_node= get_next_inst(instruction_node);
    InstNode *begin_func=NULL;

    int give_count = 0;
    int one_param_count=0;
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        Instruction *instruction = instruction_node->inst;

        if(instruction->Opcode==Call)
        {
            int num=0;
            //遍历ir找到对应的函数ir,进行一波复制
            if((begin_func=find_func_begin(start,instruction->user.use_list->Val->name))!=NULL)
            {
                Value *v_func=begin_func->inst->user.use_list->Val;
                //判断被内联的函数有无参数
                //有参
                if(begin_func->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num!=0)
                {
                    //记一下第一个alloca的数值，比它小的都是参数，要区别处理(原value的alias为NULL)
                    num= get_name_index(&get_next_inst(begin_func)->inst->user.value);

                    //在one_param里填满参数
                    get_param_list(v_func,&give_count);
                }

                //无参数直接上手
                //复制ir
                begin_func=get_next_inst(begin_func);     //函数中的第一条ir
                int index=1;
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
                        if(begin_func->inst->user.use_list->Val->VTy->ID!=Int && begin_func->inst->user.use_list->Val->VTy->ID!=Float)
                            ins_copy= ins_new_unary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val->alias);
                        else
                            ins_copy= ins_new_unary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val);
                    }
                    else   //2操作数
                    {
                        if((begin_func->inst->user.use_list->Val->VTy->ID==Int || begin_func->inst->user.use_list->Val->VTy->ID==Float) && (begin_func->inst->user.use_list[1].Val->VTy->ID==Int || begin_func->inst->user.use_list[1].Val->VTy->ID==Float))
                            ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val,begin_func->inst->user.use_list[1].Val);
                        else if(begin_func->inst->user.use_list->Val->VTy->ID!=Int && begin_func->inst->user.use_list->Val->VTy->ID!=Float && begin_func->inst->user.use_list[1].Val->VTy->ID!=Int && begin_func->inst->user.use_list[1].Val->VTy->ID!=Float)
                        {
                            //其中对参数的store要特别处理
                            if(begin_func->inst->Opcode==Store && v_func->pdata->symtab_func_pdata.param_num!=0 && get_name_index(begin_func->inst->user.use_list->Val)<num)
                            {
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,one_param[one_param_count++]->inst->user.use_list->Val,begin_func->inst->user.use_list[1].Val->alias);
                            }
                            else
                                ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val->alias,begin_func->inst->user.use_list[1].Val->alias);
                        }

                        else if(begin_func->inst->user.use_list->Val->VTy->ID!=Int && begin_func->inst->user.use_list->Val->VTy->ID!=Float)
                            ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val->alias,begin_func->inst->user.use_list[1].Val);
                        else
                            ins_copy= ins_new_binary_operator(begin_func->inst->Opcode,begin_func->inst->user.use_list->Val,begin_func->inst->user.use_list[1].Val->alias);
                    }


                    if(v->name!=NULL)  //左值有名字
                    {
                        Value *v_left_now=ins_get_value_with_name_and_index(ins_copy,index);
                        index++;
                        //左值的pdata可能需要copy
                        //v_left_now->pdata->var_pdata.iVal=begin_func->inst->user.value.pdata->var_pdata.iVal;
                        v_left_now->pdata=begin_func->inst->user.value.pdata;
                        v_left_now->alias=begin_func->inst->user.value.alias;     //数组的话，需要alias--->v_array才能打印

                        //说明有左值
                        //建立原%1与现%1的alias关系
                        begin_func->inst->user.value.alias=v_left_now;
                    }

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
                one_param_count=0;

                //处理最后的一句ret和call
                //将ret的值替换call的左值
                //ret void的情况不用管
                if(ins_get_lhs(begin_func->inst)!=NULL)
                {
                    Value *v_call=&instruction->user.value;
                    Value *v_return=begin_func->inst->user.use_list->Val;

                    if(v_return->VTy->ID!=Int)
                        value_replaceAll(v_call,v_return->alias);
                    else
                        value_replaceAll(v_call,v_return);
                }
                //删掉当前这句call
                InstNode *pre=get_prev_inst(instruction_node);
                deleteIns(instruction_node);
                instruction_node=pre;
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
    //TODO kk现在写死了,等待判断哪些函数要内联
//    begin_func=find_func_begin(start,"kk");
//    while(begin_func->inst->Opcode!=FunEnd)
//    {
//        InstNode *next=get_next_inst(begin_func);
//        deleteIns(begin_func);
//        begin_func=next;
//    }
//    //然后再把这句FunEnd删了
//    deleteIns(begin_func);

    InstNode *temp = get_next_inst(start);
    //找到第一个function的
    while(temp->inst->Parent->Parent == NULL){
        temp = get_next_inst(temp);
    }
    BasicBlock *block = temp->inst->Parent;

    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        renameV(currentFunction);
    }
}

//无br跳转的重命名
void renameV(Function *currentFunction) {
    bool haveParam = false;
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = entry->head_node;

    //currNode的第一条是FunBegin,判断一下是否有参
    Value *funcValue = currNode->inst->user.use_list->Val;
    if (funcValue->pdata->symtab_func_pdata.param_num > 0)
        haveParam = true;

    //开始时候为1或__
    int countVariable = 0;
    if (haveParam){
        //更新第一个基本块
        countVariable += funcValue->pdata->symtab_func_pdata.param_num;
        currNode->inst->Parent->id = countVariable;
    }
    countVariable++;

    currNode = get_next_inst(currNode);
    while (currNode != get_next_inst(end->tail_node)) {
        if (currNode->inst->Opcode != br && currNode->inst->Opcode != br_i1) {
            // 普通的instruction语句
            char *insName = currNode->inst->user.value.name;

            //如果不为空那我们可以进行重命名
            if (insName != NULL && insName[0] == '%') {
                char newName[10];
                clear_tmp(insName);
                newName[0] = '%';
                int index = 1;
                int rep_count = countVariable;
                while (rep_count) {
                    newName[index] = (rep_count % 10) + '0';
                    rep_count /= 10;
                    index++;
                }
                int j = 1;
                for (int i = index - 1; i >= 1; i--) {
                    insName[j] = newName[i];
                    j++;
                }
                insName[j] = '\0';
                countVariable++;
            }
        }
        currNode = get_next_inst(currNode);
    }
}