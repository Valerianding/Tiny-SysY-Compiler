//
// Created by tom on 23-2-25.
//

#ifndef C22V1_OFFSET_H
#define C22V1_OFFSET_H
#include "bblock.h"
#include "hash_map.h"
/**
 * @details 使用哈希表来存放栈帧的信息key=Value*,value=offset*
 */
typedef struct _offset{
    int offset_sp;
}offset;


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
HashMap *offset_init(InstNode*ins);


/**
 * @details 哈希表创建时加入一个节点
 * @param hashMap
 * @param key
 * @param name 这个是用来标识该变量是参数还是临时变量（参数会有%i<%param_num）
 * @param sub_sp 用来记录参数的开辟
 * @param add_sp 用来记录临时变量的开辟
 */
void hashmap_add(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp);


/**
 * @details 销毁哈希表释放内存
 * @param hashMap
 */
void offset_free(HashMap*hashMap);


#endif //C22V1_OFFSET_H
