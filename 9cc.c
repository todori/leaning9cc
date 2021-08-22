#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// トークンの種類
typedef enum{
	TK_RESERVED,	// 記号
	TK_NUM,				// 整数
	TK_EOF,				// 入力の終わりを表すトークン
}TokenKind;

typedef struct Token Token;

//トークンの型
struct Token{
	TokenKind kind;	// トークンの型
	Token *next;		// 次のトークン
	int val;				// kindがTK_NUMの場合、その数値
	char *str;			// トークンの文字列
};

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// エラー箇所を報告する
// 内容を表示するための改良版
void error_at(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos , " "); // pos個の空白を出力
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

//エラーを報告するための関数
//printfと同じ引数を取る
void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

//次のトークンが期待している記号のときには、トークンを１つ読み進めて
//真を返す。それ以外の場合には偽を返す。
bool consume(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next; // トークンを１つ読み進める
	return true;
}

//次のトークンが期待している記号のときには、トークンを1つ読み進める。
//それ以外の場合にはエラーを報告する
void expect(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		error_at(token->str, "'%c'ではありません", op);
	token = token->next; // トークンを１つ読み進める
}

//次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
//それ以外の場合にはエラーを報告する
int expect_number(){
	if(token->kind != TK_NUM)
		error_at(token->str, "数ではありません");
	int val = token->val;
	token = token->next; // トークンを１つ読み進める
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

//新しいトークンを作成してcurにつなげる
//この関数宣言で Tokenのポインタ変数を返す
Token *new_token(TokenKind kind, Token *cur, char *str){
	Token *tok = calloc(1, sizeof(Token)); //新しいトークン
	tok->kind = kind; // 新しいトークンの型
	tok->str = str; // 新しいトークンの文字列
	cur->next = tok; // cur->nextに新しいトークンを代入する
	return tok; // 新しいトークンを返す
}


// 入力文字列をトークナイズしてそれを返す
// この宣言でTokenのポインタを返す
Token *tokenize(){
	
	char *p = user_input; // user_inputをTokenizeするpへ渡す
 	Token head; // 戻り値
	head.next = NULL; // 
	Token *cur = &head; // 現在のToken位置

	while(*p){ //文字列の最後(0x00)以外はASCIIコードで何らかの値を持つ。
		// 空白文字をスキップ
		if(isspace(*p)){
			p++; // 次の文字へ
			continue;
		}

		if(*p == '+' || *p == '-'){ // 文字が'+'か'-'の場合
			// 新しくTK_RESERVED型で文字列pのTokenを作成し、
			// curの次のToken(cur.next)へそのTokenを代入
			// そして、戻り値としてその作成したTokenのポインタがcurに代入される
			cur = new_token(TK_RESERVED, cur, p++); 
			// 次の文字へ
			continue;
		}

		if(isdigit(*p)){ // 数字の場合
			// 新しくTK_NUM型で文字列pのTokenを作成し、
			// curの次のToken(cur.next)へそのTokenを代入
			// そして、戻り値としてその作成したTokenのポインタがcurに代入される
			cur = new_token(TK_NUM, cur, p);
			// cur Tokenの数値を第1引数: p文字列から抽出し、数字でないアドレスを第2引数 :&pへ入れる
			// 例えば"123ABC"の場合、戻り値が数値の123, &pは文字列"ABC"の先頭アドレス
			cur->val = strtol(p , &p, 10);
			continue;
		}

		error_at(p, "トークナイズできません。");

	}

	// 新しいTK_EOF型ので文字列pのTokenを作成し、
	// curの次のToken(cur.next)へそのTokenを代入する
	new_token(TK_EOF, cur, p);

	// 最初のTokenは何も値がないため、head.nextのTokenポインタを返す
	return head.next;
}

int main(int argc, char **argv){
	if(argc != 2){
		error("引数の個数が正しくありません\n");
		return 1;
	}


	user_input = argv[1]; // エラー表示用
	token = tokenize(); // tkenize()内でuser_inputを操作してTokenizeしている


	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// 式の最初は数でなければならないので、それをチェックして
	// 最初のmov命令を出力
	printf("	mov rax, %d\n", expect_number()); 

	// '+<数>' あるいは '-<数>'というトークンの並びを消費しつつ
	// アセンブリを出力
	while(!at_eof()){
		if(consume('+')){
			printf("	add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("	sub rax, %d\n", expect_number());
	}

	printf("	ret\n");
	return 0;
}
