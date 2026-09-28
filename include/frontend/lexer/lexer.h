#ifndef LEXER_HPP
#define LEXER_HPP

#include "utils.h"
#include "token.h"
#include "tokenlist.h"
#include "internTables.h"
#include <stdio.h>


/**
 * @brief implements the actual lexing analysis phase.
 * @details simulates the automata to recognise the r
 * -egular language (cool). scannes the entire input
 * buffer and generates token stream for valid tokens
 * and detects lexical if any.
 * 
 * @param source pointer to the input buffer which is to be read.
 * @param len length of source/input buffer.
 * @param it pointer to the InternTables in which the entries for
 * the keywords, const strings and const integers will be stored 
 * to allow efficient access of info the later phases of compiler.
 * 
 * @returns TokenList* pointer to the array storing Token generate
 * -d by the lexer.
 */
TokenList *lex(const char *source, size_t len,  InternTables *it);
#endif