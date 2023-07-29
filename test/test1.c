int loopcount;
int n = 10;
int main(){
    loopcount = getint();
    int result = 0;
    while(loopcount < n){
        result = result + 1;
        loopcount = loopcount + 1;
    }
    return result;
}