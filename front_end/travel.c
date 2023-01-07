
#include "travel.h"

extern Symtab *this;
extern struct _InstNode *instruction_list;

//记录临时变量组,形如%1,%2,%3...
//t_index转字符串用的sprintf，itoa好像识别不了
//目前设置的能容纳3位数的临时变量
extern char t[5];
extern int return_stmt_num[10];
extern int return_index;
extern insnode_stack S_continue;
extern insnode_stack S_break;
extern insnode_stack S_return;
extern bool c_b_flag[2];

char t_num[3] = {0};
int t_index = 0;

void create_instruction_list(past root,Value* v_return)
{
    if (root != NULL) {
        if (strcmp(bstr2cstr(root->nodeType, '\0'), "VarDecl") == 0)
            create_var_decl(root,v_return);
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

    //按照llvm，有无返回值都生成了这个value
    Value *v_result=create_tmp_value();
    //有返回值
    if(v->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
    {
        v_result->VTy->ID=v->pdata->symtab_func_pdata.return_type.ID;
    }
    instruction= ins_new_unary_operator(Call,v);

    instruction->user.value=*v_result;

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    //参数传递
    if(root->right!=NULL)
        create_params_stmt(root->right);

    return v_result;
}

void create_blockItemList(past root,Value* v_return)
{
    scope_forward(this);
    create_instruction_list(root->left,v_return);
    scope_back(this);
}

//cal_expr()能使得等式右边永远只有一个值
void create_assign_stmt(past root,Value* v_return) {

    Value *v=NULL;
    if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "ID") == 0)
        //左id,从符号表中取出之前存入的这个变量
        v = symtab_dynamic_lookup(this, bstr2cstr(root->left->sVal, '\0'));
    else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "LValArray") == 0)
        v=handle_assign_array(root->left->right->left, symtab_dynamic_lookup(this,bstr2cstr(root->left->left->sVal,'\0')));

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
        //取出右值
        v1 = cal_expr(root->right);

        //赋值右边为普通a,b,c,d
    else if (strcmp(bstr2cstr(root->right->nodeType, '\0'), "ID") == 0)
        //先load出右值,要返回创建出的value
        v1=create_load_stmt(bstr2cstr(root->right->sVal,'\0'));
        //是数组
    else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "LValArray") == 0)
        v1= handle_assign_array(root->right->right, symtab_dynamic_lookup(this,bstr2cstr(root->right->left->sVal,'\0')));
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
            v = create_load_stmt(bstr2cstr(root->left->sVal, '\0'));
        }
            //返回表达式
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "expr") == 0){
            v = cal_expr(root->left);
        }
            //整数
        else if(strcmp(bstr2cstr(root->left->nodeType, '\0'), "num_int") == 0){
            v=(Value*) malloc(sizeof (Value));
            value_init_int(v,root->left->iVal);
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
    else
    {
        instruction = ins_new_unary_operator(Return, NULL);
        //将这个instruction加入总list
        InstNode *node = new_inst_node(instruction);
        ins_node_add(instruction_list,node);
    }
}

//只考虑了num_int
past array_all_zeros(past init_val_list)
{
    past p=init_val_list;
    while(p!=NULL && strcmp(bstr2cstr(p->nodeType, '\0'), "InitValList") != 0)
    {
        //不是全0就照常处理
        if(p->iVal!=0)
            return NULL;
            //其他维度全0,随便返回了一个不是NULL的
        else if(p->next==NULL)
            return init_val_list;
            //是全0,且是一维全0
        else if(strcmp(bstr2cstr(p->next->nodeType, '\0'), "InitValList") == 0)
            return p->next;
        p=p->next;
    }
}

