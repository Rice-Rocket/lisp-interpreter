#include "lexer.h"

#include <_ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    Token tok = {.type = Eof};
    insert_token(tokens, tok);

    return tokens;
}
