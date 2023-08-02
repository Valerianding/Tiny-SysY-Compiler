int test(){
    int a = getint();
    a = a + 3;
    int b = getint();
    b = b + 3;
    return a + b;
}

int main(){
    return test();
}