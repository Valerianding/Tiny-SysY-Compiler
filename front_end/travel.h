#ifndef TRAVEL_H
#define TRAVEL_H

#include "instruction.h"
#include "bblock.h"
#include "value_stack.h"
#include "past_stack.h"
#include "insnode_stack.h"
#include "bblock.h"
#include "symtab.h"
#include <stdio.h>
#include <stdbool.h>

void create_blockItemList(past root,Value* v_return);
void create_instruction_list(past root,Value* v_return);
void  create_assign_stmt(past root,Value* v_return);
void create_return_stmt(past root,Value* v_return);
void create_var_decl(past root,Value* v_return,bool is_global);
void create_if_stmt(past root,Value* v_return);
void create_if_else_stmt(past root,Value* v_return);
void create_func_def(past root);
struct _Value *create_call_func(past root);
struct _Value *cal_expr(past expr,int* convert);
struct _Value* cal_logic_expr(past logic_expr);
struct _Value *create_param_value();
void create_store_stmt(Value* v1,Value* v2);
struct _Value* create_load_stmt(char *name);
struct _Value* create_return_load(Value *v_return);
void declare_all_alloca(struct _mapList* func_map, bool flag);
void declare_global_alloca(struct _mapList* func_map);
void create_params_stmt(past func_params);
void printf_llvm_ir(struct _InstNode *instruction_node,char* file_name);
void create_while_stmt(past root,Value* v_return);
InstNode *true_location_handler(int type,Value *v_real,int true_goto_location);
InstNode *false_location_handler(int type,Value *v_real,int false_goto_location);
void create_continue_stmt(past root,Value* v_return);
void create_break_stmt(past root,Value* v_return);
void reduce_continue();
void reduce_break();
void reduce_return();
void reduce_and(int false_index);
void reduce_or(int true_index,int false_index);
void handle_global_array(Value* v_array,bool is_global,past vars);
void assign_global_array(past p,Value* v_array,int i,int level);
void fix_array(struct _InstNode *instruction_node);

void printf_array(Value *v_array, int begin_index,FILE* fptr);
void handle_one_dimention(past init_val_list,Value *v_array,Value* begin_offset_value,int start_layer,int cur_layer,int carry[]);

Value *handle_assign_array(past root,Value *v_array,int location,int dimension);      //0??????????????????1????????????;dimension?????????,-1?????????v_array????????????
past array_all_zeros(past init_val_list);

//??????-1,???????????????????????????????????????????????????index???0,1,2
void borrow_save(Value* v_array,int carry[]);

int handle_and_or(past root,bool flag);

bool begin_tmp(const char* name);
bool begin_global(const char* name);
char* no_global_name(const char *name);

char* c2ll(char* file_name);


//?????????
void print_array(struct _InstNode *instruction_node);

#endif