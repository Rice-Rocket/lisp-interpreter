#ifndef __parser_h__
#define __parser_h__

#include "lexer.h"

typedef enum {
    ParseOk,
    ParseMissingLParen,
    ParseMissingRParen,
} ParseResultType;

typedef struct AST {
    Token *tok;
    int n_children;
    int child_cap;
    struct AST **children;
} AST;

typedef struct {
    ParseResultType type;
    AST *ast;
} ASTResult;

void init_children(AST *ast);
void insert_child(AST *ast, AST *child);
void print_ast(AST *ast, int depth);

ASTResult parse(TokenVector *tokens);

#endif // __parser_h__
