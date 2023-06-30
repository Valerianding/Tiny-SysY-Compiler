int test(){
   int a = getint();
   int b = getint();
   int c = a - b;
   while(c < 10){
       int temp = a + b;
       c = c + 1;
   }
   return 0;
}