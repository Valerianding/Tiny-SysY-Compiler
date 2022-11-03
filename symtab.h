#ifndef SYMTAB_H
#define SYMTAB_H
#include "sc_map.h"

struct _Symtab
{
    /* data */
    //struct sc_map_value value_map;
};

typedef struct _Symtab Symtab;

void* symtab_loopup(Symtab* this, char* name);
void symtab_insert_value_name(Symtab* this, char* name, void *V);

#endif