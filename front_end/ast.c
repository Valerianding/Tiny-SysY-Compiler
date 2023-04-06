#include "ast.h"
#include "symtab.h"
#include "bblock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern Symtab *this;

past newAstNode()
{
    past node = malloc(sizeof(ast));
    if(node == NULL)
    {
        printf("run out of memory.\n");
        exit(0);
    }
    memset(node, 0, sizeof(ast));
    return node;
}

//处理终结符num_int
past newNumInt(int value)
{
    past var = newAstNode();
    var->nodeType = bfromcstr("num_int");
    var->iVal=value;
    return var;
}

//处理终结符num_float
past newNumFloat(float value)
{
    past var =newAstNode();
    var->nodeType = bfromcstr("num_float");
    var->fVal=value;
    return var;
}

//加减等表达式
past newExpr(past left,int oper, past right)
{
    past var = newAstNode();
    var->nodeType = bfromcstr("expr");
    var->iVal = oper;
    var->left = left;
    var->right = right;
    //用于区分是logicExpr还是普通expr
    var -> sVal = bfromcstr("#");
    return var;
}

//一些逻辑表达式
past newLogicExpr( past left,char* logic_oper,past right)
{
    past var = newAstNode();
    var->nodeType = bfromcstr("logic_expr");
    var -> sVal = bfromcstr(logic_oper);
    var->left = left;
    var->right = right;
    return var;
}

//走入下一大步(比如离开大段递归)，创一个全新结点
past newAnotherNode(char* nodeType, past left, past right)
{
    past root = newAstNode();
    root->nodeType = bfromcstr(nodeType);
    root->left = left;
    root->right = right;
    return root;
}

/*
//走入下一大步(比如离开大段递归)，创一个全新结点
past newUnaryNode(char* nodeType, int op, past right)
{
    past root = newAstNode();
    root->nodeType = bfromcstr(nodeType);
    root->left=NULL;
    root->iVal=op;
    root->right = right;
    return root;
}*/

//还处在这一大步，没有往下一大步走，适合于递归
past newFollowNode(char* nodeType, past thisNode, past follow)
{
    past root = NULL;
    //还没有根节点，比如递归刚刚开始
    /*
     * FuncFParams
    : FuncFParam                                   {$$ = newFollowNode("FuncFParams",$1,NULL);}       比如这行是递归刚刚开始
    | FuncFParams COMMA FuncFParam                 {$$ = newFollowNode("FuncFParams",$1,$3);}         这行是递归不是第一次了
    ;
     */
    bstring node_type= bfromcstr(nodeType);
    if(bstrcmp(node_type, thisNode->nodeType) != 0){
        root = newAstNode();
        root->nodeType = node_type;
        root->left = thisNode; root->left->next = follow;
    }

        //已经有根节点
        //不是第一次递归了，将下一次的follow存入next
    else{
        root = thisNode;
        thisNode = thisNode->left;
        while(thisNode->next != NULL)
        {
            thisNode = thisNode->next;
        }
        thisNode->next = follow;
    }
    return root;
}

//对于一些前缀(其实就是node_type，但是尔强当时没给)，比如const int，不如将它们用这样的结点存起来
//这个我也记不得后来在哪改了一下，反正理解的话就是相当于这一个node表明type，如果是const int目前做出了2个node，可以改
past prefixNode(char *prefix,past more)
{
    past root = newAstNode();
    bstring type;
    if(prefix==NULL)
    {
        type=more->sVal;
    }
    else
        type= bfromcstr(prefix);

    root->sVal=type;
    root->nodeType = bfromcstr("prefix");
    return root;
}

//处理终结符Ident
past newIdent(bstring strVal)
{
    past var = newAstNode();
    var->nodeType = bfromcstr("ID");
    var->sVal = strVal;
    return var;
}

int get_array_total_occupy(Value* a,int begin)
{
    //Value *a= symtab_lookup_withmap(this,name,map);
    int occupy=1;
    for(int i=begin;i<a->pdata->symtab_array_pdata.dimention_figure;i++)
    {
        occupy*=a->pdata->symtab_array_pdata.dimentions[i];
    }
    occupy*=4;
    return occupy;
}



