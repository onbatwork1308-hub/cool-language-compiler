#ifndef TOKENLIST_H
#define TOKENLIST_H

#include "token.h"

/**
 * @brief dyanamically increasing list of tokens.
 */
typedef struct TokenList {
    Token *tokens;
    int count;
    int capacity;
} TokenList;

/**
 * @brief creates and initializes the object of Tokenlist.
 * @returns pointer to the created object.
 */
TokenList *tokenlist_create(void);

/**
 * @brief appends the given token to the end of list/array.
 * @param t1 pointer to the TokenList to which token is to
 * be appended.
 * @param tok token which is to be appended.
 * @returns pointer to the modified(enlarged)/unmodified 
 * TokenList
 */
TokenList *tokenlist_append(TokenList *t1, Token tok);

/**
 * @brief prints the TokenList in human readable form.
 * @param t1 pointer to the TokenList to be printed.
 * @pre t1 is non-null pointer to the TokenList object
 * @post prints the contents of t1 in human readable 
 * form to the stdo buffer.
 */
void tokenlist_print(const TokenList *t1);

/**
 * @brief destructor for the TokenList objects. frees the
 * dynamic/heap memory allocated to the object to avoid m
 * -emory leaks.
 */
void tokenlist_destroy(TokenList *t1);
#endif