#ifndef TYPE_DEF
#define TYPE_DEF

#include <stdbool.h>


#define nullptr 0

enum TypeID {
    Var_INT,
    Var_FLOAT,
    Var_initINT,
    Var_initFLOAT,
    INT,
    FLOAT,
};

typedef struct _Type{
    enum TypeID   ID : 8;            // The current base type of this type.
    unsigned SubclassData : 24; // Space for subclasses to store data.
                               // Note that this should be synchronized with
                               // MAX_INT_BITS value in IntegerType class.

//    /// Keeps track of how many Type*'s there are in the ContainedTys list.
//    unsigned NumContainedTys;
//
//
//    /* 看不懂的代 */
//    /// A pointer to the array of Types contained by this Type. For example, this
//    /// includes the arguments of a function type, the elements of a structure,
//    /// the pointee of a pointer, the element type of an array, etc. This pointer
//    /// may be 0 for types that don't contain other types (Integer, Double,
//    /// Float).
//    struct _Type * const *ContainedTys;
}Type;

bool isVoidTy(Type *this);
bool isFloatTy(Type *this);
bool isIntegerTy(Type *this);
bool isFunctionTy(Type *this);
bool isFloatingPointTy(Type *this);
bool isPointerTy(Type *this);
bool isConstTy(Type *this);

#endif