//将var插入符号表
void insert_var_into_symtab(past type,past p)
{
    //int a;
    if(strcmp(bstr2cstr(p->nodeType,'\0'),"ID")==0)
    {
        Value *v=(Value*) malloc(sizeof (Value));
        value_init(v);
        if(is_global_map(this))
        {
            v->name=(char*) malloc(1+strlen(bstr2cstr(p->sVal,0)));
            strcpy(v->name,"@");
            strcat(v->name,bstr2cstr(p->sVal,0));
        }
        else
        {
            v->name=(char*) malloc(strlen(bstr2cstr(p->sVal,0)));
            strcpy(v->name,bstr2cstr(p->sVal,0));
        }
        v->pdata->var_pdata.map_list= getCurMapList(this);
        if(is_global_map(this))
            v->pdata->define_flag=1;
        else
            v->pdata->define_flag=0;
        if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
        {
            if(is_global_map(this))
                v->VTy->ID=GlobalVarFloat;
            else
                v->VTy->ID=Var_FLOAT;
        }
        else
        {
            if(is_global_map(this))
                v->VTy->ID=GlobalVarInt;
            else
                v->VTy->ID=Var_INT;
        }
        symtab_insert_value_name(this,bstr2cstr(p->sVal,0),v);
    }

    else if(strcmp(bstr2cstr(p->nodeType,'\0'),"IdentArray")==0)
    {
        Value *v=(Value*) malloc(sizeof (Value));
        value_init(v);

        if(is_global_map(this))
        {
            v->name=(char*) malloc(1+strlen(bstr2cstr(p->left->sVal,0)));
            strcpy(v->name,"@");
            strcat(v->name,bstr2cstr(p->left->sVal,0));
        }
        else
        {
            v->name=(char*) malloc(strlen(bstr2cstr(p->left->sVal,0)));
            strcpy(v->name,bstr2cstr(p->left->sVal,0));
        }

        v->pdata->symtab_array_pdata.map_list= getCurMapList(this);
        if(is_global_map(this))
            v->pdata->define_flag=1;
        else
            v->pdata->define_flag=0;

        if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
        {
            if(is_global_map(this))
                v->VTy->ID=GlobalArrayFloat;
            else
                v->VTy->ID=ArrayTy_FLOAT;
        }
        else
        {
            if(is_global_map(this))
                v->VTy->ID=GlobalArrayInt;
            else
                v->VTy->ID=ArrayTy_INT;
        }

        //v->VTy->ID=ArrayTyID;
        v->pdata->symtab_array_pdata.is_init=0;

        //加入维度具体数值
        past one_dimention = p->left->next;
        int dimention_figure=0;
        while(one_dimention!=NULL)
        {
            if(strcmp(bstr2cstr(one_dimention->nodeType,'\0'),"num_int")==0)
                v->pdata->symtab_array_pdata.dimentions[dimention_figure++]=one_dimention->iVal;
            else
                //是const常数或常数的expr
            {
                if(strcmp(bstr2cstr(one_dimention->nodeType,'\0'),"ID")==0)
                {
                    Value *v_id_n= symtab_dynamic_lookup_first(this,bstr2cstr(one_dimention->sVal,'\0'));
                    v->pdata->symtab_array_pdata.dimentions[dimention_figure++]=v_id_n->pdata->var_pdata.iVal;
                }
                else if(strcmp(bstr2cstr(one_dimention->left->nodeType,'\0'),"expr")==0)
                {
                    int result= cal_easy_expr(one_dimention);
                    v->pdata->symtab_array_pdata.dimentions[dimention_figure++]=result;
                }
            }
            one_dimention=one_dimention->next;
        }
        v->pdata->symtab_array_pdata.dimention_figure=dimention_figure;

        symtab_insert_value_name(this,bstr2cstr(p->left->sVal,0),v);
    }

        //int a=1型,有初值并放了初值
    else if(strcmp(bstr2cstr(p->nodeType,'\0'),"VarDef_init")==0)
    {
        Value *v=(Value*) malloc(sizeof (Value));
        value_init(v);
        v->pdata->var_pdata.map_list=getCurMapList(this);
        if(is_global_map(this))
            v->pdata->define_flag=1;
        else
            v->pdata->define_flag=0;

        //比如int a=8，为常数;其他复杂的情况在这一步暂时认为是无初值!
        if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"num_int")==0){
            v->pdata->var_pdata.iVal=p->right->iVal;
            if(is_global_map(this))
                v->VTy->ID=GlobalVarInt;
            else
                v->VTy->ID=Var_INT;
        }
        else if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"num_float")==0){
            v->pdata->var_pdata.fVal=p->right->fVal;
            if(is_global_map(this))
                v->VTy->ID=GlobalVarFloat;
            else
                v->VTy->ID=Var_FLOAT;
        }
        else if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"ID")==0)
        {
            Value *v_num= symtab_dynamic_lookup_first(this,bstr2cstr(p->right->sVal,'\0'));
            //非参数
            if(v_num!=NULL)
            {
                if(v_num->VTy->ID==Const_FLOAT)
                {
                    v->pdata->var_pdata.fVal=v_num->pdata->var_pdata.fVal;
                    //v->VTy->ID=Const_FLOAT;
                    if(is_global_map(this))
                        v->VTy->ID=GlobalVarFloat;
                    else
                        v->VTy->ID=Var_FLOAT;
                }
                    //Const_Int
                else if(v_num->VTy->ID==Const_INT)
                {
                    v->pdata->var_pdata.iVal=v_num->pdata->var_pdata.iVal;
                    //v->VTy->ID=Const_INT;
                    if(is_global_map(this))
                        v->VTy->ID=GlobalVarInt;
                    else
                        v->VTy->ID=Var_INT;
                }
                else
                {
                    if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
                    {
                        if(is_global_map(this))
                            v->VTy->ID=GlobalVarFloat;
                        else
                            v->VTy->ID=Var_FLOAT;
                    }
                    else
                    {
                        if(is_global_map(this))
                            v->VTy->ID=GlobalVarInt;
                        else
                            v->VTy->ID=Var_INT;
                    }
                }
            }
            //参数
            else
            {
                if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
                {
                    if(is_global_map(this))
                        v->VTy->ID=GlobalVarFloat;
                    else
                        v->VTy->ID=Var_FLOAT;
                }
                else
                {
                    if(is_global_map(this))
                        v->VTy->ID=GlobalVarInt;
                    else
                        v->VTy->ID=Var_INT;
                }
            }
        }
        //是expr
        else if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"expr")==0)
        {
            if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
            {
                if(is_global_map(this))
                    v->VTy->ID=GlobalVarFloat;
                else
                    v->VTy->ID=Var_FLOAT;
            }
            else
            {
                if(is_global_map(this))
                    v->VTy->ID=GlobalVarInt;
                else
                    v->VTy->ID=Var_INT;
            }
        }
        else
        {
            if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
            {
                if(is_global_map(this))
                    v->VTy->ID=GlobalVarFloat;
                else
                    v->VTy->ID=Var_FLOAT;
            }
            else
            {
                if(is_global_map(this))
                    v->VTy->ID=GlobalVarInt;
                else
                    v->VTy->ID=Var_INT;
            }
        }

        if(is_global_map(this))
        {
            v->name=(char*) malloc(1+strlen(bstr2cstr(p->left->sVal,0)));
            strcpy(v->name,"@");
            strcat(v->name,bstr2cstr(p->left->sVal,0));
        }
        else
        {
            v->name=(char*) malloc(strlen(bstr2cstr(p->left->sVal,0)));
            strcpy(v->name,bstr2cstr(p->left->sVal,0));
        }
        symtab_insert_value_name(this,bstr2cstr(p->left->sVal,0),v);
    }

        //TODO 怎么存初始化的值{1,2,3,4}
    else if((strcmp(bstr2cstr(p->nodeType,'\0'),"VarDef_array_init")==0) || (strcmp(bstr2cstr(p->nodeType,'\0'),"ConstDef_array_init")==0))
    {
        Value *v=(Value*) malloc(sizeof (Value));
        value_init(v);
        if(is_global_map(this))
            v->pdata->define_flag=1;
        else
            v->pdata->define_flag=0;
        v->pdata->symtab_array_pdata.is_init=1;

        if(is_global_map(this))
        {
            v->name=(char*) malloc(1+sizeof (bstr2cstr(p->left->left->sVal, '\0')));
            strcpy(v->name,"@");
            strcat(v->name,bstr2cstr(p->left->left->sVal,'\0'));
        }
        else
        {
            v->name=(char*) malloc(strlen(bstr2cstr(p->left->left->sVal,0)));
            strcpy(v->name,bstr2cstr(p->left->left->sVal,0));
        }

        if(strcmp(bstr2cstr(p->nodeType,'\0'),"ConstDef_array_init")==0) {
            if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
            {
                if(is_global_map(this))
                    v->VTy->ID=GlobalArrayConstFLOAT;
                else
                    v->VTy->ID=ArrayTyID_ConstFLOAT;
            }
            else {
                if(is_global_map(this))
                    v->VTy->ID=GlobalArrayConstINT;
                else
                    v->VTy->ID=ArrayTyID_ConstINT;
            }
            //v->VTy->ID=ArrayTyID_Const;
            v->pdata->symtab_array_pdata.is_init=1;
        }
        else
        {
            if(is_global_map(this)){
                if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
                    v->VTy->ID=GlobalArrayFloat;
                else
                    v->VTy->ID=GlobalArrayInt;
            }
            else{
                if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
                    v->VTy->ID=ArrayTy_FLOAT;
                else
                    v->VTy->ID=ArrayTy_INT;
            }
            v->pdata->symtab_array_pdata.is_init=1;
        }
            //v->VTy->ID=ArrayTyID_Init;
        v->pdata->symtab_array_pdata.map_list= getCurMapList(this);

        //加入维度具体数值
        past one_dimention = p->left->left->next;
        int dimention_figure=0;
        while(one_dimention!=NULL)
        {
            if(strcmp(bstr2cstr(one_dimention->nodeType,'\0'),"num_int")==0)
                v->pdata->symtab_array_pdata.dimentions[dimention_figure++]=one_dimention->iVal;
            else if(strcmp(bstr2cstr(one_dimention->nodeType,'\0'),"expr")==0)
            {
                int result= cal_easy_expr(one_dimention);
                v->pdata->symtab_array_pdata.dimentions[dimention_figure++]=result;
            }
            else if(strcmp(bstr2cstr(one_dimention->nodeType,'\0'),"ID")==0)
            {
                Value *v_con= symtab_dynamic_lookup_first(this,bstr2cstr(one_dimention->sVal,'\0'));
                v->pdata->symtab_array_pdata.dimentions[dimention_figure++]=v_con->pdata->var_pdata.iVal;
            }
            one_dimention=one_dimention->next;
        }
        v->pdata->symtab_array_pdata.dimention_figure=dimention_figure;

        symtab_insert_value_name(this,bstr2cstr(p->left->left->sVal,0),v);
    }
    else if(strcmp(bstr2cstr(p->nodeType,'\0'),"ConstDef")==0)
    {
        Value *v=(Value*) malloc(sizeof (Value));
        value_init(v);
        v->pdata->var_pdata.map_list= getCurMapList(this);
        if(is_global_map(this))
            v->pdata->define_flag=1;
        else
            v->pdata->define_flag=0;

        if(is_global_map(this))
        {
            v->name=(char*) malloc(1+strlen(bstr2cstr(p->left->sVal,0)));
            strcpy(v->name,"@");
            strcat(v->name,bstr2cstr(p->left->sVal,0));
        }
        else
        {
            v->name=(char*) malloc(strlen(bstr2cstr(p->left->sVal,0)));
            strcpy(v->name,bstr2cstr(p->left->sVal,0));
        }

        if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"num_int")==0){
            v->pdata->var_pdata.iVal=p->right->iVal;
            v->VTy->ID=Const_INT;
        }
        else if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"num_float")==0 && strcmp(bstr2cstr(type->sVal,'\0'),"float")==0){
            v->pdata->var_pdata.fVal=p->right->fVal;
            v->VTy->ID=Const_FLOAT;
        }
        else if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"num_float")==0)
        {
            v->pdata->var_pdata.iVal=(int)p->right->fVal;
            v->VTy->ID=Const_INT;
        }
        else if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"ID")==0)
        {
            Value *v_num= symtab_dynamic_lookup_first(this,bstr2cstr(p->right->sVal,'\0'));
            if(v_num->VTy->ID==Const_FLOAT)
            {
                v->pdata->var_pdata.fVal=v_num->pdata->var_pdata.fVal;
                v->VTy->ID=Const_FLOAT;
            }
                //Const_Int
            else
            {
                v->pdata->var_pdata.iVal=v_num->pdata->var_pdata.iVal;
                v->VTy->ID=Const_INT;
            }
        }

        else if(strcmp(bstr2cstr(p->right->nodeType,'\0'),"expr")==0)
        {
            if(strcmp(bstr2cstr(type->sVal,'\0'),"float")==0)
            {
                float result= cal_easy_expr_f(p->right);
                v->pdata->var_pdata.fVal=result;
                v->VTy->ID=Const_FLOAT;
            }
            else
            {
                int result= cal_easy_expr(p->right);
                v->pdata->var_pdata.iVal=result;
                v->VTy->ID=Const_INT;
            }
        }

        symtab_insert_value_name(this,bstr2cstr(p->left->sVal,0),v);
    }

    if(p->next!=NULL)
        insert_var_into_symtab(type,p->next);
}

