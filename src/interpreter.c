#include "parser.h"

int eval_op(char *op, int x, int y) {
    if (*op == '+') {
        return x + y;
    }
    if (*op == '-') {
        return x - y;
    }
    if (*op == '*') {
        return x * y;
    }
    if (*op == '/') {
        return x / y;
    }

    return 0;
}

int eval(AST *ast) {
    if (ast->tok) {
        if (ast->tok->type == Number) {
            return atoi(ast->tok->str);
        }

        if (ast->tok->type == Identifier) {
            return 0;
        }
    }

    char *op = ast->children[0]->tok->str;

    int x = eval(ast->children[1]);

    for (int i = 2; i < ast->n_children; i++) {
        x = eval_op(op, x, eval(ast->children[i]));
    }

    return x;
}
