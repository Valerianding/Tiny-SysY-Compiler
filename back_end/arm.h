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

#endif //C22V1_ARM_H