//init_val_list是第一维的第一个num_int
//dimension：是要处理第几维
//主要需要的返回就是处理第一维的返回
past handle_one_dimention(past init_val_list,Value *v_array,int dimension,Value* bit)
{
    //是一维全0,返回下一个init_val_list
    if(array_all_zeros(init_val_list)!=NULL && dimension==1)
        return array_all_zeros(init_val_list);
        //其他维的全0，不处理
    else if(array_all_zeros(init_val_list)!=NULL)
        return NULL;

    //多维数组，先处理好数组第一维
    //TODO 目前只考虑了num_int的情况
    //赋初值,生成亿些GMP
    int offset=0;
    bool first_init=true;
    //v_last保存最内一层的上一层，只有当前维度第一个赋值才需要for轮完
    Value *v_last_second;
    //如果是第一维度，下一个就是initValList，如果不是第一维度，下一个就是NULL
    while (init_val_list!=NULL && strcmp(bstr2cstr(init_val_list->nodeType, '\0'), "InitValList") != 0)
    {
        if(first_init==true)
        {
            for(int i=0;i<v_array->pdata->symtab_array_pdata.dimention_figure-1;i++)
            {
                //第二个操作数是偏移量值,每一层的offset都是从0开始
                Value *v_offset=(Value*) malloc(sizeof (Value));
                Value *v_gmp=create_tmp_value();
                Instruction *gmp;
                //第一条
                if(i==0)
                {
                    value_init_int(v_offset,dimension-1);
                    gmp= ins_new_binary_operator(GMP,bit,v_offset);
                }
                else
                {
                    Value *v_prev=&get_last_inst(instruction_list)->inst->user.value;
                    value_init_int(v_offset,0);
                    gmp= ins_new_binary_operator(GMP,v_prev,v_offset);
                }

                gmp->user.value=*v_gmp;

                //将这个instruction加入总list
                InstNode *node_gmp_first = new_inst_node(gmp);
                ins_node_add(instruction_list,node_gmp_first);

                v_last_second=v_gmp;
            }
            first_init=false;
        }

        //!如果是0就不处理
        if(init_val_list->iVal!=0)
        {
            //不是这层第一次
            Value *v_offset=(Value*) malloc(sizeof (Value));
            value_init_int(v_offset,offset++);
            Value *v_gmp=create_tmp_value();
            Instruction *gmp_not_first = ins_new_binary_operator(GMP,v_last_second,v_offset);
            gmp_not_first->user.value=*v_gmp;
            //将这个instruction加入总list
            InstNode *node_gmp_not_first = new_inst_node(gmp_not_first);
            ins_node_add(instruction_list,node_gmp_not_first);

            //把最后一次GMP单拎出来
            //进行一次store

            //将num_int值包装成value
            Value *num=(Value*) malloc(sizeof (Value));
            value_init_int(num,init_val_list->iVal);
            create_store_stmt(num,v_gmp);
        }

        init_val_list=init_val_list->next;
    }
    //offset=0;
    return init_val_list;
}

