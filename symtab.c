#include "symtab.h"

void symtab_init(Symtab* this){
    sc_map_init_sv(&this->value_map, 0, 0);
}

void symtab_insert_value_name(Symtab* this, char* name, void *V){
    sc_map_put_sv(&this->value_map, name, V);
}