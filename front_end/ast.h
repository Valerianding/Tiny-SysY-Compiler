//
// Created by 12167 on 2022-10-18.
//

#ifndef LAB2_AST_H
#define LAB2_AST_H

#include "bstrlib.h"
#include "../ir/symtab.h"
//#include "travel.h"
typedef struct _ast ast;
typedef struct _ast *past;

struct _ast{
    union{                       //num_int,num_float具体的值
        int iVal;
        float fVal;
    };
    bstring sVal;                //有名称的node存一下名称，比如变量a的sVal是a
    bstring nodeType;            //节点类型
    past left;
    past next;
    past right;
};

past TRoot;

//创建节点
past newAstNode();

//在递归情况调用
past newFollowNode(char* nodeType,past thisNode,past follow);

//在大多数非递归情况下调用
past newAnotherNode(char* nodeType,past left,past right);

//将int等前缀包装成节点类型
past prefixNode(char *prefix,past more);

//将整数型的数包装成节点类型
past newNumInt(int value);

//将浮点型的浮点数包装成节点类型
past newNumFloat(float value);

//遇表达式调用
past newExpr(past left,int oper, past right);

//遇逻辑表达式调用
past newLogicExpr(past left,char* logicOper, past right);

//将变量名、函数名等名字包装成一个节点类型
past newIdent(bstring value);

//打印生成的语法树
void showAst(past node, int nest);

//将变量插入符号表
void insert_var_into_symtab(past type,past p);
//TODO 目前还未考虑数组

//将函数参数插入到对应的作用域表中
void insert_func_params(past params);

//将函数插入至对应的符号表
void insert_func_into_symtab(past return_type,past pname,past params);

//根据数组名，拿到具体数组需开辟的大小
int get_array_total_occupy(Value *a,int begin);

#endif

