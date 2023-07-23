#include "fix_array.h"
int get_dimension(Value* value)
{
    Instruction *instruction = (Instruction*)value;
    int dimension = -1;
    //第一条一定是GEP，循环一定被执行至少一次
    while(instruction->Opcode == GEP)
    {
        //1.找到value1
        Value *v = ins_get_lhs(instruction);
        //2.将instruction换为v的instruction
        instruction = (Instruction*)v;
        dimension++;
    }
    return dimension;
}

Value *get_alloca_alias(Value* value)
{
    Instruction *instruction = (Instruction*)value;
    while (instruction->Opcode != Alloca){
        //1.找到value1
        Value *v = ins_get_lhs(instruction);
        //2.将instruction换为v的instruction
        instruction = (Instruction*)v;
    }
    return ins_get_dest(instruction);
}

Value *get_source_value(Value* value)
{
    Instruction *instruction = (Instruction*)value;
    Value *v_source = NULL;
    //第一条一定是GEP，循环一定被执行至少一次
    while(instruction->Opcode == GEP)
    {
        //1.找到value1
        v_source = ins_get_lhs(instruction);
        //2.将instruction换为v的instruction
        instruction = (Instruction*)v_source;
    }
    return v_source;
}

//专门的一维不处理
bool can_cut(Instruction* instruction)
{
    //一定不能cut的情况是offset是0或者是一维数组
    if(instruction->user.value.pdata->var_pdata.is_offset != 1 || instruction->user.value.alias->pdata->symtab_array_pdata.dimention_figure==1)
        return false;

    if(instruction->user.value.use_list == NULL)
        return true;
    else
    {
        Use* use=instruction->user.value.use_list;
        bool cut=true;
        while(use!=NULL)
        {
            Value *left_user = &use->Parent->value;
            Instruction *instruction1 = (Instruction*)left_user;
            //看用到的use有没有正的偏移值，如果没有就不能噶掉这条，如果有就噶
            if(instruction1->Opcode != GEP || left_user->pdata->var_pdata.is_offset==0)
            {
                cut=false;
                break;
            }
            use=use->Next;
        }
        return cut;
    }
}

bool can_cut_after_(Instruction* instruction)
{
    //一维数组不处理
    if(instruction->user.value.alias->pdata->symtab_array_pdata.dimention_figure==1)
        return false;

    if(instruction->user.value.use_list == NULL)
        return true;
    else
    {
        Use* use=instruction->user.value.use_list;
        bool cut=true;
        while(use!=NULL)
        {
            Value *left_user=&use->Parent->value;
            Instruction *instruction1 = (Instruction*)left_user;
            //看用到的use有没有正的偏移值，如果没有就不能噶掉这条，如果有就噶
            //没有
            if(instruction1->Opcode != GEP || ins_get_rhs(instruction1)->pdata->var_pdata.is_offset == 0)
            {
                cut=false;
                break;
            }
            use=use->Next;
        }
        return cut;
    }
}

//所有use都不是gep了，想标-1就可以标-1了
bool use_not_gep(Instruction* instruction)
{

    if(instruction->user.value.use_list != NULL)
    {
        Use* use=instruction->user.value.use_list;
        bool flag=true;
        while(use!=NULL)
        {
            Value *left_user=&use->Parent->value;
            Instruction *instruction1 = (Instruction*)left_user;
            //看用到的use有没有正的偏移值，如果没有就不能噶掉这条，如果有就噶
            //没有
            if(instruction1->Opcode == GEP)
            {
                flag=false;
                break;
            }
            use=use->Next;
        }
        return flag;
    }
    return true;
}

