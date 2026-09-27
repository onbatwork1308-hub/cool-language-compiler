#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "token.h"
#include "internTables.h"
#include "lexer.h"
#include "tokenlist.h"

static const char *lex_error_messages[] = {
    [LEX_ERR_ILLEGAL_CHAR]        = "unexpected character",
    [LEX_ERR_UNTERMINATED_STRING] = "unterminated string constant",
    [LEX_ERR_UNTERMINATED_COMMENT]= "unterminated comment",
    [LEX_ERR_ILLEGAL_ESCAPE] = "illegal escape character",
    [LEX_ERR_STRING_TOO_LONG]     = "string constant too long",
    [LEX_ERR_NULL_IN_STRING]      = "null character in string",
};

bool report_lex_error(TokenList *t1) {
    bool had_error = false;
    for(int i = 0; i < t1->count; i++) {
        Token tok = t1->tokens[i];
        if(tok.type == TOKEN_ERROR) {
            had_error = true;
            fprintf(stderr, "lexical error at line %d: %s: \"%s\"\n",
            tok.line, lex_error_messages[tok.error_kind], tok.lexeme);
        }
    }

    return had_error;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "usage: %s <source-file.cl>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    if(!fp) {
        perror("could not open the source file.\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = malloc(size + 1);
    if(!buffer) {
        perror("Unable to allocate buffer.\n");
        exit(1);
    }

    size_t read = fread(buffer, sizeof(char), size, fp);
    if (read != (size_t)size) {
        perror("failed to read source file");
        exit(1);
    }
    buffer[size] = '\0';

    fclose(fp);

    InternTables *it = internTables_create();
    if(!it) {
        perror("Unable to allocate memory for interntables.\n");
        exit(1);
    }
    // printf("DEBUG: size passed to lex() = %ld\n", size);
    TokenList *tokens = lex(buffer, size, it);
    // printf("DEBUG: token count = %d\n", tokens->count);

    bool had_error = report_lex_error(tokens);
    tokenlist_print(tokens);
    free(buffer);

    /*Destruction phase*/
    tokenlist_destroy(tokens);
    internTables_destroy(it);

    return had_error ? 1 : 0;
}