#include "type.h"

void new_type(Type *this, enum TypeID tid)
{
    this->ID = tid;
    this->SubclassData = 0;
}

unsigned getSubclassData(Type *this)
{
    return this->SubclassData;
}

void setSubclassData(Type *this, unsigned val)
{
    this->SubclassData = val;
    // Ensure we don't have any accidental truncation.
    // assert(getSubclassData() == val && "Subclass data too large for field");
}

//===--------------------------------------------------------------------===//
// Accessors for working with types.
//
////////////////////////////////////////////////////////////////////////////////////////////