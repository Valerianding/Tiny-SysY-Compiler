
#！/bin/bash
cd test_cases
file_c=".c"
file_ll=".ll"
file_bc=".bc"
_opt="_opt"
_dis="_dis"
for file in $(ls *[.c])
do
  filename=${file%.*}
  echo ${filename}
  clang -S -emit-llvm -Xclang -disable-O0-optnone $filename$file_c -o $filename$file_ll
  opt -mem2reg $filename$file_ll -o $filename$file_bc
  llvm-dis $filename$file_bc -o $filename$file_ll
  rm -rf $filename$file_bc
done