Value *handle_assign_array(past root,Value *v_array)
{
    Value *v_last;
    //root是第一维值
    for (int i=0;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
    {
        //包装偏移的num_int为Value
        Value *v_num=(Value*) malloc(sizeof (Value));
        value_init_int(v_num,root->iVal);

        Instruction *gmp;
        if(i==0)
        {
            gmp= ins_new_binary_operator(GMP,v_array,v_num);
        }
        else
        {
            gmp= ins_new_binary_operator(GMP,v_last,v_num);
        }

        Value *v1=create_tmp_value();
        gmp->user.value=*v1;

        v_last=v1;

        //将这个instruction加入总list
        InstNode *node_gmp = new_inst_node(gmp);
        ins_node_add(instruction_list,node_gmp);

        root=root->next;
    }
    return v_last;
}

//还未考虑数组
void create_var_decl(past root,Value* v_return) {

    //到达第一个var的节点
    past vars = root->right->left;

    //纯定义语句不需要处理了
    //有初值
    if (strcmp(bstr2cstr(root->right->left->nodeType, '\0'), "VarDef_init") == 0) {
        while (vars != NULL) {
            //左值
            Value *v=symtab_dynamic_lookup(this, bstr2cstr(vars->left->sVal, '\0'));

            //初始化的值
            //root->right是VarDefList
            Value *v1=NULL;
            if (strcmp(bstr2cstr(root->right->left->right->nodeType, '\0'), "num_int") == 0)
            {
                v1=(Value*) malloc(sizeof (Value));
                value_init_int(v1, root->right->left->right->iVal);
            }
            else if(strcmp(bstr2cstr(root->right->left->right->nodeType, '\0'), "num_float") == 0)
            {
                v1=(Value*) malloc(sizeof (Value));
                value_init_float(v1,root->right->left->right->fVal);
            }
            else if(strcmp(bstr2cstr(root->right->left->right->nodeType, '\0'), "ID") == 0)
                v1= create_load_stmt(bstr2cstr(root->right->left->right->sVal,'\0'));
            else if (strcmp(bstr2cstr(root->right->left->right->nodeType, '\0'), "expr") == 0)
                v1 = cal_expr(root->right->left->right);

            //将值store下来
            create_store_stmt(v1,v);

            vars = vars->next;
        }
    }
        //有初值数组
    else if(strcmp(bstr2cstr(root->right->left->nodeType, '\0'), "VarDef_array_init") == 0)
    {
        past ident_array=root->right->left->left;         //到IdentArray结点
        Value *v_array = symtab_dynamic_lookup(this,bstr2cstr(ident_array->left->sVal,'\0'));
        past init_val_list = root->right->left->right->left;

        //生成bitcast,使读取的内存空间通过指针i8*，i8步长读取
        //TODO bitcast 打印不方便
        //<result> = bitcast <ty> <value> to <ty2>
        //The ‘bitcast’ instruction converts value to type ty2 without changing any bits.
        Value *v1=create_tmp_value();
        Instruction *ins_bitcast = ins_new_unary_operator(bitcast,v_array);
        ins_bitcast->user.value=*v1;

        //将这个instruction加入总list
        InstNode *node_bitcast = new_inst_node(ins_bitcast);
        ins_node_add(instruction_list,node_bitcast);

        //一维数组,拷贝式赋初值,memcpy
        //call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %5, i8* align 16 bitcast ([5 x i32]* @__const.if_if_Else.c to i8*), i64 20, i1 false)
        if(v_array->pdata->symtab_array_pdata.dimention_figure==1)
        {
            //TODO
        }
        else
        {
            //TODO memset是否还需要我处理?
            //再来一条bitcast
            //!!!后续第一条一直用v2
            Value *v2=create_tmp_value();
            Instruction *ins_bitcast2= ins_new_unary_operator(bitcast,v1);
            ins_bitcast2->user.value=*v2;
            //将这个instruction加入总list
            InstNode *node_bitcast2 = new_inst_node(ins_bitcast2);
            ins_node_add(instruction_list,node_bitcast2);

            //多维数组，先处理好数组第一维
            init_val_list = handle_one_dimention(init_val_list,v_array,1,v2);

            //如果init_val_list此时是InitValList，就说明还有下一维度
            int demension=2;
            while (init_val_list!=NULL && strcmp(bstr2cstr(init_val_list->nodeType, '\0'), "InitValList") == 0)
            {
                past next_dimension= init_val_list->left;
                handle_one_dimention(next_dimension,v_array,demension++,v2);
                init_val_list=init_val_list->next;

            }
        }
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
    else
        instruction= ins_new_unary_operator(Label,NULL);
    Value *T1=(Value*) malloc(sizeof (Value));
    value_init(T1);
    if(true_goto_location>0)
        T1->pdata->instruction_pdata.true_goto_location=true_goto_location;
    instruction->user.value=*T1;

    InstNode *node_true = new_inst_node(instruction);
    ins_node_add(instruction_list,node_true);
    return node_true;
}

void create_if_stmt(past root,Value* v_return) {
    //br i1 label__,label__
    //真值
    Value *v_real= cal_logic_expr(root->left);

    //正确跳转
    InstNode *node1= true_location_handler(br_i1,v_real, t_index++);

    //生成正确的那个标识IR,示例中8:
    true_location_handler(Label,NULL,t_index-1);

    //先走完if为真的所有语句，走完后就可确定否定跳转的位置
    create_instruction_list(root->right,v_return);

    node1->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index++;

    //无break或continue的话就补，有就不补了
    if(get_last_inst(instruction_list)->inst->Opcode!=br)
    {
        //再补一条br label,使每个基本块结束都是跳转,跳转到end,示例中的br label 9
        true_location_handler(br,NULL,t_index-1);
    }

    //再补一条标号,可以理解为是false的跳转，也可以理解为是end的跳转
    true_location_handler(Label,NULL,t_index-1);

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return);
}

void create_if_else_stmt(past root,Value* v_return) {
    //100:if a>b GOTO 102
    //真值
    Value *v_real= cal_logic_expr(root->left);

    //正确跳转
    InstNode *node1 = true_location_handler(br_i1,v_real,t_index++);

    //生成正确的那个标识IR
    true_location_handler(Label,NULL,t_index-1);

    //走完if为真的语句，即可知道else从哪开始
    //走完if为真，目前最后一条是一个编号，示例的9:
    create_instruction_list(root->right->left,v_return);          //if为真走的语句

    //if为假走else的话应该走这里,示例中将前面else的label 10补齐
    node1->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index++;


    //真的走完之后接一句跳转,跳过else的部分,示例中br label 13的，在当前是br label __
    InstNode *node2= true_location_handler(br,NULL,0);

    //再补一条标号,即示例中10:
    true_location_handler(Label,NULL,t_index-1);


    //走完else部分的语句，即可知道从if为真中出来的下一条应该goto到什么位置
    create_instruction_list(root->right->right,v_return);         //if为假走的语句

    //将之前的br label__补齐，示例中补齐为br label 13
    node2->inst->user.value.pdata->instruction_pdata.true_goto_location=t_index++;

    if(get_last_inst(instruction_list)->inst->Opcode!=br)
    {
        //补一条br label，示例的br label %13，这个和后面那条都是跳到end
        true_location_handler(br,NULL,t_index-1);
    }

    //再补一条标号，示例的13:
    true_location_handler(Label,NULL,t_index-1);

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return);
}

