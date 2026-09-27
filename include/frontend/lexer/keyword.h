#ifndef KEYWORD_H
#define KEYWORD_H

#include "token.h"


typedef struct {
    const char *text;
    TokenType type;
} KeywordEntry;


/**
 * @brief this functions knows wheater the give string is cool keyword or not.
 * @param text pointer to the string recently read by the lexer.
 * @param len length of the string text.
 * @returns the matching keywords TokenType if 
 */

TokenType keyword_lookup(const char *text, int len);
#endif