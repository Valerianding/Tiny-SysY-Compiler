int main(){
    int i = 0;
    int j = getint();
    int result = 0;
    int what = 0;
    while(i < j){
        result = result + 1;
        result = result / 2;
        i = i + 1;
    }
    return result;
}