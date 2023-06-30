#include "travel.h"

extern Symtab *this;
extern struct _InstNode *instruction_list;

//记录临时变量组,形如%1,%2,%3...
//t_index转字符串用的sprintf
//目前设置的能容纳3位数的临时变量
extern char t[8];
extern int t_index;
extern int return_stmt_num[20];
extern int return_index;
extern insnode_stack S_continue;
extern insnode_stack S_break;
extern insnode_stack S_return;
extern insnode_stack S_and;
extern insnode_stack S_or;
//记录有没有需要continue和break的语句

extern char t_num[7];
int param_map=0;
Value *v_cur_func;
extern int flag_blocklist;        //默认都是非正常
char type_str[30][30]={{"unknown"},{"param_int"},{"param_float"},{"main_int"},{"main_float"},{"int"},{"float"},{"const_int"},{"const_float"},{"function"},{"void"},{"address"},{"var_int"},{"var_float"},{"globalint"},{"globalfloat"},{"array_const_int"},{"array_const_float"},{"global_array_const_int"},{"global_array_const_float"},{"array_int"},{"array_float"},{"global_arrayint"},{"global_arrayfloat"}};

InstNode * one_param[1000];   //存放单次正确位置的参数
InstNode* params[1000];      //存放所有参数

int while_scope=0;

void create_instruction_list(past root,Value* v_return,int block)
{
    if (root != NULL) {
        if ((strcmp(bstr2cstr(root->nodeType, '\0'), "VarDecl") == 0 || strcmp(bstr2cstr(root->nodeType, '\0'), "ConstDecl") == 0) && is_global_map(this)==true)
            create_var_decl(root,v_return,true,block);
        else if((strcmp(bstr2cstr(root->nodeType, '\0'), "VarDecl") == 0 || strcmp(bstr2cstr(root->nodeType, '\0'), "ConstDecl") == 0))
            create_var_decl(root,v_return,false,block);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "FuncDef") == 0)
            create_func_def(root);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "BlockItemList") == 0)
        {
            if(flag_blocklist==0)        //正常的
            {
                flag_blocklist=1;
                create_blockItemList(root,v_return,0,block);
            }
            else
                create_blockItemList(root,v_return,1,block);
        }
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "Assign_Stmt") == 0)
            create_assign_stmt(root,v_return,block);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "Return_Stmt") == 0)
            create_return_stmt(root,v_return,block);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "IF_Stmt") == 0)
            create_if_stmt(root,v_return,block);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "IfElse_Stmt") == 0)
            create_if_else_stmt(root,v_return,block);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "While_Stmt") == 0)
            create_while_stmt(root,v_return,block);
            //与不是stmt合并
            //else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Empty_Stmt") == 0)
            //  create_instruction_list(root->next,v_return);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Call_Func") == 0)
        {
            create_call_func(root,block);
            if (root->next != NULL)
                create_instruction_list(root->next,v_return,block);
        }

        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Continue_Stmt") == 0)
            create_continue_stmt(root,v_return,block);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Break_Stmt") == 0)
            create_break_stmt(root,v_return,block);
            //不是stmt
        else
            create_instruction_list(root->next,v_return,block);
    }
}

void create_continue_stmt(past root,Value* v_return,int block)
{
    //遇到continue,push就完事
    InstNode *node_continue = true_location_handler(br, NULL, 0);

    //读一下最新层的while值
    InstNode *ins_location=NULL;
    insnode_top(&S_continue,&ins_location);
    node_continue->inst->user.value.pdata->instruction_pdata.true_goto_location=ins_location->inst->user.value.pdata->instruction_pdata.true_goto_location;

    //block不正常，补编号
    if(block==1)
    {
        true_location_handler(Label,NULL,t_index);
        t_index++;
    }

    if (root->next != NULL)
        create_instruction_list(root->next,v_return,block);
}

void create_break_stmt(past root,Value* v_return,int block)
{
    //遇到break,push就完事
    InstNode *node_break = true_location_handler(br, NULL, 0);
    node_break->inst->user.value.pdata->map_list= getCurMapList(this);
    node_break->inst->user.value.pdata->instruction_pdata.false_goto_location=-while_scope;
    //是0就代表是break
    insnode_push(&S_break,node_break);

    //block不正常，补编号
    if(block==1)
    {
        true_location_handler(Label,NULL,t_index);
        t_index++;
    }

    if (root->next != NULL)
        create_instruction_list(root->next,v_return,block);
}

void reduce_return()
{
    //先取出最终要去向的label
    InstNode * go_location;
    insnode_pop(&S_return,&go_location);

    //再为return一个一个赋值
    while(!insnode_is_empty(S_return))
    {
        InstNode * return_point;
        insnode_pop(&S_return,&return_point);
        return_point->inst->user.value.pdata->instruction_pdata.true_goto_location=go_location->inst->user.value.pdata->instruction_pdata.true_goto_location;
    }
}

void reduce_break_solo(int location)
{
    InstNode *breaks=NULL;
    while(insnode_top(&S_break,&breaks))
    {
        //如果while的scope一样，则说明可以消除
        if(-breaks->inst->user.value.pdata->instruction_pdata.false_goto_location== while_scope+1)
        {
            insnode_pop(&S_break,&breaks);
            breaks->inst->user.value.pdata->instruction_pdata.true_goto_location=location;
        } else
            break;
    }
}

struct _Value *create_call_func(past root,int block)
{
    Instruction *instruction;
    Value *v=NULL;
    //从符号表中取出之前放入的函数信息
    v= symtab_lookup_withmap(this, bstr2cstr(root->left->sVal, '\0'),&this->value_maps->next->map);

    //如果是库函数，则v==NULL
    //将库函数信息存放在全局表的next表中
    //目前其实只存了参数个数
    if(v==NULL)
    {
        //可能已经存成库函数了
        v=symtab_lookup_withmap(this, bstr2cstr(root->left->sVal, '\0'),&this->value_maps->next->next->map);
        //第一次读到库函数，赋个type
        if(v==NULL)
        {
            v=(Value*) malloc(sizeof (Value));
            value_init(v);
            v->VTy->ID=FunctionTyID;

            int params_count=0;
            if(root->right!=NULL)
            {
                past param=root->right->left;
                while(param!=NULL)
                {
                    params_count++;
                    param=param->next;
                }
            }
            v->pdata->symtab_func_pdata.param_num=params_count;
            v->pdata->map_list=this->value_maps->next->next;

            v->name=(char*) malloc(sizeof(bstr2cstr(root->left->sVal,0)));
            strcpy(v->name,bstr2cstr(root->left->sVal,0));
            symtab_insert_withmap(this,&this->value_maps->next->next->map,v->name,v);

            //赋个type
            if(strcmp(v->name,"getint")==0 || strcmp(v->name,"getch")==0 || strcmp(v->name,"getarray")==0 || strcmp(v->name,"getfarray")==0)
                v->pdata->symtab_func_pdata.return_type.ID=Var_INT;
            else if(strcmp(v->name,"getfloat")==0)
                v->pdata->symtab_func_pdata.return_type.ID=Var_FLOAT;
            else
                v->pdata->symtab_func_pdata.return_type.ID=Unknown;
        }
    }

    //参数传递
    if(root->right!=NULL)
        create_params_stmt(root->right,v);

    instruction= ins_new_unary_operator(Call,v);

    //按照llvm，有无返回值都生成了这个value
    //如果无返回值就不生成
    Value *v_result=NULL;
    if(v->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
    {
        v_result= ins_get_value_with_name(instruction);
        v_result->VTy->ID=v->pdata->symtab_func_pdata.return_type.ID;
    }

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    return v_result;
}

void create_blockItemList(past root,Value* v_return,int flag_block,int block)
{
    scope_forward(this);
    create_instruction_list(root->left,v_return,flag_block);
    scope_back(this);
    if(flag_block==1 && root->next!=NULL)
        create_instruction_list(root->next,v_return,block);
}

//cal_expr()能使得等式右边永远只有一个值
void create_assign_stmt(past root,Value* v_return,int block) {
    if(is_global_map(this))
        return;

    Value *v=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
    {
        v = symtab_dynamic_lookup(this, bstr2cstr(root->left->sVal, '\0'))->alias;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "LValArray") == 0)
    {
        Value *v_array=symtab_dynamic_lookup(this,bstr2cstr(root->left->left->sVal,'\0'));
        if(v_array->VTy->ID==AddressTyID)
        {
            //先load出这个address
            Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
            InstNode *node = new_inst_node(ins_load);
            ins_node_add(instruction_list,node);
            Value *v_loadp= ins_get_value_with_name(ins_load);
            v_loadp->alias=v_array;
            v_loadp->VTy->ID=AddressTyID;
            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v= handle_assign_array(root->left->right->left,v_loadp,0,-1,0);
        }
        else
            v=handle_assign_array(root->left->right->left, v_array,0,-1,0);
    }

    //右值value
    Value *v1=(Value*) malloc(sizeof (Value));
    value_init(v1);
    v1->pdata->map_list= getCurMapList(this);

    //赋值右边为常数(整数),只有一句store
    if (strcmp(bstr2cstr(root->right->nodeType, '\0'), "num_int") == 0)
        value_init_int(v1,root->right->iVal);
        //赋值右边为浮点数，
    else if (strcmp(bstr2cstr(root->right->nodeType, '\0'), "num_float") == 0)
        value_init_float(v1,root->right->fVal);

        //赋值右边为表达式
    else if (strcmp(bstr2cstr(root->right->nodeType, '\0'), "expr") == 0)
    {
        //取出右值
        int convert=-1;
        v1 = cal_expr(root->right,v->VTy->ID,&convert);
    }

        //赋值右边为普通a,b,c,d
    else if (strcmp(bstr2cstr(root->right->nodeType, '\0'), "ID") == 0)
    {
        Value *v_const= symtab_dynamic_lookup(this,bstr2cstr(root->right->sVal,'\0'));
        if(v_const->VTy->ID==Const_INT || v_const->VTy->ID==Const_FLOAT)
        {
            if(v_const->VTy->ID==Const_INT)
                value_init_int(v1,v_const->pdata->var_pdata.iVal);
            else
                value_init_float(v1,v_const->pdata->var_pdata.fVal);
        }
        else
            //先load出右值,要返回创建出的value
            v1=create_load_stmt(bstr2cstr(root->right->sVal,'\0'));
    }
        //是数组
    else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "LValArray") == 0)
    {
        Value *array=symtab_dynamic_lookup(this,bstr2cstr(root->right->left->sVal,'\0'));
        if(array->VTy->ID==AddressTyID)
        {
            //先load出这个address
            Instruction *ins_load= ins_new_unary_operator(Load,array->alias);
            InstNode *node = new_inst_node(ins_load);
            ins_node_add(instruction_list,node);
            Value *v_loadp= ins_get_value_with_name(ins_load);
            v_loadp->alias=array;
            v_loadp->VTy->ID=AddressTyID;
            v_loadp->pdata->symtab_array_pdata.dimention_figure=array->pdata->symtab_array_pdata.dimention_figure;
            v1= handle_assign_array(root->right->right->left,v_loadp,1,-1,0);
        }
        else
            v1= handle_assign_array(root->right->right->left, array,1,-1,0);
    }
        //等于函数
    else
        v1= create_call_func(root->right,block);

    create_store_stmt(v1,v);
    if (root->next != NULL)
        create_instruction_list(root->next,v_return,block);
}


void create_return_stmt(past root,Value* v_return,int block) {
    Instruction *instruction;
    if (root->left != NULL)
    {
        Value *v=NULL;

        if (strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0) {
            //如果是const，直接换成const值
            Value *v_test_const= symtab_dynamic_lookup(this, bstr2cstr(root->left->sVal,'\0'));
            if(v_test_const->VTy->ID==Const_INT)
            {
                int num=v_test_const->pdata->var_pdata.iVal;
                v=(Value*) malloc(sizeof (Value));
                value_init_int(v,num);
            }
            else if(v_test_const->VTy->ID==Const_FLOAT)
            {
                float num=v_test_const->pdata->var_pdata.fVal;
                v=(Value*) malloc(sizeof (Value));
                value_init_float(v,num);
            }
            else
                v = create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
        }
            //返回表达式
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0){
            int convert=-1;
            v = cal_expr(root->left,v_cur_func->pdata->symtab_func_pdata.return_type.ID,&convert);
        }
            //整数
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0){
            v=(Value*) malloc(sizeof (Value));
            value_init_int(v,root->left->iVal);
        }
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0)
        {
            v=(Value*) malloc(sizeof (Value));
            value_init_float(v,root->left->fVal);
        }
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "LValArray") == 0){
            v = symtab_dynamic_lookup(this, bstr2cstr(root->left->left->sVal,'\0'));
            if(v->VTy->ID==AddressTyID)
                v=handle_assign_array(root->left->right->left,v->alias,1,-1,0);
            else
                v=handle_assign_array(root->left->right->left,v,1,-1,0);
        }
            //返回函数结果,Call_Func
        else
            v=create_call_func(root->left,block);

        //有多返回语句
        if(v_return!=NULL && v_return->VTy->ID!=MAIN_INT && v_return->VTy->ID!=MAIN_FLOAT)
        {
            //多返回值，就先进行一波store
            create_store_stmt(v,v_return);

            //TODO 跳转到最终结束语句
            //先生成一条，并加入return栈，和break的处理道理是一样的
            //生成一条指令，并将其压栈
            InstNode *return_node=true_location_handler(br, NULL, 0);
            insnode_push(&S_return,return_node);

            //如果基本块不正常，需要多一条跳转
            if(block==1)
            {
                InstNode *node_label=true_location_handler(Label, NULL, t_index);
                t_index++;
            }
        }
            //只有一条返回语句
        else
        {
            if(v!=NULL)
                instruction = ins_new_unary_operator(Return, v);
            else
                instruction = ins_new_zero_operator(Return);

            //将这个instruction加入总list
            InstNode *node = new_inst_node(instruction);
            ins_node_add(instruction_list,node);
        }
    }
        //ret void
    else
    {
        //有多返回语句
        if(v_return!=NULL && v_return->VTy->ID!=MAIN_INT && v_return->VTy->ID!=MAIN_FLOAT)
        {
            //先生成一条，并加入return栈，和break的处理道理是一样的
            //生成一条指令，并将其压栈
            InstNode *return_node=true_location_handler(br, NULL, 0);
            insnode_push(&S_return,return_node);
        }
        else
        {
            instruction = ins_new_zero_operator(Return);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(instruction);
            ins_node_add(instruction_list,node);
        }
    }
}

//只考虑了num_int
//返回NULL代表不是全0
past array_all_zeros(past init_val_list)
{
    past p=init_val_list;
    while(p!=NULL && (strcmp(bstr2cstr(p->nodeType, '\0'), "InitValList") == 0 || strcmp(bstr2cstr(p->nodeType, '\0'), "ConstExpList") == 0 || strcmp(bstr2cstr(p->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(p->nodeType, '\0'), "num_float") == 0))
    {
        //不是全0就照常处理
        if((strcmp(bstr2cstr(p->nodeType, '\0'), "num_int") == 0 && p->iVal!=0) || (strcmp(bstr2cstr(p->nodeType, '\0'), "num_float") == 0 && p->fVal!=0))
            return NULL;
        else if(strcmp(bstr2cstr(p->nodeType, '\0'), "InitValList") == 0 || strcmp(bstr2cstr(p->nodeType, '\0'), "ConstExpList") == 0)
            return array_all_zeros(p->left);
        //是0,且后面没东西了
        else if(p->next==NULL)
            return init_val_list;
        p=p->next;
    }
    return NULL;
}

//做倒数第i位的进位
//j代表进位的最低位是数组的倒数第几位
///与carry_save使用情景不同，所以有差别
///不允许暂时错误，必须精准，[5][2][2][2],在tmp_carry为[0][1][0][0]时，不能暂时[0][2][0][0],必须是[1][0][0][0]
int carry_last_i_save(Value* v_array,int carry[],int i)
{
    int carry_time=0;

    //将后面的清0
    for(int j=v_array->pdata->symtab_array_pdata.dimention_figure-i+1;j<v_array->pdata->symtab_array_pdata.dimention_figure;j++)
        carry[j]=0;

    //不用进位就可以直接加
    if(carry[v_array->pdata->symtab_array_pdata.dimention_figure-i] < v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-i]-1)
        carry[v_array->pdata->symtab_array_pdata.dimention_figure-i]++;
        //要进位
    else
    {
        while((carry_time==0 && carry[v_array->pdata->symtab_array_pdata.dimention_figure-i] >= v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-i]-1) || (carry_time>0 && carry[v_array->pdata->symtab_array_pdata.dimention_figure-i] >= v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-i]))
        {
            carry[v_array->pdata->symtab_array_pdata.dimention_figure-i]=0;
            carry[v_array->pdata->symtab_array_pdata.dimention_figure-i-1]++;
            carry_time++;
            i++;
        }
    }
    return v_array->pdata->symtab_array_pdata.dimention_figure-i;
}

//进位函数,比如是三维数组，返回进位的结束处的index是0,1,2
///允许加完之后暂时需要再进一次位才正确的情况，（在num_int后的InitValList和需进位的下一个num_int开始处理前都会先carry_save一次解决掉）
int carry_save(Value* v_array,int carry[])
{
    //不用进位就可以直接加
    //比如[3][1],初始为[0][0]，最内层可以放一个元素，此时0<1，再放一个元素不用进位，做完变为[0][1],像这种超出的情况，
    //再做一次，需要进位，变成[1][0],调用borrow_save，变成[0][1]

    int i=1;
    //不用进位就可以直接加
    if(carry[v_array->pdata->symtab_array_pdata.dimention_figure-1] < v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-1])
        carry[v_array->pdata->symtab_array_pdata.dimention_figure-1]++;
        //要进位
    else
    {
        while(carry[v_array->pdata->symtab_array_pdata.dimention_figure-i] >= v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-i])
        {
            carry[v_array->pdata->symtab_array_pdata.dimention_figure-i]=0;
            carry[v_array->pdata->symtab_array_pdata.dimention_figure-i-1]++;
            i++;
        }
    }
    return v_array->pdata->symtab_array_pdata.dimention_figure-i;
}


//借位-1,比如是三维数组，返回借位的结束处的index是0,1,2
void borrow_save(Value* v_array,int carry[])
{
    int i=1;

    if(carry[v_array->pdata->symtab_array_pdata.dimention_figure-1]!=0)
    {
        carry[v_array->pdata->symtab_array_pdata.dimention_figure-1]--;
        return;
    }

    //多了1,保证InitValList走到下一个num_int时一定需要进位，可从进位处carry_index继续gmp
    carry[v_array->pdata->symtab_array_pdata.dimention_figure-1]=v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-1];
    while(carry[v_array->pdata->symtab_array_pdata.dimention_figure-i-1]==0)
    {
        carry[v_array->pdata->symtab_array_pdata.dimention_figure-i-1]=v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-i-1]-1;
        i++;
    }
    carry[v_array->pdata->symtab_array_pdata.dimention_figure-i-1]--;
}

//给全局数组初值赋值
void assign_global_array(past p,Value* v_array,int i,int level)
{
    int move=v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-1-level];
    while(p!=NULL)
    {
        if(strcmp(bstr2cstr(p->nodeType, '\0'), "num_int") == 0)
        {
            v_array->pdata->symtab_array_pdata.array[i++]=p->iVal;
        }
        else if(strcmp(bstr2cstr(p->nodeType, '\0'), "num_float") == 0)
        {
            v_array->pdata->symtab_array_pdata.f_array[i++]=p->fVal;
        }
        else if(strcmp(bstr2cstr(p->nodeType, '\0'), "expr") == 0)
        {
            //TODO 处理不精细
            Value *v_num= cal_expr(p,Unknown,0);
            if(v_num->VTy->ID==Int)
                v_array->pdata->symtab_array_pdata.array[i++]=v_num->pdata->var_pdata.iVal;
            else
                v_array->pdata->symtab_array_pdata.f_array[i++]=v_num->pdata->var_pdata.fVal;
        }
            //是InitValList
        else
        {
            int p_i=i+move;
            assign_global_array(p->left,v_array,i,level+1);
            i=p_i;       //走过递归后的下一个起点
        }
        p=p->next;
    }
}

void handle_global_array(Value* v_array,bool is_global,past vars,int flag)
{
    //先全部默认为0
    int ele_num= get_array_total_occupy(v_array,0);
    ele_num/=4;
    if(flag==1)       //有初始化
    {
        for(int i=0;i<ele_num;i++)
        {
            v_array->pdata->symtab_array_pdata.array[i]=0;
        }
    }

    if(v_array->pdata->symtab_array_pdata.is_init==1)
    {
        //就是全局的
        if(is_global)
        {
            past p=vars->right->left;

           // if(p!=NULL)
                assign_global_array(p,v_array,0,0);
        }

            //不是全局的
            //目前只有一维数组
        else
        {
            //换名字
            v_array->name=(char*) malloc(9+sizeof(v_cur_func->name)+1+sizeof (bstr2cstr(vars->left->sVal, '\0')));
            strcpy(v_array->name,"@__const.");
            strcat(v_array->name,v_cur_func->name);
            strcat(v_array->name,".");
            strcat(v_array->name,bstr2cstr(vars->left->left->sVal,'\0'));
            if(v_array->pdata->symtab_array_pdata.dimention_figure==1)
            {
                int i=0;
                past p=vars->right->left;
                while(p!=NULL)
                {
                    if(strcmp(bstr2cstr(p->nodeType, '\0'), "num_int") == 0)
                        v_array->pdata->symtab_array_pdata.array[i++]=p->iVal;
                    else
                        v_array->pdata->symtab_array_pdata.f_array[i++]=p->fVal;
                    p=p->next;
                }
            }
        }
    }

    Instruction *instruction= ins_new_unary_operator(GLOBAL_VAR,v_array);

    //替换一下value
    Value *v_replace= ins_get_global_value(instruction,v_array->name);
    //全局
    v_array->alias=v_replace;
    v_replace->alias=v_array;
    //拷贝一下其他信息
    v_replace->VTy->ID=v_array->VTy->ID;
    v_replace->pdata=v_array->pdata;

    //将这个instruction加入总list
    InstNode *instNode = new_inst_node(instruction);
    ins_insert_after(instNode,instruction_list);
}

