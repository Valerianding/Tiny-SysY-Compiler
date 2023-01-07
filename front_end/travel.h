#include "instruction.h"
#include "bblock.h"
#include "past_stack.h"
#include "bblock.h"
#include "symtab.h"
#include <stdio.h>
#include <stdbool.h>

void create_blockItemList(past root,int flag_func,int flag_main);
void create_instruction_list(past root);
void  create_assign_stmt(past root);
void create_return_stmt(past root);
void create_var_decl(past root);
void create_if_stmt(past root);
void create_if_else_stmt(past root);
void create_func_def(past root);
struct _Value *create_call_func(past root);
struct _Value *cal_expr(past expr);
struct _Value* cal_logic_expr(past logic_expr);
void clear_tmp(char* tmp);
struct _Value *create_tmp_value();
void create_store_stmt(Value* v1,Value* v2);
struct _Value* create_load_stmt(char *name);
Value * declare_all_alloca(struct _mapList* func_map, bool flag, bool flag_main);
void create_params_stmt(past func_params);
void printf_llvm_ir(struct _InstNode *instruction_node);
void create_while_stmt(past root);
InstNode *true_location_handler(int type,Value *v_real,int true_goto_location);
