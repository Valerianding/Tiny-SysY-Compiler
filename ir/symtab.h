#ifndef SYMTAB_H
#define SYMTAB_H

#include "sc_map.h"
#include "value.h"
#include "assert.h"



struct _mapList{
    struct sc_map_sv map;             //key-value为name-Value*
    int scope_level;                  //当前表所在的嵌套层数
    struct _mapList* next;            //并列关系，比如说 2个函数是并列关系
    struct _mapList* child;           //嵌套关系，比如说 1个函数之内就有两个for循环（第一个for循环的next指向第二个for循环）
};
typedef struct _mapList MapList;

//stack里的内容是指向map的指针
struct _Stack{
    int top;
    MapList* value_map[64];
};
typedef struct _Stack Stack;

struct _Symtab
{
    Stack S;
    MapList *value_maps;              //指向有头指针的符号表链表的指针
};
typedef struct _Symtab Symtab;


//符号表的初始化，初始化前需要先创建
void symtab_init(Symtab* this);

//重置栈，使栈回到初始状态，目前只在遍历ast使会使用一次
void stack_new(Symtab* this);

//根据变量名查找变量,在栈中有MapList*指针时才使用此方法
//应该只有前端会用到这个方法
struct _Value * symtab_dynamic_lookup(Symtab* this, char* name);

//获取现在所在的表,应该也只有前端会用到
struct sc_map_sv *getCurMap(Symtab *this);
//获取现在指向的MapList
struct _mapList* getCurMapList(Symtab *this);

//在当前表增加name-value键值对
void symtab_insert_value_name(Symtab* this, char* name, struct _Value *V);
//在指定表中增加name-value对
void symtab_insert_withmap(Symtab* this, struct sc_map_sv *map,char* name, struct _Value *V);

//从指定表中取出value*
struct _Value* symtab_lookup_withmap(Symtab *this,char* name,struct sc_map_sv* map);

//创建符号表时使用，删除一层栈,但不删除该栈元素指向的map的数据
void scope_end(Symtab* this);

//创建符号表时使用，增加一层栈，创next或child
void scope_start(Symtab *this);

//遍历ast时使用，增加一层栈，走到对应表
void scope_forward(Symtab *this);

//遍历ast时使用，删除一层栈，回到对应表
struct sc_map_sv* scope_back(Symtab *this);

//得到最近的一个函数表，
//使参数加到和函数内容一层，而不是和函数名一层
//在将函数参数添加至符号表使用
struct sc_map_sv *symtab_get_latest_func_map(Symtab *this);

//打印出各符号表中的所有元素
void showAll(struct _mapList* func_map,bool flag);

#endif
