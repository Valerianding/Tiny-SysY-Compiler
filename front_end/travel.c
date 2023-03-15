#include "travel.h"

extern Symtab *this;
extern struct _InstNode *instruction_list;

//记录临时变量组,形如%1,%2,%3...
//t_index转字符串用的sprintf
//目前设置的能容纳3位数的临时变量
extern char t[5];
extern int t_index;
extern int return_stmt_num[10];
extern int return_index;
extern insnode_stack S_continue;
extern insnode_stack S_break;
extern insnode_stack S_return;
extern insnode_stack S_and;
extern insnode_stack S_or;
//记录有没有需要continue和break的语句
extern bool c_b_flag[2];

extern char t_num[3];
int param_map=0;
Value *v_cur_func;

void create_instruction_list(past root,Value* v_return)
{

    if (root != NULL) {
        if ((strcmp(bstr2cstr(root->nodeType, '\0'), "VarDecl") == 0 || strcmp(bstr2cstr(root->nodeType, '\0'), "ConstDecl") == 0) && is_global_map(this)==true)
            create_var_decl(root,v_return,true);
        else if((strcmp(bstr2cstr(root->nodeType, '\0'), "VarDecl") == 0 || strcmp(bstr2cstr(root->nodeType, '\0'), "ConstDecl") == 0))
            create_var_decl(root,v_return,false);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "FuncDef") == 0)
            create_func_def(root);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "BlockItemList") == 0)
            create_blockItemList(root,v_return);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "Assign_Stmt") == 0)
            create_assign_stmt(root,v_return);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "Return_Stmt") == 0)
            create_return_stmt(root,v_return);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "IF_Stmt") == 0)
            create_if_stmt(root,v_return);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "IfElse_Stmt") == 0)
            create_if_else_stmt(root,v_return);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "While_Stmt") == 0)
            create_while_stmt(root,v_return);
            //与不是stmt合并
            //else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Empty_Stmt") == 0)
            //  create_instruction_list(root->next,v_return);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Call_Func") == 0)
        {
            create_call_func(root);
            if (root->next != NULL)
                create_instruction_list(root->next,v_return);
        }

        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Continue_Stmt") == 0)
            create_continue_stmt(root,v_return);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Break_Stmt") == 0)
            create_break_stmt(root,v_return);
            //不是stmt
        else
            create_instruction_list(root->next,v_return);
    }
}

void create_continue_stmt(past root,Value* v_return)
{
    c_b_flag[0]=true;

    //遇到continue,push就完事
    InstNode *node_continue = true_location_handler(br,NULL,0);

    insnode_push(&S_continue,node_continue);

    if (root->next != NULL)
        create_instruction_list(root->next,v_return);
}

void create_break_stmt(past root,Value* v_return)
{
    c_b_flag[1]=true;

    //遇到break,push就完事
    InstNode *node_break = true_location_handler(br,NULL,0);
    //是0就代表是break
    insnode_push(&S_break,node_break);

    if (root->next != NULL)
        create_instruction_list(root->next,v_return);
}

void reduce_continue()
{
    InstNode * gap[10];
    int gap_index=0;

    do{
        InstNode * zero;
        insnode_pop(&S_continue,&zero);

        //1.zero如果是-1
        if(zero->inst->user.value.pdata->instruction_pdata.true_goto_location==-1)
        {
            //存上所有continue
            InstNode * store_continue[10];
            int store_num=0;
            InstNode *continue_point;
            insnode_top(&S_continue,&continue_point);
            while(continue_point->inst->user.value.pdata->instruction_pdata.true_goto_location==0)
            {
                store_continue[store_num++]=continue_point;
                insnode_pop(&S_continue,&continue_point);
                insnode_top(&S_continue,&continue_point);
            }
            //离开while的时候
            // 1.continue_point是while起点了,已经拿出来了
            if(continue_point->inst->user.value.pdata->instruction_pdata.true_goto_location!=-1)
            {
                for(int i=0;i<store_num;i++)
                {
                    store_continue[i]->inst->user.value.pdata->instruction_pdata.true_goto_location=continue_point->inst->user.value.pdata->instruction_pdata.true_goto_location;
                }
                insnode_pop(&S_continue,&continue_point);
            }
            else
            {
                //2.是-1
                //并将store_continue数组中的东西复制到gap数组中
                for(int i=0;i<store_num;i++)
                    gap[gap_index+i]=store_continue[i];
                gap_index+=store_num;
            }
        }
            //2.如果zero是while起点
        else
        {

            for(int i=0;i<gap_index;i++)
                gap[i]->inst->user.value.pdata->instruction_pdata.true_goto_location=zero->inst->user.value.pdata->instruction_pdata.true_goto_location;
            //将gap数组清0
            gap_index=0;
        }

    } while (!insnode_is_empty(S_continue));
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

//一次只能跳出去一层
void reduce_break()
{
    do{
        InstNode * go_location;
        insnode_pop(&S_break,&go_location);

        InstNode * break_point;
        insnode_top(&S_break,&break_point);
        //确定是break了，再pop出来
        while (break_point->inst->user.value.pdata->instruction_pdata.true_goto_location==0 && !insnode_is_empty(S_break))
        {
            insnode_pop(&S_break,&break_point);
            break_point->inst->user.value.pdata->instruction_pdata.true_goto_location=go_location->inst->user.value.pdata->instruction_pdata.true_goto_location;
            insnode_top(&S_break,&break_point);
        }
        go_location->inst->user.value.pdata->instruction_pdata.true_goto_location=break_point->inst->user.value.pdata->instruction_pdata.true_goto_location;
    } while (!insnode_is_empty(S_break));
}

struct _Value *create_call_func(past root)
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
            v->pdata->symtab_func_pdata.map_list=this->value_maps->next->next;

            v->name=(char*) malloc(sizeof(bstr2cstr(root->left->sVal,0)));
            strcpy(v->name,bstr2cstr(root->left->sVal,0));
            symtab_insert_withmap(this,&this->value_maps->next->next->map,v->name,v);
        }
    }

    //参数传递
    if(root->right!=NULL)
        create_params_stmt(root->right);

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

void create_blockItemList(past root,Value* v_return)
{
    scope_forward(this);
    create_instruction_list(root->left,v_return);
    scope_back(this);
    if(root->next!=NULL)
        create_instruction_list(root->next,v_return);
}

//cal_expr()能使得等式右边永远只有一个值
void create_assign_stmt(past root,Value* v_return) {
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
            v= handle_assign_array(root->left->right->left,v_loadp,0,-1);
        }
        else
            v=handle_assign_array(root->left->right->left, v_array,0,-1);
    }

    //右值value
    Value *v1=(Value*) malloc(sizeof (Value));
    value_init(v1);
    v1->pdata->var_pdata.map= getCurMap(this);

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
        int convert=0;
        v1 = cal_expr(root->right,&convert);
    }

        //赋值右边为普通a,b,c,d
    else if (strcmp(bstr2cstr(root->right->nodeType, '\0'), "ID") == 0)
    {
        v1= symtab_dynamic_lookup(this,bstr2cstr(root->right->sVal,'\0'));
        if(v1->VTy->ID==Const_INT || v1->VTy->ID==Const_FLOAT)
            ;
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
            v1= handle_assign_array(root->right->right->left,v_loadp,1,-1);
        }
        else
            v1= handle_assign_array(root->right->right->left, array,1,-1);
    }
        //等于函数
    else
        v1= create_call_func(root->right);

    create_store_stmt(v1,v);
    if (root->next != NULL)
        create_instruction_list(root->next,v_return);
}


