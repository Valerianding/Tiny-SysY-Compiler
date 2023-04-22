#！/bin/bash
# 测试前端部分正确性
#用于判断标准输出的结果是否正确
argu=$#
single_file_name=$1
gcc -no-pie run_test_src/judge_out.c -o judge
#生成静态链接库文件
gcc -no-pie -c run_test_src/sylib.c
# ar rcs sylib.a sylib.o
ar crv sylib.a sylib.o
cp sylib.a test_cases
cp sylib.o test_cases
rm -rf sylib.a
rm -rf sylib.o
cd test_cases
file_list=()
file_err=()
exp_err=()
act_err=()
ll_err=()
as_err=()
out_err=()
file_c=".c"
file_ll=".ll"
file_as=".s"
file_obj=".o"
file_in=".in"
file_out=".out"
file_temp_out="_temp.out"
ret=0
exp_str="expect:"
act_str="actual:"
exp=0
act=0
out_value=0
ljw=1
stm=0
le_count=0

if [ $argu -ne $stm ]
then
    for file in $(ls $single_file_name*[.c])
    do
        out_value=1
        filename=${file%.*}
        echo ${filename}
        cd ../
        # ./compiler >a.log
        ./cmake-build-debug/compiler test_cases/$filename$file_c
        ret=$?
        if [ $ret -ne $stm ]2
        then
        let le_count+=1
        echo "$FILE_LL not exist"
        file_err[le_count]=$filename$file_c
        ll_err[le_count]=1
        exp_err[le_count]=$exp
        rm -rf test_cases/$filename$file_ll
        cd test_cases
        continue
        fi


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
        out_err[le_count]=0
        exp_err[le_count]=$exp
        rm -rf $filename$file_ll
        continue
        fi

        gcc -no-pie -c $filename$file_as -o $filename$file_obj
        gcc -no-pie $filename$file_obj sylib.o -o test
        FILE_OUT=$filename$file_out
        FILE_TEMP_OUT=$filename$file_temp_out
        if [ -f "$FILE_OUT" ]
        then
            if [ ! -f "$FILE_IN" ]
            then
            ./test > $FILE_TEMP_OUT
            act=$?
            echo $act >> $FILE_TEMP_OUT
            else
            ./test < $filename$file_in > $FILE_TEMP_OUT
            act=$?
            echo $act >> $FILE_TEMP_OUT
            fi
        cd ../
        ./judge test_cases/$FILE_OUT  test_cases/$FILE_TEMP_OUT
        out_value=$?
        cd test_cases
        # rm -rf $FILE_TEMP_OUT
        rm -rf test
        rm -rf $filename$file_ll
        rm -rf $filename$file_as
        rm -rf $filename$file_obj
        fi

        if [ $out_value -ne $ljw ]
        then
        let le_count+=1
        file_err[le_count]=$filename$file_c
        exp_err[le_count]=$exp
        act_err[le_count]=$act
        ll_err[le_count]=0
        as_err[le_count]=0
        out_err[le_count]=1
        rm -rf test
        rm -rf $filename$file_ll
        rm -rf $filename$file_as
        rm -rf $filename$file_obj
        continue
        else
        rm -rf $FILE_TEMP_OUT
        fi

        rm -rf test
        rm -rf $filename$file_ll
        rm -rf $filename$file_as
        rm -rf $filename$file_obj
        # echo $exp_str$exp
        # echo $act_str$act
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

        if [ "${out_err[i]}" = "1" ] ;
        then
        echo ${file_err[i]}"    标准输出与out存在差异"
        continue
        fi

        echo ${file_err[i]}"   返回的期望值与实际值不匹配"
        echo $exp_str${exp_err[i]}
        echo $act_str${act_err[i]}
    done

else
for file in $(ls *[.c])
do
    out_value=1
    filename=${file%.*}
    echo ${filename}
    gcc -no-pie $filename$file_c -o test
    FILE_IN=$filename$file_in
    cd ../
    # ./compiler >a.log
    ./cmake-build-debug/compiler test_cases/$filename$file_c
    ret=$?
    if [ $ret -ne $stm ]
    then
    let le_count+=1
    echo "$FILE_LL not exist"
    file_err[le_count]=$filename$file_c
    ll_err[le_count]=1
    exp_err[le_count]=$exp
    rm -rf test_cases/$filename$file_ll
    cd test_cases
    continue
    fi


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
    out_err[le_count]=0
    exp_err[le_count]=$exp
    rm -rf $filename$file_ll
    continue
    fi

    gcc -no-pie -c $filename$file_as -o $filename$file_obj
    gcc -no-pie $filename$file_obj sylib.o -o test
    FILE_OUT=$filename$file_out
    FILE_TEMP_OUT=$filename$file_temp_out

    if [ -f "$FILE_OUT" ]
    then
        if [ ! -f "$FILE_IN" ]
        then
        ./test > $FILE_TEMP_OUT
        act=$?
        echo >> $FILE_TEMP_OUT
        echo $act >> $FILE_TEMP_OUT
        else
        ./test < $filename$file_in > $FILE_TEMP_OUT
        act=$?
        echo >> $FILE_TEMP_OUT
        echo $act >> $FILE_TEMP_OUT
        fi
        cd ../
        ./judge test_cases/$FILE_OUT  test_cases/$FILE_TEMP_OUT
        out_value=$?
        cd test_cases
        # rm -rf $FILE_TEMP_OUT
        rm -rf test
        rm -rf $filename$file_ll
        rm -rf $filename$file_as
        rm -rf $filename$file_obj
        fi

        if [ $out_value -ne $ljw ]
        then
        let le_count+=1
        file_err[le_count]=$filename$file_c
        exp_err[le_count]=$exp
        act_err[le_count]=$act
        ll_err[le_count]=0
        as_err[le_count]=0
        out_err[le_count]=1
        rm -rf test
        rm -rf $filename$file_ll
        rm -rf $filename$file_as
        rm -rf $filename$file_obj
        continue
        else
        rm -rf $FILE_TEMP_OUT
        fi

        rm -rf test
        rm -rf $filename$file_ll
        rm -rf $filename$file_as
        rm -rf $filename$file_obj
        # echo $exp_str$exp
        # echo $act_str$act
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

    if [ "${out_err[i]}" = "1" ] ;
    then
    echo ${file_err[i]}"    标准输出与out存在差异"
    continue
    fi

    echo ${file_err[i]}"   返回的期望值与实际值不匹配"
    echo $exp_str${exp_err[i]}
    echo $act_str${act_err[i]}
done
fi


rm -rf sylib.a
rm -rf sylib.o
cd ../
rm -rf judge