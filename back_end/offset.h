//
// Created by ljf on 23-2-25.
//

#ifndef C22V1_OFFSET_H
#define C22V1_OFFSET_H
#include "bblock.h"
#include "hash_map.h"
#include "ast.h"
#include "ir/opt/scev.h"
/**
 * @struct  使用哈希表来存放栈帧的信息key=Value*,value=offset*
 * @details regr通用寄存器标号
 * @details regs浮点寄存器标号s0-s31
 * @details memory 表示是否在内存中，true表示在内存中，false不在内存（即表示在寄存器中）
 * @details 如果是float型使用regs标识寄存器，int型使用regr标识寄存器
 */
typedef struct _offset{
    int offset_sp;
    int regr;
    int regs;
    bool memory;
//    true代表int，false代表float
    bool INTTRUE__FLOATFALSE;

//    int load;
//    int store;
}offset;


/**
 * @details 全局变量使用一个单独的HashMap来进行存放
 * @details 从value* 到 .LCPT%d_%d的映射
 * @details 第一个%d是用来区分函数的，第二个%d是用来区分函数内的标号的
 */

//LCPILabel
typedef struct _LCPILabel{
    char LCPI[100];
    bool INTTRUE__FLOATFALSE;//false表示为float类型，true表示为int类型
//    bool memory;
//    int regr;
//    int load;
//    int store;
}LCPTLabel;

/**
 * @details 初始化一个offset节点
 * @return offset指针
 */
offset *offset_node();



/**
 * @details 函数开始的时候，开辟栈帧，创建哈希表
 * @param ins
 * @return 哈希表
 */
HashMap *offset_init(InstNode*ins,int *local_var_num,int reg_save_num);


/**
 * @details 哈希表创建时加入一个节点
 * @param hashMap
 * @param key
 * @param name 这个是用来标识该变量是参数还是临时变量（参数会有%i<%param_num）
 * @param sub_sp 用来记录参数的开辟
 * @param add_sp 用来记录临时变量的开辟
 */

void hashmap_add(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp,int *local_var_num,int reg_save_num,int reg_flag);

//void hashmap_add_left(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp,int *local_var_num,int regri);
//void hashmap_add_right(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp,int *local_var_num,int regri);

void hashmap_alloca_add(HashMap*hashMap,Value*key,int *add_sp,int reg_save_num);
void hashmap_bitcast_add(HashMap*hashMap,Value*key,Value *value,int reg_save_num);
/**
 * @details 销毁哈希表释放内存
 * @param hashMap
 */
void offset_free(HashMap*hashMap);
unsigned HashKey(void* key);

int CompareKey(void* lhs, void* rhs);

void CleanKey(void* key);

void CleanValue(void* value);

#endif //C22V1_OFFSET_H