void create_return_stmt(past root,Value* v_return) {
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
            int convert=0;
            v = cal_expr(root->left,&convert);
        }
            //整数
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0){
            v=(Value*) malloc(sizeof (Value));
            value_init_int(v,root->left->iVal);
        }
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "LValArray") == 0){
            //TODO 要做AddressId吗
            v = symtab_dynamic_lookup(this, bstr2cstr(root->left->left->sVal,'\0'));
            v=handle_assign_array(root->left->right->left,v,1,-1);   //TODO return的应该要具体取值，不能只要地址吧
        }
            //返回函数结果,Call_Func
        else
            v=create_call_func(root->left);

        //有多返回语句
        if(v_return!=NULL && v_return->VTy->ID!=MAIN_INT && v_return->VTy->ID!=MAIN_FLOAT)
        {
            //多返回值，就先进行一波store
            create_store_stmt(v,v_return);

            //TODO 跳转到最终结束语句
            //先生成一条，并加入return栈，和break的处理道理是一样的
            //生成一条指令，并将其压栈
            InstNode *return_node=true_location_handler(br,NULL,0);
            insnode_push(&S_return,return_node);
        }
            //只有一条返回语句
        else
        {
            if(v!=NULL)
                instruction = ins_new_unary_operator(Return, v);
            else
                instruction = ins_new_unary_operator(Return, NULL);

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
            InstNode *return_node=true_location_handler(br,NULL,0);
            insnode_push(&S_return,return_node);
        }
        else
        {
            instruction = ins_new_unary_operator(Return, NULL);
            //将这个instruction加入总list
            InstNode *node = new_inst_node(instruction);
            ins_node_add(instruction_list,node);
        }
    }
}

//只考虑了num_int
past array_all_zeros(past init_val_list)
{
    past p=init_val_list;
    while(p!=NULL && strcmp(bstr2cstr(p->nodeType, '\0'), "InitValList") != 0 && strcmp(bstr2cstr(p->nodeType, '\0'), "ConstExpList") != 0)
    {
        //不是全0就照常处理
        if(p->iVal!=0)
            return NULL;
            //其他维度全0,随便返回了一个不是NULL的
        else if(p->next==NULL)
            return init_val_list;
            //是全0,且是一维全0
        else if(strcmp(bstr2cstr(p->next->nodeType, '\0'), "InitValList") == 0 || strcmp(bstr2cstr(p->next->nodeType, '\0'), "ConstExpList") == 0)
            return array_all_zeros(p->next->left);
        p=p->next;
    }
}

//做倒数第i位的进位
//j代表进位的最低位是数组的倒数第几位
///与carry_save使用情景不同，所以有差别
///不允许暂时错误，必须精准，[5][2][2][2],在tmp_carry为[0][1][0][0]时，不能暂时[0][2][0][0],必须是[1][0][0][0]
int carry_last_i_save(Value* v_array,int carry[],int i)
{
    //将后面的清0
    for(int j=v_array->pdata->symtab_array_pdata.dimention_figure-i+1;j<v_array->pdata->symtab_array_pdata.dimention_figure;j++)
        carry[j]=0;

    //不用进位就可以直接加
    if(carry[v_array->pdata->symtab_array_pdata.dimention_figure-i] < v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-i]-1)
        carry[v_array->pdata->symtab_array_pdata.dimention_figure-i]++;
        //要进位
    else
    {
        while(carry[v_array->pdata->symtab_array_pdata.dimention_figure-i] >= v_array->pdata->symtab_array_pdata.dimentions[v_array->pdata->symtab_array_pdata.dimention_figure-i]-1)
        {
            carry[v_array->pdata->symtab_array_pdata.dimention_figure-i]=0;
            carry[v_array->pdata->symtab_array_pdata.dimention_figure-i-1]++;
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
    int move=v_array->pdata->symtab_array_pdata.dimentions[level];
    while(p!=NULL)
    {
        if(strcmp(bstr2cstr(p->nodeType, '\0'), "num_int") == 0)
        {
            v_array->pdata->symtab_array_pdata.array[i++]=p->iVal;
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

void handle_global_array(Value* v_array,bool is_global,past vars)
{
    //先全部默认为0
    int ele_num= get_array_total_occupy(v_array,0);
    ele_num/=4;
    for(int i=0;i<ele_num;i++)
    {
        v_array->pdata->symtab_array_pdata.array[i]=0;
    }

    if(v_array->VTy->ID==ArrayTyID_Init || v_array->VTy->ID==ArrayTyID_Const)
    {
        //就是全局的
        if(is_global)
        {
            past p=vars->right->left;

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
                    v_array->pdata->symtab_array_pdata.array[i++]=p->iVal;
                    p=p->next;
                }
            }
        }
    }

    Instruction *instruction= ins_new_binary_operator(GLOBAL_VAR,v_array,NULL);

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

    int tmp_carry[v_array->pdata->symtab_array_pdata.dimention_figure-1];         //有时会拷贝carry数组到tmp_carry作为进入handle递归前起点处的暂存，用于判断有前面有num_int时，InitValList结束后的下一个起始位置

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
                        ins_gmp= ins_new_binary_operator(GMP,begin_offset_value,v_offset);
                    }
                    else
                    {
                        Value *v_prev=&get_last_inst(instruction_list)->inst->user.value;
                        ins_gmp= ins_new_binary_operator(GMP,v_prev,v_offset);
                    }

                    Value *v_gmp= ins_get_value_with_name(ins_gmp);
                    //v_gmp->VTy->ID=AddressTyID;
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

            //!如果是0就不处理
            //p->iVal是0,直接将位置+1
            if(num!=NULL && num->pdata->var_pdata.iVal==0)
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

                    Instruction *gmp_last = ins_new_binary_operator(GMP,record[v_array->pdata->symtab_array_pdata.dimention_figure-2],v_offset);
                    //是最后一层吧
                    Value *v_gmp= ins_get_value_with_name(gmp_last);
                    //v_gmp->VTy->ID=AddressTyID;
                    v_gmp->pdata->var_pdata.iVal=v_array->pdata->symtab_array_pdata.dimention_figure-1;
                    v_gmp->alias=v_array;
                    //将这个instruction加入总list
                    InstNode *node_gmp_last = new_inst_node(gmp_last);
                    ins_node_add(instruction_list,node_gmp_last);

                    //其他除num_int的情况
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
                            num= handle_assign_array(p->right->left,v_loadp,1,-1);
                        }
                        else
                            num=handle_assign_array(p->right->left,v_arr,1,-1);

                    }
                    else if(strcmp(bstr2cstr(p->nodeType, '\0'), "ID") == 0)
                        num= create_load_stmt(bstr2cstr(p->left->sVal, '\0'));
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
                            ins= ins_new_binary_operator(GMP,v_start,v_carry_index);
                        }
                        else
                        {
                            Value *v_prev=&get_last_inst(instruction_list)->inst->user.value;
                            ins= ins_new_binary_operator(GMP,v_prev,v_carry_index);
                            //更新record
                            //record[i]=v_prev;
                        }
                        Value *v_gmp= ins_get_value_with_name(ins);
                        //v_gmp->VTy->ID=AddressTyID;
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
                            num= handle_assign_array(p->right->left,v_loadp,1,-1);
                        }
                        else
                            num=handle_assign_array(p->right->left,v_arr,1,-1);
                    }
                    else if(strcmp(bstr2cstr(p->nodeType, '\0'), "ID") == 0)
                        num= create_load_stmt(bstr2cstr(p->left->sVal, '\0'));
                    create_store_stmt(num,record[v_array->pdata->symtab_array_pdata.dimention_figure-1]);

                    //再做一次进位，到下一次的初始地址
                    carry_save(v_array,carry);
                }
            }
        }
        init_val_list=init_val_list->next;
    }
}

