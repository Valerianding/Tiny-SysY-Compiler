int main(){
    int a = 5;
    int b = 10;
    int c = 15;
    while(c > 5){
        int temp = a;
        a = b;
        b = temp;
        c = c - 1;
    }
    return a;
}