#ifndef _VARIABLE_STORAGE_SPACE_H
#define _VARIABLE_STORAGE_SPACE_H

#include "../instruction.h"
#include <assert.h>

/*
* Instruction中存放一个VarieableStorageSpace枚举值类型的指针
*  其指向一个3个元素的VarieableStorageSpace数组
*  分别对应三地址代码的    结果操作数   操作数1   操作数2
*  记录变量存放的物理位置
*/

typedef enum _VariableStorageSpace
{
    IN_MEM = -1,  //内存
    R0,R1,R2,R3,R4,R5,R6,R7,R8,      //通用寄存器
    R9_SB,R10_SL,R11_FP,R12_IP,      //分组寄存器
    SP,                              //堆栈指针，最多允许六个不同的堆栈空间
    LR,                              //链接寄存器，子程序调用保存返回地址
    PC,
    CPSR,
    SPSR,
    IN_INSTRUCT                      //直接在指令中给出立即数
} VariableStorageSpace;

VariableStorageSpace get_target_num_storage_space(struct _Instruction* this);
VariableStorageSpace get_operand_storage_space(struct _Instruction* this,int order);
void set_storage_space_list(struct _Instruction* this,VariableStorageSpace tar,VariableStorageSpace op1,VariableStorageSpace op2);
void set_storage_space(struct _Instruction* this,VariableStorageSpace vss,int order);

#endif