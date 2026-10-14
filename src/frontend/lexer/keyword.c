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
    
    if (len == 4 && text[0] == 't' && strncasecmp(text + 1, "rue", 3) == 0)
        return TOKEN_TRUE;
    if (len == 5 && text[0] == 'f' && strncasecmp(text + 1, "alse", 4) == 0)
        return TOKEN_FALSE;
        
    for(size_t i = 0; i < NUM_KEYWORDS; i++) {
        if((int)strlen(keywords[i].text) == len &&
            strncasecmp(keywords[i].text, text, len) == 0) {
                return keywords[i].type;
            }
    }

    return TOKEN_ERROR; 
}