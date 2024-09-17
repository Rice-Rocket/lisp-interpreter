#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <editline/readline.h>

#define INITIAL_CAPACITY 8

typedef struct {
    int row;
    int col;
} FileLoc;

typedef enum {
    LParen,
    RParen,
    Identifier,
    Operator,
    Number,
} TokenType;

typedef struct {
    char *str;
    TokenType type;
} Token;

typedef struct {
    size_t size;
    size_t capacity;
    Token *tokens;
} TokenVector;

typedef struct AST {
    Token *tok;
    int n_children;
    int child_cap;
    struct AST **children;
} AST;

void init_tokens(TokenVector **tok_ptr) {
    TokenVector *container;
    container = (TokenVector *)malloc(sizeof(TokenVector));
    if (!container) {
        printf("Memory allocation Failed \n");
        exit(0);
    }

    container->size = 0;
    container->capacity = INITIAL_CAPACITY;
    container->tokens = (Token *)malloc(INITIAL_CAPACITY * sizeof(Token));
    if (!container->tokens) {
        printf("Memory allocation Failed \n");
        exit(0);
    }

    *tok_ptr = container;
}

void insert_token(TokenVector *container, Token item) {
    if (container->size == container->capacity) {
        Token *temp = container->tokens;
        container->capacity <<= 1;
        container->tokens =
            realloc(container->tokens, container->capacity * sizeof(Token));
        if (!container->tokens) {
            printf("Out of memory\n");
            container->tokens = temp;
            return;
        }
    }
    container->tokens[container->size++] = item;
}

void print_tokens(TokenVector *container) {
    printf("[\n");
    for (int i = 0; i < container->size; i++) {
        printf("    Token { \"%s\" }\n", container->tokens[i].str);
    }
    printf("]\n");
}

void free_tokens(TokenVector *container) {
    free(container->tokens);
    free(container);
}

TokenVector *tokenize(char *input) {
    TokenVector *tokens;
    init_tokens(&tokens);

    char *cur_tok = malloc(0);
    enum {
        NoToken,
        IdentToken,
        NumToken,
    } cur_tok_ty = NoToken;

    for (; *input != '\0'; input++) {
        if (isalpha(*input) || *input == '_') {
            cur_tok_ty = IdentToken;
            char c = *input;

            size_t ident_len = strlen(cur_tok);
            // one for extra char, one for trailing zero
            cur_tok = realloc(cur_tok, ident_len + 1 + 1);
            cur_tok[ident_len] = c;
            cur_tok[ident_len + 1] = '\0';

            continue;
        } else if (isnumber(*input) || *input == '.') {
            cur_tok_ty = NumToken;
            char c = *input;

            size_t ident_len = strlen(cur_tok);
            // one for extra char, one for trailing zero
            cur_tok = realloc(cur_tok, ident_len + 1 + 1);
            cur_tok[ident_len] = c;
            cur_tok[ident_len + 1] = '\0';

            continue;
        } else if (cur_tok_ty != NoToken) {
            char *s = malloc(strlen(cur_tok));
            strcpy(s, cur_tok);

            if (cur_tok_ty == IdentToken) {
                Token tok = {.str = s, .type = Identifier};
                insert_token(tokens, tok);
            } else if (cur_tok_ty == NumToken) {
                Token tok = {.str = s, .type = Number};
                insert_token(tokens, tok);
            }

            cur_tok_ty = NoToken;

            free(cur_tok);
            *cur_tok = *(char *)malloc(0);
        }

        if (*input == '(') {
            Token tok = {.str = "(", .type = LParen};
            insert_token(tokens, tok);
        } else if (*input == ')') {
            Token tok = {.str = ")", .type = RParen};
            insert_token(tokens, tok);
        } else if (*input == '+' || *input == '-' || *input == '*' ||
                   *input == '/') {
            char *s = malloc(sizeof(char));
            *s = *input;
            Token tok = {.str = s, .type = Operator};
            insert_token(tokens, tok);
        }
    }

    if (cur_tok_ty != NoToken) {
        char *s = malloc(strlen(cur_tok));
        strcpy(s, cur_tok);

        if (cur_tok_ty == Identifier) {
            Token tok = {.str = s, .type = Identifier};
            insert_token(tokens, tok);
        } else if (cur_tok_ty == Number) {
            Token tok = {.str = s, .type = Number};
            insert_token(tokens, tok);
        }
    }

    free(cur_tok);

    return tokens;
}

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

AST *parse(TokenVector *tokens) {
    AST *ast = malloc(sizeof(AST));

    if (tokens->tokens->type == LParen) {
        init_children(ast);
        tokens->tokens++; // move past lparen

        while (tokens->tokens->type != RParen) {
            insert_child(ast, parse(tokens));
        }

        tokens->tokens++; // move past rparen
    } else {
        Token *tok = malloc(sizeof(Token));

        tok->str = malloc(strlen(tokens->tokens->str));
        strcpy(tok->str, tokens->tokens->str);
        tok->type = tokens->tokens->type;
        ast->n_children = 0;
        ast->child_cap = 0;
        ast->tok = tok;

        tokens->tokens++; // move past identifier/operator/number
    }

    return ast;
}

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
    if (ast->tok->type == Number) {
        return atoi(ast->tok->str);
    }

    if (ast->tok->type == Identifier) {
        return 0;
    }

    char *op = ast->children[0]->tok->str;

    int x = eval(ast->children[1]);

    for (int i = 2; i < ast->n_children; i++) {
        x = eval_op(op, x, eval(ast->children[i]));
    }

    return x;
}

int main() {
    puts("Lisp Version 0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char *input = readline("Lisp > ");
        add_history(input);

        TokenVector *tokens = tokenize(input);

        AST *ast = parse(tokens);
        int v = eval(ast);

        printf("%d\n", v);

        free(input);
    }

    return 0;
}
