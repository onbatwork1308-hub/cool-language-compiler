#ifndef LEXER_HPP
#define LEXER_HPP

#include "utils.h"
#include "token.h"
#include "tokenlist.h"
#include "internTables.h"
#include <stdio.h>



TokenList *lex(const char *source, size_t len,  InternTables *it);
#endif