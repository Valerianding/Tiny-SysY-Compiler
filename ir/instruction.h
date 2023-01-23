#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "user.h"
#include "cds.h"

struct _BasicBlock;

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
}Opcode;

enum InstructionType{
    FUNCDEF,
    CAL,
};

typedef enum _RegorMem
{
    IN_REGISTERRT_TEST,
    IN_MEMORY_TEST,
    IN_INSTRUCTION_TEST
} RegorMem;

typedef struct _VarSpace
{
    bool isLive;
    RegorMem place;
    int order;

} VarSpace;

typedef struct _Instruction{
    User user;
    int Opcode;
    struct _BasicBlock *Parent;
    enum InstructionType type;
    int i; //指令的编号

    HashMap *value_VarSpace;  //key: value*    value: VarSpace
}Instruction;


Instruction *ins_new_binary_operator(int Op, Value *S1, Value *S2);
Instruction* ins_new(int op_num);
Instruction* ins_new_binary_operator(int Op, Value *S1, Value *S2);
Instruction *ins_new_unary_operator(int Op,Value *S1);
Instruction *ins_set_parent(Instruction *this,struct _BasicBlock *parent);
Value *ins_get_value(Instruction *ins);
void print_ins_opcode(Instruction *this);
#endif