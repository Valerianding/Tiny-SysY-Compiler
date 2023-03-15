// test if-{if}-else

int main(){
    int a = 10;
    int b = 1;
    if(a > 10){
        a = a -1;
        b = b + 1;
    }else{
        a = a + 1;
        b = b - 1;
    }
    return a;
}