int cal_easy_expr(past expr)
{
    stack *s=stackInit();  //用于后序遍历二叉树
    stack *S=stackInit();
    past p1 = expr;
    past q = NULL;     //记录刚刚访问过的结点
    //记录后缀表达式
    past str[100];
    int i = 0;

    while (p1 != NULL || stackSize(s))
    {
        if (p1 != NULL)
        {
            stackPush(s, p1);
            p1 = p1->left;
        }
        else
        {
            stackTop(s, (void**)&p1);     //往上走了才pop掉
            if ((p1->right == NULL) || (p1->right) == q)
            {
                //开始往上走
                q = p1;              //保存到q，作为下一次处理结点的前驱
                stackTop(s, (void**)&p1);
                stackPop(s);
                str[i++] = p1;
                p1 = NULL;         //p置于NULL可继续退层，否则会重复访问刚访问结点的左子树
            }
            else
                p1 = p1->right;
        }
    }
    str[i]=NULL;

    int *x1= malloc(4),*x2= malloc(4);
    past *p=str;
    while(*p)
    {
        int *data= malloc(4) ;
        if(strcmp(bstr2cstr((*p)->nodeType, '\0'), "expr") != 0)
        {
            if(strcmp(bstr2cstr((*p)->nodeType, '\0'), "num_int") == 0)
                *data=(*p)->iVal;
            else if(strcmp(bstr2cstr((*p)->nodeType, '\0'), "num_float") == 0)
                *data=(int)(*p)->fVal;
            //CONST_INT
            else
            {
                Value *v= symtab_dynamic_lookup_first(this,bstr2cstr((*p)->sVal, '\0'));
                if(v->VTy->ID==Const_INT)
                    *data=v->pdata->var_pdata.iVal;
                else
                    *data=(int)v->pdata->var_pdata.fVal;
            }

            stackPush(S,data);
        }

        else
        {
            int *result= malloc(4);
            stackTop(S, (void*)&x2);
            stackPop(S);
            stackTop(S, (void*)&x1);
            stackPop(S);
            switch((*p)->iVal)
            {
                case '+': *result = *x1 + *x2; break;
                case '-': *result = *x1 - *x2; break;
                case '*': *result = *x1 * *x2; break;
                case '/': *result = *x1 / *x2; break;
                case '%': *result = *x1 % *x2; break;
            }
            stackPush(S,result);
        }
        p++;
    }
    int *result= malloc(4);
    stackTop(S,(void*)&result);
    stackPop(S);
    return *result;
}

