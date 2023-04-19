#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "user.h"
#include "cds.h"


typedef enum OpcodeType{
    Add = 0,
    Sub = 1,
    Mul = 2,
    Div = 3,
    Mod = 4,
    Call = 5,
    FunBegin = 6,
    Return = 7,
    Store = 8,
    Load = 9,
    Alloca = 10,
    GIVE_PARAM = 11,
    ALLBEGIN = 12,
    LESS = 13,
    GREAT = 14,
    LESSEQ = 15,
    GREATEQ = 16,
    EQ = 17,
    NOTEQ = 18,
    Br_i1 = 19,
    Br = 20,
    br_i1_true = 21,
    br_i1_false = 22,
    Label = 23,
    tmp = 24,
    XOR = 25,
    zext = 26,
    bitcast = 27,
    GEP = 28,
    MEMCPY = 29,
    MEMSET = 30,
    zeroinitializer = 31,
    GLOBAL_VAR = 32,
    FunEnd = 33,
    Phi = 34,
    CopyOperation = 35,  //消除phi函数
}Opcode;


typedef struct _Instruction{
    User user;
    int Opcode;
    struct _BasicBlock *Parent;
    int _reg_[3];
    int i; //指令的编号

}Instruction;


Instruction* ins_new(int op_num);
Instruction* ins_new_binary_operator(int Op, Value *S1, Value *S2);
Instruction *ins_new_unary_operator(int Op,Value *S1);
Instruction *ins_new_zero_operator(int Op);
void ins_set_parent(Instruction *this,struct _BasicBlock *parent);
// a = b + c 获取a
Value *ins_get_dest(Instruction *ins);
void print_ins_opcode(Instruction *this);
void clear_tmp(char* tmp);
Value *ins_get_value_with_name(Instruction *ins);
//获取全局value并赋个name
Value *ins_get_global_value(Instruction *ins,char* name);
// a = b + c 获取b
// a = b 获取b
Value *ins_get_lhs(Instruction *ins);
// a = b + c 获取c
Value *ins_get_rhs(Instruction *ins);
#endif