int arr1[10][2][3][4][5][6][2];
int arr2[10][2][3][2][4][8][7];
void loop1(int x, int y) {
    int a, b, c, d, e, f, g;
    a = 0;
    while (a < x && a < y) {
        b = 0;
        while (b < 2) {
            c = 0;
            while (c < 3) {
                d = 0;
                while (d < 4) {
                    e = 0;
                    while (e < 5) {
                        f = 0;
                        while (f < 6) {
                            g = 0;
                            while (g < 2) {
                                arr1[a][b][c][d][e][f][g] = a + b + c + d + e + f + g + x + y;
                                g = g + 1;
                            }
                            f = f + 1;
                        }
                        e = e + 1;
                    }
                    d = d + 1;
                }
                c = c + 1;
            }
            b = b + 1;
        }
        a = a + 1;
    }
}