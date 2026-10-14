#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "tokenlist.h"
#include "syntax_error.h"

ASTNode *parse(TokenList *t1, SyntaxErrorList *errors);
#endif