Value *handle_assign_array(past root,Value *v_array,int flag,int dimension)
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
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "expr") == 0)
        {
            int convert=0;
            v_num= cal_expr(root,&convert);
        }
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "ID") == 0)
            v_num= create_load_stmt(bstr2cstr(root->sVal, '\0'));


        if(i==0)
        {
            if(v_array->VTy->ID!=AddressTyID)
                gmp= ins_new_binary_operator(GMP,v_array->alias,v_num);
            else
                gmp=ins_new_binary_operator(GMP,v_array,v_num);
        }
        else
        {
            gmp= ins_new_binary_operator(GMP,v_last,v_num);
        }


        Value *v1= ins_get_value_with_name(gmp);
        //v1->VTy->ID=AddressTyID;
        v1->pdata->var_pdata.iVal=i;
        if(v_array->VTy->ID!=AddressTyID)
            v1->alias=v_array;
        else
            v1->alias=v_array->alias;

        v_last=v1;

        //将这个instruction加入总list
        InstNode *node_gmp = new_inst_node(gmp);
        ins_node_add(instruction_list,node_gmp);

        root=root->next;
    }
    //load
    if(flag==1)
    {
        Instruction *ins_load= ins_new_unary_operator(Load,v_last);
        Value *v_load= ins_get_value_with_name(ins_load);
        v_last=v_load;

        //将这个instruction加入总list
        InstNode *node_load = new_inst_node(ins_load);
        ins_node_add(instruction_list,node_load);
    }

    return v_last;
}

//还未考虑数组
void create_var_decl(past root,Value* v_return,bool is_global) {

    //到达第一个var的节点
    past vars = root->right->left;

    while (vars!=NULL)
    {
        //纯定义语句不需要处理了
        //有初值
        if (strcmp(bstr2cstr(vars->nodeType, '\0'), "VarDef_init") == 0 && !is_global)
        {
            //左值
            Value *v=symtab_dynamic_lookup(this, bstr2cstr(vars->left->sVal, '\0'))->alias;

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
                v1= create_load_stmt(bstr2cstr(vars->right->sVal,'\0'));
            else if (strcmp(bstr2cstr(vars->right->nodeType, '\0'), "expr") == 0)
            {
                int convert=0;
                v1 = cal_expr(vars->right,&convert);
            }
            else if(strcmp(bstr2cstr(vars->right->nodeType, '\0'), "Call_Func") == 0)
                v1= create_call_func(vars->right);

            //将值store下来
            create_store_stmt(v1,v);
        }
            //有初值数组
        else if((strcmp(bstr2cstr(vars->nodeType, '\0'), "VarDef_array_init") == 0) || (strcmp(bstr2cstr(vars->nodeType,'\0'),"ConstDef_array_init")==0))
        {
            if(is_global==false)
            {
                past ident_array=vars->left;         //到IdentArray结点
                Value *v_array = symtab_dynamic_lookup(this,bstr2cstr(ident_array->left->sVal,'\0'));
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

                //一维数组,拷贝式赋初值,memcpy
                //call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %5, i8* align 16 bitcast ([5 x i32]* @__const.if_if_Else.c to i8*), i64 20, i1 false)
                if(v_array->pdata->symtab_array_pdata.dimention_figure==1)
                {
                    //以全局，memcpy的方式处理
                    handle_global_array(v_array,is_global,vars);

                    Instruction *instruction= ins_new_binary_operator(MEMCPY,v1,v_array);
                    //将这个instruction加入总list
                    InstNode *node = new_inst_node(instruction);
                    ins_node_add(instruction_list,node);
                }
                    //
                else
                {
                    //TODO memset是否还需要我处理?
                    int mem= get_array_total_occupy(v_array,0);
                    Value *v_mem=(Value*) malloc(sizeof (Value));
                    value_init_int(v_mem,mem);
                    Instruction *mem_set= ins_new_binary_operator(MEMSET,v1,v_mem);
                    //将这个instruction加入总list
                    InstNode *node_mem_set = new_inst_node(mem_set);
                    ins_node_add(instruction_list,node_mem_set);

                    if(vars->right->left!=NULL)
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
            }
                //TODO 是全局有初始化数组
            else
            {
                past ident_array=vars->left;         //到IdentArray结点
                Value *v_array = symtab_dynamic_lookup(this,bstr2cstr(ident_array->left->sVal,'\0'));
                handle_global_array(v_array,true,vars);
            }
        }
        else if(strcmp(bstr2cstr(vars->nodeType, '\0'), "IdentArray") == 0 && is_global)
        {
            past ident_array=vars->left;         //到IdentArray结点
            Value *v_array = symtab_dynamic_lookup(this,bstr2cstr(ident_array->sVal,'\0'));
            handle_global_array(v_array,true,vars);
        }

        vars=vars->next;
    }


    if(root->next!=NULL)
        create_instruction_list(root->next,v_return);
}

InstNode *true_location_handler(int type,Value *v_real,int true_goto_location)
{
    Instruction *instruction;
    if(type==br)
        instruction= ins_new_unary_operator(br,NULL);
    else if(type==br_i1)
        instruction= ins_new_unary_operator(br_i1,v_real);
    else if(type==br_i1_true)
        instruction= ins_new_unary_operator(br_i1_true,NULL);
    else if(type==br_i1_false)
        instruction= ins_new_unary_operator(br_i1_false,NULL);
    else
        instruction= ins_new_unary_operator(Label,NULL);
    Value *T1= ins_get_value(instruction);
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
    if(type==br_i1)
        instruction= ins_new_unary_operator(br_i1,v_real);
    Value *f1= ins_get_value(instruction);
    f1->pdata->instruction_pdata.false_goto_location=false_goto_location;
    f1->pdata->instruction_pdata.true_goto_location=-2;

    InstNode *node_false = new_inst_node(instruction);
    ins_node_add(instruction_list,node_false);
    return node_false;
}

//root是&&或||,flag为是不是root为||，root->left是&&
//结果为0是恒为假，为1是恒为真；为-1则表示没有经过短路判断
//TODO 如果是b==1 && 0 && a==1这种常数在中间的情况，目前没有做短路，有llvm方式一样
int handle_and_or(past root,bool flag)
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
            result=handle_and_or(root->left,true);
        }
        else
            result=handle_and_or(root->left,false);

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
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "Call_Func") == 0)
        {
            Value *v_load= create_call_func(root->left);
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
            //num_int
        else
        {
            //直接失败短路
            if(root->left->iVal==0 && strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0)
                return 0;

            //直接成功为1
            if(root->left->iVal!=0 && strcmp(bstr2cstr(root->sVal, '\0'), "||") == 0)
                return 1;
        }

        if(v1!=NULL && strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0)
        {
            InstNode *ins1 = true_location_handler(br_i1,v1,t_index++);
            //入栈
            insnode_push(&S_and,ins1);
        }
            //  ||
        else if(v1!=NULL)
        {
            InstNode *ins1= false_location_handler(br_i1,v1,t_index++);
            insnode_push(&S_or,ins1);
        }
    }

    //子层特殊则本层处理一下,左边递归完走到这里，处理下留下来的right，即1 && 2 || 3的2应用false_location_handler
    //TODO 删了一个&& v1!=NULL,不知道当时为什么加这条，删了可能有不良影响
    if(flag_notice)
    {
        if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "logic_expr") == 0)
            v1= cal_logic_expr(root->left->right);
            //ID
        else
        {
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
        InstNode *ins1= false_location_handler(br_i1,v1,t_index++);
        insnode_push(&S_or,ins1);

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
            handle_and_or(root->right,false);
        else
        {
            if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "logic_expr") == 0)
                v2= cal_logic_expr(root->right);
            else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "ID") == 0)
            {
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
            else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "Call_Func") == 0)
            {
                Value *v_load= create_call_func(root->right);
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

            if(strcmp(bstr2cstr(root->sVal, '\0'), "&&") == 0 && v2!=NULL)
            {
                InstNode *ins1 = true_location_handler(br_i1,v2,t_index++);
                //入栈
                insnode_push(&S_and,ins1);
            }
                //  ||
            else if(v2!=NULL)
            {
                InstNode *ins1= true_location_handler(br_i1,v2,t_index++);
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
    while (!insnode_is_empty(S_and))
    {
        InstNode *node=NULL;
        insnode_pop(&S_and,&node);
        node->inst->user.value.pdata->instruction_pdata.false_goto_location=false_index;
    }
}

void reduce_or(int true_index,int false_index)
{
    while(!insnode_is_empty(S_or))
    {
        InstNode *node=NULL;
        insnode_pop(&S_or,&node);
        if(node->inst->user.value.pdata->instruction_pdata.true_goto_location==-2)
            node->inst->user.value.pdata->instruction_pdata.true_goto_location=true_index;
        else if(node->inst->user.value.pdata->instruction_pdata.false_goto_location==-2)
            node->inst->user.value.pdata->instruction_pdata.false_goto_location=false_index;
    }
}

void create_if_stmt(past root,Value* v_return) {
    //br i1 label__,label__
    Value *v_real=NULL;
    int result=-1;          //如果是&&,||的话，保留调用结果
    //如果是0，直接跳过
    //永真则v_real就是NULL
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0)
    {
        if(root->left->iVal==0)
        {
            if(root->next!=NULL)
                create_instruction_list(root->next,v_return);
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
            v_load->VTy->ID=AddressTyID;
            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v_load= handle_assign_array(root->left->right->left,v_loadp,1,-1);
        }
        else
            v_load= handle_assign_array(root->left->right->left,v_array,1,-1);
        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
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
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
    {
        Value *v_load=NULL;
        if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
            v_load= create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
        else
            v_load= cal_expr(root->left,&convert);

        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
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
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        result=handle_and_or(root->left,false);
        //一定为假，不用走了
        if(result==0)
        {
            if(root->next!=NULL)
                create_instruction_list(root->next,v_return);
            return;
        }
        if(result==1)
            //后面都不用t_index，让t_index回归正轨
            t_index--;
    }


    InstNode *node1=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result!=1)
    {
        if(!(root->left->sVal!=NULL && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0)))
        {
            if(convert==0)
                //正确跳转
                node1= true_location_handler(br_i1,v_real, t_index++);
            else
                node1= false_location_handler(br_i1,v_real,t_index++);
        }


        //如果上一条不是标号则写编号，走了handle_and_or之后可能会多一条标号
        if(get_last_inst(instruction_list)->inst->Opcode!=Label)
            //生成正确的那个标识IR,示例中8:
            true_location_handler(Label,NULL,t_index-1);
    }

    int trur_point=t_index-1;

    //先走完if为真的所有语句，走完后就可确定否定跳转的位置
    create_instruction_list(root->right,v_return);

    //填充&&,||的情况
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && result!=1 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        reduce_and(t_index);
        reduce_or(trur_point,t_index);
        t_index++;
    }

    if(result!=1 && ((root->left->sVal==NULL) || strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") != 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") != 0))
    {
        if(convert==0)
            node1->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index++;
        else
            node1->inst->user.value.pdata->instruction_pdata.true_goto_location=t_index++;
    }

    //无break或continue的话就补，有就不补了
    if(get_last_inst(instruction_list)->inst->Opcode!=br && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result!=1)
    {
        //再补一条br label,使每个基本块结束都是跳转,跳转到end,示例中的br label 9
        true_location_handler(br,NULL,t_index-1);
    }

    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result!=1)
        //再补一条标号,可以理解为是false的跳转，也可以理解为是end的跳转
        true_location_handler(Label,NULL,t_index-1);

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return);
}

