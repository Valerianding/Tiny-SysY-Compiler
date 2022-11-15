#include "symtab.h"

//初始化链表
void init_list(MapList **M)
{
    (*M)=(MapList*) malloc(sizeof(MapList));
    (*M)->next=NULL;
    (*M)->child=NULL;
}

//符号表的初始化
void symtab_init(Symtab** this)
{
    (*this)=(Symtab*) malloc(sizeof(Symtab));
    (*this)->S.top=-1;
    //初始化链表头指针
    init_list(&(*this)->value_maps);
    (*this)->value_maps->scope_level=0;
}

//获取指向现在所在表的指针
struct sc_map_sv* getCurMap(Symtab *this)
{
    return this->S.value_map[this->S.top];
}

//删除一层栈,但不删除该栈元素指向的map的数据
void remove_top(Symtab* this)
{
    this->S.top--;
}

//增加一层栈，创next或child
void add_top(Symtab *this)
{
    //top值为嵌套层数，从0开始
    this->S.top++;

    //新map m,分配内存可能有问题
    MapList *m=(MapList*) malloc(sizeof(MapList));
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

    //让栈指针指向value_maps数组下一个map元素的地址
    this->S.value_map[this->S.top]=&(m->map);
}

//根据变量名查找变量
struct _Value* symtab_lookup(Symtab* this, char* name)
{
    Value *var;

    for(int i=this->S.top;i>=0;i--)
    {
        var = (struct _Value *)sc_map_get_sv(this->S.value_map[i],name);
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