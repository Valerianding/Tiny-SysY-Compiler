#include "variable_storage_space.h"


VariableStorageSpace* get_storage_space_list(struct _Instruction* this)
{
    /* 返回三地址代码的操作数位置列表 */
    return this->storageSpace;
}

VariableStorageSpace get_target_num_storage_space(struct _Instruction* this)
{
    /* 返回变量的结果操作数的位置 */
    return this->storageSpace[0];
}

VariableStorageSpace get_operand_storage_space(struct _Instruction* this,int order)
{
    /* 按下标返回操作数的位置 */
    return this->storageSpace[order];
}


void set_storage_space_list(struct _Instruction* this,VariableStorageSpace tar,VariableStorageSpace op1,VariableStorageSpace op2)
{
    /* 设置Instruction 操作数位置列表 包含空指针内存分配逻辑 */
    if(!this->storageSpace)
        this->storageSpace = (VariableStorageSpace*)malloc(sizeof(VariableStorageSpace)*3);
    assert(this->storageSpace && "Error:Failed to attribute memory for instruction->storagePlace");
    this->storageSpace[0] = tar;
    this->storageSpace[1] = op1;
    this->storageSpace[2] = op2;
}

void set_storage_space(struct _Instruction* this,VariableStorageSpace vss,int order)
{
    /* 按下标设置Instruction 操作数位置 包含内存分配 */
    if(!this->storageSpace)
        this->storageSpace = (VariableStorageSpace*)malloc(sizeof(VariableStorageSpace)*3);
    assert(this->storageSpace && "Error:Failed to attribute memory for instruction->storagePlace");
    this->storageSpace[order]=vss;
}






