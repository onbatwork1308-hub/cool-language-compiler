#include "token.h" 
#include "lexer.h"
#include "keyword.h"
#include "tokenlist.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define MAX_STR_CONST 1024

/**
 * @struct Scanner
 * @details a abstract interface used by lexer to scan the
 * input buffer. It provides various functionality to lexer
 * like lookahead, read maximal munch, scan strings and ski
 * -p comments, etc.
 */
typedef struct Scanner {
    const char *buffer;
    size_t size;
    size_t pos;
    size_t line;
} Scanner;

/**
 * @brief helper function used to make tokens.
 * @param type TokenType of the token to be made.
 * @param value Entry* if token is keyword or id
 * -entifier, then it the pointer to it's entry i
 * -n interntables, otherwise NULL
 * @param line line no. in the source file from w
 * -here this token was built
 * @returns Token
 */
static Token make_token(TokenType type, Entry* value, int line, char *lexeme) {
    Token tok = {.type = type, .value = value, .line = line, .lexeme = lexeme, .error_kind = LEX_ERR_NONE };
    return tok;
}

/**
 * @brief helper function to make error tokens
 * @param line line no. form source file where this token 
 * was built.
 * @param lexeme source code's copy which caused the error, 
 * used for later debuging.
 * @param error_kind specifies the exact type of error which 
 * has been detected by the 
 * scanner/lexer.
 * @returns Token with default TokenType : TOKEN_ERROR
 */
static Token make_error_token(int line, char *lexeme, LexErrorKind error_kind) {
    Token tok = {.type = TOKEN_ERROR, .value = NULL, .line = line, .lexeme = lexeme, .error_kind = error_kind };
    return tok;
}

/**
 * @struct StrBuf
 * @details string buffer which grows as the input start 
 * becoming large.
 */
typedef struct { char* buf; int len; int cap; } StrBuf;

/**
 * @brief initializes object of StrBuf
 * @param b StrBuf* pointer to the object of StrBuf type, 
 * which is to be initialized.
 */
static void strbuf_init(StrBuf *b) {
    b->cap = 32; b->len = 0;
    b->buf = (char*) malloc(b->cap);
    b->buf[0] = '\0';
}

/**
 * @brief append the character to the StrBuf.
 * @param b pointer to StrBuf in which the chara
 * -cter is to be appended.
 * @param c character which is to be appended.
 */
static void strbuf_push(StrBuf *b, char c) {
    if(b->len >= b->cap) {
        b->cap = b->cap * 2;
        b->buf = (char*) realloc(b->buf, sizeof(char) * b->cap);
    }

    b->buf[b->len++] = c;
    b->buf[b->len] = '\0';
}

/**
 * @brief destroyer of StrBuf objects in case of errors
 * @param b pointer to the StrBuf which is to be destro
 * -yed.
 * @param line line no. of source file at which this err
 * -or was detected by the scanner.
 * @param kind error kind of the detected error.
 * @returns Token with default TokenType TOKEN_ERROR.
 */
static Token strbuf_error(StrBuf *b, int line, LexErrorKind kind) {
    Token err = make_error_token(line, strdup(b->buf), kind);
    free(b->buf);
    return err;
}

/**
 * @brief helper function of scanner
 * @param s pointer to the Scanner object.
 * @returns character currently pointed by the scanner
 * pointer pos if file is not ended, otherwise '\0'
 */
static char peek(Scanner *s) {
    return (s->pos < s->size) ? s->buffer[s->pos] : '\0';
}

/**
 * @brief helper function of scanner.
 * @param s pointer to the Scanner object.
 * @returns character just next to the currently pointed
 * character by the scanner pointer pos if file is not en
 * -ded, otherwise '\0'.
 */
static char peek_next(Scanner *s) {
    return ((s->pos + 1) < s->size) ? s->buffer[s->pos + 1] : '\0';
}

/**
 * @brief helper function of scanner
 * @param s pointer to the Scanner object
 * @returns character currently pointed by the scanner pointer.
 * @post moves scanner pointer by 1 position ahead.
 */
static char advance(Scanner *s) {
    char c = s->buffer[s->pos++];
    if(c == '\n') s->line++;

    return c;
}

