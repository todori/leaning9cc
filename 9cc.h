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
	ND_EQ,	// ==
	ND_NE,	// !=
	ND_LT,	// <
	ND_LE,	// <=
	ND_ASSIGN, // =
	ND_LVAR, // ローカル変数
	ND_NUM, // 整数
}NodeKind;

typedef struct Node Node;

// 抽象構造木のノードの型
struct Node{
	NodeKind kind; // ノードの型
	Node *lhs;	// 左辺(Left Hand Side)
	Node *rhs;	// 右辺(Right Hand Side)
	int val;		// kindがND_NUMの場合のみ使う
	int offset;	// kindがND_LVARの場合のみ使う
};


// トークンの種類
typedef enum{
	TK_RESERVED,	// 記号
	TK_NUM,				// 整数
	TK_IDENT,			// 識別子(1文字変数用)
	TK_EOF,				// 入力の終わりを表すトークン
}TokenKind;

typedef struct Token Token;

//トークンの型
struct Token{
	TokenKind kind;	// トークンの型
	Token *next;		// 次のトークン
	int val;				// kindがTK_NUMの場合、その数値
	char *str;			// トークンの文字列
	int len;				// トークンの長さ
};

extern void error(char *fmt, ...);

//Tokenize
extern Token *tokenize(char *user_input);

//Paser token
extern void parse();

Token *token; // 注目しているトークン

Node *code[100]; //";"を終端とする複数行コードのノードリンク。100行まで対応する 

extern void codegen();




