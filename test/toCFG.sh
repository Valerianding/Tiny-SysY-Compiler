#!/bin/zsh

find ./ -name "*.c" | while read fname
do
	echo "$fname";
	clang -S -emit-llvm $fname -o ${fname%.c}.ll;
	opt -dot-cfg ${fname%.c}.ll;
	mv .main.dot ${fname%.c}.dot;
	dot ${fname%.c}.dot -Tpng -o ${fname%.c}.png;
	rm -rf ${fname%.c}.dot
done
