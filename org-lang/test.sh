#!/bin/bash 

assert() { 
	expected="$1" 
	input="$2" 
	./calc "$input" > tmp.s 
	cc -o tmp tmp.s 
	./tmp 
	actual="$?" 
	
	if [ "$actual" = "$expected" ]; then 
		echo "$input => $actual" 
	else 
		echo "$input => $expected expected, but got $actual" 
		exit 1 
	fi
} 

# 加算, 減算
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5;"
# 乗算, 除算
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
# 単項
assert 10 '-10+20;'
assert 10 '- -10;'
assert 10 '- - +10;'

# 比較演算子
# ==
assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'
# <, <=
assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'
# >, >=
assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'
# 変数
assert 3 'a=3; return a;'
assert 15 'a=10; b=5; return a+b;'


echo OK