float cal_easy_expr_f(past expr)
{
    stack *s=stackInit();  //用于后序遍历二叉树
    stack *S=stackInit();
    past p1 = expr;
    past q = NULL;     //记录刚刚访问过的结点
    //记录后缀表达式
    past str[100];
    int i = 0;

    while (p1 != NULL || stackSize(s))
    {
        if (p1 != NULL)
        {
            stackPush(s, p1);
            p1 = p1->left;
        }
        else
        {
            stackTop(s, (void**)&p1);     //往上走了才pop掉
            if ((p1->right == NULL) || (p1->right) == q)
            {
                //开始往上走
                q = p1;              //保存到q，作为下一次处理结点的前驱
                stackTop(s, (void**)&p1);
                stackPop(s);
                str[i++] = p1;
                p1 = NULL;         //p置于NULL可继续退层，否则会重复访问刚访问结点的左子树
            }
            else
                p1 = p1->right;
        }
    }
    str[i]=NULL;

    past x1;past x2;
    past *p=str;
    while(*p)
    {
        if(strcmp(bstr2cstr((*p)->nodeType, '\0'), "expr") != 0)
        {
            stackPush(S,(*p));
        }

        else
        {
            stackTop(S, (void*)&x2);
            stackPop(S);
            stackTop(S, (void*)&x1);
            stackPop(S);
            float *result= malloc(4);
            switch((*p)->iVal)
            {
                case '+':
                    if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_float") == 0)
                        *result = x1->fVal + x2->fVal;
                    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_int") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_float") == 0)
                        *result=(float )x1->iVal+x2->fVal;
                    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_int") == 0)
                        *result=x1->fVal+(float )x2->iVal;

                    //else if()
                    break;
                case '-':
                    if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_float") == 0)
                        *result = x1->fVal - x2->fVal;
                    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_int") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_float") == 0)
                        *result=(float )x1->iVal-x2->fVal;
                    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_int") == 0)
                        *result=x1->fVal-(float )x2->iVal;
                    //else if()
                    break;
                case '*':
                    if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_float") == 0)
                        *result = x1->fVal * x2->fVal;
                    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_int") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_float") == 0)
                        *result=(float )x1->iVal*x2->fVal;
                    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_int") == 0)
                        *result=x1->fVal*(float )x2->iVal;
                    //else if()
                    break;
                case '/':
                    if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_float") == 0)
                        *result = x1->fVal / x2->fVal;
                    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_int") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_float") == 0)
                        *result=(float )x1->iVal/x2->fVal;
                    else if(strcmp(bstr2cstr(x1->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(x2->nodeType, '\0'), "num_int") == 0)
                        *result=x1->fVal/(float )x2->iVal;
                    //else if()
                    break;
            }
            past p_result= newNumFloat((*result));
            stackPush(S,p_result);
        }
        p++;
    }
    past final_result= malloc(4);
    stackTop(S,(void*)&final_result);
    stackPop(S);
    return final_result->fVal;
}

