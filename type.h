#ifndef TYPE_DEF
#define TYPE_DEF

#include <stdbool.h>


#define nullptr 0

enum TypeID {
     // PrimitiveTypes
     HalfTyID = 0,  ///< 16-bit floating point type
     BFloatTyID,    ///< 16-bit floating point type (7-bit significand)
     FloatTyID,     ///< 32-bit floating point type
     DoubleTyID,    ///< 64-bit floating point type
     X86_FP80TyID,  ///< 80-bit floating point type (X87)
     FP128TyID,     ///< 128-bit floating point type (112-bit significand)
     PPC_FP128TyID, ///< 128-bit floating point type (two 64-bits, PowerPC)
     VoidTyID,      ///< type with no size
     LabelTyID,     ///< Labels
     MetadataTyID,  ///< Metadata
     X86_MMXTyID,   ///< MMX vectors (64 bits, X86 specific)
     X86_AMXTyID,   ///< AMX vectors (8192 bits, X86 specific)
     TokenTyID,     ///< Tokens

     ConstIntegerTyID,
     ConstFloatTyID,
  
     // Derived types... see DerivedTypes.h file.
     IntegerTyID,        ///< Arbitrary bit width integers
     FunctionTyID,       ///< Functions
     PointerTyID,        ///< Pointers
     StructTyID,         ///< Structures
     ArrayTyID,          ///< Arrays
};

typedef struct _Type{
    enum TypeID   ID : 8;            // The current base type of this type.
    unsigned SubclassData : 24; // Space for subclasses to store data.
                               // Note that this should be synchronized with
                               // MAX_INT_BITS value in IntegerType class.

    /// Keeps track of how many Type*'s there are in the ContainedTys list.
    unsigned NumContainedTys;


    /* 看不懂的代 */
    /// A pointer to the array of Types contained by this Type. For example, this
    /// includes the arguments of a function type, the elements of a structure,
    /// the pointee of a pointer, the element type of an array, etc. This pointer
    /// may be 0 for types that don't contain other types (Integer, Double,
    /// Float).
    struct _Type * const *ContainedTys;                               
}Type;

bool isVoidTy(Type *this);
bool isFloatTy(Type *this);
bool isIntegerTy(Type *this);
bool isFunctionTy(Type *this);
bool isFloatingPointTy(Type *this);
bool isPointerTy(Type *this);
bool isConstTy(Type *this);
/* 如何设置Type存在问题 */

#endif