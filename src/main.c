#include "interpreter.h"
#include "lexer.h"
#include "parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <editline/readline.h>

typedef struct {
    int row;
    int col;
} FileLoc;

int main() {
    puts("Lisp Version 0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        int has_error = 0;
        char *input = readline("Lisp > ");
        add_history(input);

        TokenVector *tokens = tokenize(input);

        ASTResult ast = parse(tokens);
        switch (ast.type) {
        case ParseOk:
            break;
        case ParseMissingLParen:
            printf("Syntax Error: Missing '('\n");
            has_error = 1;
            break;
        case ParseMissingRParen:
            printf("Syntax Error: Missing ')'\n");
            has_error = 1;
            break;
        }

        if (has_error) {
            continue;
        }

        int v = eval(ast.ast);

        printf("%d\n", v);

        free(input);
    }

    return 0;
}
