int main(){
    int a = 10;
    int b = 20;
    int c = 10;
    while(c > 5){
        int temp = a;
        a = b;
        b = temp;
        c = c - 1;
    }
    return a;
}