void create_while_stmt(past root,Value* v_return)
{
    //加进stack，continue用
    Instruction *ins_tmp= ins_new_unary_operator(tmp,NULL);
    Value *t_in=(Value*) malloc(sizeof (Value));
    value_init(t_in);
    t_in->pdata->instruction_pdata.true_goto_location=t_index;
    ins_tmp->user.value=*t_in;
    insnode_push(&S_continue, new_inst_node(ins_tmp));

    int while_recall=t_index;
    //生成一条br label跳转和标号
    true_location_handler(br,NULL,t_index++);
    //标号
    true_location_handler(Label,NULL,t_index-1);

    //真值
    Value *v_real= cal_logic_expr(root->left);
    InstNode *node_first_bri1= true_location_handler(br_i1,v_real,t_index++);
    //标号
    true_location_handler(Label,NULL,t_index-1);

    create_instruction_list(root->right,v_return);

    if(get_last_inst(instruction_list)->inst->Opcode!=br)
    {
        //生成循环，br label跳转到first_label处
        true_location_handler(br,NULL,while_recall);
    }
    node_first_bri1->inst->user.value.pdata->instruction_pdata.false_goto_location=t_index++;

    //标号
    true_location_handler(Label,NULL,t_index-1);

    //while结束，加进stack,break用
    Instruction *ins_break_tmp= ins_new_unary_operator(tmp,NULL);
    Value *t_out=(Value*) malloc(sizeof (Value));
    value_init(t_out);
    t_out->pdata->instruction_pdata.true_goto_location=t_index-1;
    ins_break_tmp->user.value=*t_out;

    Instruction *ins_1_tmp= ins_new_unary_operator(tmp,NULL);
    Value *t_1=(Value*) malloc(sizeof (Value));
    value_init(t_1);
    t_1->pdata->instruction_pdata.true_goto_location=-1;
    ins_1_tmp->user.value=*t_1;


    insnode_push(&S_continue, new_inst_node(ins_1_tmp));
    insnode_push(&S_break, new_inst_node(ins_break_tmp));

    if(root->next!=NULL)
        create_instruction_list(root->next,v_return);
}