/**
 * @brief helper function of scanner.
 * @param s pointer to the Scanner object.
 * @returns true if scanner pointer has reached the end of buffer,
 * otherwise false.
 */
static bool at_end(Scanner *s) {
    return s->pos >= s->size;
}


static Token scan_two_char_op(Scanner *s) {
    char c = peek(s);
    if(c == '<') {
        if(peek_next(s) == '-') {
            s->pos += 2;
            return make_token(TOKEN_ASSIGN, NULL, s->line, NULL);
        }
        if(peek_next(s) == '=') {
            s->pos += 2;
            return make_token(TOKEN_LEQ, NULL, s->line, NULL);
        }
        
        advance(s);
        return make_token(TOKEN_LT, NULL, s->line, NULL);
    }

    if(c == '=') {
        if(peek_next(s) == '>') {
            s->pos += 2;
            return make_token(TOKEN_DARROW, NULL, s->line, NULL);
        }

        advance(s);
        return make_token(TOKEN_EQ, NULL, s->line, NULL);
    }

    advance(s);
    char *l = (char*) malloc(sizeof(char) * 2);
    *l = c;
    l[1] = '\0';
    return make_error_token(s->line, l, LEX_ERR_ILLEGAL_CHAR);
}

static Token scan_string(Scanner *s, InternTables *it) {
    advance(s);

    char c;
    StrBuf b;
    strbuf_init(&b);

    while(((c = peek(s)) != '"')) {
        if (at_end(s)) {
            return strbuf_error(&b, s->line, LEX_ERR_UNTERMINATED_STRING);
        }

        if (b.len >= MAX_STR_CONST) {
            return strbuf_error(&b, s->line, LEX_ERR_STRING_TOO_LONG);
        }

        if (c == '\n') {
            return strbuf_error(&b, s->line, LEX_ERR_UNTERMINATED_STRING);
        }

        if (c == '\\') {
            if (peek_next(s) == 'n') {
                advance(s); advance(s); 
                strbuf_push(&b, '\n');
                continue;
            } else if (peek_next(s) == 't') {
                advance(s); advance(s);
                strbuf_push(&b, '\t');
                continue;
            } else if(peek_next(s) == 'b') {
                advance(s); advance(s);
                strbuf_push(&b, '\b');
                continue;
            } else if(peek_next(s) == 'f') {
                advance(s); advance(s);
                strbuf_push(&b, '\f');
                continue;
            } else if(peek_next(s) == '\\') {
                advance(s); advance(s);
                strbuf_push(&b, '\\');
                continue;
            } else if(peek_next(s) == '"') {
                advance(s); advance(s);
                strbuf_push(&b, '"');
                continue;
            } else {
                return strbuf_error(&b, s->line, LEX_ERR_ILLEGAL_ESCAPE);
            }
        }

        advance(s);
        strbuf_push(&b, c);
    }
    
    advance(s);
    StringEntry *str_entry = internTables_add_string(it, b.buf, b.len);
    free(b.buf);
    return make_token(TOKEN_STR_CONST, (Entry*)str_entry, s->line, NULL);
}

static Token scan_number(Scanner *s, InternTables *it) {
    char c;
    StrBuf b;
    strbuf_init(&b);

    while(!at_end(s) && isDigit(c = peek(s))) {
        strbuf_push(&b, c);
        advance(s);
    }

    IntEntry* ie = internTables_add_int(it, b.buf, b.len);
    free(b.buf);
    return make_token(TOKEN_INT_CONST, (Entry*) ie, s->line, NULL);
}

static Token scan_identifier(Scanner *s, InternTables *it) {
    char c;
    StrBuf b;
    strbuf_init(&b);
    while(!at_end(s) && (isAlphaNum(c = peek(s)) || c == '_')) {
        strbuf_push(&b, c);
        advance(s);
    }

    Entry *ide = internTables_add_id(it, b.buf, b.len);
    TokenType type =  keyword_lookup(b.buf, b.len); 
    
    if(type != TOKEN_ERROR) {
        free(b.buf);
        return make_token(type, ide, s->line, NULL);
    }

    if(isUpper(b.buf[0])) {
        free(b.buf);
        return make_token(TOKEN_TYPE_ID, ide, s->line, NULL);
    }

    free(b.buf);
    return make_token(TOKEN_OBJECT_ID, ide, s->line, NULL);
}