//目前还未考虑数组
//将函数参数插入函数名的下一级符号表
void insert_func_params(past params)
{
    struct sc_map_sv *func_map=symtab_get_latest_func_map(this);
    while (params!=NULL)
    {
        Value *v=(Value*) malloc(sizeof (Value));
        value_init(v);
        //right不为NULL，则说明参数是数组
        if(params->right!=NULL)
        {
            v->pdata->define_flag=1;
            //一维数组
            if(strcmp(bstr2cstr(params->right->nodeType,'\0'),"num_int")==0)
            {
                v->VTy->ID=AddressTyID;
                v->pdata->symtab_array_pdata.dimention_figure=1;
            }
            //多维数组
            else{
                v->pdata->symtab_array_pdata.dimention_figure=1;
                v->VTy->ID=AddressTyID;
                past get_dimension=params->right->left;        //到达num_int
                while(get_dimension!=NULL)
                {
                    v->pdata->symtab_array_pdata.dimentions[v->pdata->symtab_array_pdata.dimention_figure]=get_dimension->iVal;
                    v->pdata->symtab_array_pdata.dimention_figure++;
                    get_dimension=get_dimension->next;
                }
            }
            if(strcmp(bstr2cstr(params->left->sVal,'\0'),"float")==0)
                v->pdata->symtab_array_pdata.address_type=1;
            else
                v->pdata->symtab_array_pdata.address_type=0;
        }
        else
        {
            v->pdata->var_pdata.map_list= getCurMapList(this);
            v->pdata->define_flag=1;
            if(strcmp(bstr2cstr(params->left->sVal,'\0'),"float")==0)
                //函数参数默认为有初始值
            {v->VTy->ID=Param_FLOAT;}
            else
            {v->VTy->ID=Param_INT;}
        }
        v->name=(char*) malloc(sizeof(bstr2cstr(params->left->next->sVal,0)));
        strcpy(v->name,bstr2cstr(params->left->next->sVal,0));
        symtab_insert_withmap(this,func_map,bstr2cstr(params->left->next->sVal,0),v);

        params=params->next;
    }
}

