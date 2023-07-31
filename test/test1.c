
int main()
{
    int x[6][6][6];
    int i,j,k;
    int f;
    int N;

    N = getint ();
    f = getint ();

    i = 0;
    j = 0;
    k = 0;

    while (i<N) {
        j = 0;
        k = 0;
        while (j<N) {
            k = 0;
            while (k<N) {
                x[i][j][k] = 1;
                k = k + 1;
            }
            k = 0;
            while(k < N){
                putint(x[i][j][k]);
                putch(32);
                k = k + 1;
            }
            putch(10);
            j = j + 1;
        }
        i = i + 1;
    }

    return 0;
}