//init_val_list是最老的InitValList的第一个左值，是num_int或InitValList
//初始时的Value* begin_offset_value为第二个bitcast的左值
//cur_lpar为目前处理层次数，初始为0；每进一层递归，层次数+1
//start_layer是gmp开始的位置，可判断要进行几次gmp
//carry数组记录目前初始化到的位置
void handle_one_dimention(past init_val_list,Value *v_array,Value* begin_offset_value,int start_layer,int cur_layer,int carry[])
{
    //TODO 0的情况最终处理还需斟酌
    //元素是全0,返回下一个init_val_list
    //if(array_all_zeros(init_val_list)!=NULL)
    //  return;

    Value *v_offset;                          //存放偏移地址的Value
    Instruction *ins_gmp=NULL;

    bool first_init=true;                     //多个num_int连着时，只需走一次除了最终地址前的for循环；再遇到InitValList后又重置为false
    bool come_num=false;                       //记录处理元素的上一个处理元素是num_int还是InitValList，如果是num_int则走完了一部分，要先做一次进位
    bool have_num_before=false;                //记录该层是否遇见过num_int过，没有false,有true
    int carry_point;                     //记录前面有num_int时，下一次InitValList从数组的第几位开始get地址

    int tmp_carry[v_array->pdata->symtab_array_pdata.dimention_figure];         //有时会拷贝carry数组到tmp_carry作为进入handle递归前起点处的暂存，用于判断有前面有num_int时，InitValList结束后的下一个起始位置

    //record数组记录着0，1，2位的左值
    //比如三维数组，从第一维开始初始化(下标0)，使用begin_value；从第二维开始初始化(下标1)，则使用record[0]，只用走第三维，则使用record[1];record[2]闲置
    Value *record[v_array->pdata->symtab_array_pdata.dimention_figure];

    while(init_val_list!=NULL)
    {
        if((strcmp(bstr2cstr(init_val_list->nodeType, '\0'), "InitValList") == 0) || (strcmp(bstr2cstr(init_val_list->nodeType, '\0'), "ConstExpList") == 0))
        {
            //解决空InitValList的情况，即{},将起点记录进tmp_carry，用于地址处理
            if(init_val_list->left==NULL)
            {
                //将carry数组内容赋值进tmp_carry
                for(int i=0;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
                    tmp_carry[i]=carry[i];
            }
            else if(!have_num_before)
            {
                //将carry数组内容赋值进tmp_carry，保存递归前起点情况，是100型还是10型......
                for(int i=0;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
                    tmp_carry[i]=carry[i];
                handle_one_dimention(init_val_list->left,v_array,begin_offset_value,start_layer,cur_layer+1, carry);
            }
            else if(have_num_before)
            {
                //到InitValList，如果**上一条**是num_int，应该走完一截了，处理一下进位问题，处理完后到下一个元素应该是的地址
                //!!!!!如果上一条是num_int过来的才走
                if(come_num)
                {
                    //come_num则一定需要进位，因为必然走完一段
                    if(carry[v_array->pdata->symtab_array_pdata.dimention_figure-1] > v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-1]-1)
                        carry_point=carry_save(v_array,carry);
                }
                //将carry数组内容赋值进tmp_carry，保存递归前起点情况，是100型还是10型......
                for(int i=0;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
                    tmp_carry[i]=carry[i];

                //从头开始，使用begin_offset_value
                if(carry_point==0)
                    handle_one_dimention(init_val_list->left,v_array,begin_offset_value,carry_point,cur_layer+1, carry);
                    //不从头开始，则用record数组值
                else
                    handle_one_dimention(init_val_list->left,v_array,record[carry_point-1],carry_point,cur_layer+1, carry);
            }

            //遇到InitValList了，则come_num是false了
            come_num=false;

            //走完一个InitValList的进位情况
            //前面没走过num
            if(!have_num_before)
            {
                //但如果本位已经进过一次位了，就不用再进了
                if(tmp_carry[cur_layer]==carry[cur_layer])
                {
                    //走完进一级
                    carry[cur_layer]++;
                    for(int i=cur_layer+1;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
                        carry[i]=0;
                }
            }
                //前面走过num
                //判断tmp_carry，为carry进入递归时的起点，确定carry_point
            else
            {
                //看要修改的是倒数第几位
                int change_index=0;
                for(int i=v_array->pdata->symtab_array_pdata.dimention_figure;i>0;i--)
                {
                    if(tmp_carry[i-1]!=0)
                        break;
                    change_index++;         //有change_index个0
                }
                change_index++;

                carry_point=carry_last_i_save(v_array,carry,change_index);
            }

            //使得进入for gmp
            if((init_val_list->next!=NULL) && (strcmp(bstr2cstr(init_val_list->next->nodeType, '\0'), "InitValList") != 0) && (strcmp(bstr2cstr(init_val_list->next->nodeType, '\0'), "ConstExpList") != 0))
            {
                //借位，使得下一次num_int走num_int的进位处理分支，保证从确定Index开始，而不一定从头开始
                borrow_save(v_array,carry);
                first_init=false;
            }
        }
        else
        {
            have_num_before=true;
            come_num=true;

            if(first_init)
            {
                //解决到除了最后一层地址
                for(int i=start_layer;i<v_array->pdata->symtab_array_pdata.dimention_figure-1;i++)
                {
                    v_offset=(Value*) malloc(sizeof (Value));
                    if(i==start_layer)
                        value_init_int(v_offset,carry[start_layer]);
                    else
                        value_init_int(v_offset,0);

                    ins_gmp=NULL;

                    if(i==start_layer)
                    {
                        ins_gmp= ins_new_binary_operator(GEP, begin_offset_value, v_offset);
                    }
                    else
                    {
                        Value *v_prev=&get_last_inst(instruction_list)->inst->user.value;
                        ins_gmp= ins_new_binary_operator(GEP, v_prev, v_offset);
                    }

                    Value *v_gmp= ins_get_value_with_name(ins_gmp);
                    v_gmp->VTy->ID=AddressTyID;
                    if(v_array->VTy->ID==ArrayTy_FLOAT || v_array->VTy->ID==ArrayTyID_ConstFLOAT || v_array->VTy->ID==GlobalArrayFloat || v_array->VTy->ID==GlobalArrayConstFLOAT)
                        v_gmp->pdata->symtab_array_pdata.address_type=1;
                    v_gmp->pdata->var_pdata.iVal=i;
                    v_gmp->alias=v_array;

                    record[i]=v_gmp;

                    //将这个instruction加入总list
                    InstNode *node2 = new_inst_node(ins_gmp);
                    ins_node_add(instruction_list,node2);
                }
                first_init=false;
            }

            //最后一层，先判断是num_int，ID还是别的
            //先处理如果是0
            past p=init_val_list;
            Value *num=NULL;
            if(strcmp(bstr2cstr(p->nodeType, '\0'), "num_int") == 0)
            {
                num=(Value*) malloc(sizeof (Value));
                value_init_int(num,p->iVal);
            }
            else if(strcmp(bstr2cstr(p->nodeType, '\0'), "num_float") == 0)
            {
                num=(Value*) malloc(sizeof (Value));
                value_init_float(num,p->fVal);
            }
            else if(strcmp(bstr2cstr(p->nodeType, '\0'), "expr") == 0)
            {
                num= cal_expr(p,Unknown,0);
                if(num->VTy->ID==Int)
                    value_init_int(num,num->pdata->var_pdata.iVal);
                else
                    value_init_float(num,num->pdata->var_pdata.fVal);
            }
            //!如果是0就不处理
            //p->iVal是0,直接将位置+1
            if(num!=NULL && strcmp(bstr2cstr(p->nodeType, '\0'), "num_int") == 0 && num->pdata->var_pdata.iVal==0)
            {
                carry_save(v_array,carry);
            }
            else
            {
                //不会进位的情况，处理完后，carry数组最低位自动+1
                // 最后一次走这条分支后最低位一定溢出，下次会进入进位处理分支
                if(carry[v_array->pdata->symtab_array_pdata.dimention_figure-1] <= v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-1]-1)
                {
                    v_offset=(Value*) malloc(sizeof (Value));
                    //这里+1后可能溢出
                    value_init_int(v_offset,carry[v_array->pdata->symtab_array_pdata.dimention_figure-1]++);

                    Instruction *gmp_last=NULL;
                    if(v_array->pdata->symtab_array_pdata.dimention_figure>1)
                        gmp_last=ins_new_binary_operator(GEP, record[v_array->pdata->symtab_array_pdata.dimention_figure - 2], v_offset);
                    else
                        gmp_last= ins_new_binary_operator(GEP,begin_offset_value,v_offset);
                    //是最后一层吧
                    Value *v_gmp= ins_get_value_with_name(gmp_last);
                    v_gmp->VTy->ID=AddressTyID;
                    if(v_array->VTy->ID==ArrayTy_FLOAT || v_array->VTy->ID==ArrayTyID_ConstFLOAT || v_array->VTy->ID==GlobalArrayFloat || v_array->VTy->ID==GlobalArrayConstFLOAT)
                        v_gmp->pdata->symtab_array_pdata.address_type=1;

                    v_gmp->pdata->var_pdata.iVal=v_array->pdata->symtab_array_pdata.dimention_figure-1;
                    v_gmp->alias=v_array;
                    //将这个instruction加入总list
                    InstNode *node_gmp_last = new_inst_node(gmp_last);
                    ins_node_add(instruction_list,node_gmp_last);

                    //其他除num_int和num_float的情况
                    //进行一次store
                    //将num值包装成value
                    if(strcmp(bstr2cstr(p->nodeType, '\0'), "LValArray") == 0)
                    {
                        Value *v_arr= symtab_dynamic_lookup(this,bstr2cstr(p->left->sVal, '\0'));
                        if(v_arr->VTy->ID==AddressTyID)
                        {
                            //先load出这个address
                            Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
                            InstNode *node = new_inst_node(ins_load);
                            ins_node_add(instruction_list,node);
                            Value *v_loadp= ins_get_value_with_name(ins_load);
                            v_loadp->alias=v_array;
                            v_loadp->VTy->ID=AddressTyID;
                            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
                            num= handle_assign_array(p->right->left,v_loadp,1,-1,0);
                        }
                        else
                            num=handle_assign_array(p->right->left,v_arr,1,-1,0);

                    }
                    else if(strcmp(bstr2cstr(p->nodeType, '\0'), "ID") == 0)
                        num= create_load_stmt(bstr2cstr(p->sVal, '\0'));
                    else if(strcmp(bstr2cstr(p->nodeType, '\0'), "Call_Func") == 0)
                        num= create_call_func(p,0);
                    create_store_stmt(num,v_gmp);

                }
                    //要进位
                else
                {
                    //先进一次位，进位后为本次的开始地址
                    int carry_index= carry_save(v_array,carry);

                    Value *v_start=NULL;
                    if(carry_index!=0)
                        v_start=record[carry_index-1];
                    else
                        v_start=begin_offset_value;
                    //从变化的那一位开始gmp
                    for(int i=carry_index;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
                    {
                        Instruction *ins=NULL;
                        Value *v_carry_index=(Value*) malloc(sizeof (Value));
                        value_init_int(v_carry_index,carry[i]);

                        //注意record是岔一级使用的
                        if(i==carry_index)
                        {
                            ins= ins_new_binary_operator(GEP, v_start, v_carry_index);
                        }
                        else
                        {
                            Value *v_prev=&get_last_inst(instruction_list)->inst->user.value;
                            ins= ins_new_binary_operator(GEP, v_prev, v_carry_index);
                            //更新record
                            //record[i]=v_prev;
                        }
                        Value *v_gmp= ins_get_value_with_name(ins);
                        v_gmp->VTy->ID=AddressTyID;
                        v_gmp->pdata->var_pdata.iVal=i;
                        v_gmp->alias=v_array;
                        //更新record
                        record[i]=v_gmp;
                        //将这个instruction加入总list
                        InstNode *node_gmp_last = new_inst_node(ins);
                        ins_node_add(instruction_list,node_gmp_last);
                    }
                    //进行一波store
                    if(strcmp(bstr2cstr(p->nodeType, '\0'), "LValArray") == 0)
                    {
                        Value *v_arr= symtab_dynamic_lookup(this,bstr2cstr(p->left->sVal, '\0'));
                        if(v_arr->VTy->ID==AddressTyID)
                        {
                            //先load出这个address
                            Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
                            InstNode *node = new_inst_node(ins_load);
                            ins_node_add(instruction_list,node);
                            Value *v_loadp= ins_get_value_with_name(ins_load);
                            v_loadp->alias=v_array;
                            v_loadp->VTy->ID=AddressTyID;
                            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
                            num= handle_assign_array(p->right->left,v_loadp,1,-1,0);
                        }
                        else
                            num=handle_assign_array(p->right->left,v_arr,1,-1,0);
                    }
                    else if(strcmp(bstr2cstr(p->nodeType, '\0'), "ID") == 0)
                        num= create_load_stmt(bstr2cstr(p->sVal, '\0'));
                    else if(strcmp(bstr2cstr(p->nodeType, '\0'), "Call_Func") == 0)
                        num= create_call_func(p,0);
                    create_store_stmt(num,record[v_array->pdata->symtab_array_pdata.dimention_figure-1]);

                    //再做一次进位，到下一次的初始地址
                    carry_save(v_array,carry);
                }
            }
        }
        init_val_list=init_val_list->next;
    }
}

Value *handle_assign_array(past root,Value *v_array,int flag,int dimension,int param)
{
    Value *v_last;
    int while_num=0;

    //root是第一维值
    if(dimension==-1)
        while_num=v_array->pdata->symtab_array_pdata.dimention_figure;
    else
        while_num=dimension;
    for (int i=0;i<while_num;i++)
    {
        Value *v_num=NULL;
        Instruction *gmp;
        if(strcmp(bstr2cstr(root->nodeType, '\0'), "num_int") == 0)
        {
            //包装偏移的num_int为Value
            v_num=(Value*) malloc(sizeof (Value));
            value_init_int(v_num,root->iVal);
        }
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "num_float") == 0)
        {
            //包装偏移的num_float为Value
            v_num=(Value*) malloc(sizeof (Value));
            value_init_float(v_num,root->fVal);
        }
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "expr") == 0)
        {
            int convert=-1;
            v_num= cal_expr(root, Var_INT,&convert);
        }
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "ID") == 0)
            v_num= create_load_stmt(bstr2cstr(root->sVal, '\0'));
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "LValArray") == 0)
        {
            Value *array_exp= symtab_dynamic_lookup(this,bstr2cstr(root->left->sVal, '\0'));
            if(array_exp->VTy->ID==AddressTyID)
                v_num= handle_assign_array(root->right->left,array_exp->alias,1,-1,0);
            else
                v_num= handle_assign_array(root->right->left,array_exp,1,-1,0);
        }
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Call_Func") == 0)
            v_num= create_call_func(root,0);


        if(i==0)
        {
            if(v_array->VTy->ID!=AddressTyID)
                gmp= ins_new_binary_operator(GEP, v_array->alias, v_num);
            else
            {
                //还得load一把?
                Value *vvv_load= create_load_stmt(v_array->alias->name);
                gmp=ins_new_binary_operator(GEP, vvv_load, v_num);
                //gmp=ins_new_binary_operator(GEP,v_array,v_num);
            }
        }
        else
        {
            gmp= ins_new_binary_operator(GEP, v_last, v_num);
        }


        Value *v1= ins_get_value_with_name(gmp);
        v1->VTy->ID=AddressTyID;
        v1->pdata->var_pdata.iVal=i;
        if(v_array->VTy->ID!=AddressTyID)
            v1->alias=v_array;
        else
        {
            v1->alias=v_array->alias;
        }


        v_last=v1;

        //将这个instruction加入总list
        InstNode *node_gmp = new_inst_node(gmp);
        ins_node_add(instruction_list,node_gmp);

        root=root->next;
    }
    //load
    if(flag==1 && param==0)
    {
        Instruction *ins_load= ins_new_unary_operator(Load,v_last);
        Value *v_load= ins_get_value_with_name(ins_load);
        v_last=v_load;

        //将这个instruction加入总list
        InstNode *node_load = new_inst_node(ins_load);
        ins_node_add(instruction_list,node_load);
    }

    if(dimension!=-1 && param==0)
        v_last->VTy->ID=AddressTyID;

    return v_last;
}

//还未考虑数组
void create_var_decl(past root,Value* v_return,bool is_global,int block) {

    //到达第一个var的节点
    past vars = root->right->left;

    while (vars!=NULL)
    {
        //纯定义语句不需要处理了xxx，需要将定义标志位置1
        if(strcmp(bstr2cstr(vars->nodeType, '\0'), "ID") == 0)
        {
            Value *v=symtab_dynamic_lookup_first(this, bstr2cstr(vars->sVal, '\0'));
            v->pdata->define_flag=1;
            v=v->alias;
        }
        else if(strcmp(bstr2cstr(vars->nodeType, '\0'), "ConstDef") == 0)
        {
            Value *v=symtab_dynamic_lookup_first(this, bstr2cstr(vars->left->sVal, '\0'));
            v->pdata->define_flag=1;
        }
            //有初值
        else if (strcmp(bstr2cstr(vars->nodeType, '\0'), "VarDef_init") == 0 && !is_global)
        {
            //左值
            Value *v=symtab_dynamic_lookup_first(this, bstr2cstr(vars->left->sVal, '\0'));
            v->pdata->define_flag=1;
            v=v->alias;

            //初始化的值
            //root->right是VarDefList
            Value *v1=NULL;
            if (strcmp(bstr2cstr(vars->right->nodeType, '\0'), "num_int") == 0)
            {
                v1=(Value*) malloc(sizeof (Value));
                value_init_int(v1, vars->right->iVal);
            }
            else if(strcmp(bstr2cstr(vars->right->nodeType, '\0'), "num_float") == 0)
            {
                v1=(Value*) malloc(sizeof (Value));
                value_init_float(v1,vars->right->fVal);
            }
            else if(strcmp(bstr2cstr(vars->right->nodeType, '\0'), "ID") == 0)
            {
                Value *vv= symtab_dynamic_lookup(this,bstr2cstr(vars->right->sVal, '\0'));
                if(vv->VTy->ID==Const_INT)
                {
                    v1=(Value*) malloc(sizeof (Value));
                    value_init_int(v1, vv->pdata->var_pdata.iVal);
                }
                else if(vv->VTy->ID==Const_FLOAT)
                {
                    v1=(Value*) malloc(sizeof (Value));
                    value_init_float(v1, vv->pdata->var_pdata.fVal);
                }
                else
                    v1= create_load_stmt(bstr2cstr(vars->right->sVal,'\0'));
            }
            else if (strcmp(bstr2cstr(vars->right->nodeType, '\0'), "expr") == 0)
            {
                int convert=-1;
                v1 = cal_expr(vars->right,v->VTy->ID,&convert);
            }
            else if(strcmp(bstr2cstr(vars->right->nodeType, '\0'), "Call_Func") == 0)
                v1= create_call_func(vars->right,block);
            else if(strcmp(bstr2cstr(vars->right->nodeType, '\0'), "LValArray") == 0)
            {
                Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(vars->right->left->sVal, '\0'));
                if(v_array->VTy->ID==AddressTyID)
                    v1= handle_assign_array(vars->right->right->left,v_array->alias,1,-1,0);
                else
                    v1= handle_assign_array(vars->right->right->left,v_array,1,-1,0);
            }

            //将值store下来
            create_store_stmt(v1,v);
        }
            //有初值数组
        else if((strcmp(bstr2cstr(vars->nodeType, '\0'), "VarDef_array_init") == 0) || (strcmp(bstr2cstr(vars->nodeType,'\0'),"ConstDef_array_init")==0))
        {
//            if(vars->right->left==NULL)
//            {
//                Value *v_array = symtab_dynamic_lookup_first(this,bstr2cstr(vars->left->left->sVal,'\0'));
//                v_array->pdata->define_flag=1;
//                if(is_global)
//                {
//                    past ident_array=vars->left;         //到IdentArray结点
//                    handle_global_array(v_array,true,vars,0);
//                }
//                vars=vars->next;
//                continue;
//            }
            if(is_global==false)
            {
                past ident_array=vars->left;         //到IdentArray结点
                Value *v_array = symtab_dynamic_lookup_first(this,bstr2cstr(ident_array->left->sVal,'\0'));
                v_array->pdata->define_flag=1;
                past init_val_list = vars->right;

                //生成bitcast,使读取的内存空间通过指针i8*，i8步长读取
                //TODO bitcast 打印不方便
                //<result> = bitcast <ty> <value> to <ty2>
                //The ‘bitcast’ instruction converts value to type ty2 without changing any bits.
                Instruction *ins_bitcast = ins_new_unary_operator(bitcast,v_array->alias);
                Value *v1= ins_get_value_with_name(ins_bitcast);
                v1->alias=v_array;
                v1->pdata->var_pdata.iVal=1;

                //将这个instruction加入总list
                InstNode *node_bitcast = new_inst_node(ins_bitcast);
                ins_node_add(instruction_list,node_bitcast);

                int mem= get_array_total_occupy(v_array,0);
                Value *v_mem=(Value*) malloc(sizeof (Value));
                value_init_int(v_mem,mem);
                Instruction *mem_set= ins_new_binary_operator(MEMSET,v1,v_mem);
                //将这个instruction加入总list
                InstNode *node_mem_set = new_inst_node(mem_set);
                ins_node_add(instruction_list,node_mem_set);

                if(vars->right->left!=NULL && array_all_zeros(vars->right->left)==NULL)
                {
                    //再来一条bitcast
                    //!!!后续第一条一直用v2
                    Instruction *ins_bitcast2= ins_new_unary_operator(bitcast,v1);
                    Value *v2= ins_get_value_with_name(ins_bitcast2);
                    v2->alias=v1->alias;
                    //将这个instruction加入总list
                    InstNode *node_bitcast2 = new_inst_node(ins_bitcast2);
                    ins_node_add(instruction_list,node_bitcast2);

                    //存储num_int用的进位
                    int carry[v_array->pdata->symtab_array_pdata.dimention_figure];
                    for(int i=0;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
                        carry[i]=0;

                    //多维数组,挨个赋值型
                    handle_one_dimention(init_val_list->left,v_array,v2,0,0,carry);
                }

            }
                //TODO 是全局有初始化数组
            else
            {
                past ident_array=vars->left;         //到IdentArray结点
                Value *v_array = symtab_dynamic_lookup_first(this,bstr2cstr(ident_array->left->sVal,'\0'));
                v_array->pdata->define_flag=1;

                if(vars->right->left!=NULL && array_all_zeros(vars->right->left)==NULL)
                    handle_global_array(v_array,true,vars,1);
                else
                {
                    handle_global_array(v_array,true,vars,0);
                    v_array->pdata->symtab_array_pdata.is_init=0;
                }
            }
        }
            //无初值数组
        else if(strcmp(bstr2cstr(vars->nodeType, '\0'), "IdentArray") == 0 && is_global)
        {
            past ident_array=vars->left;         //到IdentArray结点
            Value *v_array = symtab_dynamic_lookup_first(this,bstr2cstr(ident_array->sVal,'\0'));
            v_array->pdata->define_flag=1;
            handle_global_array(v_array,true,vars,0);
        }
        else if(strcmp(bstr2cstr(vars->nodeType, '\0'), "IdentArray") == 0)
        {
            Value *v_array = symtab_dynamic_lookup_first(this,bstr2cstr(vars->left->sVal,'\0'));
            v_array->pdata->define_flag=1;
        }

        vars=vars->next;
    }


    if(root->next!=NULL)
        create_instruction_list(root->next,v_return,block);
}

