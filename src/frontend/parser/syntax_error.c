#include "syntax_error.h"
#include <stdlib.h>

SyntaxErrorList *syntax_error_list_create(void) {
    SyntaxErrorList *l = malloc(sizeof(SyntaxErrorList));
    if (!l) {
        perror("Unable to allocate memory for SyntaxErrorList");
        exit(1);
    }
    l->errors = dynarray_create(sizeof(SyntaxError));
    return l;
}

void syntax_error_list_destroy(SyntaxErrorList *l) {
    if (!l) {
        perror("Reference to \"NULL\" passed to syntax_error_list_destroy");
        exit(1);
    }
    dynarray_destroy(l->errors);   /* nothing else owned: Entry* fields are borrowed */
    free(l);
}

static void add(SyntaxErrorList *l, SyntaxErrorKind kind, TokenType expected, Token found) {
    SyntaxError e = {
        .kind = kind,
        .line = found.line,
        .expected = expected,
        .found = found.type,
        .found_value = found.value
    };
    dynarray_push(l->errors, &e);
}

void syntax_error_list_add_expected(SyntaxErrorList *l, TokenType expected, Token found) {
    add(l, SYNTAX_ERR_EXPECTED_TOKEN, expected, found);
}

void syntax_error_list_add_expected_expr(SyntaxErrorList *l, Token found) {
    add(l, SYNTAX_ERR_EXPECTED_EXPR, TOKEN_EOF /* unused */, found);
}

int syntax_error_list_count(const SyntaxErrorList *l) {
    return l->errors->count;
}

static void print_found(FILE *out, const SyntaxError *e) {
    if (e->found == TOKEN_EOF) {
        fprintf(out, "end of file");
        return;
    }
    fprintf(out, "%s", token_type_name(e->found));
    if (e->found_value)
        fprintf(out, " '%s'", entry_str(e->found_value));
}

bool syntax_error_list_report(const SyntaxErrorList *l, FILE *out) {
    for (int i = 0; i < l->errors->count; i++) {
        const SyntaxError *e = (SyntaxError*) dynarray_get(l->errors, i);

        fprintf(out, "syntax error at line %d: ", e->line);
        switch (e->kind) {
            case SYNTAX_ERR_EXPECTED_TOKEN:
                fprintf(out, "expected %s but found ", token_type_name(e->expected));
                break;
            case SYNTAX_ERR_EXPECTED_EXPR:
                fprintf(out, "expected expression but found ");
                break;
        }
        print_found(out, e);
        fprintf(out, "\n");
    }
    return l->errors->count > 0;
}