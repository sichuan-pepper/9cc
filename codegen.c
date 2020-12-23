#include "9cc.h"


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
    case ND_LTE:
        printf(
        "  cmp rax, rdi\n"
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
