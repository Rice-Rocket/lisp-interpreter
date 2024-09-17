#include "parser.h"

#include <stdio.h>
#include <string.h>

void init_children(AST *ast) {
    ast->n_children = 0;
    ast->child_cap = INITIAL_CAPACITY;
    ast->children = (AST **)malloc(INITIAL_CAPACITY * sizeof(AST));

    if (!ast->children) {
        printf("Memory allocation Failed \n");
        exit(0);
    }
}

void insert_child(AST *ast, AST *child) {
    if (ast->n_children == ast->child_cap) {
        AST **temp = ast->children;
        ast->child_cap <<= 1;
        ast->children = realloc(ast->children, ast->child_cap * sizeof(AST));
        if (!ast->children) {
            printf("Out of memory\n");
            ast->children = temp;
            return;
        }
    }
    ast->children[ast->n_children++] = child;
}

void print_ast(AST *ast, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }

    if (ast->n_children == 0) {
        printf("%s\n", ast->tok->str);
    } else {
        printf("Expr:\n");

        for (int i = 0; i < ast->n_children; i++) {
            print_ast(ast->children[i], depth + 1);
        }
    }
}

ASTResult parse(TokenVector *tokens) {
    AST *ast = malloc(sizeof(AST));

    if (tokens->tokens->type == LParen) {
        init_children(ast);
        tokens->tokens++; // move past lparen

        while (tokens->tokens->type != RParen) {
            ASTResult child = parse(tokens);
            if (child.type != ParseOk) {
                return child;
            }

            if (tokens->tokens->type == Eof) {
                ASTResult res = {.type = ParseMissingRParen, .ast = NULL};
                return res;
            }

            insert_child(ast, child.ast);
        }

        tokens->tokens++; // move past rparen
    } else if (tokens->tokens->type != Eof) {
        Token *tok = malloc(sizeof(Token));

        tok->str = malloc(strlen(tokens->tokens->str));
        strcpy(tok->str, tokens->tokens->str);
        tok->type = tokens->tokens->type;
        ast->n_children = 0;
        ast->child_cap = 0;
        ast->tok = tok;

        tokens->tokens++; // move past identifier/operator/number
    }

    ASTResult res = {.type = ParseOk, .ast = ast};
    return res;
}
