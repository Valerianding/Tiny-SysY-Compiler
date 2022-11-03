#include "value.h"
#include "use.h"
#include "user.h"
#include "instruction.h"


int main(){
    ConstantNum a, b;
    a.num.num_int = 10;
    b.num.num_int = 20;
    Value var1;
    value_set_name(&var1, "var_name");
    ins_new_binary_operator('+', (Value*)&a, (Value*)&b);
}