InstNode *true_location_handler(int type,Value *v_real,int true_goto_location)
{
    Instruction *instruction;
    if(type == br)
        instruction= ins_new_zero_operator(br);
    else if(type == br_i1)
        instruction= ins_new_unary_operator(br_i1,v_real);
    else if(type==br_i1_true)
        instruction= ins_new_zero_operator(br_i1_true);
    else if(type==br_i1_false)
        instruction= ins_new_zero_operator(br_i1_false);
    else
        instruction= ins_new_zero_operator(Label);
    Value *T1= ins_get_dest(instruction);
    if(true_goto_location>0)
        T1->pdata->instruction_pdata.true_goto_location=true_goto_location;

    //TODO 可以吗
    T1->pdata->instruction_pdata.false_goto_location=-2;

    InstNode *node_true = new_inst_node(instruction);
    ins_node_add(instruction_list,node_true);
    return node_true;
}

InstNode *false_location_handler(int type,Value *v_real,int false_goto_location)
{
    Instruction *instruction=NULL;
    if(type == br_i1)
        instruction= ins_new_unary_operator(br_i1, v_real);
    Value *f1= ins_get_dest(instruction);
    f1->pdata->instruction_pdata.false_goto_location=false_goto_location;
    f1->pdata->instruction_pdata.true_goto_location=-2;

    InstNode *node_false = new_inst_node(instruction);
    ins_node_add(instruction_list,node_false);
    return node_false;
}

//root是&&或||,flag为是不是root为||，root->left是&&
//结果为0是恒为假，为1是恒为真；为-1则表示没有经过短路判断
//TODO 如果是b==1 && 0 && a==1这种常数在中间的情况，目前没有做短路，有llvm方式一样
int handle_and_or(past root,bool flag,bool last_or)
{
    //子层是否遇到||后&&的情况的本层记录，不影响本层的flag
    bool flag_notice=false;

    int result=-1;

    //左边
    Value *v1=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        if(strcmp(bstr2cstr(root->sVal, '\0'), "||") == 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0)
        {
            flag_notice=true;
            result=handle_and_or(root->left,true,true);
        }
        else if(strcmp(bstr2cstr(root->sVal, '\0'), "||") == 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0)
            result= handle_and_or(root->left,false,false);
        else
            result=handle_and_or(root->left,false,true);

        //短路
        if(result==1 && strcmp(bstr2cstr(root->sVal, '\0'), "||") == 0)
            return 1;
        if(result==0 && strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0)
            return 0;
    }

    else
    {
        if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0)
            v1= cal_logic_expr(root->left);
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
        {
            Value *v= symtab_dynamic_lookup(this,bstr2cstr(root->left->sVal, '\0'));
            if(v->VTy->ID==Const_INT || v->VTy->ID==Const_FLOAT)
            {
                //直接失败短路
                if(((v->VTy->ID==Const_INT && v->pdata->var_pdata.iVal==0) || (v->VTy->ID==Const_FLOAT && v->pdata->var_pdata.fVal==0)) && strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0)
                    return 0;

                //直接成功为1
                if(((v->VTy->ID==Const_INT && v->pdata->var_pdata.iVal!=0) || (v->VTy->ID==Const_FLOAT && v->pdata->var_pdata.fVal!=0)) && strcmp(bstr2cstr(root->sVal, '\0'), "||") == 0)
                    return 1;
            }
            Value *v_load= create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
            //生成一条icmp ne
            //包装0
            Value *v_zero=(Value*) malloc(sizeof (Value));
            value_init_int(v_zero,0);
            Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
            //v_real
            v1= ins_get_value_with_name(ins_icmp);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(ins_icmp);
            ins_node_add(instruction_list,node);
        }
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0)
        {
            int convert=-1;
            Value *v_load= cal_expr(root->left, Unknown,&convert);
            //生成一条icmp ne
            //包装0
            Value *v_zero=(Value*) malloc(sizeof (Value));
            value_init_int(v_zero,0);
            Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
            //v_real
            v1= ins_get_value_with_name(ins_icmp);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(ins_icmp);
            ins_node_add(instruction_list,node);
        }
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "Call_Func") == 0)
        {
            Value *v_load= create_call_func(root->left,0);
            //生成一条icmp ne
            //包装0
            Value *v_zero=(Value*) malloc(sizeof (Value));
            value_init_int(v_zero,0);
            Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
            //v_real
            v1= ins_get_value_with_name(ins_icmp);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(ins_icmp);
            ins_node_add(instruction_list,node);
        }
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "LValArray") == 0)
        {
            Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(root->left->left->sVal, '\0'));
            Value *v_get= NULL;
            if(v_array->VTy->ID==AddressTyID)
                v_get=handle_assign_array(root->left->right->left,v_array->alias,1,-1,0);
            else
                v_get=handle_assign_array(root->left->right->left,v_array,1,-1,0);
            //生成一条icmp ne
            //包装0
            Value *v_zero=(Value*) malloc(sizeof (Value));
            value_init_int(v_zero,0);
            Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_get,v_zero);
            //v_real
            v1= ins_get_value_with_name(ins_icmp);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(ins_icmp);
            ins_node_add(instruction_list,node);
        }
            //num_int
        else
        {
            //直接失败短路
            if(((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0 && root->left->iVal==0) || (strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0 && root->left->fVal==0)) && strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0)
                return 0;

            //直接成功为1
            if(((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0 && root->left->iVal!=0) || (strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0 && root->left->fVal!=0)) && strcmp(bstr2cstr(root->sVal, '\0'), "||") == 0)
                return 1;
        }

        if(v1!=NULL && strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0)
        {
            InstNode *ins1 = true_location_handler(br_i1, v1, t_index++);
            //入栈
            insnode_push(&S_and,ins1);
        }
            //  ||
        else if(v1!=NULL)
        {
            InstNode *ins1= false_location_handler(br_i1, v1, t_index++);
            insnode_push(&S_or,ins1);
        }
    }

    //子层特殊则本层处理一下,左边递归完走到这里，处理下留下来的right，即1 && 2 || 3的2应用false_location_handler
    //TODO 删了一个&& v1!=NULL,不知道当时为什么加这条，删了可能有不良影响
    if(flag_notice)
    {
        if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "logic_expr") == 0)
            v1= cal_logic_expr(root->left->right);
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0)
        {
            int convert=-1;
            Value *v_load= cal_expr(root->left->right,Unknown,&convert);
            //生成一条icmp ne
            //包装0
            Value *v_zero=(Value*) malloc(sizeof (Value));
            value_init_int(v_zero,0);
            Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
            //v_real
            v1= ins_get_value_with_name(ins_icmp);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(ins_icmp);
            ins_node_add(instruction_list,node);
        }
        else if(strcmp(bstr2cstr(root->left->right->nodeType, '\0'), "Call_Func") == 0)
        {
            Value *v_load= create_call_func(root->left->right,0);
            //生成一条icmp ne
            //包装0
            Value *v_zero=(Value*) malloc(sizeof (Value));
            value_init_int(v_zero,0);
            Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
            //v_real
            v1= ins_get_value_with_name(ins_icmp);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(ins_icmp);
            ins_node_add(instruction_list,node);
        }
        else if(strcmp(bstr2cstr(root->left->right->nodeType, '\0'), "LValArray") == 0)
        {
            Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(root->left->right->left->sVal, '\0'));
            Value *v_get= NULL;
            if(v_array->VTy->ID==AddressTyID)
                v_get=handle_assign_array(root->left->right->left,v_array->alias,1,-1,0);
            else
                v_get=handle_assign_array(root->left->right->left,v_array,1,-1,0);
            //生成一条icmp ne
            //包装0
            Value *v_zero=(Value*) malloc(sizeof (Value));
            value_init_int(v_zero,0);
            Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_get,v_zero);
            //v_real
            v1= ins_get_value_with_name(ins_icmp);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(ins_icmp);
            ins_node_add(instruction_list,node);
        }
            //ID
        else
        {
            Value *v= symtab_dynamic_lookup(this,bstr2cstr(root->left->sVal, '\0'));

            Value *v_load= create_load_stmt(bstr2cstr(root->left->right->sVal, '\0'));
            //生成一条icmp ne
            //包装0
            Value *v_zero=(Value*) malloc(sizeof (Value));
            value_init_int(v_zero,0);
            Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
            //v_real
            v1= ins_get_value_with_name(ins_icmp);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(ins_icmp);
            ins_node_add(instruction_list,node);
        }


        //一定是在&&中但用||
        InstNode *ins1= false_location_handler(br_i1, v1, t_index++);
        insnode_push(&S_or,ins1);

        //加一条tmp
        Instruction *ins_tmp= ins_new_zero_operator(tmp);
        insnode_push(&S_and, new_inst_node(ins_tmp));
        //消调&&栈
        reduce_and(t_index-1);
    }

    if(v1!=NULL)
        //生成一条标号
        true_location_handler(Label,NULL,t_index-1);


    //特殊情况下right跟着下一步left走,就不处理了
    if(!flag)
    {
        //右边
        Value *v2=NULL;
        if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->right->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->right->sVal, '\0'), "||") == 0))
            handle_and_or(root->right,false,true);
        else
        {
            if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "logic_expr") == 0)
                v2= cal_logic_expr(root->right);
            else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "ID") == 0)
            {
                Value *v= symtab_dynamic_lookup(this,bstr2cstr(root->right->sVal, '\0'));
                if(v->VTy->ID==Const_INT || v->VTy->ID==Const_FLOAT)
                {
                    //直接失败,生成br_i1_false
                    if(((v->VTy->ID==Const_INT && v->pdata->var_pdata.iVal==0) || (v->VTy->ID==Const_FLOAT && v->pdata->var_pdata.fVal==0)) && strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0) {
                        InstNode *ins_false= true_location_handler(br_i1_false,NULL,t_index++);
                        insnode_push(&S_and,ins_false);
                    }

                    //直接成功,生成br_i1_true
                    if(((v->VTy->ID==Const_INT && v->pdata->var_pdata.iVal!=0) || (v->VTy->ID==Const_FLOAT && v->pdata->var_pdata.fVal!=0)) && strcmp(bstr2cstr(root->sVal, '\0'), "||") == 0) {
                        InstNode *ins_true= true_location_handler(br_i1_true,NULL,t_index++);
                        insnode_push(&S_or,ins_true);
                    }
                    goto L;
                }
                Value *v_load= create_load_stmt(bstr2cstr(root->right->sVal, '\0'));
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v2= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
            else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "expr") == 0)
            {
                int convert=-1;
                Value *v_load= cal_expr(root->right,Unknown,&convert);
                //加一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v2= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
            else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "Call_Func") == 0)
            {
                Value *v_load= create_call_func(root->right,0);
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v2= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
            else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "LValArray") == 0)
            {
                Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(root->right->left->sVal, '\0'));
                Value *v_get= handle_assign_array(root->right->right->left,v_array,1,-1,0);
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_get,v_zero);
                //v_real
                v2= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
                //num_int
            else
            {
                //直接失败,生成br_i1_false
                if(root->right->iVal==0 && strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0) {
                    InstNode *ins_false= true_location_handler(br_i1_false,NULL,t_index++);
                    insnode_push(&S_and,ins_false);
                }

                //直接成功,生成br_i1_true
                if(root->right->iVal!=0 && strcmp(bstr2cstr(root->sVal, '\0'), "||") == 0) {
                    InstNode *ins_true= true_location_handler(br_i1_true,NULL,t_index++);
                    insnode_push(&S_or,ins_true);
                }
            }

            L:
            if(strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0 && v2!=NULL)
            {
                InstNode *ins1 = true_location_handler(br_i1, v2, t_index++);
                //入栈
                insnode_push(&S_and,ins1);
            }
                //  ||
            else if(v2!=NULL)
            {
                InstNode *ins1=NULL;
                if(last_or==true)
                    ins1= true_location_handler(br_i1, v2, t_index++);
                else
                    ins1= false_location_handler(br_i1,v2,t_index++);
                insnode_push(&S_or,ins1);
            }

            //所有递归离开的最后一次不加Label，其实不能用层数判断
            //if(v1==NULL)
            true_location_handler(Label,NULL,t_index-1);
        }

    }
    return -1;
}

void reduce_and(int false_index)
{
    //先弹出顶层的一条tmp
    InstNode *node=NULL;
    insnode_pop(&S_and,&node);
    insnode_top(&S_and,&node);
    while (!insnode_is_empty(S_and) && node->inst->Opcode!=tmp)
    {
        insnode_pop(&S_and,&node);
        node->inst->user.value.pdata->instruction_pdata.false_goto_location=false_index;
        insnode_top(&S_and,&node);
    }
}

void reduce_or(int true_index,int false_index)
{
    //先弹出顶层的一条tmp
    InstNode *node=NULL;
    insnode_pop(&S_or,&node);
    insnode_top(&S_or,&node);
    while(!insnode_is_empty(S_or) && node->inst->Opcode!=tmp)
    {
        insnode_pop(&S_or,&node);
        if(node->inst->user.value.pdata->instruction_pdata.true_goto_location==-2)
            node->inst->user.value.pdata->instruction_pdata.true_goto_location=true_index;
        else if(node->inst->user.value.pdata->instruction_pdata.false_goto_location==-2)
            node->inst->user.value.pdata->instruction_pdata.false_goto_location=false_index;
        insnode_top(&S_or,&node);
    }
}

void create_if_stmt(past root,Value* v_return,int block) {
    //语句为空，则直接跳过不处理
//    if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "Empty_Stmt") == 0 || strcmp(bstr2cstr(root->right->nodeType, '\0'), "Block_EMPTY") == 0)
//        if(root->next!=NULL)
//        {
////            if(strcmp(bstr2cstr(root->next->nodeType, '\0'), "BlockItemList") == 0)
////                flag_blocklist=0;
//            create_instruction_list(root->next,v_return);
//            return;
//        }

    //br i1 label__,label__
    Value *v_real=NULL;
    int result=-1;          //如果是&&,||的话，保留调用结果
    //如果是0，直接跳过
    //永真则v_real就是NULL
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0)
    {
        if((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0 && root->left->iVal==0) || (strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0 && root->left->fVal==0))
        {
            if(root->next!=NULL) {
//                if(strcmp(bstr2cstr(root->next->nodeType, '\0'), "BlockItemList") == 0)
//                    flag_blocklist=0;
                create_instruction_list(root->next,v_return,1);         //TODO block真该给1吗
            }
            return;
        }
    }

    int convert=0;      //0:不需要翻转br_i1的顺序,1:需要翻转

    //真值
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") != 0  && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") != 0)
        v_real= cal_logic_expr(root->left);
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "LValArray") == 0)
    {
        Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(root->left->left->sVal, '\0'));
        Value *v_load=NULL;
        if(v_array->VTy->ID==AddressTyID)
        {
            //先load出这个address
            Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
            InstNode *node = new_inst_node(ins_load);
            ins_node_add(instruction_list,node);
            Value *v_loadp= ins_get_value_with_name(ins_load);
            v_loadp->alias=v_array;
            v_loadp->VTy->ID=AddressTyID;
            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v_load= handle_assign_array(root->left->right->left,v_loadp,1,-1,0);
        }
        else
            v_load= handle_assign_array(root->left->right->left,v_array,1,-1,0);
        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
        {
            if(get_last_inst(instruction_list)->inst->Opcode==zext)
            {
                //zext前面一定是xor
                deleteIns(get_last_inst(instruction_list));
            }
            else
            {
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v_real= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
        }
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "Call_Func") == 0)
    {
        Value *v_load=NULL;
        if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
            v_load= create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0)
            v_load= cal_expr(root->left,Unknown,&convert);
        else
            v_load= create_call_func(root->left,0);

        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
        {
            if(get_last_inst(instruction_list)->inst->Opcode==zext)
            {
                //zext前面一定是xor
                deleteIns(get_last_inst(instruction_list));
            }
            else
            {
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v_real= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
        }
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        result=handle_and_or(root->left,false,true);
        //一定为假，不用走了
        if(result==0)
        {
            if(root->next!=NULL)
            {
//                if(strcmp(bstr2cstr(root->next->nodeType, '\0'), "BlockItemList") == 0)
//                    flag_blocklist=0;
                create_instruction_list(root->next,v_return,block);
            }
            return;
        }
        //生成一条tmp入栈
        Instruction *ins_tmp= ins_new_zero_operator(tmp);
        insnode_push(&S_or, new_inst_node(ins_tmp));
        insnode_push(&S_and, new_inst_node(ins_tmp));
    }


    InstNode *node1=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result!=1)
    {
        if(!(root->left->sVal!=NULL && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0)))
        {
            if(convert==0)
                //正确跳转
                node1= true_location_handler(br_i1, v_real, t_index++);
            else
                node1= false_location_handler(br_i1, v_real, t_index++);
        }


        //如果上一条不是标号则写编号，走了handle_and_or之后可能会多一条标号
        if(get_last_inst(instruction_list)->inst->Opcode!=Label)
            //生成正确的那个标识IR,示例中8:
            true_location_handler(Label,NULL,t_index-1);
    }

    int trur_point=t_index-1;

    //先走完if为真的所有语句，走完后就可确定否定跳转的位置
    if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "BlockItemList") == 0)
        flag_blocklist=0;
    create_instruction_list(root->right,v_return,0);

    //填充&&,||的情况
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && result!=1 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        reduce_and(t_index);
        reduce_or(trur_point,t_index);
        t_index++;
    }

    if(result==-1 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0)
    {
        if(root->left->sVal==NULL || (root->left->sVal!=NULL && strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") != 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") != 0))
        {
            if(convert==0)
                node1->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index++;
            else
                node1->inst->user.value.pdata->instruction_pdata.true_goto_location=t_index++;
        }
    }


    if(get_last_inst(instruction_list)->inst->Opcode!=Return)
    {
        //无break或continue的话就补，有就不补了
        if(get_last_inst(instruction_list)->inst->Opcode != br && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result != 1)
        {
            //再补一条br label,使每个基本块结束都是跳转,跳转到end,示例中的br label 9
            true_location_handler(br, NULL, t_index - 1);
        }
    }
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result!=1)
        //再补一条标号,可以理解为是false的跳转，也可以理解为是end的跳转
        true_location_handler(Label,NULL,t_index-1);

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return,block);
}

void create_if_else_stmt(past root,Value* v_return,int block) {
    Value *v_real=NULL;
    int result=-1;

    int convert=0;

    //真值
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") != 0  && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") != 0)
        v_real= cal_logic_expr(root->left);
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "LValArray") == 0)
    {
        Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(root->left->left->sVal, '\0'));
        Value *v_load=NULL;
        if(v_array->VTy->ID==AddressTyID)
        {
            //先load出这个address
            Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
            InstNode *node = new_inst_node(ins_load);
            ins_node_add(instruction_list,node);
            Value *v_loadp= ins_get_value_with_name(ins_load);
            v_loadp->alias=v_array;
            v_loadp->VTy->ID=AddressTyID;
            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v_load= handle_assign_array(root->left->right->left,v_loadp,1,-1,0);
        }
        else
            v_load= handle_assign_array(root->left->right->left,v_array,1,-1,0);
        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
        {
            if(get_last_inst(instruction_list)->inst->Opcode==zext)
            {
                //zext前面一定是xor
                deleteIns(get_last_inst(instruction_list));
            }
            else
            {
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v_real= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
        }
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "Call_Func") == 0)
    {
        Value *v_load=NULL;
        if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
            v_load= create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0)
            v_load= cal_expr(root->left,Unknown,&convert);
        else
            v_load=create_call_func(root->left,0);

        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
        {
            if(get_last_inst(instruction_list)->inst->Opcode==zext)
            {
                //zext前面一定是xor
                deleteIns(get_last_inst(instruction_list));
            }
            else
            {
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v_real=ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
        }
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        result=handle_and_or(root->left,false,true);
        //生成一条tmp入栈
        Instruction *ins_tmp= ins_new_zero_operator(tmp);
        insnode_push(&S_or, new_inst_node(ins_tmp));
        insnode_push(&S_and, new_inst_node(ins_tmp));
    }

    InstNode *node1=NULL;
    //正确跳转
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result==-1)
    {
        if(!(root->left->sVal!=NULL && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))) {
            if(convert==0)
                //正确跳转
                node1= true_location_handler(br_i1, v_real, t_index++);
            else
                node1= false_location_handler(br_i1, v_real, t_index++);
        }

        if(get_last_inst(instruction_list)->inst->Opcode!=Label)
            //生成正确的那个标识IR
            true_location_handler(Label,NULL,t_index-1);
    }

    int trur_point=t_index-1;       //and_or使用

    if(((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1) || (strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0) && result==-1) || ((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0) && root->left->iVal!=0) || ((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0) && root->left->fVal!=0) || result==1)
    {
        //走完if为真的语句，即可知道else从哪开始
        //走完if为真，目前最后一条是一个编号，示例的9:
        if(strcmp(bstr2cstr(root->right->left->nodeType, '\0'), "BlockItemList") == 0)
            flag_blocklist=0;
        create_instruction_list(root->right->left,v_return,0);          //if为真走的语句

        //填充&&,||的情况
        if(root->left->sVal!=NULL && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
        {
            reduce_and(t_index);
            reduce_or(trur_point,t_index);
            if(result!=1)        //直接短路的根本没有这些语句，不需要reduce
                t_index++;
        }
    }

    InstNode *node2=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result==-1)
    {
        if(!(root->left->sVal!=NULL && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0)))
        {
            if(convert==0)
                //if为假走else的话应该走这里,示例中将前面else的label 10补齐
                node1->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index++;
            else
                node1->inst->user.value.pdata->instruction_pdata.true_goto_location=t_index++;
        }

        //可能是return,return的话后面就没有了
        if(get_last_inst(instruction_list)->inst->Opcode != br)
            //真的走完之后接一句跳转,跳过else的部分,示例中br label 13的，在当前是br label __
            node2= true_location_handler(br, NULL, 0);

        //再补一条标号,即示例中10:
        true_location_handler(Label,NULL,t_index-1);
    }

    if((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1) || (strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result==-1) || ((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0) && root->left->iVal==0) || ((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0) && root->left->fVal==0) || result==0)
    {
        //走完else部分的语句，即可知道从if为真中出来的下一条应该goto到什么位置
        if(strcmp(bstr2cstr(root->right->right->nodeType, '\0'), "BlockItemList") == 0)
            flag_blocklist=0;
        create_instruction_list(root->right->right,v_return,0);         //if为假走的语句
    }

    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1) {
        //将之前的br label__补齐，示例中补齐为br label 13
        if (node2 != NULL)
            node2->inst->user.value.pdata->instruction_pdata.true_goto_location = t_index++;
        else
            t_index++;                 //t_index++因为后面都用的t_index-1
    }

    if(get_last_inst(instruction_list)->inst->Opcode!=Return)
    {
        if(get_last_inst(instruction_list)->inst->Opcode != br && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result == -1)
        {
            //补一条br label，示例的br label %13，这个和后面那条都是跳到end
            true_location_handler(br, NULL, t_index - 1);
        }
    }
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result==-1)
        //再补一条标号，示例的13:
        true_location_handler(Label,NULL,t_index-1);

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return,block);
}

