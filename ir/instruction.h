#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "user.h"
#include "../cds.h"

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
    Label,
    tmp,

    bitcast,
    //memcpy,
    GMP,
};

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

struct _Instruction{
    User user;
    int Opcode;
    struct _BasicBlock *Parent;
    enum InstructionType type;
    int i; //指令的编号

    HashMap *value_VarSpace;  //key: value*    value: VarSpace
};
typedef struct _Instruction Instruction;


Instruction *ins_new_binary_operator(int Op, Value *S1, Value *S2);
Instruction* ins_new(int op_num);
Instruction* ins_new_binary_operator(int Op, Value *S1, Value *S2);
Instruction *ins_new_unary_operator(int Op,Value *S1);
void print_ins_opcode(Instruction *this);


/// Unlink this instruction from its current basic block and insert it into
/// the basic block that MovePos lives in, right after MovePos.
Instruction *ins_set_parent(Instruction *this,struct _BasicBlock *parent);
//https://llvm.org/doxygen/InstrTypes_8h_source.html
/// Construct a binary instruction, given the opcode and the two
/// operands.  Optionally (if InstBefore is specified) insert the instruction
/// into a BasicBlock right before the specified instruction.  The specified
/// Instruction is allowed to be a dereferenced end iterator.
#endif