//TODO 还未完成
void create_func_def(past root) {
    Instruction *instruction_begin;
    instruction_begin = ins_new_unary_operator(FunBegin, NULL);

    //拿出存在符号表中的函数信息
    Value *v=symtab_lookup_withmap(this, bstr2cstr(root->left->next->sVal, '\0'), &this->value_maps->next->map);
    instruction_begin->user.value=*v;


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
    if(return_stmt_num[return_index]>1 || strcmp(v->name,"main")==0)
    {
        v_return=create_tmp_value();
        v_return->pdata->var_pdata.alias=t_index-1;

        //有多个返回值
        if(return_stmt_num[return_index]>1)
            v_return->VTy->ID=v->pdata->symtab_func_pdata.return_type.ID;
        else
        {
            if(v->pdata->symtab_func_pdata.return_type.ID==Var_INT)
                v_return->VTy->ID=MAIN_INT;
            else
                v_return->VTy->ID=MAIN_FLOAT;
        }

        Instruction *instruction= ins_new_unary_operator(Alloca, v_return);
        instruction->user.value=*v_return;
        //将这个instruction加入总list
        InstNode *node = new_inst_node(instruction);
        ins_node_add(instruction_list,node);
    }


    //将参数从0开始对应
    //有参
    if (root->left->next->left != NULL) {
        int param_map=0;

        //params走到第一个FuncParam处
        past params=root->left->next->left->left;

        //先生成参数的alloca
        while(params!=NULL)
        {
            //参数Value
            Value *param= symtab_lookup_withmap(this,bstr2cstr(params->left->next->sVal, '\0'), &v->pdata->symtab_func_pdata.map_list->map);

            param->pdata->var_pdata.alias=t_index++;
            //printf("%d\t",param->pdata->var_pdata.alias);

            Instruction *instruction= ins_new_unary_operator(Alloca,param);
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
            Value *param= symtab_lookup_withmap(this,bstr2cstr(params->left->next->sVal, '\0'), &v->pdata->symtab_func_pdata.map_list->map);

            //TODO 怎么处理参数的%0,%1。真的要store吗；打印是肯定要的，IR呢
            Value *num_param= (Value*) malloc(sizeof (Value));
            value_init_int(num_param,param_map++);
            create_store_stmt(num_param,param);

            params=params->next;
        }
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

    //多return的情况下触发
    if(v_return!=NULL && v_return->VTy->ID!=MAIN_INT && v_return->VTy->ID!=MAIN_FLOAT)
    {
        //不是读到return才会触发这个
        //1.先生成标号
        InstNode *instNode=true_location_handler(Label,NULL,t_index++);
        //入栈
        insnode_push(&S_return,instNode);
        //2.再生成一条load
        Value *final_ret = create_return_load(v_return);
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
    if(return_stmt_num[return_index]>1)
        reduce_return();
    insnode_stack_new(&S_break);
    insnode_stack_new(&S_continue);

    return_index++;

    if (root->next != NULL)
        create_instruction_list(root->next,v_return);
}

//先后序遍历树，得到后缀表达式并存入数组，再通过后缀表达式得到表达式的值
//目前做的有点复杂，其实应该可以直接后序遍历树就ok的，但目前感觉这样做也蛮清晰的，有时间再改吧
struct _Value *cal_expr(past expr) {
    //最后从栈中弹出的
    past final_result = newAstNode();
    //记录后缀表达式
    past str[100];
    int i = 0;

    //后序遍历语法树
    past_stack PS1;    //语法树的stack
    init_past_stack(&PS1);

    past p = expr;
    past q = NULL;    //记录刚刚访问过的结点
    while (p != NULL || !is_empty(PS1)) {
        if (p != NULL) {
            push(&PS1, p);
            p = p->left;
        } else {
            top(&PS1, &p);     //往上走了才pop掉

            if ((p->right == NULL) || (p->right) == q) {
                //开始往上走
                q = p;              //保存到q，作为下一次处理结点的前驱
                pop(&PS1, &p);
                str[i++] = p;
                p = NULL;         //p置于NULL可继续退层，否则会重复访问刚访问结点的左子树
            } else
                p = p->right;
        }
    }
    str[i]=NULL;

    //计算后缀表达式
    past_stack PS2;
    init_past_stack(&PS2);
    past x1, x2;
    past *pp = str;
    while (*pp) {
        if (strcmp(bstr2cstr((*pp)->nodeType, '\0'), "expr") != 0)
            push(&PS2, (*pp));

        else {
            //如果读到+-*/%就从栈中弹出两个计算,算不动就给临时变量存
            pop(&PS2, &x2);
            pop(&PS2, &x1);
            //都是常数，直接算出来压进栈
            if (strcmp(bstr2cstr(x1->nodeType, '\0'), "num_int") == 0 &&
                strcmp(bstr2cstr(x2->nodeType, '\0'), "num_int") == 0) {
                switch ((*pp)->iVal) {
                    case '+':
                        x1->iVal = x1->iVal + x2->iVal;
                        break;
                    case '-':
                        x1->iVal = x1->iVal - x2->iVal;
                        break;
                    case '*':
                        x1->iVal = x1->iVal * x2->iVal;
                        break;
                    case '/':
                        x1->iVal = x1->iVal / x2->iVal;
                        break;
                    case '%':
                        x1->iVal = x1->iVal % x2->iVal;
                        break;
                }
                push(&PS2, x1);
            } else if (strcmp(bstr2cstr(x1->nodeType, '\0'), "num_int") == 0 &&
                       strcmp(bstr2cstr(x2->nodeType, '\0'), "ID") == 0) {
                Instruction *instruction = NULL;
                Value *v1=(Value*) malloc(sizeof (Value));
                value_init_int(v1, x1->iVal);

                Value *v2=NULL;

                //如果x2->sVal是a,b,c...需要load
                v2=symtab_dynamic_lookup(this, bstr2cstr(x2->sVal, '\0'));
                if(v2!=NULL)
                {
                    v2=create_load_stmt(bstr2cstr(x2->sVal, '\0'));
                }
                    //是中间临时变量，默认无初值
                else{
                    v2=(Value*) malloc(sizeof (Value));
                    value_init(v2);
                    v2->name = (char *) malloc(sizeof(bstr2cstr(x2->sVal, '\0')));
                    strcpy(v2->name, bstr2cstr(x2->sVal, '\0'));
                    v2->pdata->var_pdata.map= getCurMap(this);
                    v2->VTy->ID = Var_INT;
                }

                //临时变量左值,v_tmp的pdata是没有实际内容的
                Value *v_tmp=create_tmp_value();
                past tmp = newIdent(bfromcstr(v_tmp->name));
                v_tmp->VTy->ID = Var_INT;

                switch ((*pp)->iVal) {
                    case '+':
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
                }
                instruction->user.value = *v_tmp;
                //将这个instruction加入总list
                InstNode *node = new_inst_node(instruction);
                ins_node_add(instruction_list,node);
                push(&PS2, tmp);
            } else if (strcmp(bstr2cstr(x1->nodeType, '\0'), "ID") == 0 &&
                       strcmp(bstr2cstr(x2->nodeType, '\0'), "num_int") == 0) {
                Instruction *instruction = NULL;

                Value *v1=NULL;

                //如果x1->sVal是a,b,c...需要load
                v1=symtab_dynamic_lookup(this, bstr2cstr(x1->sVal, '\0'));
                if(v1!=NULL)
                {
                    v1=create_load_stmt(bstr2cstr(x1->sVal, '\0'));
                }
                else{
                    v1=(Value*) malloc(sizeof (Value));
                    value_init(v1);
                    v1->name = (char *) malloc(sizeof(bstr2cstr(x1->sVal, '\0')));
                    strcpy(v1->name, bstr2cstr(x1->sVal, '\0'));
                    v1->pdata->var_pdata.map= getCurMap(this);
                    v1->VTy->ID = Var_INT;
                }

                Value *v2=(Value*) malloc(sizeof (Value));
                value_init_int(v2, x2->iVal);


                //临时变量左值,v_tmp的pdata是没有实际内容的
                Value *v_tmp=create_tmp_value();
                past tmp = newIdent(bfromcstr(v_tmp->name));
                v_tmp->VTy->ID = Var_INT;

                switch ((*pp)->iVal) {
                    case '+':
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
                }
                instruction->user.value = *v_tmp;
                //将这个instruction加入总list
                InstNode *node = new_inst_node(instruction);
                ins_node_add(instruction_list,node);
                push(&PS2, tmp);
            } else if (strcmp(bstr2cstr(x1->nodeType, '\0'), "ID") == 0 &&
                       strcmp(bstr2cstr(x2->nodeType, '\0'), "ID") == 0) {
                Instruction *instruction = NULL;
                Value *v1=NULL;

                //如果x1->sVal是a,b,c...需要load
                v1=symtab_dynamic_lookup(this, bstr2cstr(x1->sVal, '\0'));
                if(v1!=NULL)
                {
                    v1=create_load_stmt(bstr2cstr(x1->sVal, '\0'));
                }
                else{
                    v1=(Value*) malloc(sizeof (Value));
                    value_init(v1);
                    v1->name = (char *) malloc(sizeof(bstr2cstr(x1->sVal, '\0')));
                    strcpy(v1->name, bstr2cstr(x1->sVal, '\0'));
                    v1->pdata->var_pdata.map= getCurMap(this);
                    v1->VTy->ID = Var_INT;
                }

                Value *v2=NULL;

                //如果x2->sVal是a,b,c...需要load
                v2=symtab_dynamic_lookup(this, bstr2cstr(x2->sVal, '\0'));
                if(v2!=NULL)
                {
                    v2=create_load_stmt(bstr2cstr(x2->sVal, '\0'));
                }
                else{
                    v2=(Value*) malloc(sizeof (Value));
                    value_init(v2);
                    v2->name = (char *) malloc(sizeof(bstr2cstr(x2->sVal, '\0')));
                    strcpy(v2->name, bstr2cstr(x2->sVal, '\0'));
                    v2->pdata->var_pdata.map= getCurMap(this);
                    v2->VTy->ID = Var_INT;
                }

                //临时变量左值,v_tmp的pdata是没有实际内容的
                Value *v_tmp=create_tmp_value();
                past tmp = newIdent(bfromcstr(v_tmp->name));
                v_tmp->VTy->ID = Var_INT;

                switch ((*pp)->iVal) {
                    case '+':
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
                }
                instruction->user.value = *v_tmp;
                //将这个instruction加入总list
                InstNode *node = new_inst_node(instruction);
                ins_node_add(instruction_list,node);
                push(&PS2, tmp);
            }
        }
        pp++;
    }

    //弹出最终值
    pop(&PS2, &final_result);
    //将past类型的final_result转成value*
    Value *v_final=(Value*) malloc(sizeof (Value));
    value_init(v_final);

    //只可能弹出常数或临时变量tx
    if (strcmp(bstr2cstr(final_result->nodeType, '\0'), "num_int") == 0)
        value_init_int(v_final, final_result->iVal);
    else {
        v_final->VTy->ID = Var_INT;
        v_final->name = (char *) malloc(sizeof(bstr2cstr(final_result->sVal, '\0')));
        strcpy(v_final->name, bstr2cstr(final_result->sVal, '\0'));
        v_final->pdata->var_pdata.map= getCurMap(this);
    }

    //pdata暂时只存了变量名
    return v_final;
}

//将临时变量t0 clear为t,或将%1 clear为%，后面跟的数量可重新赋值
void clear_tmp(char* tmp)
{
    char *p=tmp;
    p++;
    while(*p)
    {
        *p='\0';
        p++;
    }
}

//原来的直接算出真值的函数，先注释掉，以后可能还是会用到
struct _Value* cal_logic_expr(past logic_expr)
{
    Value *v1=NULL;
    Value *v2=NULL;

    //value1
    if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "expr") == 0)
        v1= cal_expr(logic_expr->left);
    else if(strcmp(bstr2cstr(logic_expr->left->nodeType, '\0'), "ID") == 0)
        v1= create_load_stmt(bstr2cstr(logic_expr->left->sVal,'\0'));
    else
    {
        //TODO num_int,目前还都没有考虑float
        //num_int
        v1=(Value*) malloc(sizeof (Value));
        value_init_int(v1,logic_expr->left->iVal);
    }

    //value2
    if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "expr") == 0)
        v2= cal_expr(logic_expr->right);
    else if(strcmp(bstr2cstr(logic_expr->right->nodeType, '\0'), "ID") == 0)
        v2= create_load_stmt(bstr2cstr(logic_expr->right->sVal,'\0'));
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
    Value *v_tmp=create_tmp_value();
    v_tmp->VTy->ID=Var_INT;

    //v_tmp是真值类型
    instruction->user.value=*v_tmp;

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    //返回左值即logic_expr的真值
    return v_tmp;
}