void create_while_stmt(past root,Value* v_return,int block)
{
    Value *v_real=NULL;
    int result=-1;

    InstNode *ins_false=NULL;
    //如果是0，直接跳过
    //永真则v_real就是NULL
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0)
    {
        if((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0 && root->left->iVal==0) || (strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0 && root->left->fVal==0))
        {
            if(root->next!=NULL)
                create_instruction_list(root->next,v_return,1);
            return;
        }
    }

    int convert=0;

    //加进stack，continue用
    Instruction *ins_tmp= ins_new_zero_operator(tmp);
    Value *t_in= ins_get_dest(ins_tmp);
    t_in->pdata->instruction_pdata.true_goto_location=t_index;
    insnode_push(&S_continue, new_inst_node(ins_tmp));

    int while_recall=t_index;

    //是常数则不用生成这些，可以少一组标号标识
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0)
    {
        //生成一条br label跳转和标号
        true_location_handler(br, NULL, t_index++);
        //标号
        true_location_handler(Label,NULL,t_index-1);
    }

    //真值
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") != 0  && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") != 0)
        v_real= cal_logic_expr(root->left);
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "LValArray") == 0)
    {
        Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(root->left->left->sVal, '\0'));
        Value *v_load=NULL;
        if(v_array->VTy->ID==AddressTyID)
        {
            //先load出这个address
            Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
            InstNode *node = new_inst_node(ins_load);
            ins_node_add(instruction_list,node);
            Value *v_loadp= ins_get_value_with_name(ins_load);
            v_loadp->alias=v_array;
            v_loadp->VTy->ID=AddressTyID;
            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v_load= handle_assign_array(root->left->right->left,v_loadp,1,-1,0);
        }
        else
            v_load= handle_assign_array(root->left->right->left,v_array,1,-1,0);
        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
        {
            if(get_last_inst(instruction_list)->inst->Opcode==zext)
            {
                //zext前面一定是xor
                deleteIns(get_last_inst(instruction_list));
            }
            else
            {
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v_real= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
        }
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "Call_Func") == 0)
    {
        Value *v_load=NULL;
        if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
            v_load= create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0)
            v_load= cal_expr(root->left,Unknown,&convert);
        else
            v_load=create_call_func(root->left,0);

        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
        {
            if(get_last_inst(instruction_list)->inst->Opcode==zext)
            {
                //zext前面一定是xor
                deleteIns(get_last_inst(instruction_list));
            }
            else
            {
                //生成一条icmp ne
                //包装0
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v_load,v_zero);
                //v_real
                v_real= ins_get_value_with_name(ins_icmp);
                //将这个instruction加入总list
                InstNode *node = new_inst_node(ins_icmp);
                ins_node_add(instruction_list,node);
            }
        }
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        result=handle_and_or(root->left,false,true);

        if(result==0)
        {
            //生成一条br_i1_false
            ins_false=true_location_handler(br_i1_false,NULL,t_index++);
        }
        //生成一条tmp入栈
        Instruction *tmp_= ins_new_zero_operator(tmp);
        insnode_push(&S_or, new_inst_node(tmp_));
        insnode_push(&S_and, new_inst_node(tmp_));
    }

    InstNode *node_first_bri1=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && !(root->left->sVal!=NULL && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0  || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0)) )
    {
        if(convert==0)
            node_first_bri1= true_location_handler(br_i1, v_real, t_index++);
        else
            node_first_bri1= false_location_handler(br_i1, v_real, t_index++);
    }
        //是常数则直接跳转
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") == 0)
        node_first_bri1= true_location_handler(br, NULL, t_index++);

    //标号
    //为真的话，条件式就没有IR，可以直接沿用之前的IR
    if(result!=1 && get_last_inst(instruction_list)->inst->Opcode!=Label)
        true_location_handler(Label,NULL,t_index-1);

    int trur_point=t_index-1;

    flag_blocklist=0;
    if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "BlockItemList") == 0)
        flag_blocklist=0;

    while_scope++;
    create_instruction_list(root->right,v_return,0);
    while_scope--;

    //填充&&,||的情况
    if(result==-1 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        reduce_and(t_index);
        reduce_or(trur_point,t_index);
        if(result==-1)
            t_index++;
    }

    if(get_last_inst(instruction_list)->inst->Opcode != br)
    {
        //生成循环，br label跳转到first_label处
        true_location_handler(br, NULL, while_recall);
    }

    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_float") != 0 && result==-1)
    {
        //如果是&& ||则没有node_first_bri1这条
        if(node_first_bri1!=NULL)
        {
            if(convert==0)
                node_first_bri1->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index++;
            else
                node_first_bri1->inst->user.value.pdata->instruction_pdata.true_goto_location=t_index++;
        }

        //标号
        true_location_handler(Label,NULL,t_index-1);
    }
    else
        //标号
        true_location_handler(Label,NULL,t_index++);

    if(ins_false!=NULL)
        ins_false->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index-1;

    //while完了，弹出while的起始位置
    InstNode *out_continue= NULL;
    insnode_pop(&S_continue, &out_continue);

    //在while结束时就开始reduce,如果scope_level相等就约束，不等的就不加入
    //栈中只会有br语句的ins_node,没有tmp
    reduce_break_solo(t_index-1);

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return,block);
}

void create_func_def(past root) {
    while_scope=0;
    Instruction *instruction_begin;
    //拿出存在符号表中的函数信息
    Value *v=symtab_lookup_withmap(this, bstr2cstr(root->left->next->sVal, '\0'), &this->value_maps->next->map);
    instruction_begin = ins_new_unary_operator(FunBegin, v);

    v_cur_func=v;

    //将这个instruction加入总list
    InstNode *instNode_begin = new_inst_node(instruction_begin);
    ins_node_add(instruction_list,instNode_begin);


    //有参
    if (root->left->next->left != NULL) {
        t_index=v->pdata->symtab_func_pdata.param_num+1;
    }
    else
        t_index++;

    //如果有多个返回值，则多生成一条alloca
    Value *v_return=NULL;
    //一定是函数里的第一条指令
    if(return_stmt_num[return_index]>1 || strcmp(v->name,"main")==0  || (return_stmt_num[return_index]==1 && v->pdata->symtab_func_pdata.return_type.ID==VoidTyID))
    {
        if(v->pdata->symtab_func_pdata.return_type.ID != VoidTyID)
        {
            Instruction *instruction= ins_new_zero_operator(Alloca);
            v_return= ins_get_value_with_name(instruction);            //只有v_return没有alias
            //将这个instruction加入总list
            InstNode *node = new_inst_node(instruction);
            ins_node_add(instruction_list,node);
        }
        else
        {
            v_return=(Value*) malloc(sizeof (Value));
            value_init(v_return);
        }

        //有多个返回值
        if(return_stmt_num[return_index]>1 || (return_stmt_num[return_index]==1 && v->pdata->symtab_func_pdata.return_type.ID==VoidTyID))
            v_return->VTy->ID=v->pdata->symtab_func_pdata.return_type.ID;
        else
        {
            if(v->pdata->symtab_func_pdata.return_type.ID==Var_INT)
                v_return->VTy->ID=MAIN_INT;
            else
                v_return->VTy->ID=MAIN_FLOAT;
        }
    }


    //将参数从0开始对应
    //有参
    if (root->left->next->left != NULL) {

        //params走到第一个FuncParam处
        past paramss=root->left->next->left->left;

        //先生成参数的alloca
        while(paramss!=NULL)
        {
            //参数Value
            char *nn=bstr2cstr(paramss->left->next->sVal, '\0');
            Value *param= symtab_lookup_withmap(this,nn, &v->pdata->map_list->map);

            Instruction *instruction= ins_new_unary_operator(Alloca,param);
            Value *v_param= ins_get_value_with_name(instruction);
            param->alias=v_param;v_param->alias=param;
            if(param->VTy->ID==AddressTyID)
            {
                param->VTy->ID=AddressTyID;
                v_param->VTy->ID=AddressTyID;
                v_param->pdata->var_pdata.is_offset=1;
                param->pdata->var_pdata.is_offset=1;

                //传递一下信息
                v_param->pdata=param->pdata;
            }
            else{
                if(param->VTy->ID==Param_INT)
                    v_param->VTy->ID=Var_INT;
                else if(param->VTy->ID==Param_FLOAT)
                    v_param->VTy->ID=Var_FLOAT;
            }
            //将这个instruction加入总list
            InstNode *node = new_inst_node(instruction);
            ins_node_add(instruction_list,node);

            paramss=paramss->next;
        }

        //普通变量的allcoa
        declare_all_alloca(v->pdata->map_list,true);

        //进行参数的store
        paramss=root->left->next->left->left;
        while(paramss!=NULL)
        {
            //参数Value
            Value *param= symtab_lookup_withmap(this,bstr2cstr(paramss->left->next->sVal, '\0'), &v->pdata->map_list->map)->alias;

            Value *v_num_param=create_param_value();
            v_num_param->VTy->ID=param->VTy->ID;
            v_num_param->pdata=param->pdata;
            create_store_stmt(v_num_param,param);

            paramss=paramss->next;
        }
        param_map=0;
    }
    else
        //普通变量的allcoa
        //是函数进来的,先声明一顿alloca
        declare_all_alloca(v->pdata->map_list,true);


    if(strcmp(v->name,"main")==0)
    {
        //main函数还立即跟着一个store 0 %1
        Value *v0=(Value*) malloc(sizeof (Value));
        value_init_int(v0,0);
        create_store_stmt(v0,v_return);
    }

    //进入func的blocklist
    if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "BlockItemList") == 0)
        flag_blocklist=0;
    create_instruction_list(root->right,v_return,0);

    //如果没有return，即void返回
    if(return_stmt_num[return_index]==0)
    {
        Instruction *ins_ret_void= ins_new_zero_operator(Return);
        //将这个instruction加入总list
        InstNode *node_void = new_inst_node(ins_ret_void);
        ins_node_add(instruction_list,node_void);
    }

    //多return的情况下触发
    if(v_return!=NULL && v_return->VTy->ID!=MAIN_INT && v_return->VTy->ID!=MAIN_FLOAT)
    {
        //不是读到return才会触发这个
        //1.先生成标号
        InstNode *instNode=NULL;
        if(get_last_inst(instruction_list)->inst->Opcode!=Label)
        {
            instNode=true_location_handler(Label,NULL,t_index++);
        }
        else
            instNode= get_last_inst(instruction_list);
        //入栈
        insnode_push(&S_return,instNode);
        Value *final_ret=NULL;
        //对于有返回值的情况
        if(v->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
        {
            //2.再生成一条load
            final_ret= create_return_load(v_return);
        }

        //3.最后生成ret
        Instruction *ins_final= NULL;
        if(final_ret==NULL)
            ins_final=ins_new_zero_operator(Return);
        else
            ins_final=ins_new_unary_operator(Return,final_ret);
        //将这个instruction加入总list
        InstNode *node = new_inst_node(ins_final);
        ins_node_add(instruction_list,node);
    }


    //清空index
    t_index=0;

    if(return_stmt_num[return_index]>1 || (return_stmt_num[return_index]==1 && v->pdata->symtab_func_pdata.return_type.ID==VoidTyID))
        reduce_return();
    insnode_stack_new(&S_break);
    insnode_stack_new(&S_continue);

    return_index++;

    if(get_last_inst(instruction_list)->inst->Opcode==Label)
    {
        deleteIns(get_last_inst(instruction_list));
    }

    //FuncEnd
    Instruction *ins_end= ins_new_unary_operator(FunEnd,v);
    //将这个instruction加入总list
    InstNode *node_end = new_inst_node(ins_end);
    ins_node_add(instruction_list,node_end);

    if (root->next != NULL)
        create_instruction_list(root->next,v_return,0);
}

//处理!
//void travel_expr(past* str,int length)
//{
//    bool need_xor=false;
//    for(int i=length-1;i>=0;i--)
//    {
//        if(str[i]->iVal=='!')
//        {
//            if(need_xor)
//                str[i]->sVal= bfromcstr("special_!");
//        }
//        if(str[i]->iVal=='+' || str[i]->iVal=='-')
//            need_xor=true;
//    }
//}

//先后序遍历树，得到后缀表达式并存入数组，再通过后缀表达式得到表达式的值
//目前做的有点复杂，其实应该可以直接后序遍历树就ok的，但目前感觉这样做也蛮清晰的，有时间再改吧
struct _Value *cal_expr(past expr,int type,int* real) {
    //最后从栈中弹出的
    Value *final_result = (Value*) malloc(sizeof (Value));
    value_init(final_result);

    //记录后缀表达式
    past str[20000];
    int i = 0;

    //后序遍历语法树
    past_stack PS1;    //语法树的stack
    init_past_stack(&PS1);

    bool first_not_expr=false;

    past p = expr;
    past q = NULL;    //记录刚刚访问过的结点
    while (p != NULL || !is_empty(PS1)) {
        if (p != NULL) {
            if(strcmp(bstr2cstr(p->nodeType, '\0'), "expr") != 0)
                first_not_expr=true;
            else
                first_not_expr=false;

            push(&PS1, p);
            if(first_not_expr && strcmp(bstr2cstr(p->nodeType, '\0'), "expr") != 0)
            {
                p=NULL;
            }
            else
                p = p->left;
        } else {
            top(&PS1, &p);     //往上走了才pop掉

            if ((p->right == NULL) || (p->right) == q || first_not_expr) {
                //开始往上走
                q = p;              //保存到q，作为下一次处理结点的前驱
                pop(&PS1, &p);
                str[i++] = p;
                p = NULL;         //p置于NULL可继续退层，否则会重复访问刚访问结点的左子树
                first_not_expr=false;
            } else
                p = p->right;
        }
    }
    str[i]=NULL;

