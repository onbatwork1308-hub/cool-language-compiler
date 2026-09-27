#include "keyword.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>

static const KeywordEntry keywords[] = {
    {"class", TOKEN_CLASS},
    {"if", TOKEN_IF},
    {"then", TOKEN_THEN},
    {"else", TOKEN_ELSE},
    {"fi", TOKEN_FI},
    {"while", TOKEN_WHILE},
    {"loop", TOKEN_LOOP},
    {"pool", TOKEN_POOL},
    {"inherits", TOKEN_INHERITS},
    {"isvoid", TOKEN_ISVOID},
    {"let", TOKEN_LET},
    {"case", TOKEN_CASE},
    {"of", TOKEN_OF},
    {"esac", TOKEN_ESAC},
    {"new", TOKEN_NEW},
    {"not", TOKEN_NOT},
    {"in", TOKEN_IN}
};

#define NUM_KEYWORDS sizeof(keywords) / sizeof(keywords[0])

TokenType keyword_lookup(const char *text, int len) {
    
    for(size_t i = 0; i < NUM_KEYWORDS; i++) {
        if((int)strlen(keywords[i].text) == len &&
            strncasecmp(keywords[i].text, text, len) == 0) {
                return keywords[i].type;
            }
    }

    return TOKEN_ERROR; 
}