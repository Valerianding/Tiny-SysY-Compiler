#ifndef SYMTAB_H
#define SYMTAB_H

#include "sc_map.h"
#include "value.h"
#include "assert.h"

//stack里的内容是指向map的指针
struct _Stack{
    int top;
    struct sc_map_sv* value_map[64];
};

typedef struct _Stack Stack;

struct _mapList{
    struct sc_map_sv map;             //key-value为name-Value*
    int scope_level;                  //当前表所在的嵌套层数
    struct _mapList* next;            //并列关系，比如说 2个函数是并列关系
    struct _mapList* child;           //嵌套关系，比如说 1个函数之内就有两个for循环（第一个for循环的next指向第二个for循环）
};
typedef struct _mapList MapList;

struct _Symtab
{
    Stack S;
    //指向有头指针的符号表链表的指针
    //map个数只增不减，有新作用域了就直接往后面加
    MapList *value_maps;
};

typedef struct _Symtab Symtab;

//符号表的初始化
void symtab_init(Symtab** this);

//根据变量名查找变量
struct _Value * symtab_lookup(Symtab* this, char* name);

//获取现在所在的表
struct sc_map_sv *getCurMap(Symtab *this);

//增加name-value键值对
void symtab_insert_value_name(Symtab* this, char* name, struct _Value *V);

//删除一层栈,但不删除该栈元素指向的map的数据
void remove_top(Symtab* this);

//增加一层栈，创next或child
void add_top(Symtab *this);

#endif
