int test(int a){
    a = a + 1;
    return a;
}

int test1(int b){
    int result = test(b);
    return result + 1;
}

int main(){
    int i = test(1);
    return 1;
}