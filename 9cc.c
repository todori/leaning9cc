#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//抽象構文木のノードの種類
typedef enum{
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // 整数
}NodeKind;

typedef struct Node Node;

// 抽象構造木のノードの型
struct Node{
	NodeKind kind; // ノードの型
	Node *lhs;	// 左辺(Left Hand Side)
	Node *rhs;	// 右辺(Right Hand Side)
	int val;		// kindがND_NUMの場合のみ使う
};

//新しいノードを作成し、そのノードを返す
//引数は新しいノードの種類、左辺、右辺
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

// 新しい数値型のノードを作成し、そのノードを返す
Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

// Prototypoe 宣言
Node *expr();
Node *mul();
Node *primary();


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

		if(strchr("+-*/()", *p)){
			// char *strchr(const char *s, int c);
			// sが示す文字列の中に最初に(charに変換された)cが出現する位置を特定する
			// 文字列中にcがなければNULLを返す。
			// よって、このifでは、"+-*/()"のいずれかがあれば0で無い値がstrchrから返されるのでTrueになり、なければNULL : 0 が戻るので、Falseになる
			//
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


// 再帰下降構文解析
//生成規則 expr = mul ( "+" mul | "-" mul)*
//関数内のcunsumeでTokenを1つ進める
Node *expr(){
	Node *node = mul();
	
	for(;;){
		if(consume('+')) 
			node = new_node(ND_ADD, node, mul());
		else if(consume('-')) 
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}

// 生成規則 mul = primary ( "*" primary | "/" primary)*
// 関数内のconsumeでTokenを1つ進める
Node *mul(){
	Node *node = primary();

	for(;;){
		if(consume('*'))
			node = new_node(ND_MUL, node, primary());
		else if(consume('/'))
			node = new_node(ND_DIV, node, primary());
		else
			return node;
	}
}

// 生成規則 primary = num | "(" expr ")"
// 関数内のconsumeとexpect_numberでTokenを1つ進める
Node *primary(){
	// 次のトークンが"("なら"(" expr ")"のはず
	if(consume('(')){
		Node *node = expr();
		expect(')');
		return node;
	}

	// そうでなければ数値のはず
	return new_node_num(expect_number());
}

// 抽象構文木を下りながらコードを生成する
// ノードの種類が数値出ない場合、左辺と右辺をそれぞで木を下りコードを生成する
void gen(Node *node){
	
	if(node->kind == ND_NUM){
		printf("	push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch(node->kind){
		case ND_ADD:
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
			printf("	sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("	imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
	}

	printf("	push rax\n");
}



int main(int argc, char **argv){
	if(argc != 2){
		error("引数の個数が正しくありません\n");
		return 1;
	}


	user_input = argv[1]; // エラー表示用
	token = tokenize(); // tokenize()内でuser_inputを操作してTokenizeしている
	Node *node = expr(); // トークン列を抽象木構造へパースする

	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// 抽象構文木を下りながらコードを生成
	gen(node);

	// スタックトップに式全体の値が残っているはずなので
	// それをRAXにロードして関数からの返り値とする
	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
