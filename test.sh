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

assert 3 'if(3>1) return 3;'
assert 1 'egg=1; orange=4; if(egg < orange) return egg;'
assert 4 'egg=1; orange=4; if(egg > orange) return egg;'
assert 7 'a=1; b=2; if(a>b) a=b; c=4; return 7;'
assert 4 'a=1; b=2; if(a>b) a=b; c=4;'
assert 2 'abc=2;'
assert 4 'foo = 3; bar = -1; ret = foo - bar;'

assert 14 'a=3; bg= 5 * 6 - 8; return a +bg / 2;'

assert 3 '2+1;'
assert 2 '1*(3-1);'
assert 0 '1!=1;'
assert 1 '2<3;'

echo OK

