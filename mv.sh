#！/bin/bash
cd test_cases
file_ll=".ll"
for file in $(ls *[.ll])
do
  filename=${file%.*}
  echo ${filename}
  mv ${file} ../ans
done