void fix_array(struct _InstNode *instruction_node)
{
    InstNode *start = instruction_node;
    //将offset值全部清0,并更新传参数组的alias
    HashMap* aliasMap = HashMapInit();
    instruction_node = get_next_inst(instruction_node);
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        Instruction *instruction = instruction_node->inst;
        instruction->user.value.pdata->var_pdata.is_offset=0;

        //更新传参数组的alias
        if(instruction->Opcode == GEP)
        {
            Value *v_array = ins_get_dest(instruction)->alias;
            if(v_array->VTy->ID == AddressTyID && !HashMapContain(aliasMap,v_array))
            {
                v_array->alias = get_source_value(ins_get_dest(instruction));
                HashMapPut(aliasMap,v_array,v_array->alias);
            }
        }
        instruction_node= get_next_inst(instruction_node);
    }
    HashMapDeinit(aliasMap);

    //fix array
    instruction_node = start;
    instruction_node= get_next_inst(instruction_node);
    int offset=0;

    bool after_ = false;
    Value *v_array=NULL;
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        Instruction *instruction = instruction_node->inst;
        int dimension;

        switch (instruction_node->inst->Opcode)
        {
            case GEP:
                /** 1. 先取出第一个操作数的value的iVal，是当前累积量
                * 2. 算出左值的累积量，替换左值的iVal*/

                dimension=instruction->user.value.pdata->var_pdata.iVal;
                //这里的v_array是上一条的v_array
                //dimension==0，即新的起点重置after_
                if(v_array!=NULL && dimension==0)
                    after_ = false;
                v_array=instruction->user.value.alias;
                if(instruction->user.use_list[1].Val->VTy->ID!=Int)
                {
                    //标记这组gep已经不能完全算出了
                    after_=true;
                }
                else
                {
                    //第一下是一维，一定是一个全新开始
                    if(dimension+1!=v_array->pdata->symtab_array_pdata.dimention_figure && dimension==0)
                    {
                        offset=instruction->user.use_list[1].Val->pdata->var_pdata.iVal*(get_array_total_occupy(v_array,dimension+1)/4);
                    }
                        //最后一维
                    else if(dimension+1==v_array->pdata->symtab_array_pdata.dimention_figure)
                        offset=instruction->user.use_list[1].Val->pdata->var_pdata.iVal+instruction->user.use_list->Val->pdata->var_pdata.iVal;
                    else
                    {
                        offset=instruction->user.use_list[1].Val->pdata->var_pdata.iVal*(get_array_total_occupy(v_array,dimension+1)/4) + instruction->user.use_list->Val->pdata->var_pdata.iVal;
                    }
                    //将左值的iVal替换为本层增加的偏移量
                    instruction->user.value.pdata->var_pdata.iVal=offset;
                    if(after_==false)
                        instruction->user.value.pdata->var_pdata.is_offset=1;
                    else
                    {
                        instruction->user.use_list[1].Val->pdata->var_pdata.is_offset=1;     //第二个偏移量必然是var_int,这里设个offset标记一下
                        //再打个alias,方便替换上前继value
                        //拿到前继ir
                        Instruction *prev = (Instruction*)instruction->user.use_list->Val;
                        if(prev->Opcode==GEP && prev->user.use_list[1].Val->pdata->var_pdata.is_offset==1)
                            instruction->user.use_list[1].Val->alias=prev->user.use_list[1].Val->alias;
                        else
                            //它的上一维gep的左值
                            instruction->user.use_list[1].Val->alias=instruction->user.use_list->Val;
                    }
                }
                break;
            default:
                after_=false;
                break;
        }
        instruction_node= get_next_inst(instruction_node);
    }

    fix_array2(start);
}

