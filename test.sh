#!/bin/bash

assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
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

# レジスタは負の値を扱えないので、結果が負にならないようにテストを行う
assert 1 ' -9+10'
assert 75  ' -5*(-9-6)'
assert 2 '-4/-2'
assert 5 '-5/-1'

echo OK

