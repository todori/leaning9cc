#include "9cc.h"

int main(int argc, char **argv){
	if(argc != 2){
		error("引数の個数が正しくありません\n");
		return 1;
	}

	char *user_input;

	user_input = argv[1]; //引数の文字列(コード)をuser_inputへ設定 
	token = tokenize(user_input); // コードからトークン列を作成する 
	parse(); // トークン列を抽象木構造へパースする
	codegen(); // 抽象構文木を下りながらコードを生成

	return 0;
}
