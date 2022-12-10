#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "user.h"
#include "liveness.h"
#include "./ir/variable_storage_space.h"

struct _BasicBlock;

typedef enum{
    Add,
    Sub,
    Mul,
    Div,
    Goto,
    IF_Goto,
    Call,
    FunBegin,
    FunEnd,
    Return,
    Assign,
};

struct _Instruction{
    User user;
    int Opcode;
    struct _BasicBlock *Parent;
    //DebugLoc DbgLoc;
    int i; //指令的编号
    livenessnode *out;
    //为三地址代码添加变量表，标记变量的存储位置
    enum _VariableStorageSpace* storageSpace;
};
typedef struct _Instruction Instruction;


Instruction *ins_new_binary_operator(int Op, Value *S1, Value *S2);
Instruction* ins_new(int op_num);
Instruction* ins_new_binary_operator(int Op, Value *S1, Value *S2);
Instruction *ins_new_unary_operator(int Op,Value *S1);
/* 目前只是暂且从parent的basicblock中取出来 */
Instruction *ins_remove_from_parent(Instruction *this);

Instruction *ins_insert_after(Instruction *this,Instruction *InsertPos);

Instruction *ins_move_before(Instruction *this,Instruction *MovePos);
/// Unlink this instruction from its current basic block and insert it into
/// the basic block that MovePos lives in, right after MovePos.
Instruction *ins_move_after(Instruction *this,Instruction *MovePos);

Instruction *ins_set_parent(Instruction *this,struct _BasicBlock *parent);


//https://llvm.org/doxygen/InstrTypes_8h_source.html
/// Construct a binary instruction, given the opcode and the two
/// operands.  Optionally (if InstBefore is specified) insert the instruction
/// into a BasicBlock right before the specified instruction.  The specified
/// Instruction is allowed to be a dereferenced end iterator.
#endif