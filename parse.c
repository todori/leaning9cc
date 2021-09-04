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
// tokenがopであるなら、tokenを1つ前に進める
bool consume(char *op){
	if(!(token->kind == TK_RESERVED || token->kind == TK_RETURN )||
		 strlen(op) != token->len ||
		 !equal(token, op))
		return false;
	token = token->next;
	return true;
}

// Tokenが変数であることを仮定して、
// 現在のTokenを返し、Tokenを1つ前に進める
Token *consume_ident(){
	Token *return_token = token;
	token = token->next;
	return return_token;
}

// 現在のTokenが変数であるかを判断する
bool is_ident(){
	if(token->kind != TK_IDENT){
		return false;
	}
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

// 変数名を検索する。見つからなかった場合NULLを返す。
// 見つかった場合、そのLVarを返す
LVar *find_lvar(Token *tok){
	for(LVar *var = locals; var ;var = var->next){
		if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
			return var;
	}
	return NULL;
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


bool at_eof(){
	return token->kind == TK_EOF;
}

// 再帰のための関数 Prototype 宣言
void program(); // program = stmt*
Node *stmt(); // stmt = expr ";" | "return" expr ";"
Node *expr(); // expr = assign
Node *assign(); // assign = equality ("=" assign)?
Node *equality(); // equality = relational( "==" relational | "!=" relational) *
Node *relational(); // relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add(); // add = mul ("+" mul | "-" mul)*
Node *mul(); // mul = unary ("*" unary | "/" unary)*
Node *unary(); // unary = ("+" | "-")? primary
Node *primary(); // primary = num | ident | "(" expr ")"




// 再帰下降構文解析

// program = stmt*
void program(){
	int i = 0;
	while(!at_eof()){
		code[i++] = stmt();
	}
	code[i] = NULL; // 末尾のためのNULL
}

// stmt = expr ";" | "return" expr ";"
Node *stmt(){
	Node *node;

	if(consume("return")){
		node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
		node->rhs = expr(); // 右側Nodeでexpr
	}
	else{
		node = expr();
	}

	expect(";");
	return node;
}


//生成規則 expr = assign
Node *expr(){
	return assign();
}

// assign = equality ("=" assign)?
// 関数内のcomsumeでTokenを1つ進める
Node *assign(){
	Node *node = equality();

	if(consume("=")){
		node = new_node(ND_ASSIGN, node, assign());
	}

	return node;
}

// equality = relational ( "==" relational || "!=" relational)*
// 関数内のconsumeでTokenを1つ進める
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
// 関数内のcomsumeでTokenを1つ進める
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
// 関数内のcomsumeでTokenを１つ進める
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
//関数内のcomsumeでTokenを１つ進める
Node *unary(){
	if(consume("+"))
		return primary();
	if(consume("-")) // '-'の場合 '0 - num'の減算ノードとして表現する 
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}

// 生成規則 primary = num | ident | "(" expr ")"
// 関数内のconsume,expectとexpect_numberでTokenを1つ進める
Node *primary(){
	// 次のトークンが"("なら"(" expr ")"のはず
	if(consume("(")){
		Node *node = expr();
		expect(")"); //Token列を次へ進める
		return node;
	}

	if(is_ident()) // 変数の場合
	{
		Token *tok = consume_ident(); // 現在のTokenを戻し、consume_ident内でTokenを1つ進める
		Node *node = calloc(1,sizeof(Node));
		node->kind = ND_LVAR; // 変数ノードを作成する
		
		LVar *lvar = find_lvar(tok);
		if(lvar){ // 変数が見つかった場合
			node->offset = lvar->offset;
		}
		else{ // 変数が見つからなかった場合
			lvar = calloc(1, sizeof(LVar)); // 新しいローカル変数型を作成
			lvar->next = locals; // 新しいローカル変数の次へこれまで作成しているローカル変数連結を設定
			lvar->name = tok->str; // 新しいローカル変数の名前を現在のTokenから設定
			lvar->len = tok->len; // 新しいローカル変数の長さを現在のTokenから設定
			lvar->offset = locals->offset + 8; // 新しいローカル変数のoffsetをこれまでの変数offset+8に設定
			locals = lvar; // localsに作成したlvarを設定
			//上記の処理でlocalsの連結を先の伸ばしていく、スタックへ積むような処理になる
			
			node->offset = lvar->offset; // ノードのoffsetを設定
		}

		return node;
	}

	// そうでなければ数値のはず
	Node *node = new_node_num(expect_number()); // expect_number内でTokenを1つ進める

	return node;
}


// mainから呼ばれる関数
void parse(){
	// ローカル変数連結の最初を0で初期化する
	locals = calloc(1, sizeof(LVar));
	program();
}


