#ifndef TOKEN_H
#define TOKEN_H

#include "table.h"
typedef enum {
    /* keywords */
    TOKEN_CLASS, TOKEN_IF, TOKEN_THEN, TOKEN_ELSE, TOKEN_FI, TOKEN_WHILE, 
    TOKEN_LOOP, TOKEN_POOL, TOKEN_FALSE, TOKEN_TRUE, TOKEN_INHERITS, 
    TOKEN_ISVOID, TOKEN_LET, TOKEN_CASE, TOKEN_OF, TOKEN_ESAC, TOKEN_NEW,
    TOKEN_NOT, TOKEN_IN,

    /* operators / punctuation */
    TOKEN_DARROW, TOKEN_AT, TOKEN_NEG, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, 
    TOKEN_DIVIDE, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE, 
    TOKEN_SEMICOLON, TOKEN_COLON, TOKEN_COMMA, TOKEN_DOT, TOKEN_ASSIGN, 
    TOKEN_EQ, TOKEN_LT, TOKEN_LEQ,

    /* literals / identifiers */
    TOKEN_INT_CONST, TOKEN_BOOL_CONST, TOKEN_STR_CONST, TOKEN_TYPE_ID, 
    TOKEN_OBJECT_ID,

    /* special */
    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef enum {
    LEX_ERR_NONE,
    LEX_ERR_ILLEGAL_CHAR,
    LEX_ERR_UNTERMINATED_STRING,
    LEX_ERR_UNTERMINATED_COMMENT,
    LEX_ERR_ILLEGAL_ESCAPE,
    LEX_ERR_STRING_TOO_LONG,
    LEX_ERR_NULL_IN_STRING
} LexErrorKind;


typedef struct Token {
    TokenType type;
    Entry* value;
    int line;
    char* lexeme;   
    LexErrorKind error_kind;
} Token;

/**
 * @brief maps the given token type with appropriate string.
 * used for debugging and diagonstics of the lexer's o/p.
 * @param type TokenType of the token.
 * @returns string mapped with that type.
 */
const char *token_type_name(TokenType type);
#endif