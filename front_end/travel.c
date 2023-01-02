
#include "travel.h"

extern Symtab *this;
extern struct _InstNode *instruction_list;
extern int instruction_uid;

//记录临时变量组,形如%1,%2,%3...
//t_index转字符串用的sprintf，itoa好像识别不了
//目前设置的能容纳3位数的临时变量
extern char t[5];
char t_num[3] = {0};
int t_index = 0;


void create_instruction_list(past root) {
    if (root != NULL) {
        if (strcmp(bstr2cstr(root->nodeType, '\0'), "VarDecl") == 0)
            create_var_decl(root);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "FuncDef") == 0)
            create_func_def(root);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "BlockItemList") == 0)
            create_blockItemList(root,0);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "Assign_Stmt") == 0)
            create_assign_stmt(root);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "Return_Stmt") == 0)
            create_return_stmt(root);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "IF_Stmt") == 0)
            create_if_stmt(root);
        else if (strcmp(bstr2cstr(root->nodeType, '\0'), "IfElse_Stmt") == 0)
            create_if_else_stmt(root);
        else if(strcmp(bstr2cstr(root->nodeType, '\0'), "Call_Func") == 0)
            create_call_func(root);
    }
}

struct _Value *create_call_func(past root)
{
    Instruction *instruction;
    Value *v=NULL;
    //从符号表中取出之前放入的函数信息
    v= symtab_lookup_withmap(this, bstr2cstr(root->left->sVal, '\0'),&this->value_maps->next->map);

    Value *v_result=NULL;
    //有返回值
    if(v->pdata->symtab_func_pdata.return_type.ID!=VoidTyID)
    {
        v_result=create_tmp_value();
        v_result->VTy->ID=v->pdata->symtab_func_pdata.return_type.ID;
        instruction= ins_new_unary_operator(Call_With_Return_Value,v);

        //打印
        printf("%d: %s=Call_With_Return_Value %s\n",instruction->i,v_result->name,v->name);
    }
    //无返回值
    else
    {
        instruction= ins_new_unary_operator(Call,v);
    }

    if(v_result!=NULL)
        instruction->user.value=*v_result;
    instruction->Parent = block_one;

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    //参数传递
    if(root->right!=NULL)
        create_params_stmt(root->right);

    return v_result;
}

void create_blockItemList(past root,int flag)
{
    scope_forward(this);
    if(flag==1)
    {
        //是函数进来的,先声明一顿alloca
        struct _mapList* cur_mapList= getCurMapList(this);
        declare_all_alloca(cur_mapList,true);
    }
    create_instruction_list(root->left);
    scope_back(this);
}

//cal_expr()能使得等式右边永远只有一个值
void create_assign_stmt(past root) {

    //左id,从符号表中取出之前存入的这个变量
    Value *v = symtab_dynamic_lookup(this, bstr2cstr(root->left->sVal, '\0'));

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
    //等于函数
    else
        v1= create_call_func(root->right);

    create_store_stmt(v1,v);
    if (root->next != NULL)
        create_instruction_list(root->next);
}


void create_return_stmt(past root) {
    Instruction *instruction;
    if (root->left != NULL) {
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

        instruction = ins_new_unary_operator(Return, NULL);
        if(v!=NULL)
            instruction->user.value = *v;
        instruction->Parent = block_one;
        //将这个instruction加入总list
        InstNode *node = new_inst_node(instruction);
        ins_node_add(instruction_list,node);

        //打印
        printf("%d: ret %s\n",instruction->i,instruction->user.value.name);
    }
}

//还未考虑数组
void create_var_decl(past root) {

    //到达第一个var的节点
    past vars = root->right->left;

    //纯定义语句不需要处理了
    //有初值
    if (strcmp(bstr2cstr(root->right->left->nodeType, '\0'), "VarDef_init") == 0) {
        while (vars != NULL) {
            Instruction *instruction;
            //左值
            Value *v=symtab_dynamic_lookup(this, bstr2cstr(vars->left->sVal, '\0'));

            //初始化的值
            Value *v1=NULL;
            if (strcmp(bstr2cstr(root->right->nodeType, '\0'), "num_int") == 0)
                value_init_int(v1, root->right->iVal);
            else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "num_float") == 0)
                value_init_float(v1,root->right->fVal);
            else if(strcmp(bstr2cstr(root->right->nodeType, '\0'), "ID") == 0)
                v1= create_load_stmt(bstr2cstr(root->right->sVal,'\0'));
            else if (strcmp(bstr2cstr(root->right->nodeType, '\0'), "expr") == 0)
                v1 = cal_expr(root->right);

            //将值store下来
            create_store_stmt(v1,v);

            vars = vars->next;
        }
    }

    if(root->next!=NULL)
        create_instruction_list(root->next);
}


