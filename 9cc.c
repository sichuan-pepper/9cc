#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
    TK_RESERVED,    // 記号
    TK_NUM,         // 整数トークン
    TK_EOF,         // 入力の終わりを表すトークン 
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
    TokenKind kind;     // トークンの型
    Token *next;        // 次の入力トークン
    int val;            //kindがTK_NUMの場合、その数値
    char *str;
    int len;
};

//現在着目しているトークン
Token *token;

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
    ND_EQ,
    ND_NEQ,
    ND_GT,
    ND_LT,
    ND_GTE,
    ND_LTE
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node{
    NodeKind kind; // ノードの型
    Node *lhs; //左辺
    Node *rhs; //右辺
    int val;
};

// 入力プログラム
char *user_input;

//エラー箇所を報告する
void error_at(char *loc, char *fmt, ...){
    //fprintf(stderr, "error_at_top\n");
    va_list ap;
    va_start(ap, fmt);
    
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときは、トークンを一つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
    if (token->kind != TK_RESERVED || 
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op){
    if(token->kind != TK_RESERVED || 
       strlen(op) != token->len ||
       memcmp(token->str, op, token->len)){
        error_at(token->str, "'%c'ではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number(){
    if(token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;
    while(*p){
        fprintf(stderr, "%c\n",*p);
        if(isspace(*p)){
            //fprintf(stderr, "space\n");
            p++;
            continue;
        }
        if(!strncmp(p, "==", 2*sizeof(char)) || 
           !strncmp(p, ">=", 2*sizeof(char)) || 
           !strncmp(p, "<=", 2*sizeof(char)) ||
           !strncmp(p, "!=", 2*sizeof(char)) ){
            //fprintf(stderr, "2ch\n");
            cur = new_token(TK_RESERVED, cur, p, 2);
            p+=2;
            continue;
        }
        if(*p == '+' || *p == '-' || *p== '*' || *p == '/' || *p == '(' || *p == ')' || *p == '>' || *p == '<'){
            //fprintf(stderr, "reserved charactor.\n");
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }
        if(isdigit(*p)){
            //fprintf(stderr, "number.\n");
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 1);
    return head.next;
}


Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr();

Node *primary(){
    // 次のトークンが"("なら、"*(" expr ")"のはず
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }
    
    // そうでなければ数値のはず
    return new_node_num(expect_number());
}

Node *unary(){
    if (consume("+")){
        return primary();
    }
    if (consume("-")){
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node *mul(){
    Node *node = unary();
    
    for(;;){
        if(consume("*")){
            node = new_node(ND_MUL, node, unary());
        }else if(consume("/")){
            node = new_node(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

Node *add(){
    Node *node = mul();
    for(;;){
        if(consume("+")) {
            node = new_node(ND_ADD, node, mul());
        }else if (consume("-")){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node *relational(){
    Node *node = add();
    for(;;){
        if(consume("<")){
            node = new_node(ND_LT, node, add());
        }else if (consume("<=")){
            node = new_node(ND_LTE, node, add());
        }else if (consume(">")){
            node = new_node(ND_GT, node, add());
        }else if (consume(">=")){
            node = new_node(ND_GTE, node, add());
        }else{
            return node;
        }
    }
}

Node *equality(){
    Node *node = relational();
    for(;;){
        if(consume("==")){
            node = new_node(ND_EQ, node, relational());
        }else if(consume("!=")){
            node = new_node(ND_NEQ, node, relational());
        }else{
            return node;
        }
    }
}

Node *expr(){
    return equality();
}





void gen(Node *node){
    if(node->kind == ND_NUM){
        //fprintf(stderr, "token->val %d.\n", node->val);
        printf("  push %d\n", node->val);
        return;
    }
    
    gen(node->lhs);
    gen(node->rhs);
    
    printf("  pop rdi\n");
    printf("  pop rax\n");
    //fprintf(stderr,"kind: %d", node->kind);
    switch(node->kind){
    case ND_EQ:
        printf(
        "  cmp rax, rdi\n"
        "  sete al\n"
        "  movzb rax, al\n"
        );
        break;
    case ND_NEQ:
        printf(
        "  cmp rax, rdi\n"
        "  setne al\n"
        "  movzb rax, al\n"
        );
        break;
    case ND_LT:
        printf(
        "  cmp rax, rdi\n"
        "  setl al\n"
        "  movzb rax, al\n"
        );
        break;
    case ND_GT:
        printf(
        "  cmp rdi, rax\n"
        "  setl al\n"
        "  movzb rax, al\n"
        );
        break;
    case ND_LTE:
        printf(
        "  cmp rax, rdi\n"
        "  setle al\n"
        "  movzb rax, al\n"
        );
        break;
    case ND_GTE:
        printf(
        "  cmp rdi, rax\n"
        "  setle al\n"
        "  movzb rax, al\n"
        );
        break;
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    }
    printf("  push rax\n");
}

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