//将函数插入符号表，目前是
void insert_func_into_symtab(past return_type,past pname,past params)
{
    Value *v=(Value*) malloc(sizeof (Value));
    value_init(v);
    v->VTy->ID=FunctionTyID;
    //!!!!!它指向的map是函数体的map，不是函数名的map,函数名永远在全局Map中
    v->pdata->symtab_func_pdata.map_list= symtab_get_latest_func_maplist(this);

    //return_type
    if(strcmp(bstr2cstr(return_type->sVal,'\0'),"float")==0)
        //TODO 分不清var_constint和var_int了
    {v->pdata->symtab_func_pdata.return_type.ID=Var_FLOAT;}
    else if(strcmp(bstr2cstr(return_type->sVal,'\0'),"int")==0)
    {v->pdata->symtab_func_pdata.return_type.ID=Var_INT;}
    else
        v->pdata->symtab_func_pdata.return_type.ID=VoidTyID;

    //将参数全部添加完毕,加入参数的类型
    int i=0;
    while(params!=NULL)
    {
        if(params->right!=NULL)
            v->pdata->symtab_func_pdata.param_type_lists[i++].ID=AddressTyID;
        else if(strcmp(bstr2cstr(params->left->sVal,'\0'),"float")==0)
        {v->pdata->symtab_func_pdata.param_type_lists[i++].ID=Var_FLOAT;}
        else
        {v->pdata->symtab_func_pdata.param_type_lists[i++].ID=Var_INT;}

        params=params->next;
    }
    //TODO num不确定最终要不要
    v->pdata->symtab_func_pdata.param_num=i;

    v->name=(char*) malloc(sizeof(bstr2cstr(pname->sVal,0)));
    strcpy(v->name,bstr2cstr(pname->sVal,0));
    symtab_insert_value_name(this, bstr2cstr(pname->sVal,0),v);
}

void showAst(past node, int layer)
{
    if(node == NULL) {
        return;
    }
    int i ;
    for(i = 0; i < layer; i ++)
        printf("  ");
    if(strcmp(bstr2cstr(node->nodeType,'\0'), "expr") == 0){
        printf("%s  %c\n", node->nodeType->data, (char)node->iVal);
    }
/*    else if(strcmp(bstr2cstr(node->nodeType,'\0'), "UnaryExpr") == 0){
        printf("%s  %c\n", node->nodeType->data, (char)node->iVal);
    }*/
    else if(strcmp(bstr2cstr(node->nodeType,'\0'),"num_int")==0){
        printf("%s  %d\n", node->nodeType->data, node->iVal);
    }
    else if(strcmp(bstr2cstr(node->nodeType,'\0'),"num_float")==0){
        printf("%s  %f\n", node->nodeType->data, node->fVal);
    }
    else{
        if(!node->sVal){
            printf("%s \n", node->nodeType->data);
        }
        else printf("%s %s \n", node->nodeType->data, node->sVal->data);
    }
    showAst(node->left, layer+1);
    showAst(node->right, layer+1);
    showAst(node->next,layer);
}