    //计算后缀表达式
    value_stack PS2;
    init_value_stack(&PS2);
    //存储数组需要past
    past_stack PS3;
    init_past_stack(&PS3);
    //存储call_func也需要past
    past_stack  PSC;
    init_past_stack(&PSC);
    Value* x1;Value *x2;
    past *pp = str;
    //与0的icmp
    Value *v_zero=(Value*) malloc(sizeof (Value));
    value_init_int(v_zero,0);
    while (*pp) {
        if (strcmp(bstr2cstr((*pp)->nodeType, '\0'), "expr") != 0)
        {
            //做成Value压栈
            Value *v1=NULL;
            if (strcmp(bstr2cstr((*pp)->nodeType, '\0'), "num_int") == 0)
            {
                v1=(Value*) malloc(sizeof (Value));
                value_init_int(v1, (*pp)->iVal);
            }
            else if(strcmp(bstr2cstr((*pp)->nodeType, '\0'), "ID") == 0)
            {
                //如果x1->sVal是a,b,c...需要load
                v1=symtab_dynamic_lookup(this, bstr2cstr((*pp)->sVal, '\0'));
            }
            else if(strcmp(bstr2cstr((*pp)->nodeType, '\0'), "num_float") == 0)
            {
                v1=(Value*) malloc(sizeof (Value));
                value_init_float(v1, (*pp)->fVal);
            }
            else if(strcmp(bstr2cstr((*pp)->nodeType, '\0'), "LValArray") == 0)
            {
                v1= symtab_dynamic_lookup(this,bstr2cstr((*pp)->left->sVal, '\0'));
                past ff=(*pp)->right->left;
                push(&PS3,(*pp)->right->left);
            }
            else if(strcmp(bstr2cstr((*pp)->nodeType, '\0'), "Call_Func") == 0)
            {
                v1=symtab_dynamic_lookup(this, bstr2cstr((*pp)->left->sVal, '\0'));
                if(v1==NULL)      //是库函数
                {
                    v1=(Value*) malloc(sizeof (Value));
                    value_init(v1);
                    v1->VTy->ID=FunctionTyID;
                }
                push(&PSC,(*pp));
            }
            push_value(&PS2, v1);
        }
        else
        {
            if((*pp)->iVal=='!')
            {
                if(*(pp+1))
                {
                    past check=*(pp+1);
                    if(check->iVal=='!')
                    {
                        pp+=2;
                        continue;
                    }
                }
            }

            //如果读到+-*/%就从栈中弹出两个计算,算不动就给临时变量存
            pop_value(&PS2, &x2);
            pop_value(&PS2, &x1);
            //都是常数，直接算出来压进栈
            if ((x1->VTy->ID==Int || x1->VTy->ID==Const_INT) &&
                (x2->VTy->ID==Int || x2->VTy->ID==Const_INT)) {
                Value *v3=(Value*) malloc(sizeof (Value));
                value_init(v3);
                if(type==Var_FLOAT)
                    v3->VTy->ID=Float;
                else
                    v3->VTy->ID=Int;
                switch ((*pp)->iVal) {
                    case '+':
                        v3->pdata->var_pdata.iVal = x1->pdata->var_pdata.iVal + x2->pdata->var_pdata.iVal;
                        break;
                    case '-':
                        v3->pdata->var_pdata.iVal = x1->pdata->var_pdata.iVal - x2->pdata->var_pdata.iVal;
                        break;
                    case '*':
                        v3->pdata->var_pdata.iVal = x1->pdata->var_pdata.iVal * x2->pdata->var_pdata.iVal;
                        break;
                    case '/':
                        v3->pdata->var_pdata.iVal = x1->pdata->var_pdata.iVal / x2->pdata->var_pdata.iVal;
                        break;
                    case '%':
                        v3->pdata->var_pdata.iVal = x1->pdata->var_pdata.iVal % x2->pdata->var_pdata.iVal;
                        break;
                        //!是只有x2的，x1都是0
                    case '!':                                     //TODO !1这种还没处理，目前无法直接判断为假,如果要处理，就定个convert值为一定真和一定假，返回后分别加入if语句那些的num_int
                     //   (*convert)=!(*convert);
                        if(x2->pdata->var_pdata.iVal==0)
                            v3->pdata->var_pdata.iVal=1;
                        else
                            v3->pdata->var_pdata.iVal=0;
                        break;
                }
                if(v3->VTy->ID!=Int)
                    v3->pdata->var_pdata.fVal=(float)v3->pdata->var_pdata.iVal;
                push_value(&PS2, v3);
            }
            else if ((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT || x1->VTy->ID==Int || x1->VTy->ID==Const_INT) &&
                     (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT || x2->VTy->ID==Int || x2->VTy->ID==Const_INT)) {
                Value *v3=(Value*) malloc(sizeof (Value));
                value_init(v3);
                if(type==Var_INT)
                    v3->VTy->ID=Int;
                else
                    v3->VTy->ID=Float;
                switch ((*pp)->iVal) {
                    case '+':
                        if((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT) && (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT))
                            v3->pdata->var_pdata.fVal = x1->pdata->var_pdata.fVal + x2->pdata->var_pdata.fVal;
                        else if((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT) && (x2->VTy->ID==Int || x2->VTy->ID==Const_INT))
                            v3->pdata->var_pdata.fVal = x1->pdata->var_pdata.fVal + (float)x2->pdata->var_pdata.iVal;
                        else if((x1->VTy->ID==Int || x1->VTy->ID==Const_INT) && (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT))
                            v3->pdata->var_pdata.fVal = (float)x1->pdata->var_pdata.iVal + x2->pdata->var_pdata.fVal;
                        break;
                    case '-':
                        if((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT) && (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT))
                            v3->pdata->var_pdata.fVal = x1->pdata->var_pdata.fVal - x2->pdata->var_pdata.fVal;
                        else if((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT) && (x2->VTy->ID==Int || x2->VTy->ID==Const_INT))
                            v3->pdata->var_pdata.fVal = x1->pdata->var_pdata.fVal - (float)x2->pdata->var_pdata.iVal;
                        else if((x1->VTy->ID==Int || x1->VTy->ID==Const_INT) && (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT))
                            v3->pdata->var_pdata.fVal = (float)x1->pdata->var_pdata.iVal - x2->pdata->var_pdata.fVal;
                        break;
                    case '*':
                        if((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT) && (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT))
                            v3->pdata->var_pdata.fVal = x1->pdata->var_pdata.fVal * x2->pdata->var_pdata.fVal;
                        else if((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT) && (x2->VTy->ID==Int || x2->VTy->ID==Const_INT))
                            v3->pdata->var_pdata.fVal = x1->pdata->var_pdata.fVal * (float)x2->pdata->var_pdata.iVal;
                        else if((x1->VTy->ID==Int || x1->VTy->ID==Const_INT) && (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT))
                            v3->pdata->var_pdata.fVal = (float)x1->pdata->var_pdata.iVal * x2->pdata->var_pdata.fVal;
                        break;
                    case '/':
                        if((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT) && (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT))
                            v3->pdata->var_pdata.fVal = x1->pdata->var_pdata.fVal / x2->pdata->var_pdata.fVal;
                        else if((x1->VTy->ID==Float || x1->VTy->ID==Const_FLOAT) && (x2->VTy->ID==Int || x2->VTy->ID==Const_INT))
                            v3->pdata->var_pdata.fVal = x1->pdata->var_pdata.fVal / (float)x2->pdata->var_pdata.iVal;
                        else if((x1->VTy->ID==Int || x1->VTy->ID==Const_INT) && (x2->VTy->ID==Float || x2->VTy->ID==Const_FLOAT))
                            v3->pdata->var_pdata.fVal = (float)x1->pdata->var_pdata.iVal / x2->pdata->var_pdata.fVal;
                        break;
                    case '!':                                     //TODO !1这种还没处理，目前无法直接判断为假,如果要处理，就定个convert值为一定真和一定假，返回后分别加入if语句那些的num_int的判断
                        if(x2->pdata->var_pdata.iVal==0)
                            v3->pdata->var_pdata.iVal=1;
                        else
                            v3->pdata->var_pdata.iVal=0;
                        break;
                }
                if(v3->VTy->ID!=Float)
                    v3->pdata->var_pdata.iVal=(int)v3->pdata->var_pdata.fVal;
                push_value(&PS2, v3);
            }
            else
            {
                Instruction *instruction=NULL;

                //加个判定，如果v1,v2都是LValArray,顺序会出问题
                past roo1=NULL;past roo2=NULL;
                if(x1->name!=NULL && x2->name!=NULL && (!begin_tmp(x1->name) && (x1->VTy->ID==ArrayTy_INT || x1->VTy->ID==ArrayTy_FLOAT || x1->VTy->ID==GlobalArrayInt || x1->VTy->ID==GlobalArrayFloat || x1->VTy->ID==ArrayTyID_ConstINT || x1->VTy->ID==ArrayTyID_ConstFLOAT || x1->VTy->ID==GlobalArrayConstINT || x1->VTy->ID==GlobalArrayConstFLOAT || x1->VTy->ID==AddressTyID)) && (!begin_tmp(x2->name) && (x2->VTy->ID==ArrayTy_INT || x2->VTy->ID==ArrayTy_FLOAT || x2->VTy->ID==GlobalArrayInt || x2->VTy->ID==GlobalArrayFloat || x2->VTy->ID==ArrayTyID_ConstINT || x2->VTy->ID==ArrayTyID_ConstFLOAT || x2->VTy->ID==GlobalArrayConstINT || x2->VTy->ID==GlobalArrayConstFLOAT || x2->VTy->ID==AddressTyID)))
                {
                    pop(&PS3,&roo2);
                    pop(&PS3,&roo1);
                }
                past call1=NULL;past call2=NULL;
                if(x1->VTy->ID==FunctionTyID && x2->VTy->ID==FunctionTyID)
                {
                    pop(&PSC,&call2);
                    pop(&PSC,&call1);
                }

                //先看v1
                Value *v1=NULL;
                if(x1->VTy->ID==FunctionTyID)
                {
                    past call;
                    if(call1==NULL)
                        pop(&PSC,&call);
                    else
                        call=call1;
                    v1= create_call_func(call,0);
                }
                else if(x1->VTy->ID==Int || x1->VTy->ID==Float)
                    v1=x1;
                else if(!begin_tmp(x1->name) && (x1->VTy->ID==Var_INT || x1->VTy->ID==GlobalVarInt || x1->VTy->ID==GlobalVarFloat || x1->VTy->ID==Param_INT || x1->VTy->ID==Var_FLOAT || x1->VTy->ID==Param_FLOAT))
                {
                    if(!begin_global(x1->name))
                        v1= create_load_stmt(x1->name);
                    else
                        v1= create_load_stmt(no_global_name(x1->name));
                    v1->VTy->ID=x1->VTy->ID;
                }
                else if(!begin_tmp(x1->name) && (x1->VTy->ID==ArrayTy_INT || x1->VTy->ID==ArrayTy_FLOAT || x1->VTy->ID==GlobalArrayInt || x1->VTy->ID==GlobalArrayFloat || x1->VTy->ID==ArrayTyID_ConstINT || x1->VTy->ID==ArrayTyID_ConstFLOAT || x1->VTy->ID==GlobalArrayConstFLOAT || x1->VTy->ID==GlobalArrayConstINT || x1->VTy->ID==AddressTyID))
                {
                    past root;
                    if(roo1==NULL)
                        pop(&PS3,&root);
                    else
                        root=roo1;
                    if(x1->VTy->ID==AddressTyID)
                        v1= handle_assign_array(root,x1->alias,1,-1,0);
                    else
                        v1= handle_assign_array(root,x1,1,-1,0);
                }
                else if(x1->VTy->ID==Const_INT)
                {
                    v1=(Value*) malloc(sizeof (Value));
                    value_init_int(v1,x1->pdata->var_pdata.iVal);
                }
                else if(x1->VTy->ID==Const_FLOAT)
                {
                    v1=(Value*) malloc(sizeof (Value));
                    value_init_float(v1,x1->pdata->var_pdata.fVal);
                }
                else
                    v1=x1;
                //v1->VTy->ID=x1->VTy->ID;

                //看v2
                Value *v2=NULL;
                if(x2->VTy->ID==FunctionTyID)
                {
                    past call;
                    if(call2==NULL)
                        pop(&PSC,&call);
                    else
                        call=call2;
                    v2= create_call_func(call,0);
                }
                else if(x2->VTy->ID==Int || x2->VTy->ID==Float)
                    v2=x2;
                else if(!begin_tmp(x2->name) && (x2->VTy->ID==Var_INT || x2->VTy->ID==GlobalVarInt || x2->VTy->ID==GlobalVarFloat || x2->VTy->ID==Param_INT || x2->VTy->ID==Var_FLOAT || x2->VTy->ID==Param_FLOAT))
                {
                    if(!begin_global(x2->name))
                        v2= create_load_stmt(x2->name);
                    else
                        v2= create_load_stmt(no_global_name(x2->name));
                    v2->VTy->ID=x2->VTy->ID;
                }
                else if(!begin_tmp(x2->name) && (x2->VTy->ID==ArrayTy_INT || x2->VTy->ID==ArrayTy_FLOAT || x2->VTy->ID==GlobalArrayFloat || x2->VTy->ID==GlobalArrayInt || x2->VTy->ID==ArrayTyID_ConstINT || x2->VTy->ID==ArrayTyID_ConstFLOAT || x2->VTy->ID==GlobalArrayConstFLOAT || x2->VTy->ID==GlobalArrayConstINT|| x2->VTy->ID==AddressTyID))
                {
                    past root;
                    if(roo2==NULL)
                        pop(&PS3,&root);
                    else
                        root=roo2;
                    if(x2->VTy->ID==AddressTyID)
                        v2= handle_assign_array(root,x2->alias,1,-1,0);
                    else
                        v2= handle_assign_array(root,x2,1,-1,0);
                }
                else if(x2->VTy->ID==Const_INT)
                {
                    v2=(Value*) malloc(sizeof (Value));
                    value_init_int(v2,x2->pdata->var_pdata.iVal);
                }
                else if(x2->VTy->ID==Const_FLOAT)
                {
                    v2=(Value*) malloc(sizeof (Value));
                    value_init_float(v2,x2->pdata->var_pdata.fVal);
                }
                else
                    v2=x2;

                //TODO 除了把'+'禁掉，还有无更好的处理
//                if((*pp)->iVal!='!' && (*pp)->iVal!='+' && get_last_inst(instruction_list)->inst->Opcode==XOR)
//                {
//                    //生成一条zext
//                    Instruction *ins_zext= ins_new_unary_operator(zext,&get_last_inst(instruction_list)->inst->user.value);
//                    Value *v_zext= ins_get_value_with_name(ins_zext);
//
//                    InstNode *node = new_inst_node(ins_zext);
//                    ins_node_add(instruction_list,node);
//
//                    v2=v_zext;
//                }

                Value *v_real=NULL;
                Instruction *ins_icmp=NULL;
                switch ((*pp)->iVal) {
                    case '+':
                        if(!((v1->VTy->ID==Int && v1->pdata->var_pdata.iVal==0) || (v2->VTy->ID==Int && v2->pdata->var_pdata.iVal==0)))
                            instruction = ins_new_binary_operator(Add, v1, v2);
                        break;
                    case '-':
                        instruction = ins_new_binary_operator(Sub, v1, v2);
                        break;
                    case '*':
                        instruction = ins_new_binary_operator(Mul, v1, v2);
                        break;
                    case '/':
                        instruction = ins_new_binary_operator(Div, v1, v2);
                        break;
                    case '%':
                        instruction = ins_new_binary_operator(Mod, v1, v2);
                        break;
                    case '!':
                        if(*real==0)
                        {
                            (*real)=!(*real);
                        } else
                        {
                            ins_icmp= ins_new_binary_operator(NOTEQ,v2,v_zero);
                            //v_real
                            v_real= ins_get_value_with_name(ins_icmp);
                            //将这个instruction加入总list
                            InstNode *node_icmp = new_inst_node(ins_icmp);
                            ins_node_add(instruction_list,node_icmp);

                            //xor
                            instruction= ins_new_unary_operator(XOR,v_real);
                            Value *v_xor= ins_get_value_with_name(instruction);
                            InstNode *node_xor = new_inst_node(instruction);
                            ins_node_add(instruction_list,node_xor);

                            //加zext
                            instruction= ins_new_unary_operator(zext,&get_last_inst(instruction_list)->inst->user.value);
                        }

                        break;
                }
                if(instruction!=NULL)
                {
                    //临时变量左值,v_tmp的pdata是没有实际内容的
                    Value *v_tmp= ins_get_value_with_name(instruction);
                    if(type!=Unknown && type!=AddressTyID)
                    {
                        if(type==GlobalVarInt)
                            v_tmp->VTy->ID=Var_INT;
                        else if(type==GlobalVarFloat)
                            v_tmp->VTy->ID=Var_FLOAT;
                        else
                            v_tmp->VTy->ID=type;
                    }
                    else
                    {
                        if(v1->VTy->ID==Float || v1->VTy->ID==Var_FLOAT || v1->VTy->ID==GlobalVarFloat ||
                           v2->VTy->ID==Float || v2->VTy->ID==Var_FLOAT || v2->VTy->ID==GlobalVarFloat)
                        {
                            v_tmp->VTy->ID=Var_FLOAT;
                        }
                        else
                            v_tmp->VTy->ID = Var_INT;
                    }

                    //将这个instruction加入总list
                    InstNode *node = new_inst_node(instruction);
                    ins_node_add(instruction_list,node);
                    push_value(&PS2, v_tmp);
                }
                else if((*pp)->iVal=='!' && (*real)==0)
                {
                    //如果是!a就将a压入就好
                    push_value(&PS2,v2);
                }
                    //0+a或a+0这种
                else
                {
                    if(v1->VTy->ID==Int && v1->pdata->var_pdata.iVal==0)
                        push_value(&PS2,v2);
                    else
                        push_value(&PS2,v1);
                }
            }
        }
        pp++;
    }

    //弹出最终值
    pop_value(&PS2, &final_result);
    final_result->pdata->map_list= getCurMapList(this);

    return final_result;
}



//原来的直接算出真值的函数，先注释掉，以后可能还是会用到
struct _Value* cal_logic_expr(past logic_expr)
{
    Value *v1=NULL;
    Value *v2=NULL;

    //value1
    if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "expr") == 0)
    {
        int convert=-1;
        v1= cal_expr(logic_expr->left,Unknown,&convert);
    }
    else if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "ID") == 0)
    {
        Value *v_test= symtab_dynamic_lookup(this,bstr2cstr(logic_expr->left->sVal, '\0'));
        if(v_test->VTy->ID==Const_INT)
        {
            v1=(Value*) malloc(sizeof (Value));
            value_init_int(v1,v_test->pdata->var_pdata.iVal);
        }
        else if(v_test->VTy->ID==Const_FLOAT)
        {
            v1=(Value*) malloc(sizeof (Value));
            value_init_float(v1,v_test->pdata->var_pdata.fVal);
        }
        else
            v1= create_load_stmt(bstr2cstr(logic_expr->left->sVal,'\0'));
    }
    else if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "LValArray") == 0)
    {
        Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(logic_expr->left->left->sVal, '\0'));
        if(v_array->VTy->ID==AddressTyID)
        {
            //先load出这个address
            Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
            InstNode *node = new_inst_node(ins_load);
            ins_node_add(instruction_list,node);
            Value *v_load= ins_get_value_with_name(ins_load);
            v_load->alias=v_array;
            v_load->VTy->ID=AddressTyID;
            v_load->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v1= handle_assign_array(logic_expr->left->right->left,v_load,1,-1,0);
        }
        else
            v1= handle_assign_array(logic_expr->left->right->left,v_array,1,-1,0);
    }
    else if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "Call_Func") == 0)
    {
        v1= create_call_func(logic_expr->left,0);
    }
    else if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "logic_expr") == 0)
    {
        Value *v_tmp= (Value*) malloc(sizeof (Value));
        value_init(v_tmp);
        v_tmp=cal_logic_expr(logic_expr->left);
        //补一条
        Instruction *ins= ins_new_unary_operator(zext,v_tmp);
        v1= ins_get_value_with_name(ins);
        v1->VTy->ID=Var_INT;
        //将这个instruction加入总list
        InstNode *node__ = new_inst_node(ins);
        ins_node_add(instruction_list,node__);
    }
//    else
//    {
//        //num_int
//        v1=(Value*) malloc(sizeof (Value));
//        value_init_int(v1,logic_expr->left->iVal);
//    }
    else if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "num_int") == 0)
    {
        //num_int
        v1=(Value*) malloc(sizeof (Value));
        value_init_int(v1,logic_expr->left->iVal);
    }
    else
    {
        v1=(Value*) malloc(sizeof (Value));
        value_init_float(v1,logic_expr->left->fVal);
    }

    //value2
    if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "expr") == 0)
    {
        int convert=-1;
        v2= cal_expr(logic_expr->right,Unknown,&convert);
    }
    else if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "ID") == 0)
    {
        Value *v_test= symtab_dynamic_lookup(this,bstr2cstr(logic_expr->right->sVal, '\0'));
        if(v_test->VTy->ID==Const_INT)
        {
            v2=(Value*) malloc(sizeof (Value));
            value_init_int(v2,v_test->pdata->var_pdata.iVal);
        }
        else if(v_test->VTy->ID==Const_FLOAT)
        {
            v2=(Value*) malloc(sizeof (Value));
            value_init_float(v2,v_test->pdata->var_pdata.fVal);
        }
        else
            v2= create_load_stmt(bstr2cstr(logic_expr->right->sVal,'\0'));
    }
    else if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "LValArray") == 0)
    {
        Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(logic_expr->right->left->sVal, '\0'));
        if(v_array->VTy->ID==AddressTyID)
        {
            //先load出这个address
            Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
            InstNode *node = new_inst_node(ins_load);
            ins_node_add(instruction_list,node);
            Value *v_load= ins_get_value_with_name(ins_load);
            v_load->alias=v_array;
            v_load->VTy->ID=AddressTyID;
            v_load->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v2= handle_assign_array(logic_expr->right->right->left,v_load,1,-1,0);
        }
        else
            v2= handle_assign_array(logic_expr->right->right->left,v_array,1,-1,0);
    }
    else if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "Call_Func") == 0)
    {
        v2= create_call_func(logic_expr->right,0);
    }
    else if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "logic_expr") == 0)
    {
        Value *v_tmp= (Value*) malloc(sizeof (Value));
        value_init(v_tmp);
        v_tmp=cal_logic_expr(logic_expr->right);
        //补一条
        Instruction *ins= ins_new_unary_operator(zext,v_tmp);
        v2= ins_get_value_with_name(ins);
        v2->VTy->ID=Var_INT;
        //将这个instruction加入总list
        InstNode *node__ = new_inst_node(ins);
        ins_node_add(instruction_list,node__);
    }
//    else
//    {
//        //num_int
//        v2=(Value*) malloc(sizeof (Value));
//        value_init_int(v2,logic_expr->right->iVal);
//    }
    else if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "num_int") == 0)
    {
        //num_int
        v2=(Value*) malloc(sizeof (Value));
        value_init_int(v2,logic_expr->right->iVal);
    }
    else
    {
        v2=(Value*) malloc(sizeof (Value));
        value_init_float(v2,logic_expr->right->fVal);
    }

    Instruction *instruction;
    //返回左值是真假类型
    if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "<") == 0)
        instruction= ins_new_binary_operator(LESS,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">") == 0)
        instruction= ins_new_binary_operator(GREAT,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">=") == 0)
        instruction= ins_new_binary_operator(GREATEQ,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "<=") == 0)
        instruction= ins_new_binary_operator(LESSEQ,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "==") == 0)
        instruction= ins_new_binary_operator(EQ,v1,v2);
    else
        instruction= ins_new_binary_operator(NOTEQ,v1,v2);

    //左值value
    //临时变量左值
    Value *v_tmp= ins_get_value_with_name(instruction);
    v_tmp->VTy->ID=Var_INT;

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    //返回左值即logic_expr的真值
    return v_tmp;
}



struct _Value *create_param_value()
{
    sprintf(t_num, "%d", param_map++);
    strcat(t,t_num);
    Value *v_tmp=(Value*) malloc(sizeof (Value));
    value_init(v_tmp);
    v_tmp->pdata->map_list= getCurMapList(this);
    v_tmp->name=(char*) malloc(strlen (t));
    strcpy(v_tmp->name,t);
    clear_tmp(t);
    return v_tmp;
}

struct _Value* create_load_stmt(char *name)
{
    Value *v=symtab_dynamic_lookup(this, name);
    Instruction *instruction=NULL;

    //还是按照load a，后端找栈帧地址自动找到v->alias
    instruction=ins_new_unary_operator(Load,v->alias);
    //最后会删除吧，自动创建instruction的user.value
    Value *v1= ins_get_value_with_name(instruction);

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    return v1;
}
struct _Value* create_return_load(Value *v_return)
{
    Instruction *instruction=NULL;

    //还是按照load a，后端找栈帧地址自动找到v->alias
    instruction=ins_new_unary_operator(Load,v_return);
    //最后会删除吧，自动创建instruction的user.value
    Value *v1= ins_get_value_with_name(instruction);

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    return v1;
}

//将v1的值存入v2的栈帧地址中
void create_store_stmt(Value* v1,Value* v2)
{
    Instruction *instruction=NULL;
    instruction= ins_new_binary_operator(Store,v1,v2);

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);
}

//第一次遍历,生成一些alloca
void declare_all_alloca(struct _mapList* func_map,bool flag)
{
    const char *key, *value;

    //TODO 先遍历本表
    sc_map_foreach (&func_map->map, key, value)
        {
            if(((Value*)value)->VTy->ID !=Const_INT && ((Value*)value)->VTy->ID !=Const_FLOAT )
            {
                if(((Value*)value)->VTy->ID != Param_INT && ((Value*)value)->VTy->ID !=Param_FLOAT && ((Value*)value)->VTy->ID !=AddressTyID)
                {
                    Instruction *instruction= ins_new_unary_operator(Alloca, (Value *) value);
                    //与符号表对应的绑定在一起
                    Value *v_alias= ins_get_value_with_name(instruction);
                    v_alias->VTy->ID=((Value *) value)->VTy->ID;
                    ((Value*)value)->alias=v_alias;v_alias->alias=((Value*)value);
                    //将这个instruction加入总list
                    InstNode *node = new_inst_node(instruction);
                    ins_node_add(instruction_list,node);
                }
            }
        }

    //函数第一层，只能走child
    if(flag)
    {
        if(func_map->child!=NULL)
            declare_all_alloca(func_map->child,false);
    }
        //其他函数层，有child有next都可以走
    else{
        if(func_map->child!=NULL)
            declare_all_alloca(func_map->child,false);
        if(func_map->next!=NULL)
            declare_all_alloca(func_map->next,false);
    }
}
void declare_global_alloca(struct _mapList* func_map)
{
    const char *key, *value;

    //TODO 先遍历本表
    sc_map_foreach (&func_map->map, key, value)
        {
            if(((Value*)value)->VTy->ID != Const_INT && ((Value*)value)->VTy->ID != Const_FLOAT)
            {
                if(((Value*)value)->VTy->ID != FunctionTyID && ((Value*)value)->VTy->ID != ArrayTy_INT && ((Value*)value)->VTy->ID !=ArrayTy_FLOAT && ((Value*)value)->VTy->ID !=GlobalArrayInt && ((Value*)value)->VTy->ID !=GlobalArrayFloat && ((Value*)value)->VTy->ID!=ArrayTyID_ConstINT && ((Value*)value)->VTy->ID!=ArrayTyID_ConstFLOAT && ((Value*)value)->VTy->ID!=GlobalArrayConstINT && ((Value*)value)->VTy->ID!=GlobalArrayConstFLOAT)
                {
                    Value *v_num=(Value*) malloc(sizeof (Value));
                    if(((Value *) value)->pdata!=NULL)
                        value_init_int(v_num,((Value *) value)->pdata->var_pdata.iVal);
                    else
                        value_init_int(v_num,0);
                    Instruction *instruction= ins_new_binary_operator(GLOBAL_VAR, (Value *) value,v_num);

                    //替换一下value,作为alias
                    Value *v_replace= ins_get_global_value(instruction,((Value*)value)->name);
                    //全局
                    ((Value*)value)->alias=v_replace;
                    v_replace->alias=((Value*)value);
                    //拷贝一下其他信息
                    v_replace->VTy->ID=((Value*)value)->VTy->ID;
                    v_replace->pdata->map_list= getCurMapList(this);
                    v_replace->pdata->var_pdata.iVal=v_num->pdata->var_pdata.iVal;

                    //将这个instruction加入总list
                    InstNode *node = new_inst_node(instruction);
                    ins_node_add(instruction_list,node);
                }
                else
                    ((Value*)value)->alias=((Value*)value);
            }
        }
    printf("\n");
    t_index=0;
}

//参数传递
void create_params_stmt(past func_params,Value * v_func)
{
    past paramss=func_params->left;
    int p_num=0;
    while(paramss!=NULL)
    {
        Instruction *instruction=NULL;
        Value *v=NULL;
        if(strcmp(bstr2cstr(paramss->nodeType, '\0'), "num_int") == 0)
        {
            v=(Value*) malloc(sizeof (Value));
            value_init_int(v, paramss->iVal);
        }
        else if(strcmp(bstr2cstr(paramss->nodeType, '\0'), "num_float") == 0)
        {
            v=(Value*) malloc(sizeof (Value));
            value_init_float(v,paramss->fVal);
        }
        else if(strcmp(bstr2cstr(paramss->nodeType, '\0'), "Call_Func") == 0)
            v= create_call_func(paramss,0);
        else if(strcmp(bstr2cstr(paramss->nodeType, '\0'), "LValArray") == 0)
        {
            Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(paramss->left->sVal, '\0'));
            if(v_array->VTy->ID==AddressTyID)
            {
                //先load出这个address
                Instruction *ins_load= ins_new_unary_operator(Load,v_array->alias);
                InstNode *node = new_inst_node(ins_load);
                ins_node_add(instruction_list,node);
                Value *v_load= ins_get_value_with_name(ins_load);
                v_load->alias=v_array;
                v_load->VTy->ID=AddressTyID;
                v_load->pdata->var_pdata.is_offset=1;
                v_load->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
                v= handle_assign_array(paramss->right->left,v_load,1,-1,0);
            }
            else
            {
                //先计数，看看传进去的是几维
                past var=paramss->right->left;
                int dimension_count=0;
                while(var!=NULL)
                {
                    dimension_count++;
                    var=var->next;
                }
                if(dimension_count==v_array->pdata->symtab_array_pdata.dimention_figure)
                    v= handle_assign_array(paramss->right->left,v_array,1,-1,0);
                else
                {
                    //1.传n维度，走n+1个gmp,这里先走n
                    Value *v_f= handle_assign_array(paramss->right->left,v_array,1,dimension_count,1);
                    //2.最后一维，要自己补0
                    Value *v_z=(Value*) malloc(sizeof (Value));
                    value_init_int(v_z,0);
                    Instruction *instruction1= ins_new_binary_operator(GEP, v_f, v_z);
                    v= ins_get_value_with_name(instruction1);
                    v->pdata->var_pdata.iVal=dimension_count+1;  //到时候以dimension_count的态度打印
                    v->pdata->var_pdata.is_offset=1;
                    v->VTy->ID=AddressTyID;
                    v->alias=v_array;
                    //将这个instruction加入总list
                    InstNode *node_bu = new_inst_node(instruction1);
                    ins_node_add(instruction_list,node_bu);
                }
            }
        }
        else if(strcmp(bstr2cstr(paramss->nodeType, '\0'), "expr") == 0)
        {
            int convert=-1;
            v= cal_expr(paramss,v_func->pdata->symtab_func_pdata.param_type_lists[p_num].ID,&convert);
        }
            //是IDent
        else
        {
            //取出value，先区别是数组还是普通id
            Value *v_test= symtab_dynamic_lookup(this,bstr2cstr(paramss->sVal, '\0'));
            if(v_test->VTy->ID==Const_INT)
            {
                v=(Value*) malloc(sizeof (Value));
                value_init_int(v, v_test->pdata->var_pdata.iVal);
            }
            else if(v_test->VTy->ID==Const_FLOAT)
            {
                v=(Value*) malloc(sizeof (Value));
                value_init_float(v, v_test->pdata->var_pdata.fVal);
            }
            //TODO 加了address，有二维传二维的情况
            else if(v_test->VTy->ID==ArrayTy_INT || v_test->VTy->ID==ArrayTy_FLOAT || v_test->VTy->ID==ArrayTyID_ConstINT || v_test->VTy->ID==ArrayTyID_ConstFLOAT || v_test->VTy->ID==GlobalArrayConstFLOAT || v_test->VTy->ID==GlobalArrayConstINT || v_test->VTy->ID==GlobalArrayFloat || v_test->VTy->ID==GlobalArrayInt )
            {
                //只走一维，补0
                //取得首地址，gmp一下
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *instruction1= ins_new_binary_operator(GEP, v_test->alias, v_zero);
                v=ins_get_value_with_name(instruction1);
                v->VTy->ID=AddressTyID;
                v->alias=v_test;
                v->pdata->var_pdata.is_offset=1;
                v->pdata->var_pdata.iVal=1;
                InstNode *node = new_inst_node(instruction1);
                ins_node_add(instruction_list,node);
            }
            else if(v_test->VTy->ID==AddressTyID)
            {
                //只走一维，补0
                //取得首地址，gmp一下
                v= create_load_stmt(bstr2cstr(paramss->sVal, '\0'));
                v->VTy->ID=AddressTyID;
                v->alias=v_test;
                v->pdata->var_pdata.is_offset=1;
                v->pdata->var_pdata.iVal=1;
            }
            else
                v= create_load_stmt(bstr2cstr(paramss->sVal, '\0'));
        }

        //传递参数的IR
        instruction= ins_new_binary_operator(GIVE_PARAM,v,v_func);
        //将这个instruction加入总list
        InstNode *node = new_inst_node(instruction);
        ins_node_add(instruction_list,node);

        paramss=paramss->next;
        p_num++;
    }
}

