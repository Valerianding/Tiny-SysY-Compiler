#！/bin/bash
# 测试前端部分正确性
cd test_cases
file_list=()
file_err=()
exp_err=()
act_err=()
ll_err=()
as_err=()
file_c=".c"
file_ll=".ll"
file_as=".s"
file_obj=".o"
file_in=".in"
exp_str="expect:"
act_str="actual:"
exp=0
act=0
ljw=1
le_count=0
for file in $(ls *[.c])
do
        filename=${file%.*}
        echo ${filename}
        gcc $filename$file_c -o test
        FILE_IN=$filename$file_in
        if [ ! -f "$FILE_IN" ]
        then
        gcc $filename$file_c -o test
        ./test
        else
        gcc -o test $filename$file_c -L./ libsy.a
        ./test < $filename$file_in
        fi
        exp=$?
        rm -rf test
        cd ../
        # ./compiler >a.log
        ./cmake-build-debug/compiler test_cases/$filename$file_c
        cd test_cases
        #记录生成ll文件失败的用例
        FILE_LL=$filename$file_ll
        if [ ! -f "$FILE_LL" ]
        then
        let le_count+=1
        echo "$FILE_LL not exist"
        file_err[le_count]=$filename$file_c
        ll_err[le_count]=1
        exp_err[le_count]=$exp
        continue
        fi

        llc $filename$file_ll
        #记录使用llvm工具链失败的用例（即生成的ir不符合规范
        FILE_AS=$filename$file_as
        if [ ! -f "$FILE_AS" ]
        then
        let le_count+=1
        echo "$FILE_AS not exist"
        file_err[le_count]=$filename$file_c
        as_err[le_count]=1
        ll_err[le_count]=0
        exp_err[le_count]=$exp
        rm -rf $filename$file_ll
        continue
        fi

        gcc -c $filename$file_as -o $filename$file_obj
        gcc $filename$file_obj sylib.o -o test
        if [ ! -f "$FILE_IN" ]
        then
        ./test
        else
        ./test < $filename$file_in
        fi
        act=$?


        rm -rf test
        rm -rf $filename$file_ll
        rm -rf $filename$file_as
        rm -rf $filename$file_obj
        # echo $exp_str$exp
        # echo $act_str$act

        if [ $exp -ne $act ]
        then
        let le_count+=1
        file_err[le_count]=$filename$file_c
        exp_err[le_count]=$exp
        act_err[le_count]=$act
        ll_err[le_count]=0
        as_err[le_count]=0

        fi
done
echo "还有问题的测试用例数："$le_count
for((i=1;i<=$le_count;i++));
do
    if [ "${ll_err[i]}" = "1" ] ;
    then
    echo ${file_err[i]}"   生成ir失败  "$exp_str${exp_err[i]}
    continue
    fi

    if [ "${as_err[i]}" = "1" ] ;
    then
    echo ${file_err[i]}"   ir不符合llvm规范  "$exp_str${exp_err[i]}
    continue
    fi

    echo ${file_err[i]}"   期望值与实际值不匹配"
    echo $exp_str${exp_err[i]}
    echo $act_str${act_err[i]}
done