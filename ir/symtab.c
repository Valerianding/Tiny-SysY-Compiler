#include "symtab.h"
#include <stdio.h>

//初始化链表
void init_list(MapList **M)
{
    (*M)=(MapList*) malloc(sizeof(MapList));
    (*M)->next=NULL;
    (*M)->child=NULL;
}

//符号表的初始化
void symtab_init(Symtab* this)
{
    this->S.top=-1;
    //初始化链表头指针
    init_list(&this->value_maps);
    this->value_maps->scope_level=0;
    scope_start(this);
}

void stack_new(Symtab* this)
{
    for(int i=0;i<300;i++)
        this->S.value_map[i]=NULL;
    this->S.top=0;
    //默认在全局
    this->S.value_map[this->S.top]=this->value_maps->next;

    //给全局表的next表开一下空间，库函数信息存在这里
    //新map m
    MapList *m=(MapList*) malloc(sizeof(MapList));
    sc_map_init_sv(&m->map,0,0);
    m->scope_level=0;
    this->value_maps->next->next=m;
}


//获取指向现在所在表的指针
struct sc_map_sv* getCurMap(Symtab *this)
{
    return &this->S.value_map[this->S.top]->map;
}

//获取现在指向的MapList
struct _mapList* getCurMapList(Symtab *this)
{
    return this->S.value_map[this->S.top];
}

bool is_global_map(Symtab* this)
{
    if(this->S.top==0)
        return true;
    return false;
}

//删除一层栈,但不删除该栈元素指向的map的数据
void scope_end(Symtab* this)
{
    this->S.top--;
}

//增加一层栈，创next或child
void scope_start(Symtab *this)
{
    //top值为嵌套层数，从0开始
    this->S.top++;

    //新map m
    MapList *m=(MapList*) malloc(sizeof(MapList));
    m->next=NULL;m->child=NULL;
    sc_map_init_sv(&m->map,0,0);
    m->scope_level=this->S.top;

    //p指向map头指针,初始时scope_level为0
    MapList *p=this->value_maps;

    while(p->scope_level<=this->S.top)
    {
        //p指向当前层的最后一个非空
        while(p->next!=NULL){
            p=p->next;
        }
        if(p->scope_level!=this->S.top)
        {
            //继续走下一层
            if(p->child!=NULL)
                p=p->child;
                //没有下一层可走了，但又需要走(scope_level与S.top嵌套数不相等),说明是第一个子嵌套
            else
            {
                p->child=m;
                break;
            }
        }
            //走到对应层了，接着创建next即可
        else
        {
            p->next=m;
            break;
        }
    }

    //让栈指针指向value_maps数组下一个maplist元素的地址
    this->S.value_map[this->S.top]=m;
}

//根据变量名查找变量
struct _Value* symtab_dynamic_lookup(Symtab* this, char* name)
{
    Value *var;

    for(int i=this->S.top;i>=0;i--)
    {
        var = (struct _Value *)sc_map_get_sv(&this->S.value_map[i]->map,name);
        if(var!=NULL && var->pdata->define_flag==1)
        {
            return var;
        }
    }
    return NULL;
}

struct _Value* symtab_dynamic_lookup_first(Symtab* this, char* name)
{
    Value *var;

    for(int i=this->S.top;i>=0;i--)
    {
        var = (struct _Value *)sc_map_get_sv(&this->S.value_map[i]->map,name);
        if(var!=NULL)
        {
            return var;
        }
    }
    return NULL;
}


//在当前map 增加name-value键值对
void symtab_insert_value_name(Symtab* this, char* name, struct _Value *V)
{
    //在当前层找不到
    assert(name!=NULL && sc_map_get_sv(getCurMap(this),name)==NULL);

    struct sc_map_sv* cur_map = getCurMap(this);
    sc_map_put_sv(cur_map,name,V);
}

//在指定表中增加name-value对
void symtab_insert_withmap(Symtab* this, struct sc_map_sv *map,char* name, struct _Value *V)
{
    //在当前层找不到
    assert(name!=NULL && sc_map_get_sv(map,name)==NULL);

    sc_map_put_sv(map,name,V);
}

//从指定表中取出value*
struct _Value* symtab_lookup_withmap(Symtab *this,char* name,struct sc_map_sv* map)
{
    return sc_map_get_sv(map,name);
}

//修改最上层同一name的value值
void symtab_update_value(Symtab* this,char *name,struct _Value *V)
{
    assert(symtab_dynamic_lookup(this,name)!=NULL);
    for(int i=this->S.top;i>=0;i--)
    {
        if(sc_map_get_sv(&this->S.value_map[i]->map,name)!=NULL)
        {
            sc_map_put_sv(&this->S.value_map[i]->map,name,V);
            return;
        }
    }
}

//得到最近的一个函数表，
//使参数加到和函数内容一层，而不是和函数名一层
struct sc_map_sv *symtab_get_latest_func_map(Symtab *this)
{
    //全局表的第一个child，即第一个函数
    MapList *func=this->value_maps->next->child;
    while(func->next!=NULL)
        func=func->next;

    //要加入参数的那个函数一定是最近的函数
    return &func->map;
}

struct _mapList *symtab_get_latest_func_maplist(Symtab *this)
{
    //全局表的第一个child，即第一个函数
    MapList *func=this->value_maps->next->child;
    while(func->next!=NULL)
        func=func->next;

    //要加入参数的那个函数一定是最近的函数
    return func;
}

//遍历ast时使用，增加一层栈，走到对应表
//进一层要把之前多余S中的删掉
void scope_forward(Symtab *this)
{
    //目前所在的那个map
    MapList *m=this->S.value_map[this->S.top];

    if(this->S.value_map[this->S.top+1]==NULL)
        m=m->child;
    else if(this->S.value_map[this->S.top+1]->next!=NULL)
        m=this->S.value_map[this->S.top+1]->next;
    else
        m=m->next;

    //top值为嵌套层数，从0开始
    this->S.top++;

    this->S.value_map[this->S.top]=m;

    for(int i=this->S.top+1;i<300;i++)
        this->S.value_map[i]=NULL;
}

//遍历ast时使用，回到对应表
struct sc_map_sv* scope_back(Symtab *this)
{
    this->S.top--;
    return &this->S.value_map[this->S.top]->map;
}

struct sc_map_sv* symtab_param_map(Symtab *this)
{
    struct sc_map_sv* param_map=NULL;
    //目前所在的那个map
    MapList *m=this->S.value_map[this->S.top];

    if(this->S.value_map[this->S.top+1]==NULL)
        param_map=&m->child->map;
    else
        param_map=&this->S.value_map[this->S.top+1]->next->map;
    return param_map;
}

//打印出各符号表中的所有元素
void showAll(struct _mapList* func_map,bool flag)
{
    const char *key, *value;
    //先遍历本表
    sc_map_foreach (&func_map->map, key, value)
        {
            //打印
            printf("%s\t",key);
        }
    printf("\n");
    //函数第一层，只能走child
    if(flag)
    {
        if(func_map->child!=NULL)
            showAll(func_map->child,false);
    }
        //其他函数层，有child有next都可以走
    else{
        if(func_map->child!=NULL)
            showAll(func_map->child,false);
        if(func_map->next!=NULL)
            showAll(func_map->next,false);
    }
}
