#include"register_allocation.h"
struct queue *head;
int *non_available_colors;
extern Symtab *this;
char _type_str[30][30]={{"unknown"},{"param_int"},{"param_float"},{"main_int"},{"main_float"},{"int"},{"float"},{"const_int"},{"const_float"},{"function"},{"void"},{"address"},{"var_int"},{"var_float"},{"globalint"},{"globalfloat"},{"array_const_int"},{"array_const_float"},{"global_array_const_int"},{"global_array_const_float"},{"array_int"},{"array_float"},{"global_arrayint"},{"global_arrayfloat"}};
int *RIG;
struct variable *list_of_variables;
struct tac_var * _tac_var;
int num_of_nodes;
int edge_num;
int var_num=0;
char *func_name_reg;
int k=6;
int rig_num;
struct var_fist_last * var_f_l;
struct reg_now *reg_now_tac;
//char varnum[1000][40];
int func_start;
int func_end;
int _b_start;
int _b_end;
int _numnum;
int block_num;
int if_neicun=0;

struct name_num
{
    int num;
    char *name;
    int ifparam;
}*live;

struct  edge
{
    int a;
    int b;
}*_bian;

void minimize_RIG()
{
   for(int i = 0; i <rig_num; i++)
   {
       if(list_of_variables[i].neighbor_count < k)
       {
            list_of_variables[i].color = REMOVED;
       }
   }
}

void test_minimize_RIG()
{
    for(int i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].color == REMOVED)
        {
            printf("variable: %s has been removed\n", list_of_variables[i].name);
        }
    }        
}


void init_non_available_colors()
{
    int temp = k / 32;
    temp += (k % 32) != 0;
    non_available_colors = (int *)malloc(temp * sizeof(int));
}

void reset_non_available_colors()
{
    int i;
    int temp = k / 32;
    temp += (k % 32) != 0;
    for(i = 0; i < temp; i++)
    {
        non_available_colors[i] = 0;
    }
}

int first_fit_coloring()
{
    // printf("???");
    head = NULL;

    for(int i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].color == NO_COLOR)
        {
            __push(i);
            break;
        }
    }
    while(head != NULL)
    {
        int index = __pop();
        if(visit(index))    return 1;
    }
    return 0;
}

int visit(int index)
{
    int is_removed = 0;
    if(list_of_variables[index].color == REMOVED)
    {
        is_removed = 1;
    }

    for(int i = 0; i < rig_num; i++)
    {
        if(CHECKBIT(index,i))
        {
            if(list_of_variables[i].color > -1)
            {
                SET(list_of_variables[i].color);
                // non_available_colors[list_of_variables[i].color /32] |= (1 << (i % 32) );
            }
            if(list_of_variables[i].color == NO_COLOR && !is_removed)
            {
                __push(i);
            }
        }
    }

    int color = -1; 
    for(int i = 0; i < k; i++)
    {
        if(!CHECK(i))
        //if(!(non_available_colors[i/32] & (1 << (i % 32) )))
        {
            color = i;
            break;
        }
    }

    reset_non_available_colors();

    if(color == -1)
    {
        return 1;
    }
    else
    {
        list_of_variables[index].color = color;
    }
    return 0;
} 

void reg_init()
{
    for(int i=0;i<10000;i++)
    {
        _var[i].def=-1;
        _var[i].use=-1;
        _var[i].name=NULL;
    }
    return ;
}

