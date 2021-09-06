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

assert 12 '{a=5; b=7; c=a+b; return c;}'
assert 4 'a=1; b=2; if(a>b){c=3; b=2;} else{for(a=1; a < 3; a=a+1){ return 4;}}'
assert 5 'a=1; for(b=1; b<3; b=b+1) a=a+2; return a;'
assert 6 'a=1; while(a <= 5) a = a+1; return a;'
assert 5 'if(3<1) return 1; else return 5;'
assert 1 'if(3>1) return 1; else return 5;'
assert 1 'b=0; while(b<3)b=b+1; if(b>2) return 1; else return 2;'

assert 3 'b = 0;
r = 0;
while(b < 3)
 	b = b + 1;
if(b > 2) 
	for(c=0; c < b; c = c+1) 
		r = r + c;
else return 2;'
assert 1 'egg=1; orange=4; if(egg < orange) return egg;'
assert 6 'eg=1; or=4; if(eg > or) return eg; return 6;'
assert 7 'a=1; b=2; if(a>b) a=b; c=4; return 7;'
assert 1 'a=1; b=2; if(a<b) return a; return b;'
assert 2 'abc=2;'
assert 4 'foo = 3; bar = -1; ret = foo - bar;'

assert 14 'a=3; bg= 5 * 6 - 8; return a +bg / 2;'

assert 3 '2+1;'
assert 2 '1*(3-1);'
assert 0 '1!=1;'
assert 1 '2<3;'

echo OK

