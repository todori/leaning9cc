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

assert 2 'abc=2;'
assert 4 'foo = 3; bar = -1; ret = foo - bar;'

assert 3 '2+1;'
assert 2 '1*(3-1);'
assert 1 '1==1;'
assert 0 '2>3;'

echo OK

