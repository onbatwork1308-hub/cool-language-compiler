#include "token.h"

/* token.c, or a small addition to keyword.c/wherever fits your layout */
static const char *token_type_names[] = {
    [TOKEN_CLASS] = "CLASS", [TOKEN_IF] = "IF", [TOKEN_THEN] = "THEN",
    [TOKEN_ELSE] = "ELSE", [TOKEN_FI] = "FI", [TOKEN_WHILE] = "WHILE",
    [TOKEN_LOOP] = "LOOP", [TOKEN_POOL] = "POOL", [TOKEN_FALSE] = "FALSE",
    [TOKEN_TRUE] = "TRUE", [TOKEN_INHERITS] = "INHERITS",
    [TOKEN_ISVOID] = "ISVOID", [TOKEN_LET] = "LET", [TOKEN_CASE] = "CASE",
    [TOKEN_OF] = "OF", [TOKEN_ESAC] = "ESAC", [TOKEN_NEW] = "NEW",
    [TOKEN_NOT] = "NOT", [TOKEN_IN] = "IN",

    [TOKEN_DARROW] = "DARROW", [TOKEN_AT] = "AT", [TOKEN_NEG] = "NEG",
    [TOKEN_PLUS] = "PLUS", [TOKEN_MINUS] = "MINUS", [TOKEN_MUL] = "MUL",
    [TOKEN_DIVIDE] = "DIVIDE", [TOKEN_LPAREN] = "LPAREN",
    [TOKEN_RPAREN] = "RPAREN", [TOKEN_LBRACE] = "LBRACE",
    [TOKEN_RBRACE] = "RBRACE", [TOKEN_SEMICOLON] = "SEMICOLON",
    [TOKEN_COLON] = "COLON", [TOKEN_COMMA] = "COMMA", [TOKEN_DOT] = "DOT",
    [TOKEN_ASSIGN] = "ASSIGN", [TOKEN_EQ] = "EQ", [TOKEN_LT] = "LT",
    [TOKEN_LEQ] = "LEQ",

    [TOKEN_INT_CONST] = "INT_CONST", [TOKEN_BOOL_CONST] = "BOOL_CONST",
    [TOKEN_STR_CONST] = "STR_CONST", [TOKEN_TYPE_ID] = "TYPE_ID",
    [TOKEN_OBJECT_ID] = "OBJECT_ID",

    [TOKEN_ERROR] = "ERROR", [TOKEN_EOF] = "EOF"
};

const char *token_type_name(TokenType type) {
    return token_type_names[type];
}