#include "9cc.h"


// 抽象構文木を下りながらコードを生成する
// ノードの種類が数値出ない場合、左辺と右辺をそれぞで木を下りコードを生成する
void gen(Node *node){
	
	if(node->kind == ND_NUM){ //木終端の数字である場合、それをスタックはプッシュ
		printf("	push %d\n", node->val);
		return;
	}

	gen(node->lhs); // 左のノードをパース。数字をプッシュしたら戻る。
	gen(node->rhs); // 右のノードをパース。数字をプッシュしたら戻る。

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

void codegen(Node *node){
	
	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// 抽象構造木のノード列を下りながらコードを生成
	gen(node);

	// スタックトップに式全体の値が残っているはずなので
	// それをRAXにロードして関数からの返り値とする
	printf("	pop rax\n");
	printf("	ret\n");

}