void create_if_else_stmt(past root,Value* v_return) {
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
            v_load->VTy->ID=AddressTyID;
            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v_load= handle_assign_array(root->left->right->left,v_loadp,1,-1);
        }
        else
            v_load= handle_assign_array(root->left->right->left,v_array,1,-1);
        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
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
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
    {
        Value *v_load=NULL;
        if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
            v_load= create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
        else
            v_load= cal_expr(root->left,&convert);

        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
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
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        result=handle_and_or(root->left,false);
    }

    InstNode *node1=NULL;
    //正确跳转
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1)
    {
        if(strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") != 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") != 0) {
            if(convert==0)
                //正确跳转
                node1= true_location_handler(br_i1,v_real, t_index++);
            else
                node1= false_location_handler(br_i1,v_real,t_index++);
        }

        if(get_last_inst(instruction_list)->inst->Opcode!=Label)
            //生成正确的那个标识IR
            true_location_handler(Label,NULL,t_index-1);
    }

    int trur_point=t_index-1;       //and_or使用

    if((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1) || ((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0) && root->left->iVal!=0) || result==1)
    {
        //走完if为真的语句，即可知道else从哪开始
        //走完if为真，目前最后一条是一个编号，示例的9:
        create_instruction_list(root->right->left,v_return);          //if为真走的语句

        //填充&&,||的情况
        if(strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0)
        {
            reduce_and(t_index);
            reduce_or(trur_point,t_index);
            t_index++;
        }
    }

    InstNode *node2=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1)
    {
        if( strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") != 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") != 0)
        {
            if(convert==0)
                //if为假走else的话应该走这里,示例中将前面else的label 10补齐
                node1->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index++;
            else
                node1->inst->user.value.pdata->instruction_pdata.true_goto_location=t_index++;
        }

        //可能是return,return的话后面就没有了
        if(get_last_inst(instruction_list)->inst->Opcode!=br)
            //真的走完之后接一句跳转,跳过else的部分,示例中br label 13的，在当前是br label __
            node2= true_location_handler(br,NULL,0);

        //再补一条标号,即示例中10:
        true_location_handler(Label,NULL,t_index-1);
    }

    if((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1) || ((strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0) && root->left->iVal==0) || result==0)
    {
        //走完else部分的语句，即可知道从if为真中出来的下一条应该goto到什么位置
        create_instruction_list(root->right->right,v_return);         //if为假走的语句
    }

    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1) {
        //将之前的br label__补齐，示例中补齐为br label 13
        if (node2 != NULL)
            node2->inst->user.value.pdata->instruction_pdata.true_goto_location = t_index++;
        else
            t_index++;                 //t_index++因为后面都用的t_index-1
    }

    if(get_last_inst(instruction_list)->inst->Opcode!=br && strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1)
    {
        //补一条br label，示例的br label %13，这个和后面那条都是跳到end
        true_location_handler(br,NULL,t_index-1);
    }

    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1)
        //再补一条标号，示例的13:
        true_location_handler(Label,NULL,t_index-1);

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return);
}

