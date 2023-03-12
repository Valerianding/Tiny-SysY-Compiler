// test if-{if}-else
int if_if_Else() {
    int a = 10;
    int b = 10;
    if(b > 10)
        a = a - 1;
    else
        a = a + 1;
    return a;
}

int main(){
    return (if_if_Else());
}