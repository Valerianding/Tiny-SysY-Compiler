echo "start"
cmake ./
make
tar -cvf  submit.tar ./back_end ./container ./front_end ./ir ./libs ./math ./src cds.h tacmain.c util.h lrparser.tab.h lrparser.tab.c lex.yy.c
mv submit.tar ../compiler_fengliu/
cd ../
cd compiler_fengliu
git pull
rm -rf ./back_end ./container ./front_end ./ir ./libs ./math ./src cds.h tacmain.c util.h lrparser.tab.h lrparser.tab.c lex.yy.c
tar -xvf submit.tar
rm -rf submit.tar
git add .
git commit -m '佳雯push'
git push 
echo "succ"