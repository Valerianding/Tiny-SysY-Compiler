//
// Created by ljf on 23-2-25.
//
#ifndef C22V1_ARM_H
#define C22V1_ARM_H
#include "instruction.h"
#include "bblock.h"
#include "value.h"
#include "type.h"
#include "user.h"
#include "function.h"
#include "offset.h"
#include "travel.h"


/**
 * 判断立即数int是否有效
 * @param value
 * @return
 */
bool imm_is_valid(unsigned value);

void arm_open_file(char argv[]);
void arm_close_file();
/**
 * @details 获取哈希表的大小
 * @param hashMap
 */
int get_siezof_sp(HashMap*hashMap);

/**
 * @details 获取变量相对于栈帧首地址的偏移量
 * @param hashMap
 * @param value key
 * @return 返回偏移量，负数为参数，正数为临时变量
 */
int get_value_offset_sp(HashMap *hashMap,Value*value);

/**
 * @details 进行指令的翻译，总体翻译逻辑
 * @param ins
 */
void arm_translate_ins(InstNode *ins);

/**
 * @details 进行每条具体指令的翻译
 * @param ins
 * @param head 用来记录FunBegin时的函数名，确定当前是main函数还是其他函数，main函数和其他函数的return语句翻译不一样
 * @param hashMap 辅助翻译
 * @return
 */
InstNode *_arm_translate_ins(InstNode *ins,InstNode *head,HashMap*hashMap,int stack_size);


/**
 * @details 获取跳转位置，分别获取true和false
 * @param value
 * @return
 */
int get_value_pdata_inspdata_true(Value*value);
int get_value_pdata_inspdata_false(Value*value);


/**
 * @details 翻译函数开始的参数，需要把传过来的参数str，这个和普通的str指令是有区别的，所以单独处理，在FuncBegin处理时用到该函数
 * @param hashMap
 * @param value
 * @param name
 * @param ri
 */
void give_param_str(HashMap*hashMap,Value*value,char *name,int *ri);


/**
 * 具体对于不同指令进行翻译，对于一些还没有处理的或者说不用处理的指令直接printf指令名（后续要处理或删掉）
 * @param ins
 * @return
 */

InstNode * arm_trans_Add(InstNode *ins,HashMap*hashMap);
InstNode * arm_trans_Sub(InstNode *ins,HashMap*hashMap);
InstNode * arm_trans_Mul(InstNode *ins,HashMap*hashMap);
InstNode * arm_trans_Div(InstNode *ins,HashMap*hashMap);
InstNode * arm_trans_Module(InstNode *ins,HashMap*hashMap);
InstNode * arm_trans_Call(InstNode *ins,HashMap*hashMap);
InstNode * arm_trans_FunBegin(InstNode *ins,int*stack_size);
InstNode * arm_trans_Return(InstNode *ins,InstNode *head,HashMap*hashMap,int stack_size);

//load和store指令已经被删除掉，对于数组是会有的,对于全局变量也是会有的，全局变量在用之前都是会被load的。
InstNode * arm_trans_Store(InstNode *ins,HashMap *hashMap);
InstNode * arm_trans_Load(InstNode *ins,HashMap *hashMap);


InstNode * arm_trans_Alloca(InstNode *ins,HashMap*hashMap);
InstNode * arm_trans_GIVE_PARAM(HashMap *hashMap,int param_num);
InstNode * arm_trans_ALLBEGIN(InstNode *ins);
InstNode * arm_trans_LESS_GREAT_LEQ_GEQ_EQ_NEQ(InstNode *ins,HashMap*hashMap);
InstNode * arm_trans_CopyOperation(InstNode*ins,HashMap*hashMap);
InstNode * arm_trans_br_i1(InstNode *ins);
InstNode * arm_trans_br(InstNode *ins);
InstNode * arm_trans_br_i1_true(InstNode *ins);
InstNode * arm_trans_br_i1_false(InstNode *ins);
InstNode * arm_trans_Label(InstNode *ins);
InstNode * arm_trans_tmp(InstNode *ins);
InstNode * arm_trans_XOR(InstNode *ins);
InstNode * arm_trans_zext(InstNode *ins);
InstNode * arm_trans_bitcast(InstNode *ins);
InstNode * arm_trans_GMP(InstNode *ins,HashMap *hashMap);
InstNode * arm_trans_MEMCPY(InstNode *ins);
InstNode * arm_trans_zeroinitializer(InstNode *ins);
InstNode * arm_trans_GLOBAL_VAR(InstNode *ins);
InstNode *arm_trans_Phi(InstNode *ins);
InstNode *arm_trans_MEMSET(InstNode *ins);
void FuncBegin_hashmap_add(HashMap*hashMap,Value *value,char *name,int *local_stack);
void FuncBegin_hashmap_alloca_add(HashMap*hashMap,Value *value,int *local_stack);
void FuncBegin_hashmap_bitcast_add(HashMap*hashMap,Value *value0,Value *value1,int *local_stack);
void usage_of_global_variables();
int array_suffix(Value*array,int which_dimension);
void multiply_and_add_instructions_for_translated_arrays(InstNode*instNode,HashMap*hashMap);
void handle_reg_save(int reg);
void printf_stmfd_rlist();
void printf_ldmfd_rlist();
#endif //C22V1_ARM_H
