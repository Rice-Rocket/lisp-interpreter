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

    char *ident = malloc(0);
    int has_ident = 0;

    for (; *input != '\0'; input++) {
        if (isalpha(*input)) {
            has_ident = 1;
            char c = *input;

            size_t ident_len = strlen(ident);
            // one for extra char, one for trailing zero
            ident = realloc(ident, ident_len + 1 + 1);
            ident[ident_len] = c;
            ident[ident_len + 1] = '\0';

            continue;
        } else {
            if (has_ident) {
                has_ident = 0;
                char *s = malloc(strlen(ident));
                strcpy(s, ident);

                Token tok = {.str = s, .type = Identifier};
                insert_token(tokens, tok);

                free(ident);
                *ident = *(char *)malloc(0);
            }
        }

        if (*input == '(') {
            Token tok = {.str = "(", .type = LParen};
            insert_token(tokens, tok);
        } else if (*input == ')') {
            Token tok = {.str = ")", .type = RParen};
            insert_token(tokens, tok);
        }
    }

    if (has_ident) {
        char *s = malloc(strlen(ident));
        strcpy(s, ident);

        Token tok = {.str = s, .type = Identifier};
        insert_token(tokens, tok);
    }

    free(ident);

    return tokens;
}

int main() {
    puts("Lisp Version 0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char *input = readline("Lisp > ");
        add_history(input);

        TokenVector *tokens = tokenize(input);
        print_tokens(tokens);

        free(input);
        free_tokens(tokens);
    }

    return 0;
}
