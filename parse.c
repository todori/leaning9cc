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

//次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
//それ以外の場合にはエラーを報告する
int expect_number(Token *token){
	if(token->kind != TK_NUM)
		error("数ではありません");
	int val = token->val;
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
Node *expr(Token **, Token *); // expr = equality
Node *equality(Token **, Token *); // equality = relational( "==" relational | "!=" relational) *
Node *relational(Token **, Token *); // relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add(Token **, Token *); // add = mul ("+" mul | "-" mul)*
Node *mul(Token **, Token *); // mul = unary ("*" unary | "/" unary)*
Node *unary(Token **, Token *); // unary = ("+" | "-")? primary
Node *primary(Token ** , Token *); // primary = num | "(" expr ")"


// 再帰下降構文解析
//生成規則 expr = equality
Node *expr(Token **rest, Token *tok){
	return equality(rest, tok);
}

// equality = relational ( "==" relational || "!=" relational)*
Node *equality(Token **rest, Token *tok){
	Node *node = relational(&tok, tok);

	for(;;){
		if(equal(tok, "=="))
			node = new_node(ND_EQ, node, relational(&tok, tok->next));
		else if(equal(tok, "!="))
			node = new_node(ND_NE, node, relational(&tok, tok->next));
		else{
			*rest = tok; // 現在のトークン配列の場所をrestへ反映
			return node;
		}
	}
}

// relational = add ( "<" add | "<=" add | ">" add | ">=" add)*
Node *relational(Token **rest, Token *tok){
	Node *node = add(&tok, tok);

	for(;;){
		if(equal(tok, "<"))
			node = new_node(ND_LT, node, add(&tok, tok->next));
		else if(equal(tok, "<="))
			node = new_node(ND_LE, node, add(&tok, tok->next));
		else if(equal(tok, ">"))
			node = new_node(ND_LT, add(&tok, tok->next), node);
		else if(equal(tok, ">="))
			node = new_node(ND_LE, add(&tok, tok->next), node);
		else{
			*rest = tok;
			return node;
		}
	}
}

// add = mul ( "+" mul | "-" mul)*
Node *add(Token **rest, Token *tok){
	Node *node = mul(&tok, tok);

	for(;;){
		if(equal(tok, "+")) 
			node = new_node(ND_ADD, node, mul(&tok, tok->next));
		else if(equal(tok, "-")) 
			node = new_node(ND_SUB, node, mul(&tok, tok->next));
		else{
			*rest = tok;
			return node;
		}
	}
}

// 生成規則 mul = unary ( "*" unary | "/" unary)*
// 関数内のconsumeでTokenを1つ進める
Node *mul(Token **rest, Token *tok){
	Node *node = unary(&tok, tok);

	for(;;){
		if(equal(tok, "*")){
			node = new_node(ND_MUL, node, unary(&tok, tok->next));
		}
		else if(equal(tok, "/")){
			node = new_node(ND_DIV, node, unary(&tok, tok->next));
		}
		else{
			*rest = tok;
			return node;
		}
	}
}

//生成規則 unary = ("+" | "-")?primary
//
Node *unary(Token **rest, Token *tok){
	if(equal(tok, "+"))
		return primary(&tok, tok->next);
	if(equal(tok, "-")) // '-'の場合 '0 - num'の減算ノードとして表現する 
		return new_node(ND_SUB, new_node_num(0), primary(&tok, tok->next));
	return primary(rest, tok);
}

// 生成規則 primary = num | "(" expr ")"
// 関数内のconsumeとexpect_numberでTokenを1つ進める
Node *primary(Token **rest, Token *tok){
	// 次のトークンが"("なら"(" expr ")"のはず
	if(equal(tok, "(")){
		Node *node = expr(&tok, tok->next);
		*rest = skip(tok, ")"); //Token列を次へ進める
		return node;
	}

	// そうでなければ数値のはず
	Node *node = new_node_num(expect_number(tok));
	*rest = tok->next; // Token列を次へ進める

	return node;
}

Node *parse(Token *token){
	Node *node = expr(&token, token);
	return node;
}