void create_if_stmt(past root) {
    //100:if a>b GOTO 102
    Instruction *instruction1;

    //真值
    past logic_expr=root->left;
    Value *v1=NULL;Value *v2=NULL;
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

    Value *v3=(Value*) malloc(sizeof (Value));
    //goto到+2的地方
    value_init_int(v3, instruction_uid + 2);

    if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "<") == 0)
        instruction1= ins_new_binary_operator(IFLESS_GOTO,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">") == 0)
        instruction1= ins_new_binary_operator(IFGREAT_GOTO,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">=") == 0)
        instruction1= ins_new_binary_operator(IF_GREATEQ_GOTO,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">=") == 0)
        instruction1= ins_new_binary_operator(IF_LESSEQ_GOTO,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "=") == 0)
        instruction1= ins_new_binary_operator(IFEQ_GOTO,v1,v2);
    else
        instruction1= ins_new_binary_operator(IFNOTEQ_GOTO,v1,v2);

    instruction1->Parent = block_one;
    instruction1->user.value = *v3;

    //打印
    printf("%d: 类型%d %s,%d goto %d\n",instruction_uid-1,instruction1->Opcode,v1->name,v2->pdata->var_pdata.iVal,v3->pdata->var_pdata.iVal);

    //将这个instruction加入总list
    InstNode *node1 = new_inst_node(instruction1);
    ins_node_add(instruction_list,node1);

    //101:GOTO ____
    Instruction *instruction2;
    instruction2 = ins_new_unary_operator(Goto, NULL);
    instruction2->Parent = block_one;

    //具体goto位置还没有确定下来，但是先将该指令添加到链中
    InstNode *node2 = new_inst_node(instruction2);
    ins_node_add(instruction_list,node2);

    //先走完if为真的所有语句，走完后就可确定goto跳转的位置
    create_instruction_list(root->right);

    //此时的uid应该已经指向下一条
    Value *v4=(Value*) malloc(sizeof (Value));
    value_init_int(v4, instruction_uid);
    node2->inst->user.value=*v4;

    printf("%d: goto %d\n",instruction2->i,v4->pdata->var_pdata.iVal);

    if(root->next!=NULL)
        create_instruction_list(root->next);
}

void create_if_else_stmt(past root) {
    //100:if a>b GOTO 102
    Instruction *instruction1;
    //真值
    past logic_expr=root->left;
    Value *v1=NULL;Value *v2=NULL;
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

    Value *v3=(Value*) malloc(sizeof (Value));
    //goto到+2的地方
    value_init_int(v3, instruction_uid + 2);

    if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "<") == 0)
        instruction1= ins_new_binary_operator(IFLESS_GOTO,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">") == 0)
        instruction1= ins_new_binary_operator(IFGREAT_GOTO,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">=") == 0)
        instruction1= ins_new_binary_operator(IF_GREATEQ_GOTO,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), ">=") == 0)
        instruction1= ins_new_binary_operator(IF_LESSEQ_GOTO,v1,v2);
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "=") == 0)
        instruction1= ins_new_binary_operator(IFEQ_GOTO,v1,v2);
    else
        instruction1= ins_new_binary_operator(IFNOTEQ_GOTO,v1,v2);

    instruction1->Parent = block_one;
    instruction1->user.value = *v3;

    //打印
    printf("%d: 类型%d %s,%d goto %d\n",instruction_uid-1,instruction1->Opcode,v1->name,v2->pdata->var_pdata.iVal,v3->pdata->var_pdata.iVal);

    //将这个instruction加入总list
    InstNode *node1 = new_inst_node(instruction1);
    ins_node_add(instruction_list,node1);

    //goto___
    Instruction *instruction2;
    instruction2 = ins_new_unary_operator(Goto, NULL);
    instruction2->Parent = block_one;

    InstNode *node2 = new_inst_node(instruction2);
    ins_node_add(instruction_list,node2);

    //走完if为真的语句，即可知道else从哪开始
    create_instruction_list(root->right->left);          //if为真走的语句

    //真的走完之后接一句goto,跳过else的部分
    Instruction *instruction3;
    instruction3 = ins_new_unary_operator(Goto, NULL);
    instruction3->Parent = block_one;
    InstNode *node3 = new_inst_node(instruction3);
    ins_node_add(instruction_list,node3);

    //if为假走else的话应该走这里
    Value *v4=(Value*) malloc(sizeof (Value));
    value_init_int(v4, instruction_uid);
    instruction2->user.value=*v4;

    //打印
    printf("%d: goto %d\n",instruction2->i,v4->pdata->var_pdata.iVal);

    //走完else部分的语句，即可知道从if为真中出来的下一条应该goto到什么位置
    create_instruction_list(root->right->right);         //if为假走的语句

    Value *v5=(Value*) malloc(sizeof (Value));
    value_init_int(v5, instruction_uid);
    instruction3->user.value=*v5;

    //打印
    printf("%d: goto %d\n",instruction3->i,v5->pdata->var_pdata.iVal);

    if(root->next!=NULL)
        create_instruction_list(root->next);
}

