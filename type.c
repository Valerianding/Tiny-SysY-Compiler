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

/// Return the type id for the type. This will return one of the TypeID enum
/// elements defined above.
enum TypeID getTypeID(Type *this) { return this->ID; }

/// Return true if this is 'void'.
bool isVoidTy(Type *this) { return getTypeID(this) == VoidTyID; }

/// Return true if this is 'half', a 16-bit IEEE fp type.
bool isHalfTy(Type *this) { return getTypeID(this) == HalfTyID; }

/// Return true if this is 'bfloat', a 16-bit bfloat type.
bool isBFloatTy(Type *this) { return getTypeID(this) == BFloatTyID; }

/// Return true if this is a 16-bit float type.
bool is16bitFPTy(Type *this)
{
    return getTypeID(this) == BFloatTyID || getTypeID(this) == HalfTyID;
}

/// Return true if this is 'float', a 32-bit IEEE fp type.
bool isFloatTy(Type *this) { return getTypeID(this) == FloatTyID; }

/// Return true if this is 'double', a 64-bit IEEE fp type.
bool isDoubleTy(Type *this) { return getTypeID(this) == DoubleTyID; }

/// Return true if this is x86 long double.
bool isX86_FP80Ty(Type *this) { return getTypeID(this) == X86_FP80TyID; }

/// Return true if this is 'fp128'.
bool isFP128Ty(Type *this) { return getTypeID(this) == FP128TyID; }

/// Return true if this is powerpc long double.
bool isPPC_FP128Ty(Type *this) { return getTypeID(this) == PPC_FP128TyID; }

/// Return true if this is one of the six floating-point types
bool isFloatingPointTy(Type *this)
{
    return getTypeID(this) == HalfTyID || getTypeID(this) == BFloatTyID ||
           getTypeID(this) == FloatTyID || getTypeID(this) == DoubleTyID ||
           getTypeID(this) == X86_FP80TyID || getTypeID(this) == FP128TyID ||
           getTypeID(this) == PPC_FP128TyID;
}


/// Return true if this is 'label'.
bool isLabelTy(Type *this)
{
    return getTypeID(this) == LabelTyID;
}

/// Return true if this is 'metadata'.
bool isMetadataTy(Type *this)
{
    return getTypeID(this) == MetadataTyID;
}

/// Return true if this is 'token'.
bool isTokenTy(Type *this)
{
    return getTypeID(this) == TokenTyID;
}

/// True if this is an instance of IntegerType.
bool isIntegerTy(Type *this)
{
    return getTypeID(this) == IntegerTyID;
}

/// Return true if this is an IntegerType of the given width.
bool isIntegerTyWidth(Type *this, unsigned Bitwidth);


/// Return true if this is an integer type or a pointer type.
bool isIntOrPtrTy(Type *this)
{
    return isIntegerTy(this) || isPointerTy(this);
}

/// True if this is an instance of FunctionType.
bool isFunctionTy(Type *this)
{
    return getTypeID(this) == FunctionTyID;
}

/// True if this is an instance of StructType.
bool isStructTy(Type *this)
{
    return getTypeID(this) == StructTyID;
}

/// True if this is an instance of ArrayType.
bool isArrayTy(Type *this)
{
    return getTypeID(this) == ArrayTyID;
}

/// True if this is an instance of PointerType.
bool isPointerTy(Type *this)
{
    return getTypeID(this) == PointerTyID;
}

/// True if this is an instance of an opaque PointerType.
bool isOpaquePointerTy(Type *this);


/// Return true if this type could be converted with a lossless BitCast to
/// type 'Ty'. For example, i8* to i32*. BitCasts are valid for types of the
/// same size only where no re-interpretation of the bits is done.
/// Determine if this type could be losslessly bitcast to Ty
bool canLosslesslyBitCastTo(Type *Ty);

/// Return true if this type is empty, that is, it has no elements or all of
/// its elements are empty.
bool isEmptyTy(Type *this);

/// Return true if the type is "first class", meaning it is a valid type for a
/// Value.
bool isFirstClassType(Type *this)
{
    return getTypeID(this) != FunctionTyID && getTypeID(this) != VoidTyID;
}


/// Return true if the type is an aggregate type. This means it is valid as
/// the first operand of an insertvalue or extractvalue instruction. This
/// includes struct and array types, but does not include vector types.
bool isAggregateType(Type *this)
{
    return getTypeID(this) == StructTyID || getTypeID(this) == ArrayTyID;
}




////////////////////////////////////////////////////////////////////////////////////////////