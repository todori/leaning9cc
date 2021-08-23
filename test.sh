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

assert 1 '0==0'
assert 0 '1==0'
assert 1 '1!=2'
assert 0 '3!=3'

assert 1 '3<4'
assert 1 '-1 < 0'
assert 0 ' -2 < -3'

assert 1 '3<=3'
assert 0 '4<=3'

assert 1 '3>-1'
assert 0 '9>11'

assert 1 '8>=6'
assert 1 '8>=8'
assert 0 '6>=8'


echo OK

