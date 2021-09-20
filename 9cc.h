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
	ND_RETURN, // return
	ND_IF,		// if
	ND_IFEL,	// if-else
	ND_ELSE,	// else
	ND_WHILE,	// while
	ND_FOR,		// for
	ND_FORCOND1, // for condition
	ND_FORCOND2, // for condition	
	ND_NULL,	// null node for for statement
	ND_BLOCK, // {}
	ND_FUNCTION_CALL, // function call
}NodeKind;

typedef struct Node Node;

// 抽象構造木のノードの型
struct Node{
	NodeKind kind; // ノードの型
	Node *lhs;	// 左辺(Left Hand Side)
	Node *rhs;	// 右辺(Right Hand Side)
	int val;		// kindがND_NUMの場合のみ使う
	int offset;	// kindがND_LVARの場合のみ使う
	Node *stmtLink; // ND_BLOCKの場合、stmtのノードを保持するポインタ
	char *f_name; // ND_FUNCTION_CALLの場合に使用。関数名
	int f_name_len; //
	Node *parameterLink; // ND_FUNCTION_CALLの場合、関数の引数ノードを保持するポインタ	
	int parameterNumber;
};


// トークンの種類
typedef enum{
	TK_RETURN,		// return
	TK_CONTROL,		// 制御構文 if-else
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



typedef struct LVar LVar;

// ローカル変数の型
struct LVar{
	LVar *next; // 次の変数かNULL;
	char *name; // 変数の名前
	int len;		// 名前の長さ
	int offset;	// RBPからのオフセット
};

// ローカル変数
LVar *locals;

typedef struct FName FName;

// 関数名の型
struct FName{
	char *name; // 関数名
	int len;		// 関数名長さ
	FName *next;
};

// 関数名の型のリンク
// アセンブラヘッダでの宣言に使用する
// parseでリンクを生成する
FName *functionNames;

extern void error(char *fmt, ...);

//Tokenize
extern Token *tokenize(char *user_input);

//Paser token
extern void parse();

// 注目しているトークン
// tokenize内でトークン連結を作成
Token *token;

// ローカル変数。連結リストとして表す
// parse内で初期化と連結リストを作成する
LVar *locals;

//";"を終端とする複数行コードのノードリンク。100行まで対応する 
Node *code[100];  

extern void codegen();




