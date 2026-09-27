#ifndef TOKENLIST_H
#define TOKENLIST_H

#include "token.h"

typedef struct TokenList {
    Token *tokens;
    int count;
    int capacity;
} TokenList;

TokenList *tokenlist_create(void);
TokenList *tokenlist_append(TokenList *t1, Token tok);
void tokenlist_print(const TokenList *t1);
void tokenlist_destroy(TokenList *t1);
#endif