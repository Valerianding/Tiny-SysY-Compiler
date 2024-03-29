#include"register_allocation.h"
#define S 14
#define R 10
struct reg_queue *head;
int *non_available_colors;
extern Symtab *this;
char _type_str[30][30]={{"unknown"},{"param_int"},{"param_float"},{"main_int"},{"main_float"},{"int"},{"float"},{"const_int"},{"const_float"},{"function"},{"void"},{"address"},{"var_int"},{"var_float"},{"globalint"},{"globalfloat"},{"array_const_int"},{"array_const_float"},{"global_array_const_int"},{"global_array_const_float"},{"array_int"},{"array_float"},{"global_arrayint"},{"global_arrayfloat"}};
int *RIG;
struct variable *list_of_variables;
struct SString * live_in_name;
struct SString * live_out_name;
int edge_num,edge_num_global;
int var_num,var_num_global;
int KK = 10;
int rig_num;
int reg_param_num;
int block_in_num,block_out_num;
struct  reg_now * echo_tac;
struct  reg_now * echo_tac_global;
struct  reg_now * echo_tac_temp;
// BasicBlock * block_list[1000];
struct BLOCK_list * block_list;
int block_num;
int tac_cnt,tac_cnt_gloabl;
int mem_temp;
struct name_num *live;
struct name_num *live_global;
InstNode * one_param[1000];   //存放单次正确位置的参数
InstNode* params[1000];      //存放所有参数


static int enable_vfp=0; //浮点寄存器分配开关
static int flag_lr=1; //释放lr
static int flag_r11=1; //释放r11,释放了r11，那么就是8个可用寄存器

//还需要一个active(这个可以是PriorityQueue)，和location(这个可以是HashSet)。
PriorityQueue *active;
//最终的寄存器分配方案，location中的成员代表在内存，result中成员代表成功分配寄存器func->lineScanReg
HashSet *location;
//HashMap *result; //key =Value* value=value_register* (int),不需要另外建立，直接使用Function里面的lineScanReg来存放

// 浮点寄存器分配
PriorityQueue *VFPactive;
HashSet *VFPlocation;

int VFPreg[32];
int free_VFPreg_num;
int myreg[16];
int free_reg_num;

struct  reg_edge *_bian;

void live_init_block()
{
    rig_num=0;
    edge_num=0;
    live=(struct name_num *)malloc(sizeof(struct name_num)*((tac_cnt+20)*3));
    for(int i=0;i<(tac_cnt+19)*3;i++)
    {
        live[i].num=i;
        // live[i].name=NULL;
        live[i].name=(char *)malloc(MAXSTRINGSIZE);
        live[i].first_use=-1;
        live[i].last_def=-1;
        live[i].last=-1;
        live[i].first=-1;
        live[i].isin=0;
        live[i].isout=0;
    }
    // _bian=(struct reg_edge *)malloc(sizeof(struct reg_edge)*((tac_cnt+31)*(tac_cnt+30)*9/2));
    // live_in_name=NULL;
    // live_out_name=NULL;
    return ;
}

void live_init_global()
{
    rig_num=0;
    live=(struct name_num *)malloc(sizeof(struct name_num)*((tac_cnt+20)*3));
    for(int i=0;i<(tac_cnt+20)*3;i++)
    {
        live[i].num=i;
        // live[i].name=NULL;
        live[i].name=(char *)malloc(MAXSTRINGSIZE);
        live[i].first_use=-1;
        live[i].last_def=-1;
        live[i].last=-1;
        live[i].first=-1;
        live[i].isin=0;
        live[i].isout=0;
    }
    // _bian=(struct reg_edge *)malloc(sizeof(struct reg_edge)*((tac_cnt+31)*(tac_cnt+30)*9/2));
    // live_in_name=NULL;
    // live_out_name=NULL;
    return ;
}

void init_global(BasicBlock *cur)
{
    int temp_cnt=0;
    tac_cnt_gloabl=0;
    var_num_global=0;
    edge_num=0;
    mem_temp=0;
    InstNode * temp_instruction_node= get_next_inst(cur->head_node);
    while (temp_instruction_node!=NULL && temp_instruction_node->inst->Opcode!=FunEnd)
    {
        temp_cnt++;
        temp_instruction_node= get_next_inst(temp_instruction_node);
    }
    echo_tac_global = (struct reg_now *)malloc(sizeof(struct  reg_now)*(temp_cnt+30));
    for(int i=0;i<temp_cnt+30;i++)
    {
        echo_tac_global[i].dest_name=NULL;
        echo_tac_global[i].left_name=NULL;
        echo_tac_global[i].right_name=NULL;
        echo_tac_global[i].dest_use=-1;
        echo_tac_global[i].left_use=-1;
        echo_tac_global[i].right_use=-1;
        echo_tac_global[i].irnode=NULL;
        echo_tac_global[i].give_param=0;
        echo_tac_global[i].reg_3[0]=0;
        echo_tac_global[i].reg_3[1]=0;
        echo_tac_global[i].reg_3[2]=0;
        for(int j=0;j<13;j++)
            echo_tac_global[i].whoinreg[j]=-1;
        echo_tac_global[i].jumpto[0]=-1;
        echo_tac_global[i].jumpto[1]=-1;
        echo_tac_global[i].jumptoid[0]=-1;
        echo_tac_global[i].jumptoid[1]=-1;
        echo_tac_global[i].from_cnt=0;
        echo_tac_global[i].visited=0;
    }
    _bian=(struct reg_edge *)malloc(sizeof(struct reg_edge)*((temp_cnt+10)*(temp_cnt+10)/2));
    live_global=(struct name_num *)malloc(sizeof(struct name_num)*((temp_cnt+20)*3+reg_param_num));
    for(int i=0;i<(temp_cnt+20)*3+reg_param_num;i++)
    {
        live_global[i].num=i;
        live_global[i].name=(char *)malloc(MAXSTRINGSIZE);
        live_global[i].first_use=-1;
        live_global[i].last_def=-1;
        live_global[i].last=-1;
        live_global[i].first=-1;
        live_global[i].isin=0;
        live_global[i].isout=0;
        live_global[i].kua=0;
        live_global[i].iffuc=0;
    }
    for(int i=0;i<reg_param_num;i++)
    {
        sprintf(live_global[i].name,"%%%d",i);
        live_global[i].iffuc=1;
        // printf("%s\t",live_global[i].name);
        var_num_global++;
    }
    return ;
}
int in_live(char * str)
{
    //printf("in_live:%s\n",str);
    if(str==NULL)    return 0;
    for(int i=0;i<rig_num;i++)
    {
        if(supercmp(live[i].name,str)==0) return 1;
    }
    return 0;
}

