#ifndef USER_DEF
#define USER_DEF

#include "value.h"
#include "use.h"

// Use是依赖于User存在的(只有User知道它需要多少依赖以及依赖指向哪个Value), 因此User负责对Use的管理(申请与释放).
// User是一个通用的基础类, 而不同指令的操作数个数又不同, 导致Use不能作为User的成员存在(容器方式存储可以, 但是效率降低).
// 可以看到User并没有扩展额外的成员, 那么Use存储在哪里呢? 答案在新建User对象的时候.
/**
 * 
void *User::allocateFixedOperandUser(size_t Size, unsigned Us, unsigned DescBytes) {
  assert(Us < (1u << NumUserOperandsBits) && "Too many operands");

  static_assert(sizeof(DescriptorInfo) % sizeof(void *) == 0, "Required below");

  unsigned DescBytesToAllocate = DescBytes == 0 ? 0 : (DescBytes + sizeof(DescriptorInfo));
  assert(DescBytesToAllocate % sizeof(void *) == 0 && "We need this to satisfy alignment constraints for Uses");

  uint8_t *Storage = static_cast<uint8_t *>(::operator 
            new(Size + sizeof(Use) * Us + DescBytesToAllocate)
        );
  Use *Start = reinterpret_cast<Use *>(Storage + DescBytesToAllocate);
  Use *End = Start + Us;
  User *Obj = reinterpret_cast<User*>(End);
  Obj->NumUserOperands = Us;
  Obj->HasHungOffUses = false;
  Obj->HasDescriptor = DescBytes != 0;
  for (; Start != End; Start++)
    new (Start) Use(Obj);

  if (DescBytes != 0) {
    auto *DescInfo = reinterpret_cast<DescriptorInfo *>(Storage + DescBytes);
    DescInfo->SizeInBytes = DescBytes;
  }

  return Obj;
}

void *User::operator new(size_t Size, unsigned Us) {
  return allocateFixedOperandUser(Size, Us, 0);
}

void *User::operator new(size_t Size, unsigned Us, unsigned DescBytes) {
  return allocateFixedOperandUser(Size, Us, DescBytes);
}



void *User::operator new(size_t Size) {
  // Allocate space for a single Use*
  //   其在给定大小Size的基础上又增加了一个Use指针大小的空间进行内存申请. 
  void *Storage = ::operator new(Size + sizeof(Use *));

  //   然后将返回地址的起始作为Use指针并置空, 之后的地址作为User对象的地址初始化并返回. 
  //   即在每个User对象之前有一个Use指针大小的空间保存了一个默认为空的Use指针. 
  Use **HungOffOperandList = static_cast<Use **>(Storage);
  User *Obj = reinterpret_cast<User *>(HungOffOperandList + 1);
  Obj->NumUserOperands = 0;
  Obj->HasHungOffUses = true;
  Obj->HasDescriptor = false;
  *HungOffOperandList = nullptr;
  return Obj;

} 


https://llvm.org/doxygen/User_8cpp_source.html
 * 
 */ 
typedef struct _User User;

struct _User{
    // user 使用多个 value，在LLVM中， 通过函数 allocate_fixed_operand_user 及 user 的构造函数
    // （重载 new），在创建 user 的同时，创建了相应的 use，相关于在 user 之前有1个 use 或 多个 use
    // llvm 是隐式的创建，这里是显示的创建
    
    // user 继承 value
    Value value;
    
    struct _Use *use_list;
};

User *user_new();
User *user_new1(unsigned use_num);
int user_get_size();
User* user_construct(void* place, int use_num);
Use* user_get_operand_use(User* this, unsigned i);
#endif