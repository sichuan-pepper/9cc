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
    ND_LT,
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
void error_at(char *loc, char *fmt, ...);
// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...);
// 次のトークンが期待している記号のときは、トークンを一つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op);
// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op);
// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr();
Node *primary();
Node *unary();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *expr();
void gen(Node *node);
int main(int argc, char **argv);