//TODO 还未完成
void create_func_def(past root) {
    Instruction *instruction_begin,*instruction_end;
    instruction_begin = ins_new_unary_operator(FunBegin, NULL);
    instruction_begin->Parent=block_one;

    //拿出存在符号表中的函数信息
    Value *v=symtab_lookup_withmap(this, bstr2cstr(root->left->next->sVal, '\0'), &this->value_maps->next->map);
    instruction_begin->user.value=*v;

    printf("%d: funcBegin %s\n",instruction_begin->i,instruction_begin->user.value.name);

    //将这个instruction加入总list
    InstNode *instNode_begin = new_inst_node(instruction_begin);
    ins_node_add(instruction_list,instNode_begin);

    //有参
    if (root->left->next->left != NULL) {
        //params走到第一个FuncParam处
        past params=root->left->next->left->left;

        //将参数全部生成给出来
        while(params!=NULL)
        {
            Value *v_index=create_tmp_value();
            //TODO 与符号表对应的绑定在一起
            //创造栈帧中value，并加进符号表
            Value *param= symtab_lookup_withmap(this,bstr2cstr(params->left->next->sVal, '\0'), symtab_param_map(this));
            param->v_alias=v_index;

            //打印
            //printf("give_param %s(%s)\n",v_index->name,param->name);
            params=params->next;
        }
    }
    t_index++;

    //读BlockItemList
    //flag为1表示是一个函数的开始
    create_blockItemList(root->right,1);

    //end_function
    instruction_end = ins_new_unary_operator(FunEnd, NULL);
    instruction_end->user.value=*v;
    instruction_end->Parent=block_one;
    InstNode *instNode_end = new_inst_node(instruction_end);
    ins_node_add(instruction_list,instNode_end);

    //打印
    printf("%d: funcEnd %s\n",instruction_end->i,instruction_end->user.value.name);

    //清空index
    t_index=0;

    if (root->next != NULL)
        create_instruction_list(root->next);
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
                        printf("%d: %s=add %s,%d\n",instruction_uid-1,v_tmp->name,v1->name,v2->pdata->var_pdata.iVal);
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
/*
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
    else if(strcmp(bstr2cstr(logic_expr->sVal, '\0'), "=") == 0)
        instruction= ins_new_binary_operator(EQUAL,v1,v2);
    else
        instruction= ins_new_binary_operator(NOTEQ,v1,v2);

    instruction->Parent=block_one;

    //左值value
    //临时变量左值
    Value *v_tmp=create_tmp_value();
    v_tmp->VTy->ID=Var_CONSTINT;

    //v_tmp是真值类型
    instruction->user.value=*v_tmp;

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    //返回左值即logic_expr的真值
    return v_tmp;
}*/

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

    instruction=ins_new_unary_operator(Load,v->v_alias);
    instruction->Parent = block_one;
    //最后会删除吧，自动创建instruction的user.value
    Value *v1=create_tmp_value();
    instruction->user.value=*v1;

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    //打印
    printf("%d: %s=load %s\n",instruction_uid-1,v1->name,v->v_alias->name);

    return v1;
}

//将v1的值存入v2的栈帧地址中
void create_store_stmt(Value* v1,Value* v2)
{
    Instruction *instruction=NULL;
    instruction= ins_new_binary_operator(Store,v1,v2->v_alias);
    instruction->Parent = block_one;

    //将这个instruction加入总list
    InstNode *node = new_inst_node(instruction);
    ins_node_add(instruction_list,node);

    //打印
    if(v1->name!=NULL)
        printf("%d: store %s %s\n",instruction_uid-1,v1->name,v2->v_alias->name);
    else
        printf("%d: store %d %s\n",instruction_uid-1,v1->pdata->var_pdata.iVal,v2->v_alias->name);
}

//第一次遍历
void declare_all_alloca(struct _mapList* func_map,bool flag)
{
    const char *key, *value;
    //先遍历本表
    sc_map_foreach (&func_map->map, key, value)
    {
        Instruction *instruction= ins_new_unary_operator(Alloca, NULL);
        Value *v_index=create_tmp_value();
        instruction->user.value=*v_index;
        //TODO 与符号表对应的绑定在一起
        ((Value*)value)->v_alias=v_index;
        //将这个instruction加入总list
        InstNode *node = new_inst_node(instruction);
        ins_node_add(instruction_list,node);

        //打印
        printf("%d: %s=alloca(%s)\n",instruction_uid-1,v_index->name,key);
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

        //打印
        printf("%d: give_param %d\n",instruction->i,v->pdata->var_pdata.iVal);

        params=params->next;
    }
}
