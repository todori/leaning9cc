#include "9cc.h"


// エラー箇所を報告する
// 内容を表示するための改良版
void error_at(char *loc,char *user_input, char *fmt, ...){
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



//新しいトークンを作成してcurにつなげる
//この関数宣言で Tokenのポインタ変数を返す
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
	Token *tok = calloc(1, sizeof(Token)); //新しいトークン
	tok->kind = kind; // 新しいトークンの型
	tok->str = str; // 新しいトークンの文字列
	tok->len = len; // 新しいトークンの長さ
	cur->next = tok; // cur->nextに新しいトークンを代入する
	return tok; // 新しいトークンを返す
}


// 文字列比較 , Tokenize用
bool startSwitch(char *p, char *q){
	return memcmp(p , q, strlen(q)) == 0;
}

// 入力文字列をトークナイズしてそれを返す
// この宣言でTokenのポインタを返す
Token *tokenize(char *user_input){
	
	char *p = user_input; // user_inputをTokenizeするpへ渡す
 	Token head; // 戻り値
	head.next = NULL; // 
	Token *cur = &head; // 現在のToken位置

	while(*p){ //文字列の最後(0x00)以外はASCIIコードで何らかの値を持つ。
		// 空白文字をスキップ
		if(isspace(*p)){ // isspce is defined in ctype.h
			p++; // 次の文字へ
			continue;
		}

		// 2文字記号の判断を1文字記号より先に行う
		if( startSwitch(p, "==") || startSwitch(p, "!=") ||
				startSwitch(p, ">=") || startSwitch(p, "<=")) {
			cur = new_token(TK_RESERVED, cur, p, 2);
			p+=2;
			continue;
		}
			

		// 1文字記号の判断
		if(strchr("+-*/()<>", *p)){
			// char *strchr(const char *s, int c);
			// sが示す文字列の中に最初に(charに変換された)cが出現する位置を特定する
			// 文字列中にcがなければNULLを返す。
			// よって、このifでは、"+-*/()"のいずれかがあれば0で無い値がstrchrから返されるのでTrueになり、なければNULL : 0 が戻るので、Falseになる
			//
			// 新しくTK_RESERVED型で文字列pのTokenを作成し、
			// curの次のToken(cur.next)へそのTokenを代入
			// そして、戻り値としてその作成したTokenのポインタがcurに代入される
			cur = new_token(TK_RESERVED, cur, p++, 1); 
			// 次の文字へ
			continue;
		}

		if(isdigit(*p)){ // 数字の場合, isdigit is defined in ctype.h
			// 新しくTK_NUM型で文字列pのTokenを作成し、
			// curの次のToken(cur.next)へそのTokenを代入
			// そして、戻り値としてその作成したTokenのポインタがcurに代入される
			cur = new_token(TK_NUM, cur, p, 0); //数値は文字列数を比較しないため len = 0

			// cur Tokenの数値を第1引数: p文字列から抽出し、数字でないアドレスを第2引数 :&pへ入れる
			// 例えば"123ABC"の場合、戻り値が数値の123, &pは文字列"ABC"の先頭アドレス
			cur->val = strtol(p , &p, 10);
			continue;
		}

		error_at(p,user_input, "トークナイズできません。");

	}

	// 新しいTK_EOF型ので文字列pのTokenを作成し、
	// curの次のToken(cur.next)へそのTokenを代入する
	new_token(TK_EOF, cur, p, 1);

	// 最初のTokenは何も値がないため、head.nextのTokenポインタを返す
	return head.next;
}