void printf_array(Value *v_array, int begin_index,FILE* fptr)
{
    for(int i=begin_index;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
    {
        printf("[");
        fprintf(fptr,"[");
        printf("%d ",v_array->pdata->symtab_array_pdata.dimentions[i]);
        fprintf(fptr,"%d ",v_array->pdata->symtab_array_pdata.dimentions[i]);
        printf("x ");
        fprintf(fptr,"x ");
    }
    printf("i32");
    fprintf(fptr,"i32");
    for(int i=begin_index;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
    {
        printf("]");
        fprintf(fptr,"]");
    }
}

bool all_zeros(Value* v_array,int begin,int move)
{
    for(int i=begin;i<begin+move;i++)
    {
        if(v_array->pdata->symtab_array_pdata.array[i]!=0)
            return false;
    }
    return true;
}

void printf_global_array(Value* v_array,FILE* fptr)
{
    printf_array(v_array,0,fptr);
    if(v_array->pdata->symtab_array_pdata.is_init==0)
        return;
    printf(" [");
    fprintf(fptr," [");
    //一维
    if(v_array->pdata->symtab_array_pdata.dimention_figure==1)
    {

        for(int i=0;i<v_array->pdata->symtab_array_pdata.dimentions[0];i++)
        {
            if(i==0)
            {
                if(v_array->VTy->ID==GlobalArrayInt || v_array->VTy->ID==ArrayTy_INT || v_array->VTy->ID==ArrayTyID_ConstINT || v_array->VTy->ID==GlobalArrayConstINT)
                {
                    printf("i32 %d",v_array->pdata->symtab_array_pdata.array[i]);
                    fprintf(fptr,"i32 %d",v_array->pdata->symtab_array_pdata.array[i]);
                } else
                {
                    printf("i32 %f",v_array->pdata->symtab_array_pdata.f_array[i]);
                    fprintf(fptr,"i32 %f",v_array->pdata->symtab_array_pdata.f_array[i]);
                }
            }
            else
            {
                if(v_array->VTy->ID==GlobalArrayInt || v_array->VTy->ID==ArrayTy_INT || v_array->VTy->ID==ArrayTyID_ConstINT || v_array->VTy->ID==GlobalArrayConstINT){
                    printf(", i32 %d",v_array->pdata->symtab_array_pdata.array[i]);
                    fprintf(fptr,", i32 %d",v_array->pdata->symtab_array_pdata.array[i]);
                } else
                {
                    printf(", i32 %f",v_array->pdata->symtab_array_pdata.f_array[i]);
                    fprintf(fptr,", i32 %f",v_array->pdata->symtab_array_pdata.f_array[i]);
                }
            }
        }
    }
        //目前只能打印二维
    else
    {
        int ele_num= get_array_total_occupy(v_array,0);
        ele_num/=4;
        int i=0;
        int move=v_array->pdata->symtab_array_pdata.dimentions[1];
        while(i<ele_num)
        {
            printf("[%d x i32]",v_array->pdata->symtab_array_pdata.dimentions[1]);
            fprintf(fptr,"[%d x i32]",v_array->pdata->symtab_array_pdata.dimentions[1]);
            if(!all_zeros(v_array,i,move))
            {
                printf("[");
                fprintf(fptr,"[");
                for(int j=i;j<i+move;j++)
                {
                    if(j==i)
                    {
                        if(v_array->VTy->ID==GlobalArrayInt || v_array->VTy->ID==ArrayTy_INT){
                            printf("i32 %d",v_array->pdata->symtab_array_pdata.array[j]);
                            fprintf(fptr,"i32 %d",v_array->pdata->symtab_array_pdata.array[j]);
                        }
                        else
                        {
                            printf("i32 %f",v_array->pdata->symtab_array_pdata.f_array[j]);
                            fprintf(fptr,"i32 %f",v_array->pdata->symtab_array_pdata.f_array[j]);
                        }
                    }
                    else
                    {
                        if(v_array->VTy->ID==GlobalArrayInt || v_array->VTy->ID==ArrayTy_INT){
                            printf(", i32 %d",v_array->pdata->symtab_array_pdata.array[j]);
                            fprintf(fptr,", i32 %d",v_array->pdata->symtab_array_pdata.array[j]);
                        }
                        else
                        {
                            printf(", i32 %f",v_array->pdata->symtab_array_pdata.f_array[j]);
                            fprintf(fptr,", i32 %f",v_array->pdata->symtab_array_pdata.f_array[j]);
                        }
                    }
                }
                printf("]");
                fprintf(fptr,"]");
            }
            else
            {
                printf(" zeroinitializer");
                fprintf(fptr," zeroinitializer");
            }
            i+=move;
            if(i<ele_num)
            {
                printf(",");
                fprintf(fptr,",");
            }
        }
    }
    printf("],");
    fprintf(fptr,"],");
}

char* c2ll(const char* file_name) {
    char *p = malloc(strlen(file_name) + 4); // 分配足够的空间存放新字符串，长度加4是因为要添加后缀".ll"
    if (p == NULL) { // 内存分配失败，返回 NULL
        return NULL;
    }
    strcpy(p, file_name); // 复制原字符串
    char *dot_pos = strrchr(p, '.'); // 查找最后一个 '.'
    if (dot_pos == NULL || dot_pos - p <= 1) { // 如果找不到 dots 或者最后一个 '.' 在第二个字符或之前，返回 NULL
        free(p);
        return NULL;
    }
    *dot_pos = '\0'; // 将最后一个 '.' 替换成 '\0'，截断字符串
    strcat(p, ".ll"); // 添加后缀
    return p;
}

bool begin_tmp(const char* name)
{
    if(name==NULL)
        return false;

    char *prefix="%";
    if(name[0]==prefix[0])
        return true;
    return false;
}

bool begin_global(const char* name)
{
    char *prefix="@";
    if(name[0]==prefix[0])
        return true;
    return false;
}

char* no_global_name(const char *name)
{
    char* name2=(char*) malloc(sizeof (name)-1);
    int i=1;
    while(name[i])
    {
        name2[i-1]=name[i];
        i++;
    }
    return name2;
}

void printf_llvm_ir(struct _InstNode *instruction_node,char *file_name,int before)
{
    bool flag_func=false;

    instruction_node= get_next_inst(instruction_node);
    const char* ll_file= c2ll(file_name);

    FILE *fptr= fopen(ll_file,"w");

    Value *v_cur_array=NULL;

    int p=0;
    int give_count=0;

    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN)
    {
        Instruction *instruction=instruction_node->inst;
        printf("%d(id) : %d ",instruction->i,instruction->user.value.pdata->var_pdata.iVal);
        printf("%d .",instruction->user.value.pdata->var_pdata.is_offset);
        switch (instruction_node->inst->Opcode)
        {
            case Alloca:
                if(instruction->user.use_list!=NULL && (instruction->user.use_list->Val->VTy->ID==ArrayTy_INT || instruction->user.use_list->Val->VTy->ID==ArrayTy_FLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayInt || instruction->user.use_list->Val->VTy->ID==GlobalArrayFloat || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstINT || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstINT))
                {
                    printf(" %s = alloca ",instruction->user.value.name);
                    fprintf(fptr," %s = alloca ",instruction->user.value.name);
                    printf_array(instruction->user.use_list->Val,0,fptr);
                    printf(",align 16\n");
                    fprintf(fptr,",align 16\n");
                }
                else if(instruction->user.use_list!=NULL && instruction->user.use_list->Val->VTy->ID==AddressTyID)
                {
                    if(instruction->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
                    {
                        printf(" %s = alloca ",instruction->user.value.name);
                        fprintf(fptr," %s = alloca ",instruction->user.value.name);
                        printf_array(instruction->user.use_list->Val,1,fptr);
                        printf("*,align 4\n");
                        fprintf(fptr,"*,align 4\n");
                    }
                    else
                    {
                        printf(" %s = alloca i32*,align 4\n",instruction->user.value.name);
                        fprintf(fptr," %s = alloca i32*,align 4\n",instruction->user.value.name);
                    }
                }
                else
                {
                    printf(" %s = alloca i32,align 4\n",instruction->user.value.name);
                    fprintf(fptr," %s = alloca i32,align 4\n",instruction->user.value.name);
                }
                break;
            case Load:
                if(instruction->user.use_list->Val->VTy->ID==AddressTyID && instruction->user.use_list->Val->pdata->var_pdata.is_offset==1)
                {
                    if(before)
                        instruction->user.value.VTy->ID=AddressTyID;
                    instruction->user.value.pdata->var_pdata.is_offset=1;
                    if(instruction->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
                    {
                        printf(" %s = load ",instruction->user.value.name);
                        fprintf(fptr," %s = load ",instruction->user.value.name);
                        printf_array(instruction->user.use_list->Val,1,fptr);
                        printf("*,");
                        fprintf(fptr,"*,");
                        printf_array(instruction->user.use_list->Val,1,fptr);
                        printf("** %s,align 4\n",instruction->user.use_list->Val->name);
                        fprintf(fptr,"** %s,align 4\n",instruction->user.use_list->Val->name);
                    }
                    else
                    {
                        printf(" %s = load i32*,i32** %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                        fprintf(fptr," %s = load i32*,i32** %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                } else
                {
                    printf(" %s = load i32,i32* %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    fprintf(fptr," %s = load i32,i32* %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                }
                break;
            case Store:
                if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==0)
                {
                    printf(" store i32 %d,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32 %d,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==1)
                {
                    printf(" store i32* %d,i32** %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32* %d,i32** %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float || instruction->user.use_list->Val->VTy->ID==Const_FLOAT)
                {
                    printf(" store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                }
                else if(instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==0)
                {
                    printf(" store i32 %s,i32* %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32 %s,i32* %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    if(instruction->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
                    {
                        printf(" store ");
                        fprintf(fptr," store ");
                        printf_array(instruction->user.use_list[1].Val,1,fptr);
                        printf("* %s,",instruction->user.use_list->Val->name);
                        fprintf(fptr,"* %s,",instruction->user.use_list->Val->name);
                        printf_array(instruction->user.use_list->Val,1,fptr);
                        printf("** %s,align 4\n",instruction->user.use_list[1].Val->name);
                        fprintf(fptr,"** %s,align 4\n",instruction->user.use_list[1].Val->name);
                    } else{
                        printf(" store i32* %s,i32** %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," store i32* %s,i32** %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case br:
                printf(" br label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                fprintf(fptr," br label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                break;
            case br_i1:
                printf(" br i1 %s,label %%%d,label %%%d\n\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                fprintf(fptr," br i1 %s,label %%%d,label %%%d\n\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case br_i1_false:
                printf(" br i1 false,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                fprintf(fptr," br i1 false,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case br_i1_true:
                printf(" br i1 true,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                fprintf(fptr," br i1 true,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case EQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }

                break;
            case LESS:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case NOTEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case GREAT:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case GREATEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case LESSEQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case FunBegin:
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID==VoidTyID)
                {
                    printf("define dso_local void @%s(",instruction->user.use_list->Val->name);
                    fprintf(fptr,"define dso_local void @%s(",instruction->user.use_list->Val->name);
                } else
                {
                    printf("define dso_local i32 @%s(",instruction->user.use_list->Val->name);
                    fprintf(fptr,"define dso_local i32 @%s(",instruction->user.use_list->Val->name);
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
                                printf_array(node2->inst->user.use_list->Val,1,fptr);
                                printf("* %%0");
                                fprintf(fptr,"* %%0");
                                node2= get_next_inst(node2);
                            }
                            else {
                                printf("i32* %%0");
                                fprintf(fptr,"i32* %%0");
                            }
                        }
                        else
                        {
                            printf("i32 %%0");
                            fprintf(fptr,"i32 %%0");
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
                                fprintf(fptr,",");
                                printf_array(node2->inst->user.use_list->Val,1,fptr);
                                printf("* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                fprintf(fptr,"* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                node2= get_next_inst(node2);
                            }
                            else
                            {
                                printf(",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                                fprintf(fptr,",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            }
                        }
                        else
                        {
                            printf(",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            fprintf(fptr,",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                        }
                    }
                    p--;
                }
                printf(") #0{\n");
                fprintf(fptr,") #0{\n");
                break;
            case Return:
                if(instruction->user.use_list==NULL)
                {
                    printf(" ret void\n");
                    fprintf(fptr," ret void\n");
                }
                else if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    printf(" ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
                    fprintf(fptr," ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    printf(" ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
                    fprintf(fptr," ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
                }
                else
                {
                    printf(" ret i32 %s\n",instruction->user.use_list->Val->name);
                    fprintf(fptr," ret i32 %s\n",instruction->user.use_list->Val->name);
                }

//                printf("}\n\n");
//                fprintf(fptr,"}\n\n");
                break;
            case Call:
                get_param_list(instruction->user.use_list->Val,&give_count);
                if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        printf(" %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
                        fprintf(fptr," %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                        //是库函数
                    else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
                    {
                        printf(" %s = call i32 (",instruction->user.value.name);
                        fprintf(fptr," %s = call i32 (",instruction->user.value.name);
                    }
                    else
                    {
                        printf(" %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
                        fprintf(fptr," %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
                    }
                }
                else     //voidTypeID
                {
                    //非库函数
                    if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
                    {
                        printf(" call void @%s(",instruction->user.use_list->Val->name);
                        fprintf(fptr," call void @%s(",instruction->user.use_list->Val->name);
                    }
                        //是库函数
                    else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
                    {
                        printf(" call void (");
                        fprintf(fptr," call void (");
                    }
                    else
                    {
                        printf(" call void (...) @%s (",instruction->user.use_list->Val->name);
                        fprintf(fptr," void i32 (...) @%s (",instruction->user.use_list->Val->name);
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
                                fprintf(fptr,"i32*,");
                            }
                            else
                            {
                                printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                printf("*,");
                                fprintf(fptr,"*,");
                            }
                        }
                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                        {
                            printf("i32*,");
                            fprintf(fptr,"i32*,");
                        }
                        else
                        {
                            printf("i32,");
                            fprintf(fptr,"i32,");
                        }
                    }
                    printf("...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);
                    fprintf(fptr,"...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);

                    for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
                    {
                        if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                        {
                            v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                            if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                            {
                                printf("i32*,");
                                fprintf(fptr,"i32*,");
                            }
                            else
                            {
                                printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                printf("*,");
                                fprintf(fptr,"*,");
                            }
                        }
                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                        {
                            printf("i32*,");
                            fprintf(fptr,"i32*,");
                        }
                        else
                        {
                            printf("i32,");
                            fprintf(fptr,"i32,");
                        }
                    }
                    printf("...)*)(");
                    fprintf(fptr,"...)*)(");
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
                                fprintf(fptr,"i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                            {
                                v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                                if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                                {
                                    printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                    fprintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                                else
                                {
                                    printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                    printf("* %s",one_param[i]->inst->user.use_list->Val->name);
                                    fprintf(fptr,"* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                            {
                                printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                fprintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                            else
                            {
                                printf("i32 %s",one_param[i]->inst->user.use_list->Val->name);
                                fprintf(fptr,"i32 %s",one_param[i]->inst->user.use_list->Val->name);
                            }

                        }
                        else
                        {
                            if(one_param[i]->inst->user.use_list->Val->VTy->ID==Int)
                            {
                                printf(",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                                fprintf(fptr,",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
                            {
                                v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
                                if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
                                {
                                    printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                    fprintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                }
                                else
                                {
                                    printf(",");
                                    fprintf(fptr,",");
                                    printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
                                    printf("* ");
                                    fprintf(fptr,"* ");
                                    printf("%s",one_param[i]->inst->user.use_list->Val->name);
                                    fprintf(fptr,"%s",one_param[i]->inst->user.use_list->Val->name);
                                }
                            }
                            else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
                            {
                                printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                                fprintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                            else
                            {
                                printf(",i32 %s",one_param[i]->inst->user.use_list->Val->name);
                                fprintf(fptr,",i32 %s",one_param[i]->inst->user.use_list->Val->name);
                            }
                        }
                    }
                }

                printf(")\n");
                fprintf(fptr,")\n");
                break;
            case Label:
                printf("%d:                  ; preds = ",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                fprintf(fptr,"%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                if(instruction->Parent){
                    BasicBlock *parent = instruction->Parent;
                    HashSetFirst(parent->preBlocks);
                    for(BasicBlock *block = HashSetNext(parent->preBlocks); block != NULL; block = HashSetNext(parent->preBlocks)){
                        printf("%%%d ",block->id);
                    }
                }
                printf("\n");
                break;
            case Add:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Sub:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sub nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= sub nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sub nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= sub nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sub nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= sub nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sub nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= sub nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sub nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sub nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sub nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= sub nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Mul:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= mul nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= mul nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= mul nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= mul nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= mul nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= mul nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= mul nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= mul nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= mul nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= mul nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= mul nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= mul nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Div:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sdiv i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= sdiv i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sdiv i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= sdiv i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sdiv i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= sdiv i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sdiv i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= sdiv i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sdiv i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sdiv i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else if(instruction->user.use_list[1].Val->VTy->ID==Float)
                    {
                        printf(" %s= sdiv i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr," %s= sdiv i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                    else
                    {
                        printf(" %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Mod:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= srem i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= srem i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s= srem i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= srem i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= srem i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= srem i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s= srem i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= srem i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case bitcast:
                //第一条bitcast
                if(instruction->user.value.pdata->var_pdata.iVal==1)
                {
                    //v_cur_array=instruction->user.use_list->Val->alias;
                    printf(" %s=bitcast ",instruction->user.value.name);
                    fprintf(fptr," %s=bitcast ",instruction->user.value.name);
                    printf_array(instruction->user.use_list->Val->alias,0,fptr);
                    printf("* %s to i8*\n",instruction->user.use_list->Val->name);
                    fprintf(fptr,"* %s to i8*\n",instruction->user.use_list->Val->name);
                }
                else
                {
                    printf(" %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
                    fprintf(fptr," %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
                    printf_array(instruction->user.use_list->Val->alias,0,fptr);
                    printf("*\n");
                    fprintf(fptr,"*\n");
                }
                break;

            case GEP:
                if(instruction->user.value.alias!=NULL)
                    v_cur_array=instruction->user.value.alias;
                //printf("%d...\n",instruction->user.value.pdata->var_pdata.iVal);
                printf(" %s=getelementptr inbounds ",instruction->user.value.name);
                fprintf(fptr," %s=getelementptr inbounds ",instruction->user.value.name);
                if(instruction->user.value.pdata->var_pdata.iVal<0)
                {
                    printf_array(v_cur_array,0,fptr);
                    printf(",");
                    fprintf(fptr,",");
                    printf_array(v_cur_array,0,fptr);
                    printf("* %s,i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    fprintf(fptr,"* %s,i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                }
                else
                {
                    if(instruction->user.use_list->Val->pdata->var_pdata.is_offset==0)
                    {
                        //是对数组参数的最后一个自造的gmp
                        if(instruction->user.value.VTy->ID==AddressTyID && instruction->user.value.pdata->var_pdata.is_offset==1)
                        {
                            printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal-1,fptr);
                            printf(",");
                            fprintf(fptr,",");
                            printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal-1,fptr);
                            printf("* ");
                            fprintf(fptr,"* ");
                            if(instruction->user.use_list[1].Val->VTy->ID==Int)
                            {
                                printf("%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                                fprintf(fptr,"%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                            }
                            else
                            {
                                printf("%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                                fprintf(fptr,"%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                            }
                        }
                            //正常的
                        else{
                            printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
                            printf(",");
                            fprintf(fptr,",");
                            printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
                            printf("* ");
                            fprintf(fptr,"* ");
                            if(instruction->user.use_list[1].Val->VTy->ID==Int)
                            {
                                printf("%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                                fprintf(fptr,"%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                            }
                            else
                            {
                                printf("%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                                fprintf(fptr,"%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                            }
                        }

                    }
                    else {
                        Value *array=instruction->user.value.alias;
                        if(array->pdata->symtab_array_pdata.dimentions[0]==0)
                        {
                            printf_array(array,1,fptr);
                            printf(",");
                            fprintf(fptr,",");
                            printf_array(array,1,fptr);
                            if(instruction->user.use_list[1].Val->VTy->ID==Int)
                            {
                                printf("* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                                fprintf(fptr,"* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                            }
                            else
                            {
                                printf("* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                                fprintf(fptr,"* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                            }
                        }
                        else
                        {
                            if(instruction->user.use_list[1].Val->VTy->ID==Int)
                            {
                                printf("i32,i32* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                                fprintf(fptr,"i32,i32* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                            }
                            else
                            {
                                printf("i32,i32* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                                fprintf(fptr,"i32,i32* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                            }
                        }
                    }
                }

                printf("\n");
                fprintf(fptr,"\n");
                //}
                break;
            case MEMSET:
                flag_func=true;
                printf(" call void @llvm.memset.p0i8.i64(i8* align 16 %s, i8 0, i64 %d, i1 false)\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                fprintf(fptr," call void @llvm.memset.p0i8.i64(i8* align 16 %s, i8 0, i64 %d, i1 false)\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                break;
            case MEMCPY:
                flag_func=true;
                printf(" call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %s, i8* align 16 bitcast (",instruction->user.use_list->Val->name);
                fprintf(fptr," call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %s, i8* align 16 bitcast (",instruction->user.use_list->Val->name);
                printf_array(instruction->user.use_list[1].Val,0,fptr);
                printf("* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
                       get_array_total_occupy(instruction->user.use_list[1].Val,0));
                fprintf(fptr,"* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
                        get_array_total_occupy(instruction->user.use_list[1].Val,0));
                break;
            case GLOBAL_VAR:
                if(instruction->user.use_list->Val->VTy->ID!=ArrayTy_INT && instruction->user.use_list->Val->VTy->ID!=ArrayTy_FLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayInt && instruction->user.use_list->Val->VTy->ID!=GlobalArrayFloat && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_ConstINT && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_ConstFLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayConstFLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayConstINT)
                {
                    if(instruction->user.use_list->Val->VTy->ID==GlobalVarInt)
                    {
                        printf("%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr,"%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    } else
                    {
                        printf("%s=dso_local global i32 %f,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                        fprintf(fptr,"%s=dso_local global i32 %f,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
                    }
                }
                else
                {
                    v_cur_array=instruction->user.use_list->Val;
                    if(instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstINT || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstINT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstFLOAT)
                    {
                        printf("%s= internal constant ",instruction->user.use_list->Val->name);
                        fprintf(fptr,"%s= internal constant ",instruction->user.use_list->Val->name);
                        printf_global_array(instruction->user.use_list->Val,fptr);
                        printf("align 4\n");
                        fprintf(fptr,"align 4\n");
                    }
                    else
                    {
                        printf("%s=dso_local global ",instruction->user.use_list->Val->name);
                        fprintf(fptr,"%s=dso_local global ",instruction->user.use_list->Val->name);
                        printf_global_array(instruction->user.use_list->Val,fptr);
                        if(instruction->user.use_list->Val->pdata->symtab_array_pdata.is_init==0)
                        {
                            printf(" zeroinitializer, align 4\n");
                            fprintf(fptr," zeroinitializer, align 4\n");
                        }
                        else
                        {
                            printf("align 4\n");
                            fprintf(fptr,"align 4\n");
                        }
                    }
                }
                break;
            case GIVE_PARAM:
                params[give_count++]=instruction_node;
                if(instruction->user.use_list->Val->VTy->ID==Int)
                    printf("give param %d,func:%s\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                else
                    printf("give param %s,func:%s\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                break;
            case FunEnd:
                printf("}\n\n");
                fprintf(fptr,"}\n\n");
                break;
            case zext:
                printf(" %s = zext i1 %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                fprintf(fptr," %s = zext i1 %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                break;
            case XOR:
                printf(" %s = xor i1 %s, true\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                fprintf(fptr," %s = xor i1 %s, true\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                break;
            case Phi:{
                Value *insValue = ins_get_dest(instruction);
                HashSet *phiSet = instruction->user.value.pdata->pairSet;
                HashSetFirst(phiSet);
                printf(" %s( %s) = phi i32",instruction->user.value.name, instruction->user.value.alias->name);
                fprintf(fptr," %s = phi i32",instruction->user.value.name);
                unsigned int size=HashSetSize(phiSet);
                int i=0;
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    BasicBlock *from = phiInfo->from;
                    Value *incomingVal = phiInfo->define;
                    if(i + 1 == size)      //最后一次
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                            printf("[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
                            fprintf(fptr,"[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
                        }else if(incomingVal != NULL){
                            printf("[%s , %%%d]",incomingVal->name,from->id);
                            fprintf(fptr,"[%s , %%%d]",incomingVal->name,from->id);
                        }else{
                            //是NULL的话就
                            printf("[ undef, %%%d] ",from->id);
                            fprintf(fptr,"[ undef, %%%d] ",from->id);
                        }
                    }
                    else
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                            printf("[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
                            fprintf(fptr,"[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
                        }else if(incomingVal != NULL){
                            printf("[%s , %%%d], ",incomingVal->name,from->id);
                            fprintf(fptr,"[%s , %%%d], ",incomingVal->name,from->id);
                        }else{
                            printf("[ undef, %%%d], ",from->id);
                            fprintf(fptr,"[ undef, %%%d], ",from->id);
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
//        Value *v,*vl,*vr;
//        v= ins_get_dest(instruction_node->inst);
//        vl= ins_get_lhs(instruction_node->inst);
//        vr= ins_get_rhs(instruction_node->inst);
//        if(v!=NULL)
//            printf("left:%s,\t",type_str[v->VTy->ID]);
//        if(vl!=NULL)
//            printf("value1:%s,\t",type_str[vl->VTy->ID]);
//        if(vr!=NULL)
//            printf("value2:%s,\t",type_str[vr->VTy->ID]);
//        printf("\n\n");

//        if(instruction->isCritical){
//            printf("isCritical\n\n");
//        }
        instruction_node= get_next_inst(instruction_node);
    }
    if(flag_func)
    {
        fprintf(fptr,"\n");
        fprintf(fptr,"declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1\n");
        fprintf(fptr,"declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2\n");
    }

    fprintf(fptr,"declare dso_local i32 @getint(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @putint(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @getch(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @getarray(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @getfloat(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @getfarray(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @putch(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @putarray(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @putfloat(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @putfarray(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @putf(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @starttime(...) #1\n");
    fprintf(fptr,"declare dso_local i32 @stoptime(...) #1\n");
    fclose(fptr);
}

void fix_array(struct _InstNode *instruction_node)
{
    //将offset值全部清0
    InstNode *tem= get_next_inst(instruction_node);
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        Instruction *instruction = instruction_node->inst;
        instruction->user.value.pdata->var_pdata.is_offset=0;
        instruction_node= get_next_inst(instruction_node);
    }

    //fix array
    InstNode *start=instruction_node;
    instruction_node= get_next_inst(instruction_node);
    int offset=0;

    bool after_ = false;
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        Instruction *instruction = instruction_node->inst;
        int dimension;
        Value *v_array=NULL;

        switch (instruction_node->inst->Opcode)
        {
            case GEP:
                /** 1. 先取出第一个操作数的value的iVal，是当前累积量
                * 2. 算出左值的累积量，替换左值的iVal*/
                v_array=instruction->user.value.alias;
                dimension=instruction->user.value.pdata->var_pdata.iVal;
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
                    else if(dimension+1==v_array->pdata->symtab_array_pdata.dimention_figure)
                        offset=instruction->user.use_list[1].Val->pdata->var_pdata.iVal+instruction->user.use_list->Val->pdata->var_pdata.iVal;
                    else
                    {
                        offset=instruction->user.use_list[1].Val->pdata->var_pdata.iVal*(get_array_total_occupy(v_array,dimension+1)/4)+instruction->user.use_list->Val->pdata->var_pdata.iVal;
                    }
                    //将左值的iVal替换为本层增加的偏移量
                    instruction->user.value.pdata->var_pdata.iVal=offset;
                    if(after_==false)
                        instruction->user.value.pdata->var_pdata.is_offset=1;
                    else
                    {
                        instruction->user.use_list[1].Val->pdata->var_pdata.is_offset=1;     //第二个偏移量必然是var_int,这里设个offset标记一下
                        //再打个alias标记为起始的
                        if(get_prev_inst(instruction_node)->inst->user.use_list[1].Val->pdata->var_pdata.is_offset==1)
                            instruction->user.use_list[1].Val->alias=get_prev_inst(instruction_node)->inst->user.use_list[1].Val->alias;
                        else
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

        Value *v_te=NULL;
        InstNode *node=NULL;
        if(instruction_node!=NULL && instruction->Opcode==GEP && get_next_inst(instruction_node)!=NULL)
        {
            node=get_next_inst(instruction_node);
            v_te=get_next_inst(instruction_node)->inst->user.use_list[1].Val;
        }

        //这两条可以合并了
        if((instruction->Opcode==GEP && instruction->user.value.pdata->var_pdata.is_offset==1 && get_next_inst(instruction_node)->inst->user.value.pdata->var_pdata.is_offset==1))
        {
            //对第一条的左值进行处理
            if(instruction->user.value.use_list!=NULL)
            {
                Use* use=instruction->user.value.use_list;
                bool cut=true;
                while(use!=NULL)
                {
                    Value left_user=use->Parent->value;
                    //看用到的use有没有正的偏移值，如果没有就不能噶掉这条，如果有就噶
                    //没有
                    if(left_user.pdata->var_pdata.is_offset==0)
                    {
                        cut=false;
                        break;
                    }
                    use=use->Next;
                }
                if(cut)
                {
                    use=instruction->user.value.use_list;
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

                        use_set_value(use_store,v_array->alias);
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
            }
            else
            {
                InstNode *now=instruction_node;
                instruction_node= get_prev_inst(instruction_node);
                //直接噶了
                deleteIns(now);
            }
        }
            //定义不会出现这种情况，只可能在赋值时出现，赋值一次都是多条，不存在复用
        else if((instruction->Opcode==GEP && instruction->user.value.pdata->var_pdata.iVal>=0 && instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==1 && get_next_inst(instruction_node)->inst->Opcode==GEP && get_next_inst(instruction_node)->inst->user.use_list[1].Val!=NULL && get_next_inst(instruction_node)->inst->user.use_list[1].Val->pdata->var_pdata.is_offset==1))
        {
            //直接找到gep的最后一条,其他的全噶了
            while(get_next_inst(instruction_node)->inst->Opcode==GEP && get_next_inst(instruction_node)->inst->user.use_list[1].Val->pdata->var_pdata.is_offset==1)
            {
                InstNode *now=instruction_node;
                instruction_node= get_prev_inst(instruction_node);
                //直接噶了
                deleteIns(now);
                instruction_node= get_next_inst(instruction_node);
            }
            //到最后一条了
            Value *v_offset=(Value*) malloc(sizeof (Value));
            value_init_int(v_offset,instruction_node->inst->user.value.pdata->var_pdata.iVal);

            replace_lhs_operand(instruction_node->inst,instruction_node->inst->user.use_list[1].Val->alias);
            replace_rhs_operand(instruction_node->inst,v_offset);
            instruction_node->inst->user.value.pdata->var_pdata.iVal=-2;
        }
        //上一种情况，比如d[b][3],但是下一条不是GEP了，或者下条是GEP,但是无法简化
        //将iVal还原回原本维度
        else if(instruction->Opcode==GEP && instruction->user.value.pdata->var_pdata.iVal>=0 && instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==1)
        {
            //拿到上一条GEP的维度+1
            int pre_dimen= get_prev_inst(instruction_node)->inst->user.value.pdata->var_pdata.iVal;
            instruction->user.value.pdata->var_pdata.iVal=pre_dimen+1;
        }
        //一维数组,is_offset=1
        else
        {
            if(instruction->Opcode==GEP && instruction->user.value.alias->pdata->symtab_array_pdata.dimention_figure==1 && instruction->user.value.pdata->var_pdata.is_offset==1)
                instruction->user.value.pdata->var_pdata.iVal=-1;
        }
        instruction_node= get_next_inst(instruction_node);
    }
}

void travel_finish_type(struct _InstNode *instruction_node)
{
    instruction_node= get_next_inst(instruction_node);
    Instruction *instruction=NULL;
    while(instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN)
    {
        instruction=instruction_node->inst;
        switch (instruction_node->inst->Opcode)
        {
            case Alloca:
                //如果是存返回值的
                if(instruction->user.use_list==NULL)
                    instruction->user.value.VTy->ID= get_prev_inst(instruction_node)->inst->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID;
                else
                {
                    //普通alloca
                    if(instruction->user.use_list->Val->VTy->ID==Param_FLOAT)
                        instruction->user.use_list->Val->VTy->ID=Var_FLOAT;
                    else if(instruction->user.use_list->Val->VTy->ID==Param_INT)
                        instruction->user.use_list->Val->VTy->ID=Var_INT;
                    instruction->user.value.VTy->ID=instruction->user.use_list->Val->VTy->ID;
                }

                break;
            case Load:
                //说明是address
                if(instruction->user.use_list->Val->VTy->ID==Unknown)
                {
                    Value *v_array= get_prev_inst(instruction_node)->inst->user.value.alias;
                    if(v_array->VTy->ID==ArrayTy_INT || v_array->VTy->ID==ArrayTyID_ConstINT || v_array->VTy->ID==GlobalArrayInt || v_array->VTy->ID==GlobalArrayConstINT)
                        instruction->user.value.VTy->ID=Var_INT;
                    else if(v_array->VTy->ID==AddressTyID)
                    {
                        if(instruction->user.use_list->Val->pdata->var_pdata.is_offset==1)
                            instruction->user.value.VTy->ID=AddressTyID;
                        else if(v_array->pdata->symtab_array_pdata.address_type==0)
                            instruction->user.value.VTy->ID=Var_INT;
                        else
                            instruction->user.value.VTy->ID=Var_FLOAT;
                    }
                    else
                        instruction->user.value.VTy->ID=Var_FLOAT;
                }
                else if(instruction->user.use_list->Val->VTy->ID==AddressTyID)
                {
                    if(instruction->user.value.VTy->ID==Unknown && instruction->user.use_list->Val->pdata->var_pdata.is_offset==0)
                    {
                        Value *v_array= get_prev_inst(instruction_node)->inst->user.value.alias;
                        if(v_array->VTy->ID==ArrayTy_INT || v_array->VTy->ID==ArrayTyID_ConstINT || v_array->VTy->ID==GlobalArrayInt || v_array->VTy->ID==GlobalArrayConstINT)
                            instruction->user.value.VTy->ID=Var_INT;
                        else if(v_array->VTy->ID==AddressTyID)
                        {
                            if(v_array->pdata->symtab_array_pdata.address_type==0)
                                instruction->user.value.VTy->ID=Var_INT;
                            else
                                instruction->user.value.VTy->ID=Var_FLOAT;
                        }
                        else
                            instruction->user.value.VTy->ID=Var_FLOAT;
                    }
                }
                else if(instruction->user.use_list->Val->VTy->ID==Param_INT)
                {
                    instruction->user.use_list->Val->VTy->ID=Var_INT;
                    instruction->user.value.VTy->ID=instruction->user.use_list->Val->VTy->ID;
                }
                else if(instruction->user.use_list->Val->VTy->ID==Param_FLOAT)
                {
                    instruction->user.use_list->Val->VTy->ID=Var_FLOAT;
                    instruction->user.value.VTy->ID=instruction->user.use_list->Val->VTy->ID;
                }
                else if(instruction->user.use_list->Val->VTy->ID==GlobalVarInt)
                    instruction->user.value.VTy->ID=Var_INT;
                else if(instruction->user.use_list->Val->VTy->ID==GlobalVarFloat)
                    instruction->user.value.VTy->ID=Var_FLOAT;
                else
                    instruction->user.value.VTy->ID=instruction->user.use_list->Val->VTy->ID;
                break;
            case bitcast:
            case zext:
//            case EQ:
//            case NOTEQ:
//            case GREAT:
//            case GREATEQ:
//            case LESS:
//            case LESSEQ:
                instruction->user.value.VTy->ID=instruction->user.use_list->Val->VTy->ID;
                break;
            case Label:
                if(get_next_inst(instruction_node)->inst->Opcode==Label && instruction->user.value.pdata->instruction_pdata.true_goto_location==get_next_inst(instruction_node)->inst->user.value.pdata->instruction_pdata.true_goto_location)
                {
                    deleteIns(get_next_inst(instruction_node));
                }
                break;
            case br:
                //如果有无用ir就删去
                if(get_next_inst(instruction_node)->inst->Opcode!=Label)
                {
                    InstNode *insnode_tmp=get_next_inst(instruction_node);
                    while(insnode_tmp!=NULL && insnode_tmp->inst->Opcode!=Label && insnode_tmp->inst->Opcode!=FunEnd)
                    {
                        InstNode *now=insnode_tmp;
                        insnode_tmp= get_prev_inst(insnode_tmp);
                        deleteIns(now);

                        insnode_tmp= get_next_inst(insnode_tmp);
                    }
                }
                break;
        }
        instruction_node= get_next_inst(instruction_node);
    }
}

void move_give_param(struct _InstNode *instruction_node)
{
    instruction_node= get_next_inst(instruction_node);
    Instruction *instruction=NULL;

    int give_count=0;
    for(int i=0;i<1000;i++)
        params[i]=NULL;
    for(int k=0;k<1000;k++)
        one_param[k]=NULL;

    while(instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN)
    {
        instruction=instruction_node->inst;
        switch (instruction_node->inst->Opcode)
        {
            case GIVE_PARAM:
                params[give_count++]=instruction_node;
                break;
            case Call:
                get_param_list(instruction->user.use_list->Val,&give_count);
                //将one_param的ir都后移到该条call前面
                Value *v_func=instruction->user.use_list->Val;
                int num=v_func->pdata->symtab_func_pdata.param_num;
                for(int i=0;i<num;i++)
                {
                    InstNode *param=one_param[i];
                    removeIns(param);
                    ins_insert_before(param,instruction_node);
                }
                break;
        }
        instruction_node= get_next_inst(instruction_node);
    }
}

void get_param_list(Value* v_func,int* give_count)
{
    //用于返回的InsNode[]
//    InstNode *one_param[50];
//    for(int i=0;i<50;i++)
//        one_param[i]=NULL;

    //扫一下params数组
    //还是得从后往前走
    int start=0;
    for(int i=*give_count-1;i>0;i--)
    {
        if(strcmp(v_func->name,params[i]->inst->user.use_list[1].Val->name)==0)
        {
            start=i;
            break;
        }
    }
    //找出one_param参数列表
    int record_start=start;int j=0;
    while (j<v_func->pdata->symtab_func_pdata.param_num)
    {
        if(strcmp(v_func->name,params[record_start]->inst->user.use_list[1].Val->name)==0)
        {
            one_param[v_func->pdata->symtab_func_pdata.param_num-j-1]=params[record_start];
            j++;
        }
        record_start--;
    }

    if(v_func->pdata->symtab_func_pdata.param_num!=0)
    {
        j=0;
        //将参数全部置0
        while(j<v_func->pdata->symtab_func_pdata.param_num)
        {
            if(strcmp(v_func->name,params[start]->inst->user.use_list[1].Val->name)==0)
            {
                params[start--]=NULL;
                j++;
            }
        }
        j=0;
        for(int k=0;k<*give_count;k++)
        {
            if(params[k]!=NULL)
                params[j++]=params[k];
        }
        (*give_count)-=v_func->pdata->symtab_func_pdata.param_num;
    }
}

//void printf_one_llvm_ir(struct _InstNode *instruction_node,char *file_name)
//{
//    bool flag_func=false;
//
//    instruction_node= get_next_inst(instruction_node);
//    const char* ll_file= c2ll(file_name);
//
//    FILE *fptr= fopen(ll_file,"w");
//
//    Value *v_cur_array=NULL;
//
//    int p=0;
//    int give_count=0;
//
//    Instruction *instruction=instruction_node->inst;
//    printf("%d ",instruction->user.value.pdata->var_pdata.iVal);
//    printf("%d .",instruction->user.value.pdata->var_pdata.is_offset);
//    switch (instruction_node->inst->Opcode)
//    {
//        case Alloca:
//            if(instruction->user.use_list!=NULL && (instruction->user.use_list->Val->VTy->ID==ArrayTy_INT || instruction->user.use_list->Val->VTy->ID==ArrayTy_FLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayInt || instruction->user.use_list->Val->VTy->ID==GlobalArrayFloat || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstINT || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstINT))
//            {
//                printf(" %s = alloca ",instruction->user.value.name);
//                fprintf(fptr," %s = alloca ",instruction->user.value.name);
//                printf_array(instruction->user.use_list->Val,0,fptr);
//                printf(",align 16\n");
//                fprintf(fptr,",align 16\n");
//            }
//            else if(instruction->user.use_list!=NULL && instruction->user.use_list->Val->VTy->ID==AddressTyID)
//            {
//                if(instruction->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
//                {
//                    printf(" %s = alloca ",instruction->user.value.name);
//                    fprintf(fptr," %s = alloca ",instruction->user.value.name);
//                    printf_array(instruction->user.use_list->Val,1,fptr);
//                    printf("*,align 4\n");
//                    fprintf(fptr,"*,align 4\n");
//                }
//                else
//                {
//                    printf(" %s = alloca i32*,align 4\n",instruction->user.value.name);
//                    fprintf(fptr," %s = alloca i32*,align 4\n",instruction->user.value.name);
//                }
//            }
//            else
//            {
//                printf(" %s = alloca i32,align 4\n",instruction->user.value.name);
//                fprintf(fptr," %s = alloca i32,align 4\n",instruction->user.value.name);
//            }
//            break;
//        case Load:
//            if(instruction->user.use_list->Val->VTy->ID==AddressTyID && instruction->user.use_list->Val->pdata->var_pdata.is_offset==1)
//            {
//                instruction->user.value.pdata->var_pdata.is_offset=1;
//                if(instruction->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
//                {
//                    printf(" %s = load ",instruction->user.value.name);
//                    fprintf(fptr," %s = load ",instruction->user.value.name);
//                    printf_array(instruction->user.use_list->Val,1,fptr);
//                    printf("*,");
//                    fprintf(fptr,"*,");
//                    printf_array(instruction->user.use_list->Val,1,fptr);
//                    printf("** %s,align 4\n",instruction->user.use_list->Val->name);
//                    fprintf(fptr,"** %s,align 4\n",instruction->user.use_list->Val->name);
//                }
//                else
//                {
//                    printf(" %s = load i32*,i32** %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
//                    fprintf(fptr," %s = load i32*,i32** %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
//                }
//            } else
//            {
//                printf(" %s = load i32,i32* %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
//                fprintf(fptr," %s = load i32,i32* %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
//            }
//            break;
//        case Store:
//            if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==0)
//            {
//                printf(" store i32 %d,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                fprintf(fptr," store i32 %d,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//            }
//            else if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==1)
//            {
//                printf(" store i32* %d,i32** %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                fprintf(fptr," store i32* %d,i32** %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//            }
//            else if(instruction->user.use_list->Val->VTy->ID==Float || instruction->user.use_list->Val->VTy->ID==Const_FLOAT)
//            {
//                printf(" store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                fprintf(fptr," store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//            }
//            else if(instruction->user.use_list[1].Val->pdata->var_pdata.is_offset==0)
//            {
//                printf(" store i32 %s,i32* %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                fprintf(fptr," store i32 %s,i32* %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//            }
//            else
//            {
//                if(instruction->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
//                {
//                    printf(" store ");
//                    fprintf(fptr," store ");
//                    printf_array(instruction->user.use_list[1].Val,1,fptr);
//                    printf("* %s,",instruction->user.use_list->Val->name);
//                    fprintf(fptr,"* %s,",instruction->user.use_list->Val->name);
//                    printf_array(instruction->user.use_list->Val,1,fptr);
//                    printf("** %s,align 4\n",instruction->user.use_list[1].Val->name);
//                    fprintf(fptr,"** %s,align 4\n",instruction->user.use_list[1].Val->name);
//                } else{
//                    printf(" store i32* %s,i32** %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," store i32* %s,i32** %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case br:
//            printf(" br label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
//            fprintf(fptr," br label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
//            break;
//        case br_i1:
//            printf(" br i1 %s,label %%%d,label %%%d\n\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
//            fprintf(fptr," br i1 %s,label %%%d,label %%%d\n\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
//            break;
//        case br_i1_false:
//            printf(" br i1 false,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
//            fprintf(fptr," br i1 false,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
//            break;
//        case br_i1_true:
//            printf(" br i1 true,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
//            fprintf(fptr," br i1 true,label %%%d,label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
//            break;
//        case EQ:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//
//            break;
//        case LESS:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case NOTEQ:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp ne i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp ne i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp ne i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp ne i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case GREAT:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp sgt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp sgt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp sgt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp sgt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case GREATEQ:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp sge i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp sge i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp sge i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp sge i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case LESSEQ:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp sle i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp sle i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s = icmp sle i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s = icmp sle i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case FunBegin:
//            if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID==VoidTyID)
//            {
//                printf("define dso_local void @%s(",instruction->user.use_list->Val->name);
//                fprintf(fptr,"define dso_local void @%s(",instruction->user.use_list->Val->name);
//            } else
//            {
//                printf("define dso_local i32 @%s(",instruction->user.use_list->Val->name);
//                fprintf(fptr,"define dso_local i32 @%s(",instruction->user.use_list->Val->name);
//            }
//            p=instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;
//            int ii=p;
//            InstNode* node2=get_next_inst(instruction_node);
//            while (p>0)
//            {
//                if(p==instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num)
//                {
//                    if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
//                    {
//                        while(node2->inst->user.use_list==NULL || (node2->inst->user.use_list->Val!=NULL && node2->inst->user.use_list->Val->VTy->ID!=AddressTyID))
//                            node2=get_next_inst(node2);      //node2能指向真实value
//                        if(node2->inst->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
//                        {
//                            printf_array(node2->inst->user.use_list->Val,1,fptr);
//                            printf("* %%0");
//                            fprintf(fptr,"* %%0");
//                            node2= get_next_inst(node2);
//                        }
//                        else {
//                            printf("i32* %%0");
//                            fprintf(fptr,"i32* %%0");
//                        }
//                    }
//                    else
//                    {
//                        printf("i32 %%0");
//                        fprintf(fptr,"i32 %%0");
//                    }
//                }
//                else
//                {
//                    if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
//                    {
//                        while(node2->inst->user.use_list==NULL || (node2->inst->user.use_list->Val!=NULL && node2->inst->user.use_list->Val->VTy->ID!=AddressTyID))
//                            node2=get_next_inst(node2);      //node2能指向真实value
//                        if(node2->inst->user.use_list->Val->pdata->symtab_array_pdata.dimentions[0]==0)
//                        {
//                            printf(",");
//                            fprintf(fptr,",");
//                            printf_array(node2->inst->user.use_list->Val,1,fptr);
//                            printf("* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
//                            fprintf(fptr,"* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
//                            node2= get_next_inst(node2);
//                        }
//                        else
//                        {
//                            printf(",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
//                            fprintf(fptr,",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
//                        }
//                    }
//                    else
//                    {
//                        printf(",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
//                        fprintf(fptr,",i32 %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
//                    }
//                }
//                p--;
//            }
//            printf(") #0{\n");
//            fprintf(fptr,") #0{\n");
//            break;
//        case Return:
//            if(instruction->user.use_list==NULL)
//            {
//                printf(" ret void\n");
//                fprintf(fptr," ret void\n");
//            }
//            else if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                printf(" ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
//                fprintf(fptr," ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
//            }
//            else if(instruction->user.use_list->Val->VTy->ID==Float)
//            {
//                printf(" ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
//                fprintf(fptr," ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
//            }
//            else
//            {
//                printf(" ret i32 %s\n",instruction->user.use_list->Val->name);
//                fprintf(fptr," ret i32 %s\n",instruction->user.use_list->Val->name);
//            }
//
////                printf("}\n\n");
////                fprintf(fptr,"}\n\n");
//            break;
//        case Call:
//            get_param_list(instruction->user.use_list->Val,&give_count);
//            if(instruction->user.use_list->Val->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
//            {
//                //非库函数
//                if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
//                {
//                    printf(" %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
//                    fprintf(fptr," %s = call i32 @%s(",instruction->user.value.name,instruction->user.use_list->Val->name);
//                }
//                    //是库函数
//                else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
//                {
//                    printf(" %s = call i32 (",instruction->user.value.name);
//                    fprintf(fptr," %s = call i32 (",instruction->user.value.name);
//                }
//                else
//                {
//                    printf(" %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
//                    fprintf(fptr," %s = call i32 (...) @%s (",instruction->user.value.name,instruction->user.use_list->Val->name);
//                }
//            }
//            else     //voidTypeID
//            {
//                //非库函数
//                if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)!=NULL)
//                {
//                    printf(" call void @%s(",instruction->user.use_list->Val->name);
//                    fprintf(fptr," call void @%s(",instruction->user.use_list->Val->name);
//                }
//                    //是库函数
//                else if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->next->map)->pdata->symtab_func_pdata.param_num!=0)
//                {
//                    printf(" call void (");
//                    fprintf(fptr," call void (");
//                }
//                else
//                {
//                    printf(" call void (...) @%s (",instruction->user.use_list->Val->name);
//                    fprintf(fptr," void i32 (...) @%s (",instruction->user.use_list->Val->name);
//                }
//            }
//
//            //参数
//            if(symtab_lookup_withmap(this,instruction->user.use_list->Val->name,&this->value_maps->next->map)==NULL && instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num!=0)
//            {
//                for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
//                {
//                    if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
//                    {
//                        v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
//                        //就i32*
//                        if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
//                        {
//                            printf("i32*,");
//                            fprintf(fptr,"i32*,");
//                        }
//                        else
//                        {
//                            printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
//                            printf("*,");
//                            fprintf(fptr,"*,");
//                        }
//                    }
//                    else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
//                    {
//                        printf("i32*,");
//                        fprintf(fptr,"i32*,");
//                    }
//                    else
//                    {
//                        printf("i32,");
//                        fprintf(fptr,"i32,");
//                    }
//                }
//                printf("...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);
//                fprintf(fptr,"...)bitcast(i32 (...)* @%s to i32(",instruction->user.use_list->Val->name);
//
//                for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
//                {
//                    if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
//                    {
//                        v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
//                        if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
//                        {
//                            printf("i32*,");
//                            fprintf(fptr,"i32*,");
//                        }
//                        else
//                        {
//                            printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
//                            printf("*,");
//                            fprintf(fptr,"*,");
//                        }
//                    }
//                    else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
//                    {
//                        printf("i32*,");
//                        fprintf(fptr,"i32*,");
//                    }
//                    else
//                    {
//                        printf("i32,");
//                        fprintf(fptr,"i32,");
//                    }
//                }
//                printf("...)*)(");
//                fprintf(fptr,"...)*)(");
//            }
//
//            //参数
//            if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num!=0)
//            {
//                for(int i=0;i<instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num;i++)
//                {
//                    if(i==0)
//                    {
//                        if(one_param[i]->inst->user.use_list->Val->VTy->ID==Int)
//                        {
//                            printf("i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
//                            fprintf(fptr,"i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
//                        }
//                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
//                        {
//                            v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
//                            if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
//                            {
//                                printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
//                                fprintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
//                            }
//                            else
//                            {
//                                printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
//                                printf("* %s",one_param[i]->inst->user.use_list->Val->name);
//                                fprintf(fptr,"* %s",one_param[i]->inst->user.use_list->Val->name);
//                            }
//                        }
//                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
//                        {
//                            printf("i32* %s",one_param[i]->inst->user.use_list->Val->name);
//                            fprintf(fptr,"i32* %s",one_param[i]->inst->user.use_list->Val->name);
//                        }
//                        else
//                        {
//                            printf("i32 %s",one_param[i]->inst->user.use_list->Val->name);
//                            fprintf(fptr,"i32 %s",one_param[i]->inst->user.use_list->Val->name);
//                        }
//
//                    }
//                    else
//                    {
//                        if(one_param[i]->inst->user.use_list->Val->VTy->ID==Int)
//                        {
//                            printf(",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
//                            fprintf(fptr,",i32 %d",one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal);
//                        }
//                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID && one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal>0)
//                        {
//                            v_cur_array=one_param[i]->inst->user.use_list->Val->alias;
//                            if(one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal==v_cur_array->pdata->symtab_array_pdata.dimention_figure)
//                            {
//                                printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
//                                fprintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
//                            }
//                            else
//                            {
//                                printf(",");
//                                fprintf(fptr,",");
//                                printf_array(v_cur_array,one_param[i]->inst->user.use_list->Val->pdata->var_pdata.iVal,fptr);
//                                printf("* ");
//                                fprintf(fptr,"* ");
//                                printf("%s",one_param[i]->inst->user.use_list->Val->name);
//                                fprintf(fptr,"%s",one_param[i]->inst->user.use_list->Val->name);
//                            }
//                        }
//                        else if(one_param[i]->inst->user.use_list->Val->VTy->ID==AddressTyID)
//                        {
//                            printf(",i32* %s",one_param[i]->inst->user.use_list->Val->name);
//                            fprintf(fptr,",i32* %s",one_param[i]->inst->user.use_list->Val->name);
//                        }
//                        else
//                        {
//                            printf(",i32 %s",one_param[i]->inst->user.use_list->Val->name);
//                            fprintf(fptr,",i32 %s",one_param[i]->inst->user.use_list->Val->name);
//                        }
//                    }
//                }
//            }
//
//            printf(")\n");
//            fprintf(fptr,")\n");
//            break;
//        case Label:
//            printf("%d:                  ; preds = ",instruction->user.value.pdata->instruction_pdata.true_goto_location);
//            fprintf(fptr,"%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
//            if(instruction->Parent){
//                BasicBlock *parent = instruction->Parent;
//                HashSetFirst(parent->preBlocks);
//                for(BasicBlock *block = HashSetNext(parent->preBlocks); block != NULL; block = HashSetNext(parent->preBlocks)){
//                    printf("%%%d ",block->id);
//                }
//            }
//            printf("\n");
//            break;
//        case Add:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= add nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else if(instruction->user.use_list->Val->VTy->ID==Float)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= add nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= add nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= add nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= add nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case Sub:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= sub nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= sub nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= sub nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= sub nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else if(instruction->user.use_list->Val->VTy->ID==Float)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= sub nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= sub nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= sub nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= sub nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= sub nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= sub nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= sub nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= sub nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case Mul:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= mul nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= mul nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= mul nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= mul nsw i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else if(instruction->user.use_list->Val->VTy->ID==Float)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= mul nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= mul nsw i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= mul nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= mul nsw i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= mul nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= mul nsw i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= mul nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= mul nsw i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case Div:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= sdiv i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= sdiv i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= sdiv i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= sdiv i32 %d,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else if(instruction->user.use_list->Val->VTy->ID==Float)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= sdiv i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= sdiv i32 %f,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= sdiv i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= sdiv i32 %f,%f\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= sdiv i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= sdiv i32 %f,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else if(instruction->user.use_list[1].Val->VTy->ID==Float)
//                {
//                    printf(" %s= sdiv i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr," %s= sdiv i32 %s,%f\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//                else
//                {
//                    printf(" %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case Mod:
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= srem i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= srem i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s= srem i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= srem i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//                }
//            }
//            else
//            {
//                if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                {
//                    printf(" %s= srem i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr," %s= srem i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                }
//                else
//                {
//                    printf(" %s= srem i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                    fprintf(fptr," %s= srem i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                }
//            }
//            break;
//        case bitcast:
//            //第一条bitcast
//            if(instruction->user.value.pdata->var_pdata.iVal==1)
//            {
//                //v_cur_array=instruction->user.use_list->Val->alias;
//                printf(" %s=bitcast ",instruction->user.value.name);
//                fprintf(fptr," %s=bitcast ",instruction->user.value.name);
//                printf_array(instruction->user.use_list->Val->alias,0,fptr);
//                printf("* %s to i8*\n",instruction->user.use_list->Val->name);
//                fprintf(fptr,"* %s to i8*\n",instruction->user.use_list->Val->name);
//            }
//            else
//            {
//                printf(" %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
//                fprintf(fptr," %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
//                printf_array(instruction->user.use_list->Val->alias,0,fptr);
//                printf("*\n");
//                fprintf(fptr,"*\n");
//            }
//            break;
//
//        case GEP:
//            if(instruction->user.value.alias!=NULL)
//                v_cur_array=instruction->user.value.alias;
//            //printf("%d...\n",instruction->user.value.pdata->var_pdata.iVal);
//            printf(" %s=getelementptr inbounds ",instruction->user.value.name);
//            fprintf(fptr," %s=getelementptr inbounds ",instruction->user.value.name);
//            if(instruction->user.value.pdata->var_pdata.iVal<0)
//            {
//                printf_array(v_cur_array,0,fptr);
//                printf(",");
//                fprintf(fptr,",");
//                printf_array(v_cur_array,0,fptr);
//                printf("* %s,i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                fprintf(fptr,"* %s,i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//            }
//            else
//            {
//                if(instruction->user.use_list->Val->pdata->var_pdata.is_offset==0)
//                {
//                    //是对数组参数的最后一个自造的gmp
//                    if(instruction->user.value.VTy->ID==AddressTyID && instruction->user.value.pdata->var_pdata.is_offset==1)
//                    {
//                        printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal-1,fptr);
//                        printf(",");
//                        fprintf(fptr,",");
//                        printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal-1,fptr);
//                        printf("* ");
//                        fprintf(fptr,"* ");
//                        if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                        {
//                            printf("%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                            fprintf(fptr,"%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                        }
//                        else
//                        {
//                            printf("%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                            fprintf(fptr,"%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                        }
//                    }
//                        //正常的
//                    else{
//                        printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
//                        printf(",");
//                        fprintf(fptr,",");
//                        printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
//                        printf("* ");
//                        fprintf(fptr,"* ");
//                        if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                        {
//                            printf("%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                            fprintf(fptr,"%s, i32 0,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                        }
//                        else
//                        {
//                            printf("%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                            fprintf(fptr,"%s, i32 0,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                        }
//                    }
//
//                }
//                else {
//                    Value *array=instruction->user.value.alias;
//                    if(array->pdata->symtab_array_pdata.dimentions[0]==0)
//                    {
//                        printf_array(array,1,fptr);
//                        printf(",");
//                        fprintf(fptr,",");
//                        printf_array(array,1,fptr);
//                        if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                        {
//                            printf("* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                            fprintf(fptr,"* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                        }
//                        else
//                        {
//                            printf("* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                            fprintf(fptr,"* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                        }
//                    }
//                    else
//                    {
//                        if(instruction->user.use_list[1].Val->VTy->ID==Int)
//                        {
//                            printf("i32,i32* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                            fprintf(fptr,"i32,i32* %s,i32 %d",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                        }
//                        else
//                        {
//                            printf("i32,i32* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                            fprintf(fptr,"i32,i32* %s,i32 %s",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//                        }
//                    }
//                }
//            }
//
//            printf("\n");
//            fprintf(fptr,"\n");
//            //}
//            break;
//        case MEMSET:
//            flag_func=true;
//            printf(" call void @llvm.memset.p0i8.i64(i8* align 16 %s, i8 0, i64 %d, i1 false)\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//            fprintf(fptr," call void @llvm.memset.p0i8.i64(i8* align 16 %s, i8 0, i64 %d, i1 false)\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//            break;
//        case MEMCPY:
//            flag_func=true;
//            printf(" call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %s, i8* align 16 bitcast (",instruction->user.use_list->Val->name);
//            fprintf(fptr," call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %s, i8* align 16 bitcast (",instruction->user.use_list->Val->name);
//            printf_array(instruction->user.use_list[1].Val,0,fptr);
//            printf("* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
//                   get_array_total_occupy(instruction->user.use_list[1].Val,0));
//            fprintf(fptr,"* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
//                    get_array_total_occupy(instruction->user.use_list[1].Val,0));
//            break;
//        case GLOBAL_VAR:
//            if(instruction->user.use_list->Val->VTy->ID!=ArrayTy_INT && instruction->user.use_list->Val->VTy->ID!=ArrayTy_FLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayInt && instruction->user.use_list->Val->VTy->ID!=GlobalArrayFloat && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_ConstINT && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_ConstFLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayConstFLOAT && instruction->user.use_list->Val->VTy->ID!=GlobalArrayConstINT)
//            {
//                if(instruction->user.use_list->Val->VTy->ID==GlobalVarInt)
//                {
//                    printf("%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                    fprintf(fptr,"%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
//                } else
//                {
//                    printf("%s=dso_local global i32 %f,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                    fprintf(fptr,"%s=dso_local global i32 %f,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.fVal);
//                }
//            }
//            else
//            {
//                v_cur_array=instruction->user.use_list->Val;
//                if(instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstINT || instruction->user.use_list->Val->VTy->ID==ArrayTyID_ConstFLOAT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstINT || instruction->user.use_list->Val->VTy->ID==GlobalArrayConstFLOAT)
//                {
//                    printf("%s= internal constant ",instruction->user.use_list->Val->name);
//                    fprintf(fptr,"%s= internal constant ",instruction->user.use_list->Val->name);
//                    printf_global_array(instruction->user.use_list->Val,fptr);
//                    printf("align 4\n");
//                    fprintf(fptr,"align 4\n");
//                }
//                else
//                {
//                    printf("%s=dso_local global ",instruction->user.use_list->Val->name);
//                    fprintf(fptr,"%s=dso_local global ",instruction->user.use_list->Val->name);
//                    printf_global_array(instruction->user.use_list->Val,fptr);
//                    if(instruction->user.use_list->Val->pdata->symtab_array_pdata.is_init==0)
//                    {
//                        printf(" zeroinitializer, align 4\n");
//                        fprintf(fptr," zeroinitializer, align 4\n");
//                    }
//                    else
//                    {
//                        printf("align 4\n");
//                        fprintf(fptr,"align 4\n");
//                    }
//                }
//            }
//            break;
//        case GIVE_PARAM:
//            params[give_count++]=instruction_node;
//            if(instruction->user.use_list->Val->VTy->ID==Int)
//                printf("give param %d,func:%s\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
//            else
//                printf("give param %s,func:%s\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
//            break;
//        case FunEnd:
//            printf("}\n\n");
//            fprintf(fptr,"}\n\n");
//            break;
//        case zext:
//            printf(" %s = zext i1 %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
//            fprintf(fptr," %s = zext i1 %s to i32\n",instruction->user.value.name,instruction->user.use_list->Val->name);
//            break;
//        case XOR:
//            printf(" %s = xor i1 %s, true\n",instruction->user.value.name,instruction->user.use_list->Val->name);
//            fprintf(fptr," %s = xor i1 %s, true\n",instruction->user.value.name,instruction->user.use_list->Val->name);
//            break;
//        case Phi:{
//            Value *insValue = ins_get_dest(instruction);
//            HashSet *phiSet = instruction->user.value.pdata->pairSet;
//            HashSetFirst(phiSet);
//            printf(" %s( %s) = phi i32",instruction->user.value.name, instruction->user.value.alias->name);
//            fprintf(fptr," %s = phi i32",instruction->user.value.name);
//            unsigned int size=HashSetSize(phiSet);
//            int i=0;
//            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
//                BasicBlock *from = phiInfo->from;
//                Value *incomingVal = phiInfo->define;
//                if(i + 1 == size)      //最后一次
//                {
//                    if(incomingVal != NULL && isImm(incomingVal)){
//                        printf("[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
//                        fprintf(fptr,"[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
//                    }else if(incomingVal != NULL){
//                        printf("[%s , %%%d]",incomingVal->name,from->id);
//                        fprintf(fptr,"[%s , %%%d]",incomingVal->name,from->id);
//                    }else{
//                        //是NULL的话就
//                        printf("[ undef, %%%d] ",from->id);
//                        fprintf(fptr,"[ undef, %%%d] ",from->id);
//                    }
//                }
//                else
//                {
//                    if(incomingVal != NULL && isImm(incomingVal)){
//                        printf("[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
//                        fprintf(fptr,"[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
//                    }else if(incomingVal != NULL){
//                        printf("[%s , %%%d], ",incomingVal->name,from->id);
//                        fprintf(fptr,"[%s , %%%d], ",incomingVal->name,from->id);
//                    }else{
//                        printf("[ undef, %%%d], ",from->id);
//                        fprintf(fptr,"[ undef, %%%d], ",from->id);
//                    }
//                }
//                i++;
//            }
//            printf("\n");
//            //printf("a phi instruction\n");
//            break;
//        }
//        case CopyOperation:{
//            Value *dest = instruction->user.value.alias;
//            Value *src = ins_get_lhs(instruction);
//            if(isImm(src)){
//                printf(" %s(%s) = %d\n",dest->name,dest->alias->name,src->pdata->var_pdata.iVal);
//            }else{
//                printf(" %s(%s) = %s\n", dest->name,dest->alias->name,src->name);
//            }
//
//            //printf("a copy operation\n");
//            break;
//        }
//        default:
//            break;
//    }

//        Value *v,*vl,*vr;
//        v= ins_get_dest(instruction_node->inst);
//        vl= ins_get_lhs(instruction_node->inst);
//        vr= ins_get_rhs(instruction_node->inst);
//        if(v!=NULL)
//            printf("left:%s,\t",type_str[v->VTy->ID]);
//        if(vl!=NULL)
//            printf("value1:%s,\t",type_str[vl->VTy->ID]);
//        if(vr!=NULL)
//            printf("value2:%s,\t",type_str[vr->VTy->ID]);
//        printf("\n\n");
//
//        if(instruction->isCritical){
//            printf("isCritical\n\n");
//        }
//}