echo "start"
cmake ./
make
tar -cvf  submit.tar ./front_end/auto_lex.l ./front_end/lrparser.y ./back_end ./container ./front_end ./ir ./libs ./math ./src cds.h tacmain.c util.h lrparser.tab.h lrparser.tab.c lex.yy.c
# mv submit.tar ../compiler_fengliu/
cd ../
cd compiler_fengliu
git pull
git checkout main
# git branch submit
# git checkout submit
mv ../Tiny-SysY-Compiler/submit.tar ./
rm -rf ./back_end ./container ./front_end/auto_lex.l ./front_end/lrparser.y ./front_end ./ir ./libs ./math ./src cds.h tacmain.c util.h lrparser.tab.h lrparser.tab.c lex.yy.c
tar -xvf submit.tar
rm -rf submit.tar
git add .
git commit -m '佳雯push'
# git push  origin submit:submit
git push
echo "提交成功"
#echo "20秒后自动清空仓库："
#for i in $(seq 20 -1 1)
#do
#    echo -e "$i";sleep 1
#done
#git reset a90b003d49e84ee7e30531c85760784d54a7b581
#git push -f