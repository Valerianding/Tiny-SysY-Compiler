//
// Created by Valerian on 2023/5/14.
//

#include "inscomb.h"
/*
 * 目前做了一个基本块内的,int类型的inscomb
 */

//判断类型是不是加减,TODO 乘除未做
//检查type与是否有常数
//检查后面是否被用到过
//location为0指前面那条，location为1指后面那条
bool check(Instruction *instruction,int location)
{
    if(((location==0 && instruction->user.value.use_list!=NULL) || (location==1)) && (instruction->Opcode==Add || instruction->Opcode==Sub))
    {
        if(ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float || ins_get_rhs(instruction)->VTy->ID==Int ||
                ins_get_rhs(instruction)->VTy->ID==Float)
            return true;
        return false;
    }
    return false;
}

//获取常数value的位置,value1或value2
int get_const_location(Instruction* instruction)
{
    if(ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float)
        return 1;
    return 2;
}

Value *get_value(Instruction *instruction)
{
    //如果两边都是int,取左边作为const，右边作value
    if((ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float) && (ins_get_rhs(instruction)->VTy->ID==Int || ins_get_rhs(instruction)->VTy->ID==Float))
        return ins_get_rhs(instruction);

    if(ins_get_lhs(instruction)->VTy->ID!=Int && ins_get_lhs(instruction)->VTy->ID!=Float)
        return ins_get_lhs(instruction);
    return ins_get_rhs(instruction);
}

Value *get_const(Instruction *instruction)
{
    if(ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float)
        return ins_get_lhs(instruction);
    return ins_get_rhs(instruction);
}

//合并
void combination(Instruction *instruction_A,Instruction * instruction_B)
{
    //合并得到常值
    Value *v_A_const= get_const(instruction_A);
    Value *v_A_value= get_value(instruction_A);
    Value *v_B_const= get_const(instruction_B);

    Value *v_num=(Value*) malloc(sizeof (Value));
    //以第二条的opcode为准
    if((instruction_A->Opcode==Add && instruction_B->Opcode==Add) || (instruction_A->Opcode==Sub && instruction_B->Opcode==Sub))  //+ +
        value_init_int(v_num,v_A_const->pdata->var_pdata.iVal+v_B_const->pdata->var_pdata.iVal);
    else if(instruction_A->Opcode==Add && instruction_B->Opcode==Sub)
        value_init_int(v_num,v_B_const->pdata->var_pdata.iVal-v_A_const->pdata->var_pdata.iVal);
    else if(instruction_A->Opcode==Sub && instruction_B->Opcode==Add && get_const_location(instruction_A)==1)     //要改opcode,并且生成后value一定在右边
        value_init_int(v_num,v_B_const->pdata->var_pdata.iVal+v_A_const->pdata->var_pdata.iVal);
    else
        value_init_int(v_num,v_B_const->pdata->var_pdata.iVal-v_A_const->pdata->var_pdata.iVal);

    //B的左值作为最终左值,不动
    //替换常数值和另一个value
    if(instruction_A->Opcode==Sub && instruction_B->Opcode==Add && get_const_location(instruction_A)==1)
    {
        instruction_B->Opcode=Sub;
        replace_lhs_operand(instruction_B,v_num);
        replace_rhs_operand(instruction_B,v_A_value);
    }
    if(get_const_location(instruction_B)==1)
    {
        replace_lhs_operand(instruction_B,v_num);
        replace_rhs_operand(instruction_B,v_A_value);
    }
    else
    {
        replace_rhs_operand(instruction_B,v_num);
        replace_lhs_operand(instruction_B,v_A_value);
    }
}

void instruction_combination(Function *currentFunction)
{
    //label_cancomb(currentFunction);
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = get_next_inst(entry->head_node);

    while (currNode != get_next_inst(end->tail_node)) {
        int flag=1;
        if(check(currNode->inst,0))
        {
            Instruction *instruction=currNode->inst;
            Use* use=instruction->user.value.use_list;
            while(use!=NULL)
            {
                Value *left_user=&use->Parent->value;
                Instruction *instruction2=(Instruction*)left_user;
                //判断两条ir在同一基本块
                //如果有不在同一基本块的ir，暂时默认动不了这条ir
                if(instruction2->Parent->id==instruction->Parent->id && check(instruction2,1))
                {
                    use=use->Next;
                    //给当前这条node打条标记吧
                    if(flag)
                        currNode->inst->user.value.pdata->var_pdata.iVal=-10;
                    combination(currNode->inst,instruction2);
                }
                else        //TODO 提取加一个能否combi的判定,(好像不用，可以部分combi，部分不combi，也可以
                {
                    flag=0;
                    currNode->inst->user.value.pdata->var_pdata.iVal=0;
                    use=use->Next;
                }
            }
        }
        currNode= get_next_inst(currNode);
    }

    currNode = get_next_inst(entry->head_node);

    while (currNode != get_next_inst(end->tail_node)) {
        if(currNode->inst->user.value.pdata->var_pdata.iVal==-10)
        {
            InstNode *now=currNode;
            currNode= get_prev_inst(currNode);
            deleteIns(now);
        }
        currNode= get_next_inst(currNode);
    }
}