
#！/bin/bash
# 测试前端部分正确性
cd test_cases
file_list=()
file_err=()
exp_err=()
act_err=()
file_c=".c"
file_ll=".ll"
file_as=".s"
file_obj=".o"
exp_str="expect:"
act_str="actual:"
exp=0
act=0
le_count=0
for file in $(ls *[.c])
do 
        filename=${file%.*}
        echo ${filename}
        gcc $filename$file_c -o test
        ./test
        exp=$?
        rm -rf test
        cd ../
        ./compiler >a.log
        ./compiler test_cases/$filename$file_c
        cd test_cases
        # clang -S -emit-llvm $filename$file_c
        llc $filename$file_ll
        gcc -c $filename$file_as -o $filename$file_obj
        gcc $filename$file_obj -o test
        ./test
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
        fi
done
echo "还有问题的测试用例数："$le_count
for((i=1;i<=$le_count;i++));
do
    echo $file_err[i]$file_c
    echo $exp_str$exp_err[i]
    echo $act_str$act_err[i]
done

