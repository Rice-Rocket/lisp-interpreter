#ifndef __lexer_h__
#define __lexer_h__

#include <stdlib.h>

#define INITIAL_CAPACITY 8

typedef enum {
    LParen,
    RParen,
    Identifier,
    Operator,
    Number,
    Eof,
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

void init_tokens(TokenVector **tok_ptr);
void insert_token(TokenVector *container, Token item);
void print_tokens(TokenVector *container);
void free_tokens(TokenVector *container);

TokenVector *tokenize(char *input);

#endif // __lexer_h__
