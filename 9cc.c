#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	char *p = argv[1]; // 文字列の開始アドレスをpに代入


	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	printf("	mov rax, %ld\n", strtol(p, &p, 10)); 
	// long strtol(char *s, char **endptr, int base): *s文字列をbase基底の数値へ変換する 
	// *s = "124+4", base = 10の場合,
	// return = 124, **endptr = '+'のアドレス


	while(*p){
		if(*p == '+'){
			p++; // 次の文字へ
			printf("	add rax, %ld\n", strtol(p, &p, 10));
			continue;
		}

		if(*p == '-'){
			p++;
			printf("	sub rax, %ld\n", strtol(p, &p, 10));
			continue;
		}

		fprintf(stderr, "予期しない文字です: '%c'\n", *p);
		return 1;
	}

	printf("	ret\n");
	return 0;
}
