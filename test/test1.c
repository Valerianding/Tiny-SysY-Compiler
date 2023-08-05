int main(){
    int i = getint();
    int j = 0;
    int result = 0;
    while(j < i){
        int tmp = 0;
        int k = 0;
        int total = j * 60;
        int h = 0;
        while(k < 60){
            tmp = tmp + j;
            k = k + 1;
        }
        result = h + tmp;
        j = j + 1;
    }
    return result;
}