void create_while_stmt(past root,Value* v_return)
{
    Value *v_real=NULL;
    int result=-1;

    InstNode *ins_false=NULL;
    //如果是0，直接跳过
    //永真则v_real就是NULL
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0)
    {
        if(root->left->iVal==0)
        {
            if(root->next!=NULL)
                create_instruction_list(root->next,v_return);
            return;
        }
    }

    int convert=0;

    //加进stack，continue用
    Instruction *ins_tmp= ins_new_unary_operator(tmp,NULL);
    Value *t_in= ins_get_value(ins_tmp);
    t_in->pdata->instruction_pdata.true_goto_location=t_index;
    insnode_push(&S_continue, new_inst_node(ins_tmp));

    int while_recall=t_index;

    //是常数则不用生成这些，可以少一组标号标识
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0)
    {
        //生成一条br label跳转和标号
        true_location_handler(br,NULL,t_index++);
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
            v_load->VTy->ID=AddressTyID;
            v_loadp->pdata->symtab_array_pdata.dimention_figure=v_array->pdata->symtab_array_pdata.dimention_figure;
            v_load= handle_assign_array(root->left->right->left,v_loadp,1,-1);
        }
        else
            v_load= handle_assign_array(root->left->right->left,v_array,1,-1);
        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
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
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0 || strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
    {
        Value *v_load=NULL;
        if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
            v_load= create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
        else
            v_load= cal_expr(root->left,&convert);

        if(get_last_inst(instruction_list)->inst->Opcode!=XOR)
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
        else
            v_real=v_load;
    }
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        result=handle_and_or(root->left,false);

        if(result==0)
        {
            //生成一条br_i1_false
            ins_false=true_location_handler(br_i1_false,NULL,t_index++);
        }
    }

    InstNode *node_first_bri1=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") != 0  && strcmp(bstr2cstr(root->left->sVal, '\0'), "||") != 0)
    {
        if(convert==0)
            node_first_bri1= true_location_handler(br_i1,v_real,t_index++);
        else
            node_first_bri1= false_location_handler(br_i1,v_real,t_index++);
    }
        //是常数则直接跳转
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0)
        node_first_bri1= true_location_handler(br,NULL,t_index++);

    //标号
    //为真的话，条件式就没有IR，可以直接沿用之前的IR
    if(result!=1 && get_last_inst(instruction_list)->inst->Opcode!=Label)
        true_location_handler(Label,NULL,t_index-1);

    int trur_point=t_index-1;

    create_instruction_list(root->right,v_return);

    //填充&&,||的情况
    if(result==-1 && strcmp(bstr2cstr(root->left->nodeType, '\0'), "logic_expr") == 0 && (strcmp(bstr2cstr(root->left->sVal, '\0'), "&&") == 0 || strcmp(bstr2cstr(root->left->sVal, '\0'), "||") == 0))
    {
        reduce_and(t_index);
        reduce_or(trur_point,t_index);
        if(result==-1)
            t_index++;
    }

    if(get_last_inst(instruction_list)->inst->Opcode!=br)
    {
        //生成循环，br label跳转到first_label处
        true_location_handler(br,NULL,while_recall);
    }

    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") != 0 && result==-1)
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

    //while结束，加进stack,break用
    Instruction *ins_break_tmp= ins_new_unary_operator(tmp,NULL);
    Value *t_out= ins_get_value(ins_break_tmp);
    t_out->pdata->instruction_pdata.true_goto_location=t_index-1;

    Instruction *ins_1_tmp= ins_new_unary_operator(tmp,NULL);
    Value *t_1= ins_get_value(ins_1_tmp);
    t_1->pdata->instruction_pdata.true_goto_location=-1;

    insnode_push(&S_continue, new_inst_node(ins_1_tmp));
    insnode_push(&S_break, new_inst_node(ins_break_tmp));

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return);
}

void create_func_def(past root) {
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
            Instruction *instruction= ins_new_unary_operator(Alloca, NULL);
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
        past params=root->left->next->left->left;

        //先生成参数的alloca
        while(params!=NULL)
        {
            //参数Value
            Value *param= symtab_lookup_withmap(this,bstr2cstr(params->left->next->sVal, '\0'), &v->pdata->symtab_func_pdata.map_list->map);

            Instruction *instruction= ins_new_unary_operator(Alloca,param);
            Value *v_param= ins_get_value_with_name(instruction);
            param->alias=v_param;v_param->alias=param;
            if(param->VTy->ID==AddressTyID)
            {
                param->VTy->ID=AddressTyID;
                v_param->VTy->ID=AddressTyID;
            }
            //将这个instruction加入总list
            InstNode *node = new_inst_node(instruction);
            ins_node_add(instruction_list,node);

            params=params->next;
        }

        //普通变量的allcoa
        declare_all_alloca(v->pdata->symtab_func_pdata.map_list,true);

        //进行参数的store
        params=root->left->next->left->left;
        while(params!=NULL)
        {
            //参数Value
            Value *param= symtab_lookup_withmap(this,bstr2cstr(params->left->next->sVal, '\0'), &v->pdata->symtab_func_pdata.map_list->map)->alias;

            Value *v_num_param=create_param_value();
            if(param->VTy->ID==AddressTyID)
                v_num_param->VTy->ID=AddressTyID;
            create_store_stmt(v_num_param,param);

            params=params->next;
        }
        param_map=0;
    }
    else
        //普通变量的allcoa
        //是函数进来的,先声明一顿alloca
        declare_all_alloca(v->pdata->symtab_func_pdata.map_list,true);


    if(strcmp(v->name,"main")==0)
    {
        //main函数还立即跟着一个store 0 %1
        Value *v0=(Value*) malloc(sizeof (Value));
        value_init_int(v0,0);
        create_store_stmt(v0,v_return);
    }

    //进入func的blocklist
    create_instruction_list(root->right,v_return);

    //如果没有return，即void返回
    if(return_stmt_num[return_index]==0)
    {
        Instruction *ins_ret_void= ins_new_unary_operator(Return,NULL);
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
        Instruction *ins_final= ins_new_unary_operator(Return,final_ret);
        //将这个instruction加入总list
        InstNode *node = new_inst_node(ins_final);
        ins_node_add(instruction_list,node);
    }


    //清空index
    t_index=0;

    //解决填充continue和break
    if(c_b_flag[1]==true)
        reduce_break();
    if(c_b_flag[0]==true)
        reduce_continue();
    if(return_stmt_num[return_index]>1 || (return_stmt_num[return_index]==1 && v->pdata->symtab_func_pdata.return_type.ID==VoidTyID))
        reduce_return();
    insnode_stack_new(&S_break);
    insnode_stack_new(&S_continue);

    return_index++;

    if (root->next != NULL)
        create_instruction_list(root->next,v_return);
}

//Value *get_value_by_type(past x1)
//{
//    Value *v1=NULL;
//    if (strcmp(bstr2cstr(x1->nodeType, '\0'), "num_int") == 0)
//    {
//        v1=(Value*) malloc(sizeof (Value));
//        value_init_int(v1, x1->iVal);
//    }
//    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "ID") == 0)
//    {
//        //如果x1->sVal是a,b,c...需要load
//        v1=symtab_dynamic_lookup(this, bstr2cstr(x1->sVal, '\0'));
//        if(v1!=NULL)
//        {
//            if(v1->VTy->ID==Const_INT)
//            {
//                v1=(Value*) malloc(sizeof (Value));
//                value_init_int(v1, x1->iVal);
//            }
//            else
//                v1=create_load_stmt(bstr2cstr(x1->sVal, '\0'));
//        }
//        else{
//            //TODO 会造成地址不同
//            v1=(Value*) malloc(sizeof (Value));
//            value_init(v1);
//            v1->name = (char *) malloc(sizeof(bstr2cstr(x1->sVal, '\0')));
//            strcpy(v1->name, bstr2cstr(x1->sVal, '\0'));
//            v1->pdata->var_pdata.map= getCurMap(this);
//            v1->VTy->ID = Var_INT;
//        }
//    }
//    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "LValArray") == 0)
//    {
//        Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(x1->left->sVal, '\0'));
//        v1= handle_assign_array(x1->right->left,v_array);
//    }
//    return v1;
//}

//处理!
void travel_expr(past str[100],int length)
{
    bool need_xor=false;
    for(int i=length-1;i>=0;i--)
    {
        if(str[i]->iVal=='!')
        {
            if(need_xor)
                str[i]->sVal= bfromcstr("special_!");
        }
        if(str[i]->iVal=='+' || str[i]->iVal=='-')
            need_xor=true;
    }
}

//先后序遍历树，得到后缀表达式并存入数组，再通过后缀表达式得到表达式的值
//目前做的有点复杂，其实应该可以直接后序遍历树就ok的，但目前感觉这样做也蛮清晰的，有时间再改吧
struct _Value *cal_expr(past expr,int* convert) {
    //最后从栈中弹出的
    Value *final_result = (Value*) malloc(sizeof (Value));
    value_init(final_result);