static bool skip_block_comment(Scanner *s, Token *err_token) {
    int depth = 0;
    int start_line = s->line;  

    do {
        if (at_end(s)) {
            *err_token = make_error_token(start_line, NULL, LEX_ERR_UNTERMINATED_COMMENT);
            return false;
        }

        char c = peek(s);
        if (c == '(' && peek_next(s) == '*') {
            advance(s); advance(s);
            depth++;
        } else if (c == '*' && peek_next(s) == ')') {
            advance(s); advance(s);
            depth--;
        } else {
            advance(s);   /* consume one char of comment body, tracks line on '\n' too */
        }
    } while(depth > 0);
    
    return true;
}

static void skip_line_comment(Scanner *s) {
    while(!at_end(s) && peek(s) != '\n') {
        advance(s); 
    }
}

TokenList *lex(const char *buffer, size_t size, InternTables *it) {

    Scanner s = {.buffer = buffer, .pos = 0, .size = size, .line = 1};
    TokenList *t1 = tokenlist_create();

    while(!at_end(&s)) {
        char c = peek(&s);
        // printf("DEBUG: loop iter, pos=%zu char='%c'\n", s.pos, c);

        if(c == ' ' || c == '\t' || c == '\r' || c == '\n') { 
            advance(&s); 
            continue; 
        }

        if(c == '(' && peek_next(&s) == '*') {
            Token err_token;
            if(!skip_block_comment(&s, &err_token)) {
                t1 = tokenlist_append(t1, err_token);
            }
            continue;
        }

        if(c == '-' && peek_next(&s) == '-') {
            skip_line_comment(&s);
            continue;
        }

        if(c == '(') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_LPAREN, NULL, s.line, NULL));
            continue;
        }

        if(c == ')') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_RPAREN, NULL, s.line, NULL));
            continue;
        }

        if(c == '{') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_LBRACE, NULL, s.line, NULL));
            continue;
        }

        if(c == '}') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_RBRACE, NULL, s.line, NULL));
            continue;
        }

        if(c == '+') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_PLUS, NULL, s.line, NULL));
            continue;
        }

        if(c == '-') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_MINUS, NULL, s.line, NULL));
            continue;
        }

        if(c == '*') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_MUL, NULL, s.line, NULL));
            continue;
        }

        if(c == '/') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_DIVIDE, NULL, s.line, NULL));
            continue;
        }

        if(c == '~') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_NEG, NULL, s.line, NULL));
            continue;
        }

        if(c == ';') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_SEMICOLON, NULL, s.line, NULL));
            continue;
        }

        if(c == ':') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_COLON, NULL, s.line, NULL));
            continue;
        }

        if(c == ',') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_COMMA, NULL, s.line, NULL));
            continue;
        }

        if(c == '.') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_DOT, NULL, s.line, NULL));
            continue;
        }

        if(c == '@') {
            advance(&s);
            t1 = tokenlist_append(t1, make_token(TOKEN_AT, NULL, s.line, NULL));
            continue;
        }

        if(c == '<' || c == '=') {
            t1 = tokenlist_append(t1, scan_two_char_op(&s));
            continue;
        }

        if(c == '"') {
            t1 = tokenlist_append(t1, scan_string(&s, it));
            continue;
        }


        if(isDigit(c)) {
            t1 = tokenlist_append(t1, scan_number(&s, it));
            continue;
        }

        if(isAlpha(c)) {
            t1 = tokenlist_append(t1, scan_identifier(&s, it));
            continue;
        }

        /*Error "unexpected character"*/
        advance(&s);
        char *l = (char*) malloc(sizeof(char) * 2);
        *l = c;
        l[1] = '\0';
        t1 = tokenlist_append(t1, make_error_token(s.line, l, LEX_ERR_ILLEGAL_CHAR));

    }

    t1 = tokenlist_append(t1, make_token(TOKEN_EOF, NULL, s.line, NULL));

    return t1;
}