void minimize_RIG()
{
    for(int i = 0; i <rig_num; i++)
    {
        if(list_of_variables[i].neighbor_count < KK)
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
    int temp = KK / 32;
    temp += (KK % 32) != 0;
    non_available_colors = (int *)malloc(temp * sizeof(int));
}

void reset_non_available_colors()
{
    int i;
    int temp = KK / 32;
    temp += (KK % 32) != 0;
    for(i = 0; i < temp; i++)
    {
        non_available_colors[i] = 0;
    }
}

int first_fit_coloring()
{
    head = NULL;
    for(int i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].color == NO_COLOR)
        {
            reg__push(i);
            break;
        }
    }
    while(head != NULL)
    {
        int index = reg__pop();
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
            }
            if(list_of_variables[i].color == NO_COLOR && !is_removed)
            {
                reg__push(i);
            }
        }
    }

    int color = -1;
    for(int i = 0; i < KK; i++)
    {
        if(!CHECK(i))
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

void ir_reg_init(InstNode *instruction_node)
{
    instruction_node= get_next_inst(instruction_node);
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN)
    {
        Instruction *instruction=instruction_node->inst;
        instruction->_reg_[0]=0;
        instruction->_reg_[1]=0;
        instruction->_reg_[2]=0;
        instruction_node= get_next_inst(instruction_node);
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
    // InstNode* params[1000];
    // InstNode * one_param[1000];
    int give_count=0;
    // for(int i=0;i<1000;i++)
    //     params[i]=NULL;

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
                    // fprintf(fptr," store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
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
                printf(" br label %%%d\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                //fpintf(fptr," br label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                break;
            case br_i1:
                printf(" br i1 %s,label %%%d,label %%%d\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                //fpintf(fptr," br i1 %s,label %%%d,label %%%d\n\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case br_i1_false:
                printf(" br i1 false,label %%%d,label %%%d\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                //fpintf(fptr," br i1 false,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case br_i1_true:
                printf(" br i1 true,label %%%d,label %%%d\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                //fpintf(fptr," br i1 true,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case EQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }

                break;
            case LESS:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID == Float)
                    {
                        printf(" %s = fcmp olt float i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = fcmp olt float i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID == Float)
                    {
                        printf(" %s = fcmp olt float i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = fcmp olt float i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case NOTEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s = icmp ne i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = icmp ne i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp ne i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp ne i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp ne i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp ne i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s = icmp ne i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = icmp ne i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case GREAT:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID == Float)
                    {
                        printf(" %s = fcmp sgt float i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = fcmp sgt float i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID == Float)
                    {
                        printf(" %s = fcmp sgt float i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = fcmp sgt float i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case GREATEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID == Float)
                    {
                        printf(" %s = fcmp sge float i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = fcmp sge float i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID == Float)
                    {
                        printf(" %s = fcmp sge float i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = fcmp sge float i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case LESSEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s = fcmp sle float %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = fcmp sle float %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s = fcmp ole i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s = fcmp ole i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case FunBegin:
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID==VoidTyID)
                {
                    printf("define dso_local void @%s(",instruction->user.use_list->Val->name);
                    // fprintf(fptr,"define dso_local void @%s(",instruction->user.use_list->Val->name);
                } else
                {
                    printf("define dso_local i32 @%s(",instruction->user.use_list->Val->name);
                    // fprintf(fptr,"define dso_local i32 @%s(",instruction->user.use_list->Val->name);
                }
                p=instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;
                int ii=p;
                InstNode* node2=get_next_inst(instruction_node);
                while (p>0)
                {
                    if(p==instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num)
                    {
                        if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
                        {
                            while(node2->inst->user.use_list==NULL || (node2->inst->user.use_list->Val!=NULL && node2->inst->user.use_list->Val->VTy->ID!=AddressTyID))
                                node2=get_next_inst(node2);      //node2能指向真实value
                            if(node2->inst->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
                            {
                                // printf_array(node2->inst->user.use_list->Val,1,fptr);
                                printf("* %%0");
                                // fprintf(fptr,"* %%0");
                                node2= get_next_inst(node2);
                            }
                            else {
                                printf("i32* %%0");
                                // fprintf(fptr,"i32* %%0");
                            }
                        }
                        else
                        {
                            printf("i32 %%0");
                            // fprintf(fptr,"i32 %%0");
                        }
                    }
                    else
                    {
                        if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
                        {
                            while(node2->inst->user.use_list==NULL || (node2->inst->user.use_list->Val!=NULL && node2->inst->user.use_list->Val->VTy->ID!=AddressTyID))
                                node2=get_next_inst(node2);      //node2能指向真实value
                            if(node2->inst->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
                            {
                                printf(",");
                                // fprintf(fptr,",");
                                // printf_array(node2->inst->user.use_list->Val,1,fptr);
                                printf("* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                // fprintf(fptr,"* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                node2= get_next_inst(node2);
                            }
                            else
                            {
                                printf(",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                // fprintf(fptr,",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            }
                        }
                        else
                        {
                            printf(",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            // fprintf(fptr,",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                        }
                    }
                    p--;
                }
                printf(") #0{\n");
                // fprintf(fptr,") #0{\n");
                break;
            case Return:
                if(instruction->user.use_list==NULL)
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
                    // fprintf(fptr," ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
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
                get_param_list(instruction->user.use_list->Val,&give_count);
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        printf(" %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
                        // fprintf(fptr," %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                        //是库函数
                    else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
                    {
                        printf(" %s = call i32 (",instruction->user.value.name);
                        // fprintf(fptr," %s = call i32 (",instruction->user.value.name);
                    }
                    else
                    {
                        printf(" %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
                        // fprintf(fptr," %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                }
                else     //voidTypeID
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        printf(" call void @%s(",instruction->user.use_list->Val->name);
                        // fprintf(fptr," call void @%s(",instruction->user.use_list->Val->name);
                    }
                        //是库函数
                    else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
                    {
                        printf(" call void (");
                        // fprintf(fptr," call void (");
                    }
                    else
                    {
                        printf(" call void (...) @%s (",instruction->user.use_list->Val->name);
                        // fprintf(fptr," void i32 (...) @%s (",instruction->user.use_list->Val->name);
                    }
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
                                // fprintf(fptr,"i32*,");
                            }
                            else
                            {
                                // printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                printf("*,");
                                // fprintf(fptr,"*,");
                            }
                        }
                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                        {
                            printf("i32*,");
                            // fprintf(fptr,"i32*,");
                        }
                        else
                        {
                            printf("i32,");
                            // fprintf(fptr,"i32,");
                        }
                    }
                    printf("...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);
                    // fprintf(fptr,"...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);

                    for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
                    {
                        if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                        {
                            v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                            if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                            {
                                printf("i32*,");
                                // fprintf(fptr,"i32*,");
                            }
                            else
                            {
                                // printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                printf("*,");
                                // fprintf(fptr,"*,");
                            }
                        }
                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                        {
                            printf("i32*,");
                            // fprintf(fptr,"i32*,");
                        }
                        else
                        {
                            printf("i32,");
                            // fprintf(fptr,"i32,");
                        }
                    }
                    printf("...)*)(");
                    // fprintf(fptr,"...)*)(");
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
                                // fprintf(fptr,"i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==Float)
                            {
                                printf("i32 %f",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.fVal);
                                // fprintf(fptr,"i32 %f",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.fVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                            {
                                v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                                if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                                {
                                    printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                    // fprintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                                else
                                {
                                    // printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                    printf("* %s",one_param[i]->inst->user.use_list->Val->name);
                                    // fprintf(fptr,"* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                            {
                                printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                // fprintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                            else
                            {
                                printf("i32 %s",one_param[i]->inst->user.use_list->Val->name);
                                // fprintf(fptr,"i32 %s",one_param[i]->inst->user.use_list->Val->name);
                            }

                        }
                        else
                        {
                            if(one_param[i]->inst->user.use_list->Val->VTy->ID==Int)
                            {
                                printf(",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                                // fprintf(fptr,",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==Float)
                            {
                                printf(",i32 %f",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.fVal);
                                // fprintf(fptr,",i32 %f",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.fVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                            {
                                v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                                if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                                {
                                    printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                    // fprintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                                else
                                {
                                    printf(",");
                                    // fprintf(fptr,",");
                                    // printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                    printf("* ");
                                    // fprintf(fptr,"* ");
                                    printf("%s",one_param[i]->inst->user.use_list->Val->name);
                                    // fprintf(fptr,"%s",one_param[i]->inst->user.use_list->Val->name);
                                }
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                            {
                                printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                // fprintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                            else
                            {
                                printf(",i32 %s",one_param[i]->inst->user.use_list->Val->name);
                                // fprintf(fptr,",i32 %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                        }
                    }
                }

                printf(")\n");
                // fprintf(fptr,")\n");
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
                        // fprintf(fptr," %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        // fprintf(fptr," %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        // fprintf(fptr," %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        // fprintf(fptr," %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
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
                if(instruction->user.use_list->Val->VTy->ID==Int||instruction->user.use_list->Val->VTy->ID==Float)
                    printf("give param %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
                else
                    printf("give param %s\n",instruction->user.use_list->Val->name);
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
            case fptosi:
                if(instruction->user.use_list->Val->VTy->ID == Var_FLOAT)
                {
                    printf(" %s = fptosi float %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    // fprintf(fptr," %s = fptosi float %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                }else{
                    printf(" %s = fptosi float %f to i32\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal);
                    // fprintf(fptr," %s = fptosi float %f to i32\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal);
                }
                break;
            case sitofp:
                if(instruction->user.use_list->Val->VTy->ID == Var_INT)
                {
                    printf(" %s = sitofp i32 %s to float\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    // fprintf(fptr," %s = sitofp i32 %s to float\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                }else{
                    printf("%s = sitofp i32 %d to float\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal);
                    // fprintf(fptr," %s = sitofp i32 %d to float\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal);
                }
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
        for(int i=0;i<3;i++)
        {
            if(instruction->_reg_[i]!=0)    printf("reg%d:r%d",i,instruction->_reg_[i]);
            printf("\t\t");
        }
        printf("\n");
//        printf("Var_Float ans:\t");
        for(int i=0;i<3;i++)
        {
            if(instruction->_vfpReg_[i]!=0)    printf("VFPreg%d:s%d",i,instruction->_vfpReg_[i]);
            printf("\t\t");
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

void gcp_init(PriorityQueue* pqueue)
{
    var_num=0;
    edge_num=0;
    HashSet *tmp;
    tmp=HashSetInit();
    void *elem;
    value_live_range *cur;
    while (PriorityQueueSize(pqueue)!=0){
        PriorityQueueTop(pqueue,&elem);
        PriorityQueuePop(pqueue);
//        printf("%d\n",PriorityQueueSize(pqueue));
        cur=(value_live_range*)elem;
        var_num++;
        // printf("%s start %d    end %d\n",cur->value->name,cur->start,cur->end);
        HashSetAdd(tmp,cur);
    }
    HashSetFirst(tmp);
    while ((elem= HashSetNext(tmp))!=NULL){
        PriorityQueuePush(pqueue,elem);
    }
    live_global=(struct name_num *)malloc(sizeof(struct name_num)*(var_num+10));
    var_num=0;
    while (PriorityQueueSize(pqueue)!=0){
        PriorityQueueTop(pqueue,&elem);
        PriorityQueuePop(pqueue);
//        printf("%d\n",PriorityQueueSize(pqueue));
        cur=(value_live_range*)elem;
        live_global[var_num].name=cur->value->name;
        live_global[var_num].first=cur->start;
        live_global[var_num].last=cur->end;
        var_num++;
        HashSetAdd(tmp,cur);
    }
    HashSetFirst(tmp);
    while ((elem= HashSetNext(tmp))!=NULL){
        PriorityQueuePush(pqueue,elem);
    }
    _bian=(struct reg_edge *)malloc(sizeof(struct reg_edge)*((var_num)*(var_num+10)/2));
    for(int i=0;i<var_num;i++)
    {
        for(int j=i+1;j<var_num;j++)
        {
            // printf("now %s %s\n",live[i].name,live[j].name);
            // if((live[i].last<live[j].first||live[i].first>live[j].last)||
            //    (live[i].first==live[j].last&&live[i].first_is_use==0&&live[j].last_is_use==1)||
            //    (live[j].first==live[i].last&&live[j].first_is_use==0&&live[i].last_is_use==1))
            if(live_global[i].last<live_global[j].first||live_global[i].first>live_global[j].last)
            {
                // printf("%s %s没有在一起哦\n",live[i].name,live[j].name);
                // if(live[i].last<live[j].first||live[i].first>live[j].last)  printf("from1\n");
                // if(live[i].first==live[j].last&&live[i].first_is_use==0&&live[j].last_is_use==1)  printf("from2\n");
                // if(live[j].first==live[i].last&&live[j].first_is_use==0&&live[i].last_is_use==1)  printf("from3\n");
            }
            else
            {
                // printf("%s %s在一起了\n",live[i].name,live[j].name);
                create_bian(i,j);
            }
        }
    }
}

void gcp_allocate(InstNode* ins,Function* start)
{
    Function *curFunction=start;
    for(;curFunction!=NULL;curFunction=curFunction->Next){
//        初始化函数的活跃变量表
        curFunction->live_interval=PriorityQueueInit();
        curFunction->live_interval->set_compare(curFunction->live_interval,CompareNumerics);
        //assert(curFunction->live_interval!=NULL);
//        初始化函数的寄存器分配结果表
        curFunction->lineScanReg=HashMapInit();
        //assert(curFunction->lineScanReg!=NULL);

        if(enable_vfp==1){
            curFunction->vfp_live_interval=PriorityQueueInit();
            curFunction->vfp_live_interval->set_compare(curFunction->vfp_live_interval,CompareNumerics);
            curFunction->lineScanVFPReg=HashMapInit();
        }
//        获取函数的活跃变量表
        get_function_live_interval(curFunction);
//        active <-- {}
        active=PriorityQueueInit();
        active->set_compare(active,CompareNumerics2);
        location=HashSetInit();
        if(enable_vfp==1){
            VFPactive=PriorityQueueInit();
            VFPactive->set_compare(VFPactive,CompareNumerics2);
            VFPlocation=HashSetInit();
        }

        memset(myreg,0,sizeof(myreg));
        free_reg_num=6;
        memset(VFPreg,0, sizeof(VFPreg));
        free_VFPreg_num=14;
//      到这里为止

        // line_scan(curFunction,curFunction->live_interval);
        g_alloca(curFunction,curFunction->live_interval);
        if(enable_vfp==1){
            VFP_line_scan_alloca(curFunction,curFunction->vfp_live_interval);
        }

        label_the_result_of_linescan_register(curFunction,curFunction->entry->head_node);



//      打印寄存器分配的结果
        printf("in register:\n");
        Pair *ptr_pair;
        HashMapFirst(curFunction->lineScanReg);
        while ((ptr_pair= HashMapNext(curFunction->lineScanReg))!=NULL){
            Value *value=(Value*)ptr_pair->key;
            value_register *node=(value_register*)ptr_pair->value;
            printf("%s --> r%d\n",value->name,node->reg);
        }
        printf("in memory:\n");
        void *elem;
        value_live_range *tmp;
        HashSetFirst(location);
        while ((elem= HashSetNext(location))!=NULL){
            tmp=(value_live_range*)elem;
            printf("%s statr %d,end %d\n",tmp->value->name,tmp->start,tmp->end);
        }
        if(enable_vfp==1){
            printf("Var_Float in register:\n");
            HashMapFirst(curFunction->lineScanVFPReg);
            while ((ptr_pair= HashMapNext(curFunction->lineScanVFPReg))!=NULL){
                Value *value=(Value*)ptr_pair->key;
                value_register *node=(value_register*)ptr_pair->value;
                printf("%s --> s%d\n",value->name,node->sreg);
            }
            printf("Var_Float in memory:\n");
            HashSetFirst(VFPlocation);
            while ((elem= HashSetNext(VFPlocation))!=NULL){
                tmp=(value_live_range*)elem;
                printf("%s statr %d,end %d\n",tmp->value->name,tmp->start,tmp->end);
            }
        }



//        在这里可以把active,location,VFPactive和VFPlocation释放掉
        PriorityQueueDeinit(active);
        HashSetDeinit(location);
        if(enable_vfp==1){
            PriorityQueueDeinit(VFPactive);
            HashSetDeinit(VFPlocation);
        }
    }
}


void g_alloca(Function *curFunction,PriorityQueue*queue){
    void *elem;
    value_live_range *i;
    printf("PriorityQueueSize(queue) =%d\n",PriorityQueueSize(queue));


    printf("11\n");
    gcp_init(queue);
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
    printf("22\n");
    // test_ans();
    // print_colors();
    color_removed();

    while (PriorityQueueSize(queue)!=0){ //foreach live interval i

        PriorityQueueTop(queue,&elem);
        PriorityQueuePop(queue);

        i=(value_live_range*)elem;
        expire_old_intervals(curFunction,i);
        // if(PriorityQueueSize(active)==R){
        //     spill_at_interval(curFunction,i);
        // }else{
        //     value_register *r=(value_register*) malloc(sizeof(value_register));
        //     r->reg=get_an_availabel_register();
        //     //assert(r->reg!=-1);
        //     HashMapPut(curFunction->lineScanReg,i->value,r);
        //     PriorityQueuePush(active,i);
        // }
        int gcp_color=list_of_variables[find_gcp_live(i->value->name)].color;
        if(gcp_color >= 0)
        {
            value_register *r=(value_register*) malloc(sizeof(value_register));
            r->reg=gcp_color+3;
            //assert(r->reg!=-1);
            HashMapPut(curFunction->lineScanReg,i->value,r);
//            printf("put curFunction->lineScanReg %s\n",i->value->name);
//            printf("%s\n",i->value->name);
            PriorityQueuePush(active,i);
        }
        else
        {
            spill_at_interval(curFunction,i);
        }
    }

//     while (PriorityQueueSize(queue)!=0){ //foreach live interval i

//         PriorityQueueTop(queue,&elem);
//         PriorityQueuePop(queue);

//         i=(value_live_range*)elem;
// //        printf("analyze %s\n",i->value->name);
//         expire_old_intervals(curFunction,i);
//         if(PriorityQueueSize(active)==R){
//             spill_at_interval(curFunction,i);
//         }else{
//             value_register *r=(value_register*) malloc(sizeof(value_register));
//             r->reg=get_an_availabel_register();
//             //assert(r->reg!=-1);
//             HashMapPut(curFunction->lineScanReg,i->value,r);
// //            printf("put curFunction->lineScanReg %s\n",i->value->name);
// //            printf("%s\n",i->value->name);
//             PriorityQueuePush(active,i);
//         }
//     }

}

int find_gcp_live(char *name)
{
    if(name)
    {
        for(int i=0;i<var_num;i++)
        {
            if(strcmp(name,live_global[i].name)==0)
                return i;
        }
    }
    return -1;
}

void travel_ir(InstNode *instruction_node)
{
    bool flag_func=false;
    int temp_cnt=0;
    tac_cnt=0;
    int give_param_ir[1000];
    int give_param_ir_num;
    for(int i=0;i<1000;i++) give_param_ir[i]=0;
    give_param_ir_num=0;
    // InstNode * temp_instruction_node= get_next_inst(instruction_node);

    InstNode * temp_instruction_node= instruction_node;
    while (temp_instruction_node!=NULL && temp_instruction_node->inst->Opcode!=ALLBEGIN && temp_instruction_node->inst->Opcode!=br
           && temp_instruction_node->inst->Opcode!=br_i1
           && temp_instruction_node->inst->Opcode!=br_i1_false && temp_instruction_node->inst->Opcode!=br_i1_true
           && temp_instruction_node->inst->Opcode!=FunEnd && temp_instruction_node->inst->Opcode!=Return)
        // while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN && instruction_node->inst->Opcode!=br
        // && instruction_node->inst->Opcode!=br_i1_false && instruction_node->inst->Opcode!=br_i1_true
        // && instruction_node->inst->Opcode!=FunEnd)
    {
        // Instruction *instruction=temp_instruction_node->inst;
        temp_cnt++;
        temp_instruction_node= get_next_inst(temp_instruction_node);
    }
    // printf("\n\ntemp_cnt:%d\n\n",temp_cnt);
    echo_tac = (struct reg_now *)malloc(sizeof(struct  reg_now)*(temp_cnt+30));
    instruction_node= get_next_inst(instruction_node);
    Value *v_cur_array=NULL;
    for(int i=0;i<temp_cnt+30;i++)
    {
        echo_tac[i].dest_name=NULL;
        echo_tac[i].left_name=NULL;
        echo_tac[i].right_name=NULL;
        echo_tac[i].dest_use=-1;
        echo_tac[i].left_use=-1;
        echo_tac[i].right_use=-1;
        echo_tac[i].irnode=NULL;
        echo_tac[i].give_param=0;
        echo_tac[i].reg_3[0]=0;
        echo_tac[i].reg_3[1]=0;
        echo_tac[i].reg_3[2]=0;
    }
    int p=0;
    int if_br_ir=0;
    // InstNode* params[1000];
    // InstNode * one_param[1000];
    int give_count=0;
    // for(int i=0;i<1000;i++)
    //     params[i]=NULL;

    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN
           && instruction_node->inst->Opcode!=FunEnd)
    {
        // printf("opcode:%d\t%d\t%d\n", instruction_node->inst->Opcode,__LINE__,instruction_node->inst->i);
        Instruction *instruction=instruction_node->inst;
        echo_tac[tac_cnt].node_id=instruction->i;
        echo_tac[tac_cnt].irnode=instruction;
        // printf("%d\n",instruction_node->inst->i);
        switch (instruction_node->inst->Opcode)
        {
            // printf("cnt:%d\n",tac_cnt);
            case Alloca:
                if(instruction->user.use_list->Val!=NULL && (instruction->user.use_list->Val->VTy->ID==ArrayTy_INT || instruction->user.use_list->Val->VTy->ID==ArrayTy_FLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayInt || instruction->user.use_list->Val->VTy->ID==GlobalArrayFloat || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstINT || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstINT))
                {
//                    echo_tac[tac_cnt].dest_name=instruction->user.value.name;
//                    echo_tac[tac_cnt].dest_use=0;
                }
                else if(instruction->user.use_list->Val!=NULL && instruction->user.use_list->Val->VTy->ID==AddressTyID)
                {
//                    echo_tac[tac_cnt].dest_name=instruction->user.value.name;
//                    echo_tac[tac_cnt].dest_use=0;
                }
                else
                {
//                    echo_tac[tac_cnt].dest_name=instruction->user.value.name;
//                    echo_tac[tac_cnt].dest_use=0;
                }
                break;
            case Load:
                if(instruction->user.use_list->Val->VTy->ID==AddressTyID)
                {
                    echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                    echo_tac[tac_cnt].dest_use=0;
                    echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].left_use=1;
                } else
                {
                    echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                    echo_tac[tac_cnt].dest_use=0;
                    echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].left_use=1;
                }
                break;
            case Store:
                if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->VTy->ID!=AddressTyID)
                {
                    echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                    echo_tac[tac_cnt].right_use=1;
                }
                else if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->VTy->ID==AddressTyID)
                {
                    echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                    echo_tac[tac_cnt].right_use=1;
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float || instruction->user.use_list->Val->VTy->ID==Const_FLOAT)
                {
                    echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                    echo_tac[tac_cnt].right_use=1;
                }
                else if(instruction->user.use_list[1].Val->VTy->ID!=AddressTyID)
                {
                    echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].left_use=1;
                    echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                    echo_tac[tac_cnt].right_use=1;
                }
                else
                {
                    echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].left_use=1;
                    echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                    echo_tac[tac_cnt].right_use=1;
                }
                break;
            case br:
            {
                if_br_ir = 1;
                echo_tac[tac_cnt].jumpto[0]=instruction->user.value.pdata->instruction_pdata.true_goto_location;
                break;
            }
            case br_i1:
            {
                echo_tac[tac_cnt].dest_name=instruction->user.use_list->Val->name;
                echo_tac[tac_cnt].dest_use=1;
                echo_tac[tac_cnt].jumpto[0]=instruction->user.value.pdata->instruction_pdata.true_goto_location;
                echo_tac[tac_cnt].jumpto[1]=instruction->user.value.pdata->instruction_pdata.false_goto_location;
                if_br_ir = 1;
                break;
                // printf(" br i1 %s,label %%%d,label %%%d\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
            }
            case br_i1_false:
            {
                echo_tac[tac_cnt].jumpto[0]=instruction->user.value.pdata->instruction_pdata.true_goto_location;
                if_br_ir = 1;
                break;
            }
            case br_i1_true:
            {
                echo_tac[tac_cnt].jumpto[0]=instruction->user.value.pdata->instruction_pdata.true_goto_location;
                if_br_ir = 1;
                break;
            }
            case EQ:
                if(instruction->user.use_list->Val->VTy->ID==Int||instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        if(echo_tac[tac_cnt].right_name) echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }

                break;
            case LESS:
                if(instruction->user.use_list->Val->VTy->ID==Int||instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        if(echo_tac[tac_cnt].right_name) echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case NOTEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int||instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        if(echo_tac[tac_cnt].right_name) echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case GREAT:
                if(instruction->user.use_list->Val->VTy->ID==Int||instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        if(echo_tac[tac_cnt].right_name) echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case GREATEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int||instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        if(echo_tac[tac_cnt].right_name) echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case LESSEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int||instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int||instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        if(echo_tac[tac_cnt].right_name) echo_tac[tac_cnt].right_use=1;
                        // printf(" %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case FunBegin:
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID==VoidTyID)
                {
                    // printf("define dso_local void @%s(",instruction->user.use_list->Val->name);
                    // fprintf(fptr,"define dso_local void @%s(",instruction->user.use_list->Val->name);
                } else
                {
                    // printf("define dso_local i32 @%s(",instruction->user.use_list->Val->name);
                    // fprintf(fptr,"define dso_local i32 @%s(",instruction->user.use_list->Val->name);
                }
                p=instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;
                int ii=p;
                InstNode* node2=get_next_inst(instruction_node);
                while (p>0)
                {
                    if(p==instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num)
                    {
                        if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
                        {
                            while(node2->inst->user.use_list==NULL || (node2->inst->user.use_list->Val!=NULL && node2->inst->user.use_list->Val->VTy->ID!=AddressTyID))
                                node2=get_next_inst(node2);      //node2能指向真实value
                            if(node2->inst->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
                            {
                                // printf_array(node2->inst->user.use_list->Val,1,fptr);
                                // printf("* %%0");
                                // fprintf(fptr,"* %%0");
                                node2= get_next_inst(node2);
                            }
                            else {
                                // printf("i32* %%0");
                                // fprintf(fptr,"i32* %%0");
                            }
                        }
                        else
                        {
                            // printf("i32 %%0");
                            // fprintf(fptr,"i32 %%0");
                        }
                    }
                    else
                    {
                        if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
                        {
                            while(node2->inst->user.use_list==NULL || (node2->inst->user.use_list->Val!=NULL && node2->inst->user.use_list->Val->VTy->ID!=AddressTyID))
                                node2=get_next_inst(node2);      //node2能指向真实value
                            if(node2->inst->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
                            {
                                // printf(",");
                                // fprintf(fptr,",");
                                // printf_array(node2->inst->user.use_list->Val,1,fptr);
                                // printf("* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                // fprintf(fptr,"* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                node2= get_next_inst(node2);
                            }
                            else
                            {
                                // printf(",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                // fprintf(fptr,",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            }
                        }
                        else
                        {
                            // printf(",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            // fprintf(fptr,",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                        }
                    }
                    p--;
                }
                // printf(") #0{\n");
                // fprintf(fptr,") #0{\n");
                break;
            case Return:
                if(instruction->user.use_list==NULL)
                {
                    // printf(" ret void\n");
                    //fpintf(fptr," ret void\n");
                }
                else if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    // printf(" ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
                    //fpintf(fptr," ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    // printf(" ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
                    //fpintf(fptr," ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
                }
                else
                {
                    echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].left_use=1;
                    if_br_ir = 1;
                    break;
                    // printf(" ret i32 %s\n",instruction->user.use_list->Val->name);
                    //fpintf(fptr," ret i32 %s\n",instruction->user.use_list->Val->name);
                }

//                printf("}\n\n");
//                //fpintf(fptr,"}\n\n");
                break;
            case Call:
                for(int i=0;i<give_param_ir_num;i++)
                {
                    echo_tac[give_param_ir[i]].give_param=tac_cnt;
                    // give_param_ir[i]=0;
                }
                echo_tac_temp=(struct  reg_now *)malloc(sizeof(struct  reg_now));
                for(int i=0;i<give_param_ir_num/2;i++)
                {   
                    echo_tac_temp[0]=echo_tac[give_param_ir[i]];
                    echo_tac[give_param_ir[i]]=echo_tac[give_param_ir[give_param_ir_num -i-1]];
                    echo_tac[give_param_ir[give_param_ir_num -i-1]]=echo_tac_temp[0];
                }
                for(int i=0;i<give_param_ir_num;i++)
                {
                    // echo_tac[give_param_ir[i]].give_param=tac_cnt;
                    give_param_ir[i]=0;
                }
                give_param_ir_num=0;
                get_param_list(instruction->user.use_list->Val,&give_count);
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
                        // fprintf(fptr," %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                        //是库函数
                    else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = call i32 (",instruction->user.value.name);
                        // fprintf(fptr," %s = call i32 (",instruction->user.value.name);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
                        // fprintf(fptr," %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                }
                else     //voidTypeID
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        // printf(" call void @%s(",instruction->user.use_list->Val->name);
                        // fprintf(fptr," call void @%s(",instruction->user.use_list->Val->name);
                    }
                        //是库函数
                    else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
                    {
                        // printf(" call void (");
                        // fprintf(fptr," call void (");
                    }
                    else
                    {
                        // printf(" call void (...) @%s (",instruction->user.use_list->Val->name);
                        // fprintf(fptr," void i32 (...) @%s (",instruction->user.use_list->Val->name);
                    }
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
                                // printf("i32*,");
                                // fprintf(fptr,"i32*,");
                            }
                            else
                            {
                                // printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                // printf("*,");
                                // fprintf(fptr,"*,");
                            }
                        }
                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                        {
                            // printf("i32*,");
                            // fprintf(fptr,"i32*,");
                        }
                        else
                        {
                            // printf("i32,");
                            // fprintf(fptr,"i32,");
                        }
                    }
                    // printf("...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);
                    // fprintf(fptr,"...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);

                    for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
                    {
                        if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                        {
                            v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                            if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                            {
                                // printf("i32*,");
                                // fprintf(fptr,"i32*,");
                            }
                            else
                            {
                                // printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                // printf("*,");
                                // fprintf(fptr,"*,");
                            }
                        }
                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                        {
                            // printf("i32*,");
                            // fprintf(fptr,"i32*,");
                        }
                        else
                        {
                            // printf("i32,");
                            // fprintf(fptr,"i32,");
                        }
                    }
                    // printf("...)*)(");
                    // fprintf(fptr,"...)*)(");
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
                                // printf("i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                                // fprintf(fptr,"i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                            {
                                v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                                if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                                {
                                    // printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                    // fprintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                                else
                                {
                                    // printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                    // printf("* %s",one_param[i]->inst->user.use_list->Val->name);
                                    // fprintf(fptr,"* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                            {
                                // printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                // fprintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                            else
                            {
                                // printf("i32 %s",one_param[i]->inst->user.use_list->Val->name);
                                // fprintf(fptr,"i32 %s",one_param[i]->inst->user.use_list->Val->name);
                            }

                        }
                        else
                        {
                            if(one_param[i]->inst->user.use_list->Val->VTy->ID==Int)
                            {
                                // printf(",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                                // fprintf(fptr,",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                            {
                                v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                                if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                                {
                                    // printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                    // fprintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                                else
                                {
                                    // printf(",");
                                    // fprintf(fptr,",");
                                    // printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                    // printf("* ");
                                    // fprintf(fptr,"* ");
                                    // printf("%s",one_param[i]->inst->user.use_list->Val->name);
                                    // fprintf(fptr,"%s",one_param[i]->inst->user.use_list->Val->name);
                                }
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                            {
                                // printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                // fprintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                            else
                            {
                                // printf(",i32 %s",one_param[i]->inst->user.use_list->Val->name);
                                // fprintf(fptr,",i32 %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                        }
                    }
                }

                // printf(")\n");
                // fprintf(fptr,")\n");
                break;
            case Label:
                // printf("%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                //fpintf(fptr,"%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                echo_tac[tac_cnt].label=instruction->user.value.pdata->instruction_pdata.true_goto_location;
                break;
            case Add:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Sub:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= sub nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sub nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= sub nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sub nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= sub nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sub nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= sub nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sub nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= sub nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sub nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= sub nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sub nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Mul:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= mul nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= mul nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= mul nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= mul nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= mul nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= mul nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= mul nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= mul nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= mul nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= mul nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= mul nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= mul nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Div:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= sdiv i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sdiv i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= sdiv i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sdiv i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {

                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= sdiv i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sdiv i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= sdiv i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sdiv i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= sdiv i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sdiv i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= sdiv i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        //fpintf(fptr," %s= sdiv i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        if(echo_tac[tac_cnt].right_name)    echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Mod:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf(" %s= srem i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= srem i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= srem i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= srem i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf(" %s= srem i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr," %s= srem i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                        echo_tac[tac_cnt].dest_use=0;
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf(" %s= srem i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        //fpintf(fptr," %s= srem i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case bitcast:
                //第一条bitcast
                if(instruction->user.value.pdata->var_pdata.iVal==1)
                {
                    // echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                    // echo_tac[tac_cnt].dest_use=0;
                    // echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    // echo_tac[tac_cnt].left_use=1;
                    //v_cur_array=instruction->user.use_list->Val->alias;
                    // printf(" %s=bitcast ",instruction->user.value.name);
                    //fpintf(fptr," %s=bitcast ",instruction->user.value.name);
                    //printf_array(instruction->user.use_list->Val->alias,0,fptr);
                    // printf("* %s to i8*\n",instruction->user.use_list->Val->name);
                    //fpintf(fptr,"* %s to i8*\n",instruction->user.use_list->Val->name);
                }
                else
                {
                    // echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                    // echo_tac[tac_cnt].dest_use=0;
                    // echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    // echo_tac[tac_cnt].left_use=1;
                    // printf(" %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
                    //fpintf(fptr," %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
                    //printf_array(instruction->user.use_list->Val->alias,0,fptr);
                    // printf("*\n");
                    //fpintf(fptr,"*\n");
                }
                break;

            case GEP:
                if(instruction->user.value.alias!=NULL)
                    v_cur_array=instruction->user.value.alias;
                echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                echo_tac[tac_cnt].dest_use=0;
                //printf("%d...\n",instruction->user.value.pdata->var_pdata.iVal);
                // printf(" %s=getelementptr inbounds ",instruction->user.value.name);
                //fpintf(fptr," %s=getelementptr inbounds ",instruction->user.value.name);
                if(instruction->user.use_list->Val->VTy->ID!=AddressTyID )
                {
                    //是对数组参数的最后一个自造的gmp
                    if(instruction->user.value.VTy->ID==AddressTyID)
                    {
                        //printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal-1,fptr);
                        // printf(",");
                        //fpintf(fptr,",");
                        //printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal-1,fptr);
                        // printf("* ");
                        //fpintf(fptr,"* ");
                        if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        {
                            // echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                            // echo_tac[tac_cnt].left_use=1;
                            // printf("%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                            //fpintf(fptr,"%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        }
                        else
                        {
                            // echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                            // echo_tac[tac_cnt].left_use=1;
                            echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                            echo_tac[tac_cnt].right_use=1;
                            // printf("%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                            //fpintf(fptr,"%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        }
                    }
                        //正常的
                    else{
                        //printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
                        // printf(",");
                        //fpintf(fptr,",");
                        //printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
                        // printf("* ");
                        //fpintf(fptr,"* ");
                        if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        {
                            // echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                            // echo_tac[tac_cnt].left_use=1;
                            // printf("%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                            //fpintf(fptr,"%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        }
                        else
                        {
                            // echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                            // echo_tac[tac_cnt].left_use=1;
                            echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                            echo_tac[tac_cnt].right_use=1;
                            // printf("%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                            //fpintf(fptr,"%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        }
                    }

                }
                else {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        // printf("i32,i32* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        //fpintf(fptr,"i32,i32* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].left_use=1;
                        echo_tac[tac_cnt].right_name=instruction->user.use_list[1].Val->name;
                        echo_tac[tac_cnt].right_use=1;
                        // printf("i32,i32* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
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
                // printf("\n");
                //fpintf(fptr,"\n");
                //}
                break;
            case MEMSET:
                flag_func=true;
                // printf(" call void @llvm.memset.p0i8.i64(i8* align 16 %s, i8 0, i64 %d, i1 false)\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                //fpintf(fptr," call void @llvm.memset.p0i8.i64(i8* align 16 %s, i8 0, i64 %d, i1 false)\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                break;
            case MEMCPY:
                flag_func=true;
                // printf(" call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %s, i8* align 16 bitcast (",instruction->user.use_list->Val->name);
                //fpintf(fptr," call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %s, i8* align 16 bitcast (",instruction->user.use_list->Val->name);
                //printf_array(instruction->user.use_list[1].Val,0,fptr);
                // printf("* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
                get_array_total_occupy(instruction->user.use_list[1].Val,0);
                //fpintf(fptr,"* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
                // get_array_total_occupy(instruction->user.use_list[1].Val,0));
                break;
            case GLOBAL_VAR:
                if(instruction->user.use_list->Val->VTy->ID!=ArrayTy_INT && instruction->user.use_list->Val->VTy->ID!=ArrayTy_FLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayInt && instruction->user.use_list->Val->VTy->ID!=GlobalArrayFloat && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_ConstINT && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_ConstFLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayConstFLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayConstINT)
                {
                    echo_tac[tac_cnt].dest_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].dest_use=0;
                    // printf("%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    //fpintf(fptr,"%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                }
                else
                {
                    v_cur_array=instruction->user.use_list->Val;
                    if(instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstINT || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstINT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstFLOAT)
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf("%s= internal constant ",instruction->user.use_list->Val->name);
                        //fpintf(fptr,"%s= internal constant ",instruction->user.use_list->Val->name);
                        ////printf_global_array(instruction->user.use_list->Val,fptr);
                        // printf("align 4\n");
                        //fpintf(fptr,"align 4\n");
                    }
                    else
                    {
                        echo_tac[tac_cnt].dest_name=instruction->user.use_list->Val->name;
                        echo_tac[tac_cnt].dest_use=0;
                        // printf("%s=dso_local global ",instruction->user.use_list->Val->name);
                        //fpintf(fptr,"%s=dso_local global ",instruction->user.use_list->Val->name);
                        ////printf_global_array(instruction->user.use_list->Val,fptr);
                        if(instruction->user.use_list->Val->pdata->symtab_array_pdata.is_init==0)
                        {
                            // printf(" zeroinitializer, align 4\n");
                            //fpintf(fptr," zeroinitializer, align 4\n");
                        }
                        else
                        {
                            // printf("align 4\n");
                            //fpintf(fptr,"align 4\n");
                        }
                    }
                }
                break;
            case GIVE_PARAM:
                params[give_count++]=instruction_node;
                // echo_tac[tac_cnt].give_param=1;
                give_param_ir[give_param_ir_num++]=tac_cnt;

                if(instruction->user.use_list->Val->VTy->ID==Int||instruction->user.use_list->Val->VTy->ID==Float) ;
                    // printf("give param %d,func:%s\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                else
                {
                    echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].left_use=1;
                    // printf("give param %s,func:%s\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                break;
            case FunEnd:
                // printf("}\n\n");
                //fpintf(fptr,"}\n\n");
                break;
            case zext:
                echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                echo_tac[tac_cnt].dest_use=0;
                echo_tac[tac_cnt].left_name=instruction->user.value.name;
                echo_tac[tac_cnt].left_use=1;
                // printf(" %s = zext i1 %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                //fpintf(fptr," %s = zext i1 %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                break;
            case XOR:
                echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                echo_tac[tac_cnt].dest_use=0;
                echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                echo_tac[tac_cnt].left_use=1;
                // printf(" %s = xor i1 %s, true\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                //fpintf(fptr," %s = xor i1 %s, true\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                break;
            case fptosi:
                if(instruction->user.use_list->Val->VTy->ID == Var_FLOAT)
                {
                    echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                    echo_tac[tac_cnt].dest_use=0;
                    echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].left_use=1;
                    // printf(" %s = fptosi float %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    // fprintf(fptr," %s = fptosi float %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                }else{
                    // echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                    // echo_tac[tac_cnt].dest_use=0;
                    // printf(" %s = fptosi float %f to i32\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal);
                    // fprintf(fptr," %s = fptosi float %f to i32\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal);
                }
                break;
            case sitofp:
                if(instruction->user.use_list->Val->VTy->ID == Var_INT)
                {
                    echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                    echo_tac[tac_cnt].dest_use=0;
                    echo_tac[tac_cnt].left_name=instruction->user.use_list->Val->name;
                    echo_tac[tac_cnt].left_use=1;
                    // printf(" %s = sitofp i32 %s to float\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    // fprintf(fptr," %s = sitofp i32 %s to float\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                }else{
                    // echo_tac[tac_cnt].dest_name=instruction->user.value.name;
                    // echo_tac[tac_cnt].dest_use=0;
                    // printf("%s = sitofp i32 %d to float\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal);
                    // fprintf(fptr," %s = sitofp i32 %d to float\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal);
                }
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
                            // printf("[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
                            //fpintf(fptr,"[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
                        }else if(incomingVal != NULL){
                            // printf("[%s , %%%d]",incomingVal->name,from->id);
                            //fpintf(fptr,"[%s , %%%d]",incomingVal->name,from->id);
                        }else{
                            //是NULL的话就
                            // printf("[ undef, %%%d] ",from->id);
                            //fpintf(fptr,"[ undef, %%%d] ",from->id);
                        }
                    }
                    else
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                            // printf("[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
                            //fpintf(fptr,"[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
                        }else if(incomingVal != NULL){
                            // printf("[%s , %%%d], ",incomingVal->name,from->id);
                            //fpintf(fptr,"[%s , %%%d], ",incomingVal->name,from->id);
                        }else{
                            // printf("[ undef, %%%d], ",from->id);
                            //fpintf(fptr,"[ undef, %%%d], ",from->id);
                        }
                    }
                    i++;
                }
                // printf("\n");
                //printf("a phi instruction\n");
                break;
            }
            case CopyOperation:{
                Value *dest = instruction->user.value.alias;
                Value *src = ins_get_lhs(instruction);
                if(isImm(src)){
                    echo_tac[tac_cnt].dest_name=dest->name;
                    echo_tac[tac_cnt].dest_use=0;
                    // printf(" %s(%s) = %d\n",dest->name,dest->alias->name,src->pdata->var_pdata.iVal);
                }else{
                    echo_tac[tac_cnt].dest_name=dest->name;
                    echo_tac[tac_cnt].dest_use=0;
                    echo_tac[tac_cnt].left_name=src->name;
                    echo_tac[tac_cnt].left_use=1;
                    // printf(" %s(%s) = %s\n", dest->name,dest->alias->name,src->name);
                }

                //printf("a copy operation\n");
                break;
            }
            default:
                break;
        }
        tac_cnt++;
        if(if_br_ir)    break;
        instruction_node= get_next_inst(instruction_node);

    }

    return ;
}

void addtolive(char * name,int tacid,int ifuse,int paramid)
{
    if(ifuse)
    {
        for(int i=0;i<var_num;i++)
            if(strcmp(name,live[i].name)==0)
            {
                if(paramid==0)
                {
                    live[i].last=tacid;
                    live[i].last_is_use=1;
                    return ;
                }
                else
                {
                    live[i].last=paramid;
                    live[i].last_is_use=1;
                    return ;
                }
            }
        if(paramid==0)
        {
            live[var_num].name=name;
            live[var_num].last=tacid;
            live[var_num].first=tacid;
            live[var_num].first_is_use=1;
            live[var_num].last_is_use=1;
            live[var_num++].first_use=tacid;
        }
        else
        {
            live[var_num].name=name;
            live[var_num].last=paramid;
            live[var_num].first=tacid;
            live[var_num].first_is_use=1;
            live[var_num].last_is_use=1;
            live[var_num++].first_use=tacid;
        }
        return ;
    }
    else
    {
        for(int i=0;i<var_num;i++)
            if(strcmp(name,live[i].name)==0)
            {
                live[i].last_def=tacid;
                live[i].last_is_use=0;
                live[i].last=tacid;
                return ;
            }
        live[var_num].last=tacid;
        live[var_num].name=name;
        live[var_num].first=tacid;
        live[var_num].first_is_use=0;
        live[var_num].last_is_use=0;
        live[var_num++].last_def=tacid;
        return ;
    }
}

void create_bian(int i,int j)
{
    if(i==j)    return ;
    for(int tie=0;tie<edge_num;tie++)
    {
        if((_bian[tie].a==i&&_bian[tie].b==j)||(_bian[tie].a==j&&_bian[tie].b==i))
        {
            return ;
        }
    }
    // printf("%d %d %s %s在一起了\n",i,j,live_global[i].name,live_global[j].name);
    _bian[edge_num].a=i;
    _bian[edge_num++].b=j;
    // printf("边:%s--%s\n",live[i].name,live[j].name);
    return ;
}

void addtoin(BasicBlock *this_block)
{
    block_in_num=0;
    BasicBlock *reg_curblock=this_block;
    live_in_name=(struct SString *)malloc(sizeof(struct SString)*(var_num+1000));
    HashSetFirst(reg_curblock->in);
    for(Value *liveInVariable = HashSetNext(reg_curblock->in); liveInVariable != NULL; liveInVariable = HashSetNext(reg_curblock->in)){
        //assert(liveInVariable->name != NULL);
        if(liveInVariable->name != NULL){
            // live_in_name[block_in_num].name =(char *)malloc(sizeof(liveInVariable->name)+3);
            live_in_name[block_in_num].name =(char *)malloc(1000);
            strcpy(live_in_name[block_in_num++].name,liveInVariable->name);
        }
    }
    // printf("reg_param_num:%d\n",reg_param_num);
    // for(int i=0;i<reg_param_num;i++)
    // {
    //     live_in_name[block_in_num].name =(char *)malloc(1000);
    //     sprintf(live_in_name[block_in_num++].name,"%%%d",i);
    // }
    // printf("in:\n",reg_curblock->id,block_in_num);
    // for(int i=0;i<block_in_num;i++)
    //     printf("%s\n",live_in_name[i].name);
    return ;
}

void addtoout(BasicBlock *this_block)
{
    block_out_num=0;
    live_out_name=(struct SString *)malloc(sizeof(struct SString)*(var_num+1000));
    HashSetFirst(this_block->out);
    for(Value *liveOutVariable = HashSetNext(this_block->out); liveOutVariable != NULL; liveOutVariable = HashSetNext(this_block->out)){
        //assert(liveOutVariable->name != NULL);
        if(liveOutVariable->name != NULL){
            // live_out_name[block_out_num].name =(char *)malloc(sizeof(liveOutVariable->name)+3);
            live_out_name[block_out_num].name =(char *)malloc(1000);
            strcpy(live_out_name[block_out_num++].name,liveOutVariable->name);
        }
    }
    // printf("out:\n");
    // for(int i=0;i<block_out_num;i++)
    //     printf("%s\n",live_out_name[i]);
    return ;
}

void bian_init(BasicBlock * this_block)
{
    var_num=0;
    edge_num=0;
    block_in_num=0;
    block_out_num=0;
    // for(int i=0;i<tac_cnt;i++)  printf("%d:%s\t%d\t%s\t%d\t%s\t%d\n",i,echo_tac[i].dest_name,echo_tac[i].dest_use,echo_tac[i].left_name,echo_tac[i].left_use,echo_tac[i].right_name,echo_tac[i].right_use);
    // for(int i=0;i<var_num;i++)  printf("var_id:%d:\t%s\t%d\t%d\n",i,live[i].name,live[i].first_use,live[i].last_def);
    for(int i=0;i<tac_cnt;i++)
    {
        if(echo_tac[i].dest_use>=0)
        {
            addtolive(echo_tac[i].dest_name,i,echo_tac[i].dest_use,echo_tac[i].give_param);
        }
        if(echo_tac[i].left_use>=0)
        {
            addtolive(echo_tac[i].left_name,i,echo_tac[i].left_use,echo_tac[i].give_param);
        }
        if(echo_tac[i].right_use>=0)
        {
            addtolive(echo_tac[i].right_name,i,echo_tac[i].right_use,echo_tac[i].give_param);
        }
    }
    // printf("tacid:%d\n",this_block->id);
    // for(int i=0;i<var_num;i++)  printf("var_id:%d:\t%s\t%d\t%d\n",i,live[i].name,live[i].first,live[i].last);
    if(var_num>1000)
    {
        mem_temp=1;
    }
    if(mem_temp)    return ;
    addtoin(this_block);
    addtoout(this_block);
    for(int i=0;i<var_num;i++)
    {
        for(int j=0;j<block_in_num;j++)
        {
            if(strcmp(live_in_name[j].name,live[i].name)==0)
            {
                live[i].isin=1;
                break;
            }
        }
        for(int j=0;j<block_out_num;j++)
        {
            if(strcmp(live_out_name[j].name,live[i].name)==0)
            {
                live[i].isout=1;
                break;
            }
        }
    }
    _bian=(struct reg_edge *)malloc(sizeof(struct reg_edge)*((var_num)*(var_num)/2));
    for(int i=0;i<var_num;i++)
    {
        for(int j=i+1;j<var_num;j++)
        {
            // printf("now %s %s\n",live[i].name,live[j].name);
            if((live[i].last<live[j].first||live[i].first>live[j].last)||
               (live[i].first==live[j].last&&live[i].first_is_use==0&&live[j].last_is_use==1)||
               (live[j].first==live[i].last&&live[j].first_is_use==0&&live[i].last_is_use==1))
                // if(live[i].last<live[j].first||live[i].first>live[j].last)
            {
                // printf("%s %s没有在一起哦\n",live[i].name,live[j].name);
                // if(live[i].last<live[j].first||live[i].first>live[j].last)  printf("from1\n");
                // if(live[i].first==live[j].last&&live[i].first_is_use==0&&live[j].last_is_use==1)  printf("from2\n");
                // if(live[j].first==live[i].last&&live[j].first_is_use==0&&live[i].last_is_use==1)  printf("from3\n");
            }
            else
            {
                // printf("%s %s在一起了\n",live[i].name,live[j].name);
                create_bian(i,j);
            }
        }
    }
    // for(int i=0;i<tac_cnt;i++)  printf("%d:%s\t%d\t%s\t%d\t%s\t%d\n",i,echo_tac[i].dest_name,echo_tac[i].dest_use,echo_tac[i].left_name,echo_tac[i].left_use,echo_tac[i].right_name,echo_tac[i].right_use);
    // for(int i=0;i<var_num;i++)  printf("var_id:%d:\t%s\t%d\t%d\n",i,live[i].name,live[i].first_use,live[i].last_def);
    // for(int i=0;i<edge_num;i++) printf("%d %d:%s %d:%s\n",i,_bian[i].a,live[_bian[i].a].name,_bian[i].b,live[_bian[i].b].name);
}

void addtogloballive()
{
    int tie_flag;
    for(int i=0;i<var_num;i++)
    {
        tie_flag=1;
        for(int j=0;j<var_num_global;j++)
        {
            if(strcmp(live[i].name,live_global[j].name)==0)
            {
                if(live_global[j].first==-1)
                {
                    live[i].ys=j;
                    live_global[j].first=live[i].first+tac_cnt_gloabl-tac_cnt;
                    live_global[j].last=live[i].last+tac_cnt_gloabl-tac_cnt;
                    live_global[j].iffuc=live[i].iffuc;
                }
                else
                {
                    live[i].ys=j;
                    live_global[j].last=live[i].last+tac_cnt_gloabl-tac_cnt;
                    tie_flag=0;
                }
                break;
            }
        }
        if(tie_flag)
        {
            live[i].ys=var_num_global;
            live_global[var_num_global].first=live[i].first+tac_cnt_gloabl-tac_cnt;
            live_global[var_num_global].last=live[i].last+tac_cnt_gloabl-tac_cnt;
            live_global[var_num_global].iffuc=live[i].iffuc;
            live_global[var_num_global++].name=live[i].name;
        }
    }
    // for(int i=0;i<var_num_global;i++) printf("var id:%d\tname:%s\n",i,live_global[i].name);
    return ;
}

void addinout()
{
    int tie_flag;
    for(int i=0;i<block_in_num;i++)
    {
        tie_flag=1;
        for(int j=0;j<var_num_global;j++)
        {
            if(strcmp(live_in_name[i].name,live_global[j].name)==0)
            {
                tie_flag=0;
                break;
            }
        }
        if(tie_flag)
        {
            live_global[var_num_global++].name=live_in_name[i].name;
        }
    }
    for(int i=0;i<block_out_num;i++)
    {
        tie_flag=1;
        for(int j=0;j<var_num_global;j++)
        {
            if(strcmp(live_out_name[i].name,live_global[j].name)==0)
            {
                tie_flag=0;
                break;
            }
        }
        if(tie_flag)
        {
            live_global[var_num_global++].name=live_out_name[i].name;
        }
    }
    return ;
}

void bian_init_test(BasicBlock * this_block)
{
    var_num=0;
    for(int i=0;i<tac_cnt;i++)
    {
        if(echo_tac[i].dest_use>=0)
        {
            addtolive(echo_tac[i].dest_name,i,echo_tac[i].dest_use,echo_tac[i].give_param);
        }
        if(echo_tac[i].left_use>=0)
        {
            addtolive(echo_tac[i].left_name,i,echo_tac[i].left_use,echo_tac[i].give_param);
        }
        if(echo_tac[i].right_use>=0)
        {
            addtolive(echo_tac[i].right_name,i,echo_tac[i].right_use,echo_tac[i].give_param);
        }
    }
    for(int i=0;i<var_num;i++)
        if(is_func_param(live[i].name))
            live[i].iffuc=1;
    addtogloballive();
    if(var_num>800||var_num_global>3000)
    {
        mem_temp=1;
    }
    if(mem_temp)
        return ;
    // printf("tacid:%d\n",this_block->id);
    // for(int i=0;i<var_num;i++)  printf("var_id:%d:\t%s\t%d\t%d\n",i,live[i].name,live[i].first,live[i].last);
    // _bian=(struct reg_edge *)malloc(sizeof(struct reg_edge)*((var_num)*(var_num)/2));
    addtoout(this_block);
    addtoin(this_block);
    addinout();
    for(int i=0;i<var_num;i++)
    {
        for(int j=0;j<block_in_num;j++)
        {
            if(strcmp(live_in_name[j].name,live[i].name)==0)
            {
                live[i].isin=1;
                // live_global[live[i].ys].kua=1;
                break;
            }
        }
        for(int j=0;j<block_out_num;j++)
        {
            if(strcmp(live_out_name[j].name,live[i].name)==0)
            {
                live[i].isout=1;
                // live_global[live[i].ys].kua=1;
                break;
            }
        }
    }
    // _bian=(struct reg_edge *)malloc(sizeof(struct reg_edge)*((var_num)*(var_num)/2));
    for(int i=0;i<var_num;i++)
    {
        for(int j=i+1;j<var_num;j++)
        {
            if(live[i].iffuc==0&&live[j].iffuc==0)
            {
                // printf("now %s %s\n",live[i].name,live[j].name);
                if(((live[i].last<live[j].first||live[i].first>live[j].last)||
                    (live[i].first==live[j].last&&live[i].first_is_use==0&&live[j].last_is_use==1)||
                    (live[j].first==live[i].last&&live[j].first_is_use==0&&live[i].last_is_use==1))
                   &&live[i].isout==0&&live[j].isout==0&&live[i].isin==0&&live[i].isin==0)
                    // if(live[i].last<live[j].first||live[i].first>live[j].last)
                {
                    // printf("%s %s没有在一起哦\n",live[i].name,live[j].name);
                    // if(live[i].last<live[j].first||live[i].first>live[j].last)  printf("from1\n");
                    // if(live[i].first==live[j].last&&live[i].first_is_use==0&&live[j].last_is_use==1)  printf("from2\n");
                    // if(live[j].first==live[i].last&&live[j].first_is_use==0&&live[i].last_is_use==1)  printf("from3\n");
                }
                else
                {
                    // printf("%s %s在一起了\n",live_global[live[i].ys].name,live_global[live[j].ys].name);
                    create_bian(live[i].ys,live[j].ys);
                }
            }
        }
    }
    for(int i=0;i<var_num;i++)
    {
        for(int j=0;j<block_in_num;j++)
        {
            if(live[i].iffuc==0&&live_global[find_var_global(live_in_name[j].name)].iffuc==0)
                create_bian(live[i].ys,find_var_global(live_in_name[j].name));
        }
        for(int j=0;j<block_out_num;j++)
        {
            if(live[i].iffuc==0&&live_global[find_var_global(live_out_name[j].name)].iffuc==0)
                create_bian(live[i].ys,find_var_global(live_out_name[j].name));
        }
    }
    // for(int i=0;i<tac_cnt;i++)  printf("%d:%s\t%d\t%s\t%d\t%s\t%d\n",i,echo_tac[i].dest_name,echo_tac[i].dest_use,echo_tac[i].left_name,echo_tac[i].left_use,echo_tac[i].right_name,echo_tac[i].right_use);
    // for(int i=0;i<var_num;i++)  printf("var_id:%d:\t%s\t%d\t%d\n",i,live[i].name,live[i].first_use,live[i].last_def);
}

void reg_control(struct _InstNode *instruction_node,Function *start)
{
    printf("/*******************reg_start******************/\n");
    // while(temp->inst->Parent->Parent == NULL)   temp = get_next_inst(temp);]
//    ir_reg_init(instruction_node);
//    for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next)
//    {
//        // printf("now_func:%s\n",currentFunction->entry->head_node->inst->user.use_list->Val->name);
//        reg_control_func(currentFunction);
//    }
    printf_llvm_ir_withreg(instruction_node);
    printf("/*******************reg_end******************/\n");
    return ;
}


void printf_asm_test(char * filename_test)
{
    char new_ext[1000];
    strcpy(new_ext,filename_test);
    FILE *fp_test= fopen(new_ext,"w");
    fprintf(fp_test,"ljw\n");
    return ;
}

int is_func_param(char * str)
{
    if(str[0]!='%') return 0;
    int length=strlen(str);
    int strcnt=0;
    for(int i=1;i<length;i++)
    {
        strcnt*=10;
        strcnt+=str[i]-'0';
    }
    if(strcnt<reg_param_num)    return 1;
    return 0;
}

void jumpfromto()
{
    for(int i=0;i<tac_cnt_gloabl;i++)
    {
        if(echo_tac_global[i].jumpto[0]!=-1)
        {
            for(int j=0;j<tac_cnt_gloabl;j++)
            {
                if(echo_tac_global[j].label==echo_tac_global[i].jumpto[0])
                {
                    echo_tac_global[i].jumptoid[0]=j;
                    echo_tac_global[j].from[echo_tac_global[j].from_cnt]=i;
                    break;
                }
            }
        }
        if(echo_tac_global[i].jumpto[1]!=-1)
        {
            for(int j=0;j<tac_cnt_gloabl;j++)
            {
                if(echo_tac_global[j].label==echo_tac_global[i].jumpto[1])
                {
                    echo_tac_global[i].jumptoid[1]=j;
                    echo_tac_global[j].from[echo_tac_global[j].from_cnt]=i;
                    break;
                }
            }
        }
    }
    return ;
}


void fyreg(int i,int j)
{
    for(int x=0;x<13;x++)
    {
        if(inwhichreg(echo_tac_global[j].reg_3[0])!=x&&inwhichreg(echo_tac_global[j].reg_3[1])!=x&&inwhichreg(echo_tac_global[j].reg_3[2])!=x)
            echo_tac_global[j].whoinreg[x]=echo_tac_global[i].whoinreg[x];
    }
    return ;
}

void visittac(int tacid)
{
    while(tacid < tac_cnt_gloabl)
    {
        if(echo_tac_global[tacid].jumpto[0]!=-1)
        {
            if(echo_tac_global[echo_tac_global[tacid].jumpto[0]].from_cnt==1)
            {
                fyreg(tacid,echo_tac_global[tacid].jumpto[0]);
            }
        }
        if(echo_tac_global[tacid].jumpto[1]!=-1)
        {
            if(echo_tac_global[echo_tac_global[tacid].jumpto[1]].from_cnt==1)
            {
                fyreg(tacid,echo_tac_global[tacid].jumpto[1]);
            }
        }
        if(tacid!=0&&echo_tac_global[tacid].from_cnt==0)
            fyreg(tacid-1,tacid);
        tacid++;
    }
    // for(int i=0;i<tac_cnt_gloabl;i++)
    // {
    //     for(int j=4;j<11;j++)
    //         printf("r%d:%s ",j,find_live_name(echo_tac_global[i].whoinreg[j]));
    //     printf("\n");
    // }
}

void reg_control_func(Function *currentFunction)
{
#if reg_alloc_test
    BasicBlock *entry = currentFunction->entry;
        BasicBlock *end = currentFunction->tail;
        tac_cnt_gloabl=0;
        clear_visited_flag(entry);
        block_num=0;
        InstNode *currNode = entry->head_node;
        BasicBlock *currNodeParent = currNode->inst->Parent;
        block_list = (struct BLOCK_list *)malloc(sizeof(struct BLOCK_list)*40000);
        block_list[block_num++].reg_block=currNode->inst->Parent;
        block_list[0].reg_block->visited=1;
        while(currNode != get_next_inst(end->tail_node)){
            currNodeParent = currNode->inst->Parent;
            if(currNodeParent->visited == false){
                currNodeParent->visited = true;
                block_list[block_num++].reg_block=currNode->inst->Parent;
            }
            currNode = get_next_inst(currNode);
        }
        InstNode *headNode = entry->head_node;
        reg_param_num = headNode->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;
        printf("func block_num:%d\n\n",block_num);
        // for(int i=0;i<block_num;i++)    printf("start_id:%d\n",block_list[i].reg_block->id);
        clear_visited_flag(entry);
        init_global(block_list[0].reg_block);
        if(block_num>1000)  mem_temp=1;
        for(int i=0;i<block_num;i++)
        {
            // printf("block:%d\n",i);
            reg_control_block_temp(block_list[i].reg_block);
        }
        if(mem_temp)
        {
            printf("use mem\n");
        }
        else
        {
            jumpfromto();
            // for(int i=0;i<tac_cnt_gloabl;i++)
            //     printf("%d: %s %s %s\n",i,echo_tac_global[i].dest_name,echo_tac_global[i].left_name,echo_tac_global[i].right_name);
            // for(int i=0;i<var_num_global;i++)
            //     printf("%d:%s %d\n",i,live_global[i].name,live_global[i].first);
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
            // test_ans();
            // print_colors();
            color_removed();
        }
        add_to_echo();
        // if(mem_temp==0)
            // visittac(0);
        echo_to_ir();
        clean_reg_global();
        free(block_list);
        block_list=NULL;
#else
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    clear_visited_flag(entry);
    block_num=0;
    InstNode *currNode = entry->head_node;
    BasicBlock *currNodeParent = currNode->inst->Parent;
    block_list = (struct BLOCK_list *)malloc(sizeof(struct BLOCK_list)*40000);
    block_list[block_num++].reg_block=currNode->inst->Parent;
    block_list[0].reg_block->visited=1;
    while(currNode != get_next_inst(end->tail_node)){
        currNodeParent = currNode->inst->Parent;
        if(currNodeParent->visited == false){
            currNodeParent->visited = true;
            block_list[block_num++].reg_block=currNode->inst->Parent;
        }
        currNode = get_next_inst(currNode);
    }
    InstNode *headNode = entry->head_node;
    reg_param_num = headNode->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;
    // printf("func block_num:%d\n\n",block_num);
    // for(int i=0;i<block_num;i++)    printf("start_id:%d\n",block_list[i].reg_block->id);
    clear_visited_flag(entry);
    init_global(block_list[0].reg_block);
    for(int i=0;i<block_num;i++)
    {
        // printf("block:%d\n",i);
        reg_control_block(block_list[i].reg_block);
    }
    echo_to_ir();
    free(block_list);
    block_list=NULL;
#endif
    return ;
}

void reg_control_block(BasicBlock *cur)
{
#if all_in_memory

    travel_ir(cur->head_node);
        for(int j=0;j<tac_cnt;j++)
            reg_inmem_one_ins(j);
        return ;

#else
    mem_temp=0;
    // printf("this blcok start at %d\n",cur->head_node->inst->i);
    travel_ir(cur->head_node);
    live_init_block();
    bian_init(cur);
    if(mem_temp)
    {
        add_to_echo();
        add_to_global();
        clean_reg();
        return ;
    }
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
    // test_ans();
    // print_colors();
    color_removed();
    add_to_echo();
    add_to_global();
    // add_to_ir();
    clean_reg();
    return ;

#endif
}

void add_to_global()
{
    for(int i=0;i<tac_cnt;i++)
    {
        echo_tac_global[i+tac_cnt_gloabl]=echo_tac[i];
    }
    tac_cnt_gloabl+=tac_cnt;
    // for(int i=0;i<tac_cnt_gloabl;i++)   printf("%s\t%s\t%s\n",echo_tac_global[i].dest_name,echo_tac_global[i].left_name,echo_tac_global[i].right_name);
}

void reg_control_block_temp(BasicBlock *cur)
{
    // printf("this blcok start at %d\n",cur->head_node->inst->i);
    travel_ir(cur->head_node);
    add_to_global();
    live_init_global();
    bian_init_test(cur);
    // init_RIG();
    // create_RIG();
    // check_edge();
    // //print_info();
    // minimize_RIG();
    // init_non_available_colors();
    // while(first_fit_coloring())
    // {
    //     reset_colors();
    //     reset_queue();
    //     spill_variable();
    // }
    // // test_ans();
    // // print_colors();
    // color_removed();
    // add_to_ir();
    clean_reg();
    return ;

}

int inwhichreg(int regid)
{
    if(regid>100)
    {
        return regid-100;
    }
    else if(regid<0)
    {
        return -regid;
    }
    return regid;
}

char * find_live_name(int id)
{
    if(id>=0)
        return live_global[id].name;
    return NULL;
}

void add_to_echo()
{
    int var_uid;
    int reg_uid;
#if reg_alloc_test
    for(int i=0;i<tac_cnt_gloabl;i++)
    {
        if(mem_temp==0)
        {
            var_uid=find_var_global(echo_tac_global[i].dest_name);
            if(var_uid>=0 && echo_tac_global[i].dest_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(echo_tac_global[i].dest_use==0)
                {
                    if(reg_uid<0||live_global[var_uid].iffuc)
                    {
                        echo_tac_global[i].reg_3[0]=-4;
                    }
                    else
                    {
                        if(reg_uid==5)  reg_uid=12;
                        else    reg_uid+=6;
                        echo_tac_global[i].reg_3[0]=reg_uid;
                    }
                }
                else
                {
                    if(reg_uid<0||live_global[var_uid].iffuc)
                    {
                        echo_tac_global[i].reg_3[0]=104;
                    }
                    else
                    {
                        if(reg_uid==5)  reg_uid=12;
                        else    reg_uid+=6;
                        if(is_func_param(echo_tac_global[i].dest_name)&&live_global[var_uid].first==i)
                            echo_tac_global[i].reg_3[0]=reg_uid+100;
                        else
                            echo_tac_global[i].reg_3[0]=reg_uid;
                    }
                }
            }
            else
                echo_tac_global[i].reg_3[0]=0;
            if(echo_tac_global[i].reg_3[0]!=0)
                echo_tac_global[i].whoinreg[inwhichreg(echo_tac_global[i].reg_3[0])]=var_uid;


            var_uid=find_var_global(echo_tac_global[i].left_name);
            if(var_uid>=0 && echo_tac_global[i].left_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(echo_tac_global[i].left_use==0)
                {
                    if(reg_uid<0||live_global[var_uid].iffuc)
                    {
                        echo_tac_global[i].reg_3[1]=-4;
                    }
                    else
                    {
                        if(reg_uid==5)  reg_uid=12;
                        else    reg_uid+=6;
                        echo_tac_global[i].reg_3[1]=reg_uid;
                    }
                }
                else
                {
                    if(reg_uid<0||live_global[var_uid].iffuc)
                    {
                        echo_tac_global[i].reg_3[1]=104;
                    }
                    else
                    {
                        if(reg_uid==5)  reg_uid=12;
                        else    reg_uid+=6;
                        if(is_func_param(echo_tac_global[i].left_name)&&live_global[var_uid].first==i)
                            echo_tac_global[i].reg_3[1]=reg_uid+100;
                        else
                            echo_tac_global[i].reg_3[1]=reg_uid;
                    }
                }
            }
            else
                echo_tac_global[i].reg_3[1]=0;
            if(echo_tac_global[i].reg_3[1]!=0)
                echo_tac_global[i].whoinreg[inwhichreg(echo_tac_global[i].reg_3[1])]=var_uid;

            var_uid=find_var_global(echo_tac_global[i].right_name);
            if(var_uid>=0 && echo_tac_global[i].right_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(echo_tac_global[i].right_use==0)
                {
                    if(reg_uid<0||live_global[var_uid].iffuc)
                    {
                        echo_tac_global[i].reg_3[2]=-5;
                    }
                    else
                    {
                        if(reg_uid==5)  reg_uid=12;
                        else    reg_uid+=6;
                        echo_tac_global[i].reg_3[2]=reg_uid;
                    }
                }
                else
                {
                    if(reg_uid<0||live_global[var_uid].iffuc)
                    {
                        echo_tac_global[i].reg_3[2]=105;
                    }
                    else
                    {
                        if(reg_uid==5)  reg_uid=12;
                        else    reg_uid+=6;
                        if(is_func_param(echo_tac_global[i].right_name)&&live_global[var_uid].first==i)
                            echo_tac_global[i].reg_3[2]=reg_uid+100;
                        else
                            echo_tac_global[i].reg_3[2]=reg_uid;
                    }
                }
            }
            else
                echo_tac_global[i].reg_3[2]=0;
            if(echo_tac_global[i].reg_3[2]!=0)
                echo_tac_global[i].whoinreg[inwhichreg(echo_tac_global[i].reg_3[2])]=var_uid;
        }
        else
        {
            if(echo_tac_global[i].dest_use==0)
                echo_tac_global[i].reg_3[0]=-4;
            else if(echo_tac_global[i].dest_use==1)
                echo_tac_global[i].reg_3[0]=104;
            else
                echo_tac_global[i].reg_3[0]=-4;
            if(echo_tac_global[i].left_use==0)
                echo_tac_global[i].reg_3[1]=-5;
            else if(echo_tac_global[i].left_use==1)
                echo_tac_global[i].reg_3[1]=105;
            else
                echo_tac_global[i].reg_3[1]=0;
            if(echo_tac_global[i].right_use==0)
                echo_tac_global[i].reg_3[2]=-6;
            else if(echo_tac_global[i].right_use==1)
                echo_tac_global[i].reg_3[2]=106;
            else
                echo_tac_global[i].reg_3[2]=0;
        }
    }
#else
    for(int i=0;i<tac_cnt;i++)
    {
        if(mem_temp==0)
        {
            var_uid=find_var(echo_tac[i].dest_name);
            if(var_uid>=0 && echo_tac[i].dest_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(reg_uid<0)   echo_tac[i].reg_3[0]=-4;
                else
                {
                    if(reg_uid==5)  reg_uid=12;
                    else    reg_uid+=6;
                    if(echo_tac[i].dest_use==0)
                    {
                        if(i==live[var_uid].last_def&&live[var_uid].isout)
                            echo_tac[i].reg_3[0]=reg_uid*-1;
                        else
                            echo_tac[i].reg_3[0]=reg_uid;
                    }
                    else
                    {
                        if(i==live[var_uid].first_use&&live[var_uid].isin)
                            echo_tac[i].reg_3[0]=reg_uid+100;
                        else
                            echo_tac[i].reg_3[0]=reg_uid;
                    }
                }
            }
            else
                echo_tac[i].reg_3[0]=0;


            var_uid=find_var(echo_tac[i].left_name);
            if(var_uid>=0 && echo_tac[i].left_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(reg_uid<0)   echo_tac[i].reg_3[1]=104;
                else
                {
                    if(reg_uid==5)  reg_uid=12;
                    else    reg_uid+=6;

                    if(echo_tac[i].left_use==0)
                    {
                        if(i==live[var_uid].last_def&&live[var_uid].isout)
                            echo_tac[i].reg_3[1]=reg_uid*-1;
                        else
                            echo_tac[i].reg_3[1]=reg_uid;
                    }
                    else
                    {
                        if(i==live[var_uid].first_use&&live[var_uid].isin)
                            echo_tac[i].reg_3[1]=reg_uid+100;
                        else
                            echo_tac[i].reg_3[1]=reg_uid;
                    }

                }
            }
            else
                echo_tac[i].reg_3[1]=0;

            var_uid=find_var(echo_tac[i].right_name);
            if(var_uid>=0 && echo_tac[i].right_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(reg_uid<0)   echo_tac[i].reg_3[2]=105;
                else
                {
                    if(reg_uid==5)  reg_uid=12;
                    else    reg_uid+=6;
                    if(echo_tac[i].left_use==0)
                    {
                        if(i==live[var_uid].last_def&&live[var_uid].isout)
                            echo_tac[i].reg_3[2]=reg_uid*-1;
                        else
                            echo_tac[i].reg_3[2]=reg_uid;
                    }
                    else
                    {
                        if(i==live[var_uid].first_use&&live[var_uid].isin)
                            echo_tac[i].reg_3[2]=reg_uid+100;
                        else
                            echo_tac[i].reg_3[2]=reg_uid;
                    }
                }
            }
            else
                echo_tac[i].reg_3[2]=0;
        }
        else
        {
            if(echo_tac[i].dest_use==0)
                echo_tac[i].reg_3[0]=-4;
            else if(echo_tac[i].dest_use==1)
                echo_tac[i].reg_3[0]=104;
            else
                echo_tac[i].reg_3[0]=0;
            if(echo_tac[i].left_use==0)
                echo_tac[i].reg_3[1]=-5;
            else if(echo_tac[i].left_use==1)
                echo_tac[i].reg_3[1]=105;
            else
                echo_tac[i].reg_3[1]=0;
            if(echo_tac[i].right_use==0)
                echo_tac[i].reg_3[2]=-6;
            else if(echo_tac[i].right_use==1)
                echo_tac[i].reg_3[2]=106;
            else
                echo_tac[i].reg_3[2]=0;
        }
    }
#endif
}

void echo_to_ir()
{
    for(int i=0;i<tac_cnt_gloabl;i++)
    {
        for(int j=0;j<3;j++)
            echo_tac_global[i].irnode->_reg_[j]=echo_tac_global[i].reg_3[j];
    }
    return ;
}

void add_to_ir()
{
#if reg_alloc_test
    int var_uid;
    int reg_uid;
    for(int i=0;i<tac_cnt;i++)
    {
        var_uid=find_var(echo_tac[i].dest_name);
        if(var_uid>=0 && echo_tac[i].dest_use>=0)
        {
            reg_uid=list_of_variables[var_uid].color;
            if(reg_uid<0)   echo_tac[i].irnode->_reg_[0]=-4;
            else
            {
                if(reg_uid==5)  reg_uid=12;
                else    reg_uid+=6;
                if(echo_tac[i].dest_use==0)
                {
                        echo_tac[i].irnode->_reg_[0]=reg_uid;
                }
                else
                {
                    if(i==live[var_uid].first_use&&1==live[var_uid].first_is_use)
                        echo_tac[i].irnode->_reg_[0]=reg_uid+100;
                    else
                        echo_tac[i].irnode->_reg_[0]=reg_uid;
                }
            }
        }
        else
            echo_tac[i].irnode->_reg_[0]=0;


        var_uid=find_var(echo_tac[i].left_name);
        if(var_uid>=0 && echo_tac[i].left_use>=0)
        {
            reg_uid=list_of_variables[var_uid].color;
            if(reg_uid<0)   echo_tac[i].irnode->_reg_[1]=104;
            else
            {
                if(reg_uid==5)  reg_uid=12;
                else    reg_uid+=6;
                if(echo_tac[i].left_use==0)
                {
                    if(i==live[var_uid].last_def)
                        echo_tac[i].irnode->_reg_[1]=reg_uid*-1;
                    else
                        echo_tac[i].irnode->_reg_[1]=reg_uid;
                }
                else
                {
                    if(i==live[var_uid].first_use&&1==live[var_uid].first_is_use)
                        echo_tac[i].irnode->_reg_[1]=reg_uid+100;
                    else
                        echo_tac[i].irnode->_reg_[1]=reg_uid;
                }
            }
        }
        else
            echo_tac[i].irnode->_reg_[1]=0;

        var_uid=find_var(echo_tac[i].right_name);
        if(var_uid>=0 && echo_tac[i].right_use>=0)
        {
            reg_uid=list_of_variables[var_uid].color;
            if(reg_uid<0)   echo_tac[i].irnode->_reg_[2]=105;
            else
            {
                if(reg_uid==5)  reg_uid=12;
                else    reg_uid+=6;
                if(echo_tac[i].left_use==0)
                {
                    if(i==live[var_uid].last_def)
                        echo_tac[i].irnode->_reg_[2]=reg_uid*-1;
                    else
                        echo_tac[i].irnode->_reg_[2]=reg_uid;
                }
                else
                {
                    if(i==live[var_uid].first_use&&1==live[var_uid].first_is_use)
                        echo_tac[i].irnode->_reg_[2]=reg_uid+100;
                    else
                        echo_tac[i].irnode->_reg_[2]=reg_uid;
                }
            }
        }
        else
            echo_tac[i].irnode->_reg_[2]=0;
    }
    return ;
#else
    if(mem_temp)
    {
        int var_uid;
        int reg_uid;
        for(int i=0;i<tac_cnt;i++)
        {
            if(echo_tac[i].dest_use==0)
                echo_tac[i].irnode->_reg_[0]=-4;
            else if(echo_tac[i].dest_use==1)
                echo_tac[i].irnode->_reg_[0]=104;
            else
                echo_tac[i].irnode->_reg_[0]=0;
            if(echo_tac[i].left_use==0)
                echo_tac[i].irnode->_reg_[1]=-5;
            else if(echo_tac[i].left_use==1)
                echo_tac[i].irnode->_reg_[1]=105;
            else
                echo_tac[i].irnode->_reg_[1]=0;
            if(echo_tac[i].right_use==0)
                echo_tac[i].irnode->_reg_[2]=-6;
            else if(echo_tac[i].right_use==1)
                echo_tac[i].irnode->_reg_[2]=106;
            else
                echo_tac[i].irnode->_reg_[2]=0;
        }
    }
    else
    {
        int var_uid;
        int reg_uid;
        for(int i=0;i<tac_cnt;i++)
        {
            var_uid=find_var(echo_tac[i].dest_name);
            if(var_uid>=0 && echo_tac[i].dest_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(reg_uid<0)   echo_tac[i].irnode->_reg_[0]=-4;
                else
                {
                    if(reg_uid==5)  reg_uid=12;
                    else    reg_uid+=6;
                    if(echo_tac[i].dest_use==0)
                    {
                        if(i==live[var_uid].last_def&&live[var_uid].isout)
                            echo_tac[i].irnode->_reg_[0]=reg_uid*-1;
                        else
                            echo_tac[i].irnode->_reg_[0]=reg_uid;
                    }
                    else
                    {
                        if(i==live[var_uid].first_use&&live[var_uid].isin)
                            echo_tac[i].irnode->_reg_[0]=reg_uid+100;
                        else
                            echo_tac[i].irnode->_reg_[0]=reg_uid;
                    }
                }
            }
            else
                echo_tac[i].irnode->_reg_[0]=0;


            var_uid=find_var(echo_tac[i].left_name);
            if(var_uid>=0 && echo_tac[i].left_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(reg_uid<0)   echo_tac[i].irnode->_reg_[1]=104;
                else
                {
                    if(reg_uid==5)  reg_uid=12;
                    else    reg_uid+=6;

                    if(echo_tac[i].left_use==0)
                    {
                        if(i==live[var_uid].last_def&&live[var_uid].isout)
                            echo_tac[i].irnode->_reg_[1]=reg_uid*-1;
                        else
                            echo_tac[i].irnode->_reg_[1]=reg_uid;
                    }
                    else
                    {
                        if(i==live[var_uid].first_use&&live[var_uid].isin)
                            echo_tac[i].irnode->_reg_[1]=reg_uid+100;
                        else
                            echo_tac[i].irnode->_reg_[1]=reg_uid;
                    }

                }
            }
            else
                echo_tac[i].irnode->_reg_[1]=0;

            var_uid=find_var(echo_tac[i].right_name);
            if(var_uid>=0 && echo_tac[i].right_use>=0)
            {
                reg_uid=list_of_variables[var_uid].color;
                if(reg_uid<0)   echo_tac[i].irnode->_reg_[2]=105;
                else
                {
                    if(reg_uid==5)  reg_uid=12;
                    else    reg_uid+=6;
                    if(echo_tac[i].left_use==0)
                    {
                        if(i==live[var_uid].last_def&&live[var_uid].isout)
                            echo_tac[i].irnode->_reg_[2]=reg_uid*-1;
                        else
                            echo_tac[i].irnode->_reg_[2]=reg_uid;
                    }
                    else
                    {
                        if(i==live[var_uid].first_use&&live[var_uid].isin)
                            echo_tac[i].irnode->_reg_[2]=reg_uid+100;
                        else
                            echo_tac[i].irnode->_reg_[2]=reg_uid;
                    }
                }
            }
            else
                echo_tac[i].irnode->_reg_[2]=0;
        }
    }
    return ;
#endif
}


// void add_to_ir()
// {
//     int var_uid;
//     int reg_uid;
//     for(int i=0;i<tac_cnt;i++)
//     {
//         var_uid=find_var(echo_tac[i].dest_name);
//         if(var_uid>=0)
//         {
//             reg_uid=list_of_variables[var_uid].color;
//             if(reg_uid<0)   echo_tac[i].irnode->_reg_[0]=-4;
//             else
//             {
//                 reg_uid+=6;
//                 if(echo_tac[i].dest_use==0)
//                 {
//                     if(i==live[var_uid].last)
//                         echo_tac[i].irnode->_reg_[0]=reg_uid*-1;
//                     else
//                         echo_tac[i].irnode->_reg_[0]=reg_uid;
//                 }
//                 if(echo_tac[i].dest_use==1)
//                 {
//                     if(i==live[var_uid].first)
//                         echo_tac[i].irnode->_reg_[0]=reg_uid+100;
//                     else
//                         echo_tac[i].irnode->_reg_[0]=reg_uid;
//                 }
//             }
//         }


//         var_uid=find_var(echo_tac[i].left_name);
//         if(var_uid>=0)
//         {
//             reg_uid=list_of_variables[var_uid].color;
//             if(reg_uid<0)   echo_tac[i].irnode->_reg_[1]=104;
//             else
//             {
//                 reg_uid+=6;
//                 if(echo_tac[i].left_use==0)
//                 {
//                     if(i==live[var_uid].last)
//                         echo_tac[i].irnode->_reg_[1]=reg_uid-1;
//                     else
//                         echo_tac[i].irnode->_reg_[1]=reg_uid;
//                 }
//                 if(echo_tac[i].left_use==1)
//                 {
//                     if(i==live[var_uid].first)
//                         echo_tac[i].irnode->_reg_[1]=reg_uid+100;
//                     else
//                         echo_tac[i].irnode->_reg_[1]=reg_uid;
//                 }
//             }
//         }

//         var_uid=find_var(echo_tac[i].right_name);
//         if(var_uid>=0)
//         {
//             reg_uid=list_of_variables[var_uid].color;
//             if(reg_uid<0)   echo_tac[i].irnode->_reg_[2]=105;
//             else
//             {
//                 reg_uid+=6;
//                 if(echo_tac[i].right_use==0)
//                 {
//                     if(i==live[var_uid].last)
//                         echo_tac[i].irnode->_reg_[2]=reg_uid-1;
//                     else
//                         echo_tac[i].irnode->_reg_[2]=reg_uid;
//                 }
//                 if(echo_tac[i].right_use==1)
//                 {
//                     if(i==live[var_uid].first)
//                         echo_tac[i].irnode->_reg_[2]=reg_uid+100;
//                     else
//                         echo_tac[i].irnode->_reg_[2]=reg_uid;
//                 }
//             }
//         }
//     }
//     return ;
// }

void clean_reg()
{
    // for(int i=0;i<(tac_cnt+30)*3;i++)   free(live[i].name);
    // for(int tietie=0;tietie<block_in_num;tietie++)  free(live_in_name[tietie].name);
    // for(int tietie=0;tietie<block_out_num;tietie++)  free(live_out_name[tietie].name);
    // if(live) free(live);
    // if(_bian) free(_bian);
    // if(list_of_variables) free(list_of_variables);
    // if(RIG) free(RIG);
    // if(head) free(head);
    // if(echo_tac) free(echo_tac);
    // if(non_available_colors) free(non_available_colors);
    // if(live_in_name) free(live_in_name);
    // if(live_out_name) free(live_out_name);
    live_in_name=NULL;
    live_out_name=NULL;
    live=NULL;
#if reg_alloc_test

#else
    _bian=NULL;
#endif
    list_of_variables=NULL;
    RIG=NULL;
    head=NULL;
    non_available_colors=NULL;
    echo_tac=NULL;
    return ;
}

void clean_reg_global()
{
    _bian=NULL;
    list_of_variables=NULL;
    RIG=NULL;
    head=NULL;
    non_available_colors=NULL;
    echo_tac_global=NULL;
    live_global=NULL;
}

int check_edge()
{
    for(int x=0;x<edge_num;x++)
    {
        if(_bian[x].a==_bian[x].b)  printf("error::%d\n",x);
        for(int y=x+1;y<edge_num;y++)
        {
            if((_bian[x].a==_bian[y].a&&_bian[x].b==_bian[y].b)||(_bian[x].a==_bian[y].b&&_bian[x].b==_bian[y].a))
            {
                printf("error::%d %d\n",x,y);
            }
        }
    }
    return 0;
}

void reg_inmem_one_ins(int id)
{
    if(echo_tac[id].dest_use==0)
    {
        echo_tac[id].irnode->_reg_[0]=104;
    }
    else if(echo_tac[id].dest_use==1)
    {
        echo_tac[id].irnode->_reg_[0]=-4;
    }
    if(echo_tac[id].left_use==0)
    {
        echo_tac[id].irnode->_reg_[1]=105;
    }
    else if(echo_tac[id].left_use==1)
    {
        echo_tac[id].irnode->_reg_[1]=-5;
    }
    if(echo_tac[id].right_use==0)
    {
        echo_tac[id].irnode->_reg_[2]=106;
    }
    else if(echo_tac[id].right_use==1)
    {
        echo_tac[id].irnode->_reg_[2]=-6;
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

void reg__push(int variable_index)
{
    struct reg_queue *element = (struct reg_queue *)malloc(sizeof(struct reg_queue));
    element->variable_index = variable_index;
    element->next = NULL;

    if(head == NULL)
    {
        head = element;
    }
    else
    {
        struct reg_queue *temp = head;
        while(temp->next != NULL)
        {
            temp  = temp->next;
        }
        temp->next = element;
    }
    return ;
}

int reg__pop()
{
    struct reg_queue *temp = head;
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
    struct reg_queue *temp = head;
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
                printf("color removed failed");
                //需修改
            }
        }
    }
}

void print_non_available_colors()
{
    for(int i = 0; i < KK; i++)
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
#if reg_alloc_test
    rig_num=var_num_global;
#else
    rig_num=var_num;
#endif
    list_of_variables=NULL;
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
    // print_RIG();

    create_variable_list();
    // printf("edge num:%d\n",edge_num);
    for(int i=0;i<edge_num;i++)
    {
        create_edge(_bian[i].a, _bian[i].b);
        // printf("%d-%d\n",_bian[i].a,_bian[i].b);
    }
    // for(int i=0;i<rig_num;i++)  printf("%d  %s\t %d\n",i,live[i].name,list_of_variables[i].neighbor_count);
}


void create_edge(int first_node, int second_node)
{
    int i=first_node;
    int j=second_node;
    SETBIT(i,j);
    SETBIT(j,i);
    list_of_variables[i].neighbor_count++;
    list_of_variables[j].neighbor_count++;
    // printf("%s:%d\t%s:%d\n",live[i].name,list_of_variables[i].neighbor_count,live[j].name,list_of_variables[j].neighbor_count);
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

int find_var_global(char * str)
{
    if(str==NULL)   return -1;
    for(int i=0;i<var_num_global;i++)
    {
        if(supercmp(live_global[i].name,str)==0)
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
    // printf("rig num:%d\n",rig_num);
    for(i = 0; i < rig_num; i++)
    {
        // name = (char *)malloc(MAXSTRINGSIZE * sizeof(char));
        // sprintf(name, "%d", i);
        // list_of_variables[i].name = name;
#if reg_alloc_test
        list_of_variables[i].name = live_global[i].name;
#else
        list_of_variables[i].name = live_global[i].name;
#endif
        list_of_variables[i].color = NO_COLOR;
        list_of_variables[i].neighbor_count = 0;
        // printf("%d - %s\n",i,live[i].name);
    }
    return ;
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

void test_ans()
{
    int i, j;
    int i_color, j_color;
    for(i = 0; i < rig_num; i++)
    {
        i_color = list_of_variables[i].color;
        for(j = 0; j < rig_num; j++)
        {
            if(CHECKBIT(i,j))
            {
                j_color = list_of_variables[j].color;
                if(i_color == NO_COLOR || i_color == REMOVED)
                {
                    printf("分配有毛病\n");
                    return;
                }
                else if(i_color != SPILLED && i_color == j_color)
                {
                    printf("分配有毛病\n");
                    return;
                }
            }
        }
    }
    printf("ok的\n");
}