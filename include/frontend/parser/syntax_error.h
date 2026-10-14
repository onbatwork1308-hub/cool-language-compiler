#ifndef SYNTAX_ERROR_H
#define SYNTAX_ERROR_H

#include "table.h"
#include "token.h"
#include "dynarray.h"
#include <stdio.h>
#include <stdbool.h>

/**
 * @enum SyntaxErroKind
 * @brief Categories of syntax errors.
 */
typedef enum {
    SYNTAX_ERR_EXPECTED_TOKEN, 
    SYNTAX_ERR_EXPECTED_EXPR
} SyntaxErrorKind;

typedef struct SyntaxError {
    SyntaxErrorKind kind;
    int line;
    TokenType expected;
    TokenType found;
    Entry *found_value;
} SyntaxError;

typedef struct SyntaxErrorList {
    DynArray *errors;
} SyntaxErrorList;

SyntaxErrorList *syntax_error_list_create(void);
void syntax_error_list_destroy(SyntaxErrorList *l);

void syntax_error_list_add_expected(SyntaxErrorList *l, TokenType expected, Token found);
void syntax_error_list_add_expected_expr(SyntaxErrorList *l, Token found);

int syntax_error_list_count(const SyntaxErrorList *l);
bool syntax_error_list_report(const SyntaxErrorList *l, FILE *out);
#endif