void printf_llvm_ir_withreg(struct _InstNode *instruction_node)
{
    bool flag_func=false;

    instruction_node= get_next_inst(instruction_node);
    // const char* ll_file= c2ll(file_name);

    // FILE *fptr= fopen(ll_file,"w");

    Value *v_cur_array=NULL;

    int p=0;
    InstNode* params[50];
    InstNode * one_param[50];
    int give_count=0;
    for(int i=0;i<50;i++)
        params[i]=NULL;

    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN)
    {
        Instruction *instruction=instruction_node->inst;
        switch (instruction_node->inst->Opcode)
        {
            case Alloca:
                if(instruction->user.use_list->Val!=NULL && (instruction->user.use_list->Val->VTy->ID==ArrayTy_INT || instruction->user.use_list->Val->VTy->ID==ArrayTy_FLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayInt || instruction->user.use_list->Val->VTy->ID==GlobalArrayFloat || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstINT || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstINT))
                {
                    printf(" %s = alloca ",instruction->user.value.name);
                    //fpintf(fptr," %s = alloca ",instruction->user.value.name);
                    //printf_array(instruction->user.use_list->Val,0,fptr);
                    printf(",align 16\n");
                    //fpintf(fptr,",align 16\n");
                }
                else if(instruction->user.use_list->Val!=NULL && instruction->user.use_list->Val->VTy->ID==AddressTyID)
                {
                    printf(" %s = alloca i32*,align 4\n",instruction->user.value.name);
                    //fpintf(fptr," %s = alloca i32*,align 4\n",instruction->user.value.name);
                }
                else
                {
                    printf(" %s = alloca i32,align 4\n",instruction->user.value.name);
                    //fpintf(fptr," %s = alloca i32,align 4\n",instruction->user.value.name);
                }
                break;
            case Load:
                if(instruction->user.use_list->Val->VTy->ID==AddressTyID)
                {
                    instruction->user.value.VTy->ID=AddressTyID;
                    printf(" %s = load i32*,i32** %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    //fpintf(fptr," %s = load i32*,i32** %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                } else
                {
                    printf(" %s = load i32,i32* %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    //fpintf(fptr," %s = load i32,i32* %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                }
                break;
            case Store:
                if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->VTy->ID!=AddressTyID)
                {
                    printf(" store i32 %d,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    //fpintf(fptr," store i32 %d,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->VTy->ID==AddressTyID)
                {
                    printf(" store i32* %d,i32** %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    //fpintf(fptr," store i32* %d,i32** %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float || instruction->user.use_list->Val->VTy->ID==Const_FLOAT)
                {
                    printf(" store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    //fpintf(fptr," store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                }
                else if(instruction->user.use_list[1].Val->VTy->ID!=AddressTyID)
                {
                    printf(" store i32 %s,i32* %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    //fpintf(fptr," store i32 %s,i32* %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    printf(" store i32* %s,i32** %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    //fpintf(fptr," store i32* %s,i32** %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                break;
            case br:
                printf(" br label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                //fpintf(fptr," br label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                break;
            case br_i1:
                printf(" br i1 %s,label %%%d,label %%%d\n\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                //fpintf(fptr," br i1 %s,label %%%d,label %%%d\n\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case br_i1_false:
                printf(" br i1 false,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                //fpintf(fptr," br i1 false,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case br_i1_true:
                printf(" br i1 true,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                //fpintf(fptr," br i1 true,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case EQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }

                break;
            case LESS:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case NOTEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case GREAT:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case GREATEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case LESSEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case FunBegin:
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID==VoidTyID)
                {
                    printf("define dso_local void @%s(",instruction->user.use_list->Val->name);
                    //fpintf(fptr,"define dso_local void @%s(",instruction->user.use_list->Val->name);
                } else
                {
                    printf("define dso_local i32 @%s(",instruction->user.use_list->Val->name);
                    //fpintf(fptr,"define dso_local i32 @%s(",instruction->user.use_list->Val->name);
                }
                p=instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;
                int ii=p;
                while (p>0)
                {
                    if(p==instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num)
                    {
                        if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
                        {
                            printf("i32* %%0");
                            //fpintf(fptr,"i32* %%0");
                        }
                        else
                        {
                            printf("i32 %%0");
                            //fpintf(fptr,"i32 %%0");
                        }
                    }
                    else
                    {
                        if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
                        {
                            printf(",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            //fpintf(fptr,",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                        }
                        else
                        {
                            printf(",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            //fpintf(fptr,",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                        }
                    }
                    p--;
                }
                printf(") #0{\n");
                //fpintf(fptr,") #0{\n");
                break;
            case Return:
                if(instruction->user.use_list->Val==NULL)
                {
                    printf(" ret void\n");
                    //fpintf(fptr," ret void\n");
                }
                else if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    printf(" ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
                    //fpintf(fptr," ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    printf(" ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
                    //fpintf(fptr," ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
                }
                else
                {
                    printf(" ret i32 %s\n",instruction->user.use_list->Val->name);
                    //fpintf(fptr," ret i32 %s\n",instruction->user.use_list->Val->name);
                }

//                printf("}\n\n");
//                //fpintf(fptr,"}\n\n");
                break;
            case Call:
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        printf(" %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
                        //fpintf(fptr," %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                        //是库函数
                    else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
                    {
                        printf(" %s = call i32 (",instruction->user.value.name);
                        //fpintf(fptr," %s = call i32 (",instruction->user.value.name);
                    }
                    else
                    {
                        printf(" %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
                        //fpintf(fptr," %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                }
                else     //voidTypeID
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        printf(" call void @%s(",instruction->user.use_list->Val->name);
                        //fpintf(fptr," call void @%s(",instruction->user.use_list->Val->name);
                    }
                        //是库函数
                    else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
                    {
                        printf(" call void (");
                        //fpintf(fptr," call void (");
                        //printf(" call void (i32, ...) bitcast (i32 (...)* @%s to i32 (i32, ...)*)(",instruction->user.use_list->Val->name);
                        ////fpintf(fptr," call void (i32, ...) bitcast (i32 (...)* @%s to i32 (i32, ...)*)(",instruction->user.use_list->Val->name);
                    }
                    else
                    {
                        printf(" call void (...) @%s (",instruction->user.use_list->Val->name);
                        //fpintf(fptr," void i32 (...) @%s (",instruction->user.use_list->Val->name);
                    }
                }

                //扫一下params数组
                //还是得从后往前走
                int start=0;
                for(int i=give_count-1;i>0;i--)
                {
                    if(strcmp(instruction->user.use_list->Val->name,params[i]->inst->user.use_list[1].Val->name)==0)
                    {
                        start=i;
                        break;
                    }
                }
                //找出one_param参数列表
                int record_start=start;int j=0;
                while (j<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num)
                {
                    if(strcmp(instruction->user.use_list->Val->name,params[record_start]->inst->user.use_list[1].Val->name)==0)
                    {
                        one_param[instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-j-1]=params[record_start];
                        j++;
                    }
                    record_start--;
                }

                //参数
                if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)==NULL && instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num!=0)
                {
                    for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
                    {
                        if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                        {
                            v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                            //就i32*
                            if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                            {
                                printf("i32*,");
                                //fpintf(fptr,"i32*,");
                            }
                            else
                            {
                                //printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                printf("*,");
                                //fpintf(fptr,"*,");
                            }
                        }
                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                        {
                            printf("i32*,");
                            //fpintf(fptr,"i32*,");
                        }
                        else
                        {
                            printf("i32,");
                            //fpintf(fptr,"i32,");
                        }
                    }
                    printf("...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);
                    //fpintf(fptr,"...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);

                    for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
                    {
                        if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                        {
                            v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                            if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                            {
                                printf("i32*,");
                                //fpintf(fptr,"i32*,");
                            }
                            else
                            {
                                //printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                printf("*,");
                                //fpintf(fptr,"*,");
                            }
                        }
                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                        {
                            printf("i32*,");
                            //fpintf(fptr,"i32*,");
                        }
                        else
                        {
                            printf("i32,");
                            //fpintf(fptr,"i32,");
                        }
                    }
                    printf("...)*)(");
                    //fpintf(fptr,"...)*)(");
                }

                //参数
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num!=0)
                {
                    for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
                    {
                        if(i==0)
                        {
                            if(one_param[i]->inst->user.use_list->Val->VTy->ID==Int)
                            {
                                printf("i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                                //fpintf(fptr,"i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                            {
                                v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                                if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                                {
                                    printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                    //fpintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                                else
                                {
                                    //printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                    printf("* %s",one_param[i]->inst->user.use_list->Val->name);
                                    //fpintf(fptr,"* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                            {
                                printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                //fpintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                            else
                            {
                                printf("i32 %s",one_param[i]->inst->user.use_list->Val->name);
                                //fpintf(fptr,"i32 %s",one_param[i]->inst->user.use_list->Val->name);
                            }

                        }
                        else
                        {
                            if(one_param[i]->inst->user.use_list->Val->VTy->ID==Int)
                            {
                                printf(",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                                //fpintf(fptr,",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                            {
                                v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                                if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                                {
                                    printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                    //fpintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                                else
                                {
                                    printf(",");
                                    //fpintf(fptr,",");
                                    //printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                    printf("* ");
                                    //fpintf(fptr,"* ");
                                }
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                            {
                                printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                //fpintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                            else
                            {
                                printf(",i32 %s",one_param[i]->inst->user.use_list->Val->name);
                                //fpintf(fptr,",i32 %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                        }
                    }
                }

                printf(")\n");
                //fpintf(fptr,")\n");

                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num!=0)
                {
                    j=0;
                    //将参数全部置0
                    while(j<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num)
                    {
                        if(strcmp(instruction->user.use_list->Val->name,params[start]->inst->user.use_list[1].Val->name)==0)
                        {
                            params[start--]=NULL;
                            j++;
                        }
                    }
                    j=0;
                    for(int k=0;k<give_count;k++)
                    {
                        if(params[k]!=NULL)
                            params[j++]=params[k];
                    }
                    give_count-=instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;
                }
                break;
            case Label:
                printf("%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                //fpintf(fptr,"%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                break;
            case Add:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Sub:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sub nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sub nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sub nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sub nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sub nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sub nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sub nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sub nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sub nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sub nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sub nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sub nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Mul:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= mul nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= mul nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= mul nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= mul nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= mul nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= mul nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= mul nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= mul nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= mul nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= mul nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= mul nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= mul nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Div:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sdiv i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sdiv i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sdiv i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sdiv i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sdiv i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sdiv i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sdiv i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sdiv i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sdiv i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sdiv i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sdiv i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sdiv i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Mod:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= srem i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= srem i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s= srem i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= srem i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= srem i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= srem i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s= srem i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= srem i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case bitcast:
                //第一条bitcast
                if(instruction->user.value.pdata->var_pdata.iVal==1)
                {
                    //v_cur_array=instruction->user.use_list->Val->alias;
                    printf(" %s=bitcast ",instruction->user.value.name);
                    //fpintf(fptr," %s=bitcast ",instruction->user.value.name);
                    //printf_array(instruction->user.use_list->Val->alias,0,fptr);
                    printf("* %s to i8*\n",instruction->user.use_list->Val->name);
                    //fpintf(fptr,"* %s to i8*\n",instruction->user.use_list->Val->name);
                }
                else
                {
                    printf(" %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
                    //fpintf(fptr," %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
                    //printf_array(instruction->user.use_list->Val->alias,0,fptr);
                    printf("*\n");
                    //fpintf(fptr,"*\n");
                }
                break;

            case GEP:
                if(instruction->user.value.alias!=NULL)
                    v_cur_array=instruction->user.value.alias;
                //printf("%d...\n",instruction->user.value.pdata->var_pdata.iVal);
                printf(" %s=getelementptr inbounds ",instruction->user.value.name);
                //fpintf(fptr," %s=getelementptr inbounds ",instruction->user.value.name);
                if(instruction->user.use_list->Val->VTy->ID!=AddressTyID )
                {
                    //是对数组参数的最后一个自造的gmp
                    if(instruction->user.value.VTy->ID==AddressTyID)
                    {
                        //printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal-1,fptr);
                        printf(",");
                        //fpintf(fptr,",");
                        //printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal-1,fptr);
                        printf("* ");
                        //fpintf(fptr,"* ");
                        if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        {
                            printf("%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                            //fpintf(fptr,"%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        }
                        else
                        {
                            printf("%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                            //fpintf(fptr,"%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        }
                    }
                        //正常的
                    else{
                        //printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
                        printf(",");
                        //fpintf(fptr,",");
                        //printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
                        printf("* ");
                        //fpintf(fptr,"* ");
                        if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        {
                            printf("%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                            //fpintf(fptr,"%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        }
                        else
                        {
                            printf("%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                            //fpintf(fptr,"%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        }
                    }

                }
                else {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf("i32,i32* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr,"i32,i32* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf("i32,i32* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr,"i32,i32* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }

//                if(instruction->user.value.VTy->ID==AddressTyID && (instruction->user.value.pdata->var_pdata.iVal+1<instruction->user.use_list->Val->pdata->symtab_array_pdata.dimention_figure))
//                {
//                    for(int i= instruction->user.value.pdata->var_pdata.iVal+1;i<v_cur_array->pdata->symtab_array_pdata.dimention_figure;i++)
//                    {
//                        printf(",i32 0\n");
//                        //fpintf(fptr,",i32 0\n");
//                    }
//                }
//                else
//                {
                printf("\n");
                //fpintf(fptr,"\n");
                //}
                break;
            case MEMSET:
                flag_func=true;
                printf(" call void @llvm.memset.p0i8.i64(i8* align 16 %s, i8 0, i64 %d, i1 false)\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                //fpintf(fptr," call void @llvm.memset.p0i8.i64(i8* align 16 %s, i8 0, i64 %d, i1 false)\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                break;
            case MEMCPY:
                flag_func=true;
                printf(" call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %s, i8* align 16 bitcast (",instruction->user.use_list->Val->name);
                //fpintf(fptr," call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %s, i8* align 16 bitcast (",instruction->user.use_list->Val->name);
                //printf_array(instruction->user.use_list[1].Val,0,fptr);
                printf("* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
                       get_array_total_occupy(instruction->user.use_list[1].Val,0));
                //fpintf(fptr,"* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
                        // get_array_total_occupy(instruction->user.use_list[1].Val,0));
                break;
            case GLOBAL_VAR:
                if(instruction->user.use_list->Val->VTy->ID!=ArrayTy_INT && instruction->user.use_list->Val->VTy->ID!=ArrayTy_FLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayInt && instruction->user.use_list->Val->VTy->ID!=GlobalArrayFloat && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_ConstINT && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_ConstFLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayConstFLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayConstINT)
                {
                    printf("%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    //fpintf(fptr,"%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                }
                else
                {
                    v_cur_array=instruction->user.use_list->Val;
                    if(instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstINT || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstINT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstFLOAT)
                    {
                        printf("%s= internal constant ",instruction->user.use_list->Val->name);
                        //fpintf(fptr,"%s= internal constant ",instruction->user.use_list->Val->name);
                        ////printf_global_array(instruction->user.use_list->Val,fptr);
                        printf("align 4\n");
                        //fpintf(fptr,"align 4\n");
                    }
                    else
                    {
                        printf("%s=dso_local global ",instruction->user.use_list->Val->name);
                        //fpintf(fptr,"%s=dso_local global ",instruction->user.use_list->Val->name);
                        ////printf_global_array(instruction->user.use_list->Val,fptr);
                        if(instruction->user.use_list->Val->pdata->symtab_array_pdata.is_init==0)
                        {
                            printf(" zeroinitializer, align 4\n");
                            //fpintf(fptr," zeroinitializer, align 4\n");
                        }
                        else
                        {
                            printf("align 4\n");
                            //fpintf(fptr,"align 4\n");
                        }
                    }
                }
                break;
            case GIVE_PARAM:
                params[give_count++]=instruction_node;
                break;
            case FunEnd:
                printf("}\n\n");
                //fpintf(fptr,"}\n\n");
                break;
            case zext:
                printf(" %s = zext i1 %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                //fpintf(fptr," %s = zext i1 %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                break;
            case XOR:
                printf(" %s = xor i1 %s, true\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                //fpintf(fptr," %s = xor i1 %s, true\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                break;
            case Phi:{
                Value *insValue = ins_get_dest(instruction);
                HashSet *phiSet = instruction->user.value.pdata->pairSet;
                HashSetFirst(phiSet);
                printf(" %s( %s) = phi i32",instruction->user.value.name, instruction->user.value.alias->name);
                //fpintf(fptr," %s = phi i32",instruction->user.value.name);
                unsigned int size=HashSetSize(phiSet);
                int i=0;
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    BasicBlock *from = phiInfo->from;
                    Value *incomingVal = phiInfo->define;
                    if(i + 1 == size)      //最后一次
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                            printf("[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
                            //fpintf(fptr,"[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
                        }else if(incomingVal != NULL){
                            printf("[%s , %%%d]",incomingVal->name,from->id);
                            //fpintf(fptr,"[%s , %%%d]",incomingVal->name,from->id);
                        }else{
                            //是NULL的话就
                            printf("[ undef, %%%d] ",from->id);
                            //fpintf(fptr,"[ undef, %%%d] ",from->id);
                        }
                    }
                    else
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                            printf("[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
                            //fpintf(fptr,"[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
                        }else if(incomingVal != NULL){
                            printf("[%s , %%%d], ",incomingVal->name,from->id);
                            //fpintf(fptr,"[%s , %%%d], ",incomingVal->name,from->id);
                        }else{
                            printf("[ undef, %%%d], ",from->id);
                            //fpintf(fptr,"[ undef, %%%d], ",from->id);
                        }
                    }
                    i++;
                }
                printf("\n");
                //printf("a phi instruction\n");
                break;
            }
            case CopyOperation:{
                Value *dest = instruction->user.value.alias;
                Value *src = ins_get_lhs(instruction);
                if(isImm(src)){
                    printf(" %s(%s) = %d\n",dest->name,dest->alias->name,src->pdata->var_pdata.iVal);
                }else{
                    printf(" %s(%s) = %s\n", dest->name,dest->alias->name,src->name);
                }

                //printf("a copy operation\n");
                break;
            }
            default:
                break;
        }

        Value *v,*vl,*vr;
        v= ins_get_dest(instruction_node->inst);
        vl= ins_get_lhs(instruction_node->inst);
        vr= ins_get_rhs(instruction_node->inst);
        if(v!=NULL)
            printf("left:%s,\t",_type_str[v->VTy->ID]);
        if(vl!=NULL)
            printf("value1:%s,\t",_type_str[vl->VTy->ID]);
        if(vr!=NULL)
            printf("value2:%s,\t",_type_str[vr->VTy->ID]);
        printf("\n");
        if(use_type(instruction_node))
        {
            if(v!=NULL&&is_Immediate(v->VTy->ID)==0)
            printf("%d\t",instruction->_reg_[0]);
            if(vl!=NULL&&is_Immediate(vl->VTy->ID)==0)
                printf("%d\t",instruction->_reg_[1]);
            if(vr!=NULL&&is_Immediate(vr->VTy->ID)==0)
                printf("%d\t",instruction->_reg_[2]);   
            printf("\n");
        }
        printf("\n"); 
        instruction_node= get_next_inst(instruction_node);
    }
    if(flag_func)
    {
        //fpintf(fptr,"\n");
        //fpintf(fptr,"declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1\n");
        //fpintf(fptr,"declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2\n");
    }

    //fpintf(fptr,"declare dso_local i32 @getint(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @putint(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @getch(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @getarray(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @getfloat(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @getfarray(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @putch(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @putarray(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @putfloat(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @putfarray(...) #1\n");
    //fpintf(fptr,"declare dso_local i32 @putf(...) #1\n");
}

void reg_control(struct _InstNode *instruction_node,InstNode *temp)
{
    printf("/*******************reg******************/\n");
    // while(temp->inst->Parent->Parent == NULL)   temp = get_next_inst(temp);]

    BasicBlock *block = temp->inst->Parent;
    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next)
    {
        InstNode *currNode = block->head_node;
        while(currNode != NULL) 
        {
            BasicBlock *cblock = currNode->inst->Parent;
            reg_control_block(cblock);
            currNode = get_next_inst(currNode);
        }
    }
    printf_llvm_ir_withreg(instruction_node);
    return ;
}

void reg_control_block(BasicBlock *cur)
{
    #ifdef all_in_memory
    InstNode *temp =cur->head_node;
    for(;temp != NULL;temp = get_next_inst(temp))
    {
        // temp->inst->_reg_[0]=104;
        // temp->inst->_reg_[1]=-4;
        // temp->inst->_reg_[2]=-5;
        reg_inmem_one_ins(temp);
    }
    return ;

    #else
    init_RIG();
    create_RIG();
    check_edge();
    //print_info();
    minimize_RIG();
    init_non_available_colors();

    while(first_fit_coloring())
    {
        reset_colors();
        reset_queue();
        spill_variable();
    }

    color_removed(); 
    end_reg(); //分配并打印
    return ;
    #endif
}


void reg_inmem_one_ins(struct _InstNode *temp)
{
    if(use_type(temp))
    {
        temp->inst->_reg_[0]=104;
        temp->inst->_reg_[1]=-4;
        temp->inst->_reg_[2]=-5;
    }
    return ;
}

int use_type(struct _InstNode *temp)
{
    int opcode = temp->inst->Opcode;
    // printf("op:\t%d\n",opcode);
    if (opcode == 6 || opcode == 12 || opcode == 33)
        return 0;
    return 1;
}


int is_Immediate(int type_id)
{
    if(type_id==5||type_id==6)  return 1;
    return 0;
}

void __push(int variable_index)
{
    struct queue *element = (struct queue *)malloc(sizeof(struct queue));
    element->variable_index = variable_index;
    element->next = NULL;

    if(head == NULL)
    {
        head = element;
    }
    else
    {
        struct queue *temp = head;
        while(temp->next != NULL)
        {
            temp  = temp->next;
        }
        temp->next = element;
    }
}

int __pop()
{
    struct queue *temp = head;
    head = head->next;

    int variable_index = temp->variable_index;
    free(temp);
    return variable_index;
}

void reset_colors()
{
    for(int i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].color >= 0)
        {
            list_of_variables[i].color = NO_COLOR;
        }
    }
}

int supercmp(char *a ,char *b)
{
    //printf("%s\t%s\n",a,b);
    if(a==NULL||b==NULL)    return -1;
    return strcmp(a,b);
}

void reset_queue()
{
    struct queue *temp = head;
    while(head != NULL)
    {
        head = temp->next;
        free(temp);
        temp = head;
    }
}

void color_removed()
{
    for(int i = 0; i < rig_num; i++)
    {

        if(list_of_variables[i].color == REMOVED)
        {
            if(visit(i))
            {
                //printf("color removed failed");
                //需修改
            } 
        }
    }
}

void print_non_available_colors()
{
   for(int i = 0; i < k; i++)
   {
       printf("%d ", CHECK(i));
   }
}


void print_colors()
{
    int i, j;
    char *name;
    int color;
    for(i = 0; i < rig_num; i++)
    {
        name = list_of_variables[i].name;
        color = list_of_variables[i].color;
        if(color == SPILLED)
        {
            printf("var: %s\t is spilled onto stack\n", name);
        }
        else
        {
            printf("var: %s\tcolor: %d\n", name, color);
        }
    }

    printf("\t");
    for(i = 0; i < rig_num; i++)
    {
        printf("%d ", list_of_variables[i].color);
    }
    printf("\n\n");
    for(i = 0; i < rig_num; i++)
    {
        printf("%d\t", list_of_variables[i].color);
		for(j = 0; j < rig_num; j++)
		{
			int bit = CHECKBIT(i,j);
            if(bit)
                printf("1 ");
            else
                printf("0 ");
		}
		printf("\n");
    }
}


void init_RIG()
{
    list_of_variables=NULL;
    num_of_nodes=rig_num;
    RIG=NULL;
}


void create_RIG()
{
    int size = rig_num;
    int temp = 0;
    size = size * size;
    temp = size % 32 != 0;
    size = size / 32;
    size += temp;
    RIG = (int *)malloc(sizeof(int)*size);
    for(int i=0;i<size;i++) RIG[i]=0;
    //print_RIG();

    create_variable_list();
    for(int i=0;i<edge_num;i++)
    {
        create_edge(_bian[i].a, _bian[i].b);
        //printf("%d-%d\n",_bian[i].a, _bian[i].b);
    }
    // printf("\n\nedgenum:\n\n");
    // for(int i=0;i<rig_num;i++)  printf("%d  %s\t %d\n",i,live[i].name,list_of_variables[i].neighbor_count);
}


void create_edge(int first_node, int second_node)
{
    int i=first_node;
    int j=second_node;
    SETBIT(i,j);
    SETBIT(j,i);
    // RIG[(((i*rig_num)+j)/32)] |= (1 << ((i*rig_num+j)%32));
    // RIG[(((j*rig_num)+i)/32)] |= (1 << ((j*rig_num+i)%32));
    list_of_variables[i].neighbor_count++;
    list_of_variables[j].neighbor_count++;
    //printf("%s:%d\t%s:%d\n",live[i].name,list_of_variables[i].neighbor_count,live[j].name,list_of_variables[j].neighbor_count);
}

int find_var(char * str)
{
    if(str==NULL)   return -1;
    for(int i=0;i<rig_num;i++)
    {
        if(supercmp(live[i].name,str)==0)
            return i;
    }
    return -1;
}

void create_variable_list()
{
    int index = 0;
    list_of_variables = (struct variable *)malloc(rig_num * sizeof(struct variable));
    char *name;
    int len;
    struct variable *temp;
    int i;
    for(i = 0; i < rig_num; i++)
    {
        len=1;
        int j=i;
        while(j)
        {
            j/=10;
            len++;
        }
        name = (char *)malloc(len * sizeof(char));
        sprintf(name, "%d", i);
        list_of_variables[i].name = name;
        // printf("%s\n",list_of_variables[i].name);
        list_of_variables[i].color = NO_COLOR;
        list_of_variables[i].neighbor_count = 0;
    }
    // int index = 0;
    // list_of_variables = (struct variable *)malloc(rig_num * sizeof(struct variable));
    // char *name;
    // struct variable *temp;
    // int i;
    // for(i = 0; i < rig_num; i++)
    // { 
    //     name = (char *)malloc(MAXSTRINGSIZE * sizeof(char));
    //     sprintf(name, "%d", i);
    //     list_of_variables[i].name = name;
    //     list_of_variables[i].color = NO_COLOR;
    //     list_of_variables[i].neighbor_count = 0;
    // }
    // for(int i=0;i<rig_num;i++)
    //     printf("%d %s %d %d\n",i,list_of_variables[i].name,list_of_variables[i].color,list_of_variables[i].neighbor_count);
}


void print_RIG()
{
    int i, j;
    printf("RIG:\n");
    //for(int i=0;i<sizeof(RIG)/4;i++)  printf("%d ",RIG[i]);
    printf("\n");
    for(i = 0; i < rig_num; i++)
    {
		for(j = 0; j < rig_num; j++)
		{
			int bit = CHECKBIT(i,j);
            if(bit)
                printf("1 ");
            else
                printf("0 ");
		}
		printf("\n");
    }
}


void spill_variable()
{
    int i;
    int index_of_max = 0;
    int max = 0;
    for(i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].neighbor_count > max && list_of_variables[i].color == NO_COLOR)
        {
            max = list_of_variables[i].neighbor_count;
            index_of_max = i;
        }
    }
    list_of_variables[index_of_max].color = SPILLED;

    for(i = 0; i < rig_num; i++)
    {
        if(CHECKBIT(index_of_max,i))
        {
            list_of_variables[i].neighbor_count--;
        }
    }
}    