    //记录后缀表达式
    past str[100];
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

    travel_expr(str, i);        //TODO:check

    //计算后缀表达式
    value_stack PS2;
    init_value_stack(&PS2);
    //存储数组需要past
    past_stack PS3;
    init_past_stack(&PS3);
    Value* x1;Value *x2;
    past *pp = str;
    bool have_icmp=false;
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
            else if(strcmp(bstr2cstr((*pp)->nodeType, '\0'), "LValArray") == 0)
            {
                v1= symtab_dynamic_lookup(this,bstr2cstr((*pp)->left->sVal, '\0'));
                push(&PS3,(*pp)->right->left);
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
                    case '!':                                     //TODO !1这种还没处理，目前无法直接判断为假,如果要处理，就定个convert值为一定真和一定假，返回后分别加入if语句那些的num_int的判断
                        //(*convert)=!(*convert);
                        break;
                }
                push_value(&PS2, v3);
            }
            else
            {
                Instruction *instruction=NULL;

                //先看v1
                Value *v1=NULL;
                if(x1->VTy->ID==Int || x1->VTy->ID==Float)
                    v1=x1;
                else if(!begin_tmp(x1->name) && (x1->VTy->ID==Var_INT || x1->VTy->ID==Var_initINT || x1->VTy->ID==Param_INT))
                {
                    if(!begin_global(x1->name))
                        v1= create_load_stmt(x1->name);
                    else
                        v1= create_load_stmt(no_global_name(x1->name));
                }
                else if(!begin_tmp(x1->name) && (x1->VTy->ID==ArrayTyID || x1->VTy->ID==ArrayTyID_Init || x1->VTy->ID==ArrayTyID_Const))
                {
                    past root;
                    pop(&PS3,&root);
                    v1= handle_assign_array(root,x1,1,-1);
                }
                else if(x1->VTy->ID==Const_INT)
                {
                    v1=(Value*) malloc(sizeof (Value));
                    value_init_int(v1,x1->pdata->var_pdata.iVal);
                }
                else
                    v1=x1;


                //先看v1
                Value *v2=NULL;
                if(x2->VTy->ID==Int || x2->VTy->ID==Float)
                    v2=x2;
                else if(!begin_tmp(x2->name) && (x2->VTy->ID==Var_INT || x2->VTy->ID==Var_initINT || x2->VTy->ID==Param_INT))
                {
                    if(!begin_global(x2->name))
                        v2= create_load_stmt(x2->name);
                    else
                        v2= create_load_stmt(no_global_name(x2->name));
                }
                else if(!begin_tmp(x2->name) && (x2->VTy->ID==ArrayTyID || x2->VTy->ID==ArrayTyID_Init || x2->VTy->ID==ArrayTyID_Const))
                {
                    past root;
                    pop(&PS3,&root);
                    v2= handle_assign_array(root,x2,1,-1);
                }
                else if(x2->VTy->ID==Const_INT)
                {
                    v2=(Value*) malloc(sizeof (Value));
                    value_init_int(v2,x2->pdata->var_pdata.iVal);
                }
                else
                    v2=x2;

                //TODO 除了把'+'禁掉，还有无更好的处理
                if((*pp)->iVal!='!' && (*pp)->iVal!='+' && get_last_inst(instruction_list)->inst->Opcode==XOR)
                {
                    //生成一条zext
                    Instruction *ins_zext= ins_new_unary_operator(zext,&get_last_inst(instruction_list)->inst->user.value);
                    Value *v_zext= ins_get_value_with_name(ins_zext);

                    InstNode *node = new_inst_node(ins_zext);
                    ins_node_add(instruction_list,node);

                    v2=v_zext;
                }

                switch ((*pp)->iVal) {
                    case '+':
                        if(!((v1->VTy->ID==Int && v2->pdata->var_pdata.iVal==0) || (v2->VTy->ID==Int && v2->pdata->var_pdata.iVal==0)))
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
                        instruction = ins_new_binary_operator(Module, v1, v2);
                        break;
                    case '!':
                        if(strcmp(bstr2cstr((*pp)->sVal, '\0'), "special_!") == 0)
                        {
                            Value *v_real=NULL;
                            if(!have_icmp)
                            {
                                //与0的icmp
                                Value *v_zero=(Value*) malloc(sizeof (Value));
                                value_init_int(v_zero,0);
                                Instruction *ins_icmp= ins_new_binary_operator(NOTEQ,v2,v_zero);
                                //v_real
                                v_real= ins_get_value_with_name(ins_icmp);
                                //将这个instruction加入总list
                                InstNode *node_icmp = new_inst_node(ins_icmp);
                                ins_node_add(instruction_list,node_icmp);
                            }

                            //xor
                            if(!have_icmp)
                                instruction= ins_new_unary_operator(XOR,v_real);
                            else
                                instruction= ins_new_unary_operator(XOR,v2);

                            have_icmp=true;
                        }
                            //以!开头
                        else
                            (*convert)=!(*convert);

                        break;
                }
                if(instruction!=NULL)
                {
                    //临时变量左值,v_tmp的pdata是没有实际内容的
                    Value *v_tmp= ins_get_value_with_name(instruction);
                    v_tmp->VTy->ID = Var_INT;

                    //将这个instruction加入总list
                    InstNode *node = new_inst_node(instruction);
                    ins_node_add(instruction_list,node);
                    push_value(&PS2, v_tmp);
                }
                else if((*pp)->iVal=='!')
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
    final_result->pdata->var_pdata.map= getCurMap(this);

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
        int convert=0;
        v1= cal_expr(logic_expr->left,&convert);
    }
    else if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "ID") == 0)
        v1= create_load_stmt(bstr2cstr(logic_expr->left->sVal,'\0'));
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
            v1= handle_assign_array(logic_expr->left->right->left,v_load,1,-1);
        }
        else
            v1= handle_assign_array(logic_expr->left->right->left,v_array,1,-1);
    }
    else
    {
        //TODO num_int,目前还都没有考虑float
        //num_int
        v1=(Value*) malloc(sizeof (Value));
        value_init_int(v1,logic_expr->left->iVal);
    }

    //value2
    if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "expr") == 0)
    {
        int convert=0;
        v2= cal_expr(logic_expr->right,&convert);
    }
    else if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "ID") == 0)
        v2= create_load_stmt(bstr2cstr(logic_expr->right->sVal,'\0'));
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
            v2= handle_assign_array(logic_expr->right->right->left,v_load,1,-1);
        }
        else
            v2= handle_assign_array(logic_expr->right->right->left,v_array,1,-1);
    }
    else
    {
        //num_int
        v2=(Value*) malloc(sizeof (Value));
        value_init_int(v2,logic_expr->right->iVal);
    }

    Instruction *instruction;
    //返回左值是真假类型
    if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "<") == 0)
        instruction= ins_new_binary_operator(LESS,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">") == 0)
        instruction= ins_new_binary_operator(GREAT,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">=") == 0)
        instruction= ins_new_binary_operator(GREATEQ,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">=") == 0)
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
    v_tmp->pdata->var_pdata.map= getCurMap(this);
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
                if(((Value*)value)->VTy->ID != FunctionTyID && ((Value*)value)->VTy->ID != ArrayTyID && ((Value*)value)->VTy->ID!=ArrayTyID_Init && ((Value*)value)->VTy->ID!=ArrayTyID_Const)
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
                    v_replace->pdata->var_pdata.map= getCurMap(this);
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
void create_params_stmt(past func_params)
{
    past params=func_params->left;
    while(params!=NULL)
    {
        Instruction *instruction=NULL;
        Value *v=NULL;
        if(strcmp(bstr2cstr(params->nodeType, '\0'), "num_int") == 0)
        {
            v=(Value*) malloc(sizeof (Value));
            value_init_int(v, params->iVal);
        }
        else if(strcmp(bstr2cstr(params->nodeType, '\0'), "num_float") == 0)
        {
            v=(Value*) malloc(sizeof (Value));
            value_init_float(v,params->fVal);
        }
        else if(strcmp(bstr2cstr(params->nodeType, '\0'), "Call_Func") == 0)
            v= create_call_func(params);
        else if(strcmp(bstr2cstr(params->nodeType, '\0'), "LValArray") == 0)
        {
            Value *v_array= symtab_dynamic_lookup(this,bstr2cstr(params->left->sVal, '\0'));
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
                v= handle_assign_array(params->right->left,v_load,1,-1);
            }
            else
            {
                //先计数，看看传进去的是几维
                past var=params->right->left;
                int dimension_count=0;
                while(var!=NULL)
                {
                    dimension_count++;
                    var=var->next;
                }
                if(dimension_count==v_array->pdata->symtab_array_pdata.dimention_figure)
                    v= handle_assign_array(params->right->left,v_array,1,-1);
                else
                {
                    v= handle_assign_array(params->right->left,v_array,0,dimension_count);
                }
            }
        }
        else if(strcmp(bstr2cstr(params->nodeType, '\0'), "expr") == 0)
        {
            int convert=0;
            v= cal_expr(params,&convert);
        }
            //是IDent
        else
        {
            //取出value，先区别是数组还是普通id
            Value *v_test= symtab_dynamic_lookup(this,bstr2cstr(params->sVal, '\0'));
            if(v_test->VTy->ID==ArrayTyID || v_test->VTy->ID==ArrayTyID_Const || v_test->VTy->ID==ArrayTyID_Init)
            {
                //TODO 目前还是只做了一维数组
                //取得首地址，gmp一下
                Value *v_zero=(Value*) malloc(sizeof (Value));
                value_init_int(v_zero,0);
                Instruction *instruction1= ins_new_binary_operator(GMP,v_test->alias,v_zero);
                v=ins_get_value_with_name(instruction1);
                v->VTy->ID=AddressTyID;
                InstNode *node = new_inst_node(instruction1);
                ins_node_add(instruction_list,node);
            }
            else
                v= create_load_stmt(bstr2cstr(params->sVal, '\0'));
        }

        //传递参数的IR
        instruction= ins_new_unary_operator(GIVE_PARAM,v);
        //将这个instruction加入总list
        InstNode *node = new_inst_node(instruction);
        ins_node_add(instruction_list,node);

        params=params->next;
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
    if(v_array->VTy->ID==ArrayTyID)
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
                printf("i32 %d",v_array->pdata->symtab_array_pdata.array[i]);
                fprintf(fptr,"i32 %d",v_array->pdata->symtab_array_pdata.array[i]);
            }
            else
            {
                printf(", i32 %d",v_array->pdata->symtab_array_pdata.array[i]);
                fprintf(fptr,", i32 %d",v_array->pdata->symtab_array_pdata.array[i]);
            }
        }
    }
        //目前只能打印二维
    else
    {
        int ele_num= get_array_total_occupy(v_array,0);
        ele_num/=4;
        int i=0;
        int move=v_array->pdata->symtab_array_pdata.dimentions[0];
        while(i<ele_num)
        {
            printf("[%d x i32]",v_array->pdata->symtab_array_pdata.dimentions[0]);
            fprintf(fptr,"[%d x i32]",v_array->pdata->symtab_array_pdata.dimentions[0]);
            if(!all_zeros(v_array,i,move))
            {
                printf("[");
                for(int j=i;j<i+move;j++)
                {
                    if(j==i)
                    {
                        printf("i32 %d",v_array->pdata->symtab_array_pdata.array[j]);
                        fprintf(fptr,"i32 %d",v_array->pdata->symtab_array_pdata.array[j]);
                    }
                    else
                    {
                        printf(", i32 %d",v_array->pdata->symtab_array_pdata.array[j]);
                        fprintf(fptr,", i32 %d",v_array->pdata->symtab_array_pdata.array[j]);
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

char* c2ll(char* file_name)
{
    char *p=file_name;
    char *q=".ll";
    int i=0;
    while(p[i]!='.' || i<2)
    {
        i++;
    }
    p[i]='\0';
    strcat(p,q);
    return p;
}

bool begin_tmp(const char* name)
{
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

void printf_llvm_ir(struct _InstNode *instruction_node,char *file_name)
{
    bool flag_func=false;

    instruction_node= get_next_inst(instruction_node);
    const char* ll_file= c2ll(file_name);

    FILE *fptr= fopen(ll_file,"w");

    Value *v_cur_array=NULL;

    int p=0;
    Value* params[10];
    int give_count=0;
    for(int i=0;i<10;i++)
        params[i]=NULL;

    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN)
    {
        Instruction *instruction=instruction_node->inst;
        switch (instruction_node->inst->Opcode)
        {
            case Alloca:
                if(instruction->user.use_list->Val!=NULL && (instruction->user.use_list->Val->VTy->ID==ArrayTyID || instruction->user.use_list->Val->VTy->ID==ArrayTyID_Init || instruction->user.use_list->Val->VTy->ID==ArrayTyID_Const))
                {
                    printf(" %s = alloca ",instruction->user.value.name);
                    fprintf(fptr," %s = alloca ",instruction->user.value.name);
                    printf_array(instruction->user.use_list->Val,0,fptr);
                    printf(",align 16\n");
                    fprintf(fptr,",align 16\n");
                }
                else if(instruction->user.use_list->Val!=NULL && instruction->user.use_list->Val->VTy->ID==AddressTyID)
                {
                    printf(" %s = alloca i32*,align 4\n",instruction->user.value.name);
                    fprintf(fptr," %s = alloca i32*,align 4\n",instruction->user.value.name);
                }
                else
                {
                    printf(" %s = alloca i32,align 4\n",instruction->user.value.name);
                    fprintf(fptr," %s = alloca i32,align 4\n",instruction->user.value.name);
                }
                break;
            case Load:
                if(instruction->user.use_list->Val->VTy->ID==AddressTyID)
                {
                    instruction->user.value.VTy->ID=AddressTyID;
                    printf(" %s = load i32*,i32** %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    fprintf(fptr," %s = load i32*,i32** %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                } else
                {
                    printf(" %s = load i32,i32* %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                    fprintf(fptr," %s = load i32,i32* %s,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                }
                break;
            case Store:
                if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->VTy->ID!=AddressTyID)
                {
                    printf(" store i32 %d,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32 %d,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else if((instruction->user.use_list->Val->VTy->ID==Int || instruction->user.use_list->Val->VTy->ID==Const_INT) && instruction->user.use_list[1].Val->VTy->ID==AddressTyID)
                {
                    printf(" store i32* %d,i32** %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32* %d,i32** %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else if(instruction->user.use_list->Val->VTy->ID==Float || instruction->user.use_list->Val->VTy->ID==Const_FLOAT)
                {
                    printf(" store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32 %f,i32* %s,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->name);
                }
                else if(instruction->user.use_list[1].Val->VTy->ID!=AddressTyID)
                {
                    printf(" store i32 %s,i32* %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32 %s,i32* %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    printf(" store i32* %s,i32** %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    fprintf(fptr," store i32* %s,i32** %s,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
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
                        fprintf(fptr," %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
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
                while (p>0)
                {
                    if(p==instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num)
                    {
                        if(instruction->user.use_list->Val->pdata->symtab_func_pdata.param_type_lists[ii-p].ID==AddressTyID)
                        {
                            printf("i32* %%0");
                            fprintf(fptr,"i32* %%0");
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
                            printf(",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
                            fprintf(fptr,",i32* %%%d",instruction->user.use_list->Val->pdata->symtab_func_pdata.param_num-p);
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
                if(instruction->user.use_list->Val==NULL)
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

                printf("}\n\n");
                fprintf(fptr,"}\n\n");
                break;
            case Call:
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
                        printf(" %s = call i32 (i32, ...) bitcast (i32 (...)* @%s to i32 (i32, ...)*)(",instruction->user.value.name,instruction->user.use_list->Val->name);
                        fprintf(fptr," %s = call i32 (i32, ...) bitcast (i32 (...)* @%s to i32 (i32, ...)*)(",instruction->user.value.name,instruction->user.use_list->Val->name);
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
                        printf(" call void (i32, ...) bitcast (i32 (...)* @%s to i32 (i32, ...)*)(",instruction->user.use_list->Val->name);
                        fprintf(fptr," call void (i32, ...) bitcast (i32 (...)* @%s to i32 (i32, ...)*)(",instruction->user.use_list->Val->name);
                    }
                    else
                    {
                        printf(" call void (...) @%s (",instruction->user.use_list->Val->name);
                        fprintf(fptr," void i32 (...) @%s (",instruction->user.use_list->Val->name);
                    }
                }
                //参数
                for(int i=0;i<give_count;i++)
                {
                    if(i==0)
                    {
                        if(params[i]->VTy->ID==Int)
                        {
                            printf("i32 %d",params[i]->pdata->var_pdata.iVal);
                            fprintf(fptr,"i32 %d",params[i]->pdata->var_pdata.iVal);
                        }
                        else if(params[i]->VTy->ID==AddressTyID)
                        {
                            printf("i32* %s",params[i]->name);
                            fprintf(fptr,"i32* %s",params[i]->name);
                        }
                        else
                        {
                            printf("i32 %s",params[i]->name);
                            fprintf(fptr,"i32 %s",params[i]->name);
                        }

                    }
                    else
                    {
                        if(params[i]->VTy->ID==Int)
                        {
                            printf(",i32 %d",params[i]->pdata->var_pdata.iVal);
                            fprintf(fptr,",i32 %d",params[i]->pdata->var_pdata.iVal);
                        }
                        else if(params[i]->VTy->ID==AddressTyID)
                        {
                            printf(",i32* %s",params[i]->name);
                            fprintf(fptr,",i32* %s",params[i]->name);
                        }
                        else
                        {
                            printf(",i32 %s",params[i]->name);
                            fprintf(fptr,",i32 %s",params[i]->name);
                        }
                    }
                }
                printf(")\n");
                fprintf(fptr,")\n");

                give_count=0;
                for(int i=0;i<10;i++)
                    params[i]=NULL;
                break;
            case Label:
                printf("%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                fprintf(fptr,"%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                break;
            case Add:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= add nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
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
                    else
                    {
                        printf(" %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
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
                    else
                    {
                        printf(" %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
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
                        fprintf(fptr," %s= sdiv nsw i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sdiv i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf(" %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr," %s= sdiv i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf(" %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr," %s= sdiv i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                break;
            case Module:
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

            case GMP:
                if(instruction->user.value.alias!=NULL)
                    v_cur_array=instruction->user.value.alias;
                //printf("%d...\n",instruction->user.value.pdata->var_pdata.iVal);
                printf(" %s=getelementptr inbounds ",instruction->user.value.name);
                fprintf(fptr," %s=getelementptr inbounds ",instruction->user.value.name);
                if(instruction->user.use_list->Val->VTy->ID!=AddressTyID)
                {
                    printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
                    printf(",");
                    fprintf(fptr,",");
                    printf_array(v_cur_array,instruction->user.value.pdata->var_pdata.iVal,fptr);
                    printf("* ");
                    fprintf(fptr,"* ");
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                    {
                        printf("%s, i32 0,i32 %d\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                        fprintf(fptr,"%s, i32 0,i32 %d\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    }
                    else
                    {
                        printf("%s, i32 0,i32 %s\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                        fprintf(fptr,"%s, i32 0,i32 %s\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    }
                }
                else
                {
                    printf("i32,i32* %s,i32 %s\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                    fprintf(fptr,"i32,i32* %s,i32 %s\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
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
                printf_array(v_cur_array,0,fptr);
                printf("* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
                       get_array_total_occupy(v_cur_array,0));
                fprintf(fptr,"* %s to i8*), i64 %d, i1 false)\n",instruction->user.use_list[1].Val->name,
                        get_array_total_occupy(v_cur_array,0));
                break;
            case GLOBAL_VAR:
                if(instruction->user.use_list->Val->VTy->ID!=ArrayTyID && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_Init && instruction->user.use_list->Val->VTy->ID!=ArrayTyID_Const)
                {
                    printf("%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    fprintf(fptr,"%s=dso_local global i32 %d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                }
                else
                {
                    v_cur_array=instruction->user.use_list->Val;
                    if(instruction->user.use_list->Val->VTy->ID==ArrayTyID_Const)
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
                        if(instruction->user.use_list->Val->VTy->ID==ArrayTyID)
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
                params[give_count++]=instruction->user.use_list->Val;
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
                HashSet *phiSet = instruction->user.value.pdata->pairSet;
                HashSetFirst(phiSet);
                printf(" %s = phi i32 ",instruction->user.value.name);
                unsigned int size=HashSetSize(phiSet);
                int i=0;
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    BasicBlock *from = phiInfo->from;
                    Value *incomingVal = phiInfo->define;
                    if(i+1==size)      //最后一次
                    {
                        if(isImm(incomingVal)){
                            printf("[%d , %%%d]",incomingVal->pdata->var_pdata.iVal,from->id);
                        }else{
                            printf("[%s , %%%d]",incomingVal->name,from->id);
                        }
                    }
                    else
                    {
                        if(isImm(incomingVal)){
                            printf("[%d , %%%d], ",incomingVal->pdata->var_pdata.iVal,from->id);
                        }else{
                            printf("[%s , %%%d], ",incomingVal->name,from->id);
                        }
                    }
                    i++;
                }
                printf("\n");
                //printf("A phi instruction\n");
                break;
            }
            case CopyOperation:{
                printf(" a copy operation!\n");
            }
            default:
                break;
        }
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
}


void fix_array(struct _InstNode *instruction_node)
{
    instruction_node= get_next_inst(instruction_node);
    int offset=0;
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        Instruction *instruction = instruction_node->inst;
        int dimension;
        Value *v_array=NULL;
        //
        switch (instruction_node->inst->Opcode)
        {
            case GMP:
                /** 1. 先取出第一个操作数的value的iVal，是当前累积量
                * 2. 算出左值的累积量，替换左值的iVal*/
                v_array=instruction->user.value.alias;
                dimension=instruction->user.value.pdata->var_pdata.iVal;
                if(dimension+1!=v_array->pdata->symtab_array_pdata.dimention_figure && dimension==0)
                {
                    offset=instruction->user.use_list[1].Val->pdata->var_pdata.iVal*(get_array_total_occupy(v_array,dimension+1)/4);
                }
                else if(dimension+1==v_array->pdata->symtab_array_pdata.dimention_figure)
                    offset+=instruction->user.use_list[1].Val->pdata->var_pdata.iVal;
                else
                {
                    offset=instruction->user.use_list[1].Val->pdata->var_pdata.iVal*(get_array_total_occupy(v_array,dimension+1)/4)+instruction->user.use_list->Val->pdata->var_pdata.iVal;
                }
                //将左值的iVal替换为本层增加的偏移量
                instruction->user.value.pdata->var_pdata.iVal=offset;
        }
        instruction_node= get_next_inst(instruction_node);
    }
}


void print_array(struct _InstNode *instruction_node)
{
    instruction_node= get_next_inst(instruction_node);
    int offset=0;
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN) {
        printf("%d\n",instruction_node->inst->user.value.pdata->var_pdata.iVal);
        instruction_node= get_next_inst(instruction_node);
    }
}