#include "token.h"
#include "tokenlist.h"
#include <stdio.h>
#include <stdlib.h>

TokenList *tokenlist_create(void) {
    TokenList *t1 = malloc(sizeof(TokenList));
    if(!t1) {
        perror("Unable to allocate memory for TokenList.\n");
        exit(1);
    }

    int initial_capacity = 128;
    t1->tokens = malloc(sizeof(Token) * initial_capacity);
    t1->count = 0;
    t1->capacity = initial_capacity;

    return t1;
}

TokenList *tokenlist_append(TokenList *t1, Token tok) {
    if(t1->count == t1->capacity) {
        t1->capacity = t1->capacity * 2;
        
        Token *resized = realloc(t1->tokens, sizeof(Token) * t1->capacity);
        if(!resized) {
            perror("Unable to reallocate TokenList.\n");
            exit(1);
        }

        t1->tokens = resized;
    }
    t1->tokens[t1->count++] = tok;

    return t1;
}

void tokenlist_destroy(TokenList *t1) {
    if(!t1) {
        perror("Reference to \"NULL\" was passed.\nExpected reference to TokenList*.\n");
        exit(1);
    }

    if(t1->tokens) {
        for(int i = 0; i < t1->count; i++) {
            free(t1->tokens[i].lexeme);
        }
        free(t1->tokens);
    }
    free(t1);
}

void tokenlist_print(const TokenList *tl) {
    for (int i = 0; i < tl->count; i++) {
        Token tok = tl->tokens[i];

        if (tok.type == TOKEN_ERROR) continue;   /* report_lex_error already covers these */

        printf("#%-4d %-12s", tok.line, token_type_name(tok.type));

        if (tok.value != NULL) {
            printf(" %s", entry_str(tok.value));
        }

        printf("\n");
    }
}

