#include "9cc.h"

int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "引数の個数が正しくありません.\n");
		return 1;
	}
    
    // トークナイズしてパースする
    user_input = argv[1];
        //fprintf(stderr, "トークナイズ開始.\n");
    token = tokenize(user_input);
    //fprintf(stderr, "トークナイズ終了.\n");
    Node *node = expr();
    //fprintf(stderr, "パース終了.\n");
    
    // アセンブリの前半部分を出力    
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
    
    gen(node);
    //fprintf(stderr, "ジェネレート終了.\n");
    
    // スタックトップに指揮全体の値が残っているはずなので
    // それをRAXにロードして関数化からの返り値とする
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