//除了不能定type，其他都ok
struct _Value *create_tmp_value()
{
    sprintf(t_num, "%d", t_index++);
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
    instruction=ins_new_unary_operator(Load,v);
    //最后会删除吧，自动创建instruction的user.value
    Value *v1=create_tmp_value();
    instruction->user.value=*v1;

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
    Value *v1=create_tmp_value();
    instruction->user.value=*v1;

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
            if(((Value*)value)->VTy->ID != Param_INT && ((Value*)value)->VTy->ID !=Param_FLOAT)
            {
                Instruction *instruction= ins_new_unary_operator(Alloca, (Value *) value);
                //与符号表对应的绑定在一起
                ((Value*)value)->pdata->var_pdata.alias=t_index++;
                //将这个instruction加入总list
                InstNode *node = new_inst_node(instruction);
                ins_node_add(instruction_list,node);
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
            //是IDent
        else
            v= symtab_dynamic_lookup(this,bstr2cstr(params->sVal, '\0'));

        //传递参数的IR
        instruction= ins_new_unary_operator(GIVE_PARAM,v);
        //将这个instruction加入总list
        InstNode *node = new_inst_node(instruction);
        ins_node_add(instruction_list,node);

        params=params->next;
    }
}

void printf_array(Value *v_array, int begin_index)
{
    for(int i=begin_index;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
    {
        printf("[");
        printf("%d ",v_array->pdata->symtab_array_pdata.dimentions[i]);
        printf("x ");
    }
    printf("i32");
    for(int i=begin_index;i<v_array->pdata->symtab_array_pdata.dimention_figure;i++)
    {
        printf("]");
    }
}

void printf_llvm_ir(struct _InstNode *instruction_node)
{
    instruction_node= get_next_inst(instruction_node);
    while (instruction_node!=NULL && instruction_node->inst->Opcode!=ALLBEGIN)
    {
        Instruction *instruction=instruction_node->inst;
        switch (instruction_node->inst->Opcode)
        {
            case Alloca:
                if(instruction->user.use_list->Val->VTy->ID==ArrayTyID)
                {
                    printf(" %%%d = alloca ",instruction->user.use_list->Val->pdata->var_pdata.alias);
                    printf_array(instruction->user.use_list->Val,0);
                    printf(",align 16\n");
                }
                else if(instruction->user.use_list->Val!=NULL)
                    printf(" %%%d = alloca i32,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.alias);
                break;
            case Load:
                printf(" %s = load i32,i32* %%%d,align 4\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.alias);
                break;
            case Store:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                    printf(" store i32 %d,i32* %%%d,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.alias);
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                    printf(" store i32 %f,i32* %%%d,align 4\n",instruction->user.use_list->Val->pdata->var_pdata.fVal,instruction->user.use_list[1].Val->pdata->var_pdata.alias);
                else
                    printf(" store i32 %s,i32* %%%d,align 4\n",instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.alias);
                break;
            case br:
                printf(" br label %%%d\n\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                break;
            case br_i1:
                printf(" br i1 %s,label %%%d,label %%%d\n\n",instruction->user.use_list->Val->name,instruction->user.value.pdata->instruction_pdata.true_goto_location,instruction->user.value.pdata->instruction_pdata.false_goto_location);
                break;
            case EQ:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        printf(" %s = icmp eq i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    else
                        printf(" %s = icmp eq i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        printf(" %s = icmp eq i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    else
                        printf(" %s = icmp eq i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                break;
            case LESS:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        printf(" %s = icmp slt i32 %d,%d\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    else
                        printf(" %s = icmp slt i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        printf(" %s = icmp slt i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    else
                        printf(" %s = icmp slt i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                break;
            case FunBegin:
                //TODO 暂时用#0，未考虑参数
                printf("define dso_local i32 @%s() #0{\n",instruction->user.value.name);
                break;
            case Return:
                if(instruction->user.use_list->Val==NULL)
                    printf(" ret void\n");
                else if(instruction->user.use_list->Val->VTy->ID==Int)
                    printf(" ret i32 %d\n",instruction->user.use_list->Val->pdata->var_pdata.iVal);
                else if(instruction->user.use_list->Val->VTy->ID==Float)
                    printf(" ret i32 %f\n",instruction->user.use_list->Val->pdata->var_pdata.fVal);
                else
                    printf(" ret i32 %s\n",instruction->user.use_list->Val->name);
                printf("}\n\n");
                break;
            case Call:
                printf(" %s = call i32 @%s()\n",instruction->user.value.name,instruction->user.use_list->Val->name);
                break;
            case Label:
                printf("%d:\n",instruction->user.value.pdata->instruction_pdata.true_goto_location);
                break;
            case Add:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    printf(" %s= add nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        printf(" %s= add nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    else
                        printf(" %s= add nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                break;
            case Sub:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    printf(" %s= sub nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        printf(" %s= sub nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    else
                        printf(" %s= sub nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                break;
            case Mul:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    printf(" %s= mul nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        printf(" %s= mul nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    else
                        printf(" %s= mul nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                break;
            case Div:
                if(instruction->user.use_list->Val->VTy->ID==Int)
                {
                    printf(" %s= div nsw i32 %d,%s\n",instruction->user.value.name,instruction->user.use_list->Val->pdata->var_pdata.iVal,instruction->user.use_list[1].Val->name);
                }
                else
                {
                    if(instruction->user.use_list[1].Val->VTy->ID==Int)
                        printf(" %s= div nsw i32 %s,%d\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->pdata->var_pdata.iVal);
                    else
                        printf(" %s= div nsw i32 %s,%s\n",instruction->user.value.name,instruction->user.use_list->Val->name,instruction->user.use_list[1].Val->name);
                }
                break;
            case bitcast:
                //第一条bitcast
                if(get_next_inst(instruction_node)->inst->Opcode==bitcast)
                {
                    printf(" %s=bitcast ",instruction->user.value.name);
                    printf_array(instruction->user.use_list->Val,0);
                    printf("* %%%d to i8*\n",instruction->user.use_list->Val->pdata->symtab_array_pdata.alias);
                }
                else
                {
                    printf(" %s=bitcast i8* %s to ",instruction->user.value.name,instruction->user.use_list->Val->name);
                    printf_array(instruction->user.use_list->Val,0);
                    printf("*\n");
                }
                break;
                /*
            case GMP:
                //是全新的第一行且第一维
                if(get_prev_inst(instruction_node)->inst->Opcode!=GMP && (get_prev_inst(get_prev_inst(instruction_node))->inst->Opcode!=GMP || get_prev_inst(get_prev_inst(instruction_node))->inst->user.use_list->Val->name!=instruction->user.use_list->Val->name))
                {
                    for(int i=0;i<instruction)
                    printf(" %s=getelementptr inbounds ")
                }*/
            default:
                break;
        }
        instruction_node= get_next_inst(instruction_node);
    }
}
