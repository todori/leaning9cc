#include "9cc.h"



void error(char *fmt, ...){
	
	va_list ap;
	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// tokがopを同等なのか判断する
bool equal(Token *tok, char *op){
	return memcmp(tok->str, op, tok->len) == 0 && op[tok->len] =='\0';
}

// tokがopと同じである場合、トークン列を次へ進める
Token *skip(Token *tok, char *op){
	if(!equal(tok, op))
		error("expected '%s'", op);
	return tok->next;
}

// Global 変数のtokenを処理する
bool consume(char *op){
	if(token->kind != TK_RESERVED ||
		 strlen(op) != token->len ||
		 !equal(token, op))
		return false;
	token = token->next;
	return true;
}

// Global 変数のtokenを処理する
void expect(char *op){
	if(token->kind != TK_RESERVED ||
		 strlen(op) != token->len ||
		 !equal(token, op))
		error("'%c'ではありません", op);
	token=token->next;
}

// Global 変数のtokenを処理する
//次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
//それ以外の場合にはエラーを報告する
int expect_number(){
	if(token->kind != TK_NUM)
		error("数ではありません");
	int val = token->val;
	token = token->next;
	return val;
}


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



// 再帰のための関数 Prototype 宣言
Node *expr(); // expr = equality
Node *equality(); // equality = relational( "==" relational | "!=" relational) *
Node *relational(); // relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add(); // add = mul ("+" mul | "-" mul)*
Node *mul(); // mul = unary ("*" unary | "/" unary)*
Node *unary(); // unary = ("+" | "-")? primary
Node *primary(); // primary = num | "(" expr ")"


// 再帰下降構文解析
//生成規則 expr = equality
Node *expr(){
	return equality();
}

// equality = relational ( "==" relational || "!=" relational)*
Node *equality(){
	Node *node = relational();

	for(;;){
		if(consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if(consume("!="))
			node = new_node(ND_NE, node, relational());
		else{
			return node;
		}
	}
}

// relational = add ( "<" add | "<=" add | ">" add | ">=" add)*
Node *relational(){
	Node *node = add();

	for(;;){
		if(consume("<"))
			node = new_node(ND_LT, node, add());
		else if(consume("<="))
			node = new_node(ND_LE, node, add());
		else if(consume(">"))
			node = new_node(ND_LT, add(), node);
		else if(consume(">="))
			node = new_node(ND_LE, add(), node);
		else{
			return node;
		}
	}
}

// add = mul ( "+" mul | "-" mul)*
Node *add(){
	Node *node = mul();

	for(;;){
		if(consume("+")) 
			node = new_node(ND_ADD, node, mul());
		else if(consume("-")) 
			node = new_node(ND_SUB, node, mul());
		else{
			return node;
		}
	}
}

// 生成規則 mul = unary ( "*" unary | "/" unary)*
// 関数内のconsumeでTokenを1つ進める
Node *mul(){
	Node *node = unary();

	for(;;){
		if(consume("*")){
			node = new_node(ND_MUL, node, unary());
		}
		else if(consume("/")){
			node = new_node(ND_DIV, node, unary());
		}
		else{
			return node;
		}
	}
}

//生成規則 unary = ("+" | "-")?primary
//
Node *unary(){
	if(consume("+"))
		return primary();
	if(consume("-")) // '-'の場合 '0 - num'の減算ノードとして表現する 
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}

// 生成規則 primary = num | "(" expr ")"
// 関数内のconsumeとexpect_numberでTokenを1つ進める
Node *primary(){
	// 次のトークンが"("なら"(" expr ")"のはず
	if(consume("(")){
		Node *node = expr();
		expect(")"); //Token列を次へ進める
		return node;
	}

	// そうでなければ数値のはず
	Node *node = new_node_num(expect_number());

	return node;
}

Node *parse(){
	Node *node = expr();
	return node;
}

