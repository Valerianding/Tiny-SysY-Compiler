//
// Created by tom on 23-2-25.
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


int get_siezof_sp(InstNode*ins);
int get_value_pdata_inspdata_true(Value*value);
int get_value_pdata_inspdata_false(Value*value);
int get_value_pdata_varpdata_ival(Value*value);
float get_value_pdata_varpdata_fval(Value*value);
int get_value_alias(Value*value);
void get_value_name(Value*value,char name[]);
int get_value_offset(offset*head,Value*value);
void arm_translate_ins(InstNode *ins);
InstNode *_arm_translate_ins(InstNode *ins,InstNode *head,offset*offset_head);
bool is_int(Value*value);
bool is_float(Value*value);
InstNode * arm_trans_Add(InstNode *ins,offset*offset_head);
InstNode * arm_trans_Sub(InstNode *ins,offset*offset_head);
InstNode * arm_trans_Mul(InstNode *ins,offset*offset_head);
InstNode * arm_trans_Div(InstNode *ins,offset*offset_head);
InstNode * arm_trans_Module(InstNode *ins);
InstNode * arm_trans_Call(InstNode *ins);
InstNode * arm_trans_FunBegin(InstNode *ins,offset*offset_head);
InstNode * arm_trans_Return(InstNode *ins,InstNode *head);
InstNode * arm_trans_Store(InstNode *ins,offset*offset_head);
InstNode * arm_trans_Load(InstNode *ins,offset*offset_head);
InstNode * arm_trans_Alloca(InstNode *ins);
InstNode * arm_trans_GIVE_PARAM(InstNode *ins);
InstNode * arm_trans_ALLBEGIN(InstNode *ins);
InstNode * arm_trans_LESS(InstNode *ins);
InstNode * arm_trans_GREAT(InstNode *ins);
InstNode * arm_trans_LESSEQ(InstNode *ins);
InstNode * arm_trans_GREATEQ(InstNode *ins);
InstNode * arm_trans_EQ(InstNode *ins);
InstNode * arm_trans_NOTEQ(InstNode *ins);
InstNode * arm_trans_br_i1(InstNode *ins);
InstNode * arm_trans_br(InstNode *ins);
InstNode * arm_trans_br_i1_true(InstNode *ins);
InstNode * arm_trans_br_i1_false(InstNode *ins);
InstNode * arm_trans_Label(InstNode *ins);
InstNode * arm_trans_tmp(InstNode *ins);
InstNode * arm_trans_XOR(InstNode *ins);
InstNode * arm_trans_zext(InstNode *ins);
InstNode * arm_trans_bitcast(InstNode *ins);
InstNode * arm_trans_GMP(InstNode *ins,offset*head);
InstNode * arm_trans_MEMCPY(InstNode *ins);
InstNode * arm_trans_zeroinitializer(InstNode *ins);
InstNode * arm_trans_GLOBAL_VAR(InstNode *ins);
InstNode *arm_trans_Phi(InstNode *ins);
InstNode *arm_trans_MEMSET(InstNode *ins);
#endif //C22V1_ARM_H
