#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "user.h"
#include "cds.h"


typedef enum OpcodeType{
    Add,
    Sub,
    Mul,
    Div,
    Module,
    Call,
    FunBegin,
    Return,
    Store,
    Load,
    Alloca,
    GIVE_PARAM,
    ALLBEGIN,

    LESS,
    GREAT,
    LESSEQ,
    GREATEQ,
    EQ,
    NOTEQ,
    br_i1,
    br,
    br_i1_true,
    br_i1_false,
    Label,
    tmp,
    XOR,
    zext,
    bitcast,
    GMP,
    MEMCPY,
    MEMSET,
    zeroinitializer,
    GLOBAL_VAR,

    Phi,
}Opcode;


typedef struct _Instruction{
    User user;
    int Opcode;
    struct _BasicBlock *Parent;
    int i; //指令的编号
}Instruction;


Instruction* ins_new(int op_num);
Instruction* ins_new_binary_operator(int Op, Value *S1, Value *S2);
Instruction *ins_new_unary_operator(int Op,Value *S1);
void ins_set_parent(Instruction *this,struct _BasicBlock *parent);
Value *ins_get_value(Instruction *ins);
void print_ins_opcode(Instruction *this);
void clear_tmp(char* tmp);
Value *ins_get_value_with_name(Instruction *ins);
#endif