#include "9cc.h"


int if_LNum; // condegenで初期化

// ローカス変数の場合
// Nodeが変数の場合、そのアドレスを計算して、それをスタックへプッシュ
// プロローグでRBPの値(メモリアドレス)を設定しているので、
// そのアドレスからのオフセット分を計算した値(メモリアドレス)をRAXへ返し、
// それをスタックへプッシュする
void gen_lvar(Node *node){
	if(node->kind != ND_LVAR)
		error("代入の左辺値が変数ではありません。");

	printf("	mov rax, rbp\n"); // RBPの値(レジスタアドレス)をRAXレジスタへ代入
	printf("	sub rax, %d\n", node->offset); // RAXの値からオフセット値を引き、その結果をRAXへ
	printf("	push rax\n"); // RAXをスタックトップへプッシュ
}


// 抽象構文木を下りながらコードを生成する
void gen(Node *node){
	
	if(node->kind == ND_RETURN){
		gen(node->rhs); // exprが試行された結果がRaxへpushされる
		printf("	pop rax\n");
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");
		return;
	}
	else if(node->kind == ND_NUM){ //木終端の数字である場合、それをスタックへプッシュ
		printf("	push %d\n", node->val);
		return;
	}
	else if(node->kind == ND_LVAR){ //木終端が変数である場合、変数の値をスタックへ積む
		gen_lvar(node); // 変数のメモリアドレスを計算して、スタックトップへ積む(RSPが示す番地の値)
		printf("	pop rax\n"); // gen_lvarで計算したRSPの内容(メモリアドレス)をRAXへ代入
		printf("	mov rax, [rax]\n"); // RAXが示すメモリアドレスの値(変数の値)をRAXへ代入
		printf("	push rax\n"); // RAXの値をスタックトップへ積む
		return;
	}
	else if(node->kind == ND_ASSIGN){ // 宣言である場合。a=3などの処理を行う。
		gen_lvar(node->lhs); // 左側ノードは変数であるはずなので、そのメモリアドレスを計算してスタックトップへ積む
		gen(node->rhs); // 右側ノードのパース。数字、アドレスのプッシュ / 宣言の処理

		printf("	pop rdi\n"); // 宣言の場合、右側ノードは数値であるので、RSIは数字
		printf("	pop rax\n"); // 宣言の場合、左側ノードは変数であるので、RAXはメモリアドレスになる
		printf("	mov [rax], rdi\n"); // 変数アドレスへRDI値を代入
		printf("	push rdi\n"); // RDI値をスタックトップへ積む
		return;
	}
	else if(node->kind == ND_IF){ // if ノードである場合
		gen(node->lhs); // 左側ノードのパース 論理演算の結果がRAXへpushされる

		printf("	pop rax\n"); 
		printf("	cmp rax, 0\n"); // 結果が0(false)であることを比較
		printf("	je	.Lend%04d\n", if_LNum);

		gen(node->rhs); // 右側ノードのパース
		printf(".Lend%04d:\n", if_LNum++);
		return;

	}
	else if(node->kind == ND_IFEL){
		gen(node->lhs);

		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je	.Lelse%04d\n", if_LNum);
		gen(node->rhs);
		return;
	}
	else if(node->kind == ND_ELSE){
		gen(node->lhs);

		printf("	jmp .Lend%04d\n", if_LNum);
		printf(".Lelse%04d:\n",if_LNum);
		gen(node->rhs);
		printf(".Lend%04d:\n", if_LNum++);
		return;
	}

	gen(node->lhs); // 左のノードをパース。数字のプッシュ。変数アドレスのプッシュ、宣言の処理
	gen(node->rhs); // 右のノードをパース。

	printf("	pop rdi\n"); //後にスタックした値を先に取り出す。右側のノードの値
	printf("	pop rax\n"); //先のスタックした値を後に取り出す。左側のノードの値

	switch(node->kind){ // ノードの種類によりrax, rdiレジスタを操作する
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
		case ND_EQ:
			printf("	cmp rax, rdi\n"); // cmpの操作
			printf("	sete al\n"); // == の結果をフラグレジスタからalレジスタへ
			printf("	movzb rax, al\n"); // alは8bitなので、64bitへ展開した結果をraxへセットする。このとき上位56bitはゼロで埋める
			break;
		case ND_NE:
			printf("	cmp rax, rdi\n");
			printf("	setne al\n"); // cmpの!= の結果をフラグレジスタからalレジスタへ
			printf("	movzb rax, al\n");
			break;
		case ND_LT:
			printf("	cmp rax, rdi\n");
			printf("	setl al\n"); // cmpの< の結果をフラグレジスタからalレジスタへ
			printf("	movzb rax, al\n");
			break;
		case ND_LE:
			printf("	cmp rax, rdi\n");
			printf("	setle al\n"); // cmpの <= の結果をフラグレジスタからalレジスタへ
			printf("	movzb rax, al\n");
			break;
	}

	printf("	push rax\n"); // 演算結果が格納されているraxレジスタをスタックへプッシュする
}

void codegen(){
	
	//if_LNumの初期化
	if_LNum = 1;

	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// プロローグ
	// 変数 26個分 ('a'- 'z')の領域を確保する
	printf("	push rbp\n"); // RBPレジスタ（ベースレジスタ)の値(ベースポインタ値)を
	// RSPレジスタが指すスタックの先頭へプッシュする（メモリアクセス)
	printf("	mov rbp, rsp\n"); // RSPレジスタ値をRBPレジスタへ代入(メモリアドレス RBP = RSP)
	printf("	sub rsp, %d\n", locals->offset); // ローカル変数のオフセット分スタックを確保する 
	// 呼び出し時点のRBP	RBP
	// 
	// 
	// locals->next>offset
	// locals									RSP

	for(int i = 0; code[i]; i++){ // Codeの末尾はNULL
		gen(code[i]);
		
		// 式の評価結果としてスタックに１つの値が残っているはずなので、
		// スタックが溢れないようにポップしておく
		printf("	pop rax\n");
	}

	// スタックトップに式全体の値が残っているはずなので
	// それをRAXにロードして関数からの返り値とする
	// エピローグ
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");	
}


