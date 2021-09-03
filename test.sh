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

assert 2 'a=2;'
assert 4 '4;'
assert 3 'a=1;
b=2;
c=a+b;'

assert 3 '2+1;'
assert 2 '1*(3-1);'
assert 1 '1==1;'
assert 0 '2>3;'

echo OK