void fix_array2(struct _InstNode *instruction_node)
{
    instruction_node= get_next_inst(instruction_node);
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        Instruction *instruction = instruction_node->inst;

//        if(instruction->Opcode == GEP)
//            printf("hehr\n");

        //这两条可以合并了(在不是一维数组的情况下)
        //如果没噶掉的话，当前instruction的偏移没改
        if(instruction->Opcode==GEP && can_cut(instruction))
        {
            //对第一条的左值进行处理
            if(instruction->user.value.use_list!=NULL)
            {
                Use* use=instruction->user.value.use_list;
                while(use!=NULL)
                {
                    Value left_user=use->Parent->value;
                    //有
                    //计算这个instruction为最终偏移
                    Value *v_array=left_user.alias;

                    Value *v_offset=(Value*) malloc(sizeof (Value));
                    value_init_int(v_offset,left_user.pdata->var_pdata.iVal);

                    Use* use_store=use;
                    use=use->Next;

                    if(v_array->VTy->ID == AddressTyID)
                        use_set_value(use_store,v_array->alias);
                    else{
                        //直接从left_user找alloca
                        use_set_value(use_store,get_alloca_alias(&left_user));
                    }
                    //use_set_value(use_store,v_array->alias);
                    //ir里的第二个use
                    use_set_value(&use_store->Parent->use_list[1],v_offset);
                    //user
                    use_store->Parent->value.pdata->var_pdata.iVal=-1;
                }

                //删掉当前这条,且此时下一条的偏移已经算出
                InstNode *now=instruction_node;
                instruction_node= get_prev_inst(instruction_node);
                //直接噶了
                deleteIns(now);
            }
            else
            {
                InstNode *now=instruction_node;
                instruction_node= get_prev_inst(instruction_node);
                //直接噶了
                deleteIns(now);
            }
        }

            //定义不会出现这种情况，只可能在赋值时出现，赋值一次都是多条，不存在复用( 优化后就有可能了)
            //是前面出现过不能约的情况,看看后面能不能再化简
        else if((instruction->Opcode==GEP &&  instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==1
                 && can_cut_after_(instruction)))
        {
            //直接找到gep的最后一条,其他的全噶了
            //有没有想过后面噶不掉呢

            //还是只能噶这一条
            if(instruction->user.value.use_list!=NULL)
            {
                Use* use=instruction->user.value.use_list;
                while(use!=NULL)
                {
                    Value left_user=use->Parent->value;

                    Value *v_offset=(Value*) malloc(sizeof (Value));
                    value_init_int(v_offset,left_user.pdata->var_pdata.iVal);

                    Use* use_store=use;
                    use=use->Next;

                    use_set_value(use_store,instruction->user.use_list[1].Val->alias);
                    //ir里的第二个use
                    //如果偏移value有标志位的话，进行转移
                    if(instruction->user.use_list[1].Val->pdata->var_pdata.is_offset == 1)
                    {
                        v_offset->pdata->var_pdata.is_offset = 1;
                        v_offset->alias = instruction->user.use_list[1].Val->alias;
                    }
                    use_set_value(&use_store->Parent->use_list[1],v_offset);
                    //user
                    use_store->Parent->value.pdata->var_pdata.iVal=-2;
                }

                //删掉当前这条,且此时下一条的偏移已经算出
                InstNode *now=instruction_node;
                instruction_node= get_prev_inst(instruction_node);
                //直接噶了
                deleteIns(now);
            }
            else
            {
                InstNode *now=instruction_node;
                instruction_node= get_prev_inst(instruction_node);
                //直接噶了
                deleteIns(now);
            }
        }
        //上一种情况，比如d[b][3]的3,但是下一条不是GEP了，或者下条是GEP,但是无法简化
        //将iVal还原回原本维度
        //一维数组肯定走不到这里
        // 没噶掉的instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==1

        //GEP后面没被gep用过了，如果自身可化简，就不用还原为原先维度
        //数组传参自定义的最后一条要么-1，要么走这里
        else if(instruction->Opcode==GEP && instruction->user.value.pdata->var_pdata.iVal>=0 && instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==1)
        {
            instruction->user.value.pdata->var_pdata.iVal = get_dimension(&instruction->user.value);
        }
            //b[3][k]的3
            //没噶掉的instruction->user.value.pdata->var_pdata.is_offset==1
        else if(instruction->Opcode==GEP && instruction->user.value.pdata->var_pdata.iVal>=0 && instruction->user.value.pdata->var_pdata.is_offset==1 && instruction->user.value.alias->pdata->symtab_array_pdata.dimention_figure!=1)
        {
            instruction->user.value.pdata->var_pdata.iVal = get_dimension(&instruction->user.value);
            //instruction->user.value.pdata->var_pdata.is_offset=0;
        }
            //一维数组,is_offset=1
            //或者是它后面没有use是gep，但它的offset是1
        else if(instruction->Opcode==GEP && instruction->user.value.alias->pdata->symtab_array_pdata.dimention_figure==1 && instruction->user.value.pdata->var_pdata.is_offset==1
                 || (instruction->Opcode==GEP && instruction->user.value.pdata->var_pdata.iVal == 0 && instruction->user.value.pdata->var_pdata.is_offset==1 && use_not_gep(instruction)))
        {
            instruction->user.value.pdata->var_pdata.iVal=-1;
        }

        //第一维特判，是常数直接给-1
//        if(instruction->Opcode==GEP && get_dimension(&instruction->user.value) == 0 && instruction->user.value.pdata->var_pdata.is_offset==1)
//            instruction->user.value.pdata->var_pdata.iVal=-1;
        instruction_node= get_next_inst(instruction_node);
    }
}