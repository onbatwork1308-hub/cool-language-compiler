#include "ast.h"
#include "parser.h"
#include "dynarray.h"
#include "syntax_error.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct Parser {
    Token *tokens;           // borrowed from the tokenlist
    int size;               
    int pos;                 // 0 <= pos < size
    int depth;               // current '{' nesting, maintained by advance()
    bool panic;              // true form an error until synchronization
    SyntaxErrorList *errors; // borrowed : owned by main.c            
} Parser;

static ASTNode *parse_expr(Parser *p); 

static void parser_init(Parser *p, TokenList *t1, SyntaxErrorList *errors) {
    /*precondition: lex() always ends the list with TOKEN_EOF, so count >= 1*/
    p->tokens = t1->tokens;
    p->size = t1->count;
    p->pos = 0;
    p->depth = 0;
    p->panic = false;
    p->errors = errors;
}

static Token peek(Parser *p) {
    return p->tokens[p->pos];
}

static Token peek_next(Parser *p) {
    int i = p->pos + 1;
    if(i >= p->size) i = p->size - 1;
    return p->tokens[i];
}

static Token advance(Parser *p) {
    Token t = p->tokens[p->pos];
    if (t.type == TOKEN_EOF) return t;
    p->pos++;
    if(t.type == TOKEN_LBRACE) p->depth++;
    else if (t.type == TOKEN_RBRACE && p->depth > 0) p->depth--;
    return t;
}

static bool check(Parser *p, TokenType type) {
    return peek(p).type == type;
}

static bool at_end(Parser *p) {
    return check(p, TOKEN_EOF);
}

static bool match(Parser *p, TokenType t) {
    if(!check(p, t)) return false;
    advance(p);
    return true;
}

/*============== errors and recovery ================*/

static void error_expected(Parser *p, TokenType expected) {
    if (!p->panic) syntax_error_list_add_expected(p->errors, expected, peek(p));
    p->panic = true;
}

static void error_expected_expr(Parser *p) {
    if(!p->panic) syntax_error_list_add_expected_expr(p->errors, peek(p));
    p->panic = true;
}

static bool expect(Parser *p, TokenType type, Token *out) {
    if(check(p, type)) {
        Token t = advance(p);
        if(out) {
            *out = t;
        }
        return true; 
    }
    error_expected(p, type);
    return false;
}

/**
 * @brief Inside a class body: skip to just past the next ';' at body level, or stop at the class's own '}' (left for parse_class to consume). 
 */
static void synchronize_feature(Parser *p, int body_depth) {
    while(!at_end(p)) {
        if(p->depth == body_depth && check(p, TOKEN_RBRACE)) break;
        Token t = advance(p);
        if(t.type == TOKEN_SEMICOLON && p->depth == body_depth) break;
    }
    p->panic = false;
}

static void synchronize_class(Parser *p) {
    while (!at_end(p) && !check(p, TOKEN_CLASS)) advance(p);
    p->depth = 0;
    p->panic = false;
}

/*================= expressions ===================*/

static void parse_args(Parser *p, ASTNode *call) {
    if(!expect(p, TOKEN_LPAREN, NULL)) return;
    if( !check(p, TOKEN_RPAREN)) {
        do {
            ASTNode *a = parse_expr(p);
            if (a) ast_add_arg(call, a);
        } while (!p->panic && match(p, TOKEN_COMMA));
    }
    expect(p, TOKEN_RPAREN, NULL);
}

static ASTNode *parse_let(Parser *p) {
    typedef struct { Entry *name, *type; ASTNode *init; int line; } Binding;
    
    advance(p);
    DynArray *bindings = dynarray_create(sizeof(Binding));

    do {
        Token id, ty;
        Binding b = {0};
        b.line = peek(p).line;

        if(!expect(p, TOKEN_OBJECT_ID, &id)) break;
        if(!expect(p, TOKEN_COLON, NULL)) break;
        if(!expect(p, TOKEN_TYPE_ID, &ty)) break;

        b.name = id.value;
        b.type = ty.value;
        b.init = match(p, TOKEN_ASSIGN) ? parse_expr(p) : ast_make_no_expr(b.line);
        dynarray_push(bindings, &b);
    } while (!p->panic && match(p, TOKEN_COMMA));

    ASTNode *body = NULL;
    if (expect(p, TOKEN_IN, NULL)) body = parse_expr(p);

    /* `let a, b in e`  ==>  let a in (let b in e), built innermost-first */
    for (int i = bindings->count - 1; i >= 0; i--) {
        Binding *b = (Binding *) dynarray_get(bindings, i);
        body = ast_make_let(b->name, b->type, b->init, body, b->line);
    }
    dynarray_destroy(bindings);
    return body;
}

static ASTNode *parse_case(Parser *p) {
    int line = advance(p).line; /* CASE */
    ASTNode *scrutinee = parse_expr(p);
    expect(p, TOKEN_OF, NULL);
    ASTNode *c = ast_make_case(scrutinee, line);

    do {
        Token id, ty;
        int bl =  peek(p).line;
        if (!expect(p, TOKEN_OBJECT_ID, &id)) break;
        if (!expect(p, TOKEN_COLON, NULL)) break;
        if (!expect(p, TOKEN_TYPE_ID, &ty)) break;
        if (!expect(p, TOKEN_DARROW, NULL)) break;
        
        ASTNode *body = parse_expr(p);
        expect(p, TOKEN_SEMICOLON, NULL);
        ast_add_branch(c, ast_make_case_branch(id.value, ty.value, body, bl));
    } while(!p->panic && !check(p, TOKEN_ESAC));

    expect(p, TOKEN_ESAC, NULL);
    return c;
}

static ASTNode *parse_primary(Parser *p) {
    Token t = peek(p);

    switch (t.type) {
        case TOKEN_INT_CONST:
            advance(p);
            return ast_make_int_const(t.value, t.line);

        case TOKEN_STR_CONST:
            advance(p);
            return ast_make_str_const(t.value, t.line);

        case TOKEN_TRUE:
            advance(p);
            return ast_make_bool_const(true, t.line);

        case TOKEN_FALSE:
            advance(p);
            return ast_make_bool_const(false, t.line);

        case TOKEN_OBJECT_ID: {
            advance(p);
            if(check(p, TOKEN_LPAREN)) {
                ASTNode *d = ast_make_dispatch(NULL, t.value, t.line);
                parse_args(p, d);
                return d;
            }

            return ast_make_object_id(t.value, t.line);
        }

        case TOKEN_LPAREN: {
            advance(p);
            ASTNode *e = parse_expr(p);
            expect(p, TOKEN_RPAREN, NULL);
            return e;
        } 

        case TOKEN_LBRACE: {
            advance(p);
            ASTNode *blk = ast_make_block(t.line);
            do {
                ASTNode *e = parse_expr(p);
                if (e) ast_add_block_expr(blk, e);
                expect(p, TOKEN_SEMICOLON, NULL); 
            } while (!p->panic && !check(p, TOKEN_RBRACE));
            expect(p, TOKEN_RBRACE, NULL);
            return blk;
        }

        case TOKEN_IF: {
            advance(p);
            ASTNode *cond = parse_expr(p); expect (p, TOKEN_THEN, NULL);
            ASTNode *then_b = parse_expr(p); expect(p, TOKEN_ELSE, NULL);
            ASTNode *else_b = parse_expr(p); expect(p, TOKEN_FI, NULL);
            return ast_make_if(cond, then_b, else_b, t.line);
        }

        case TOKEN_WHILE: {
            advance(p);
            ASTNode *cond = parse_expr(p); expect (p, TOKEN_LOOP, NULL);
            ASTNode *body = parse_expr(p); expect (p, TOKEN_POOL, NULL);
            return ast_make_while(cond, body, t.line);
        }

        case TOKEN_LET: return parse_let(p);
        case TOKEN_CASE: return parse_case(p);

        case TOKEN_NEW: {
            Token ty;
            advance(p);
            if(!expect(p, TOKEN_TYPE_ID, &ty)) return NULL;
            return ast_make_new(ty.value, t.line);
        }

        default:
            error_expected_expr(p);
            return NULL;
    }
}

static ASTNode *parse_dispatch_expr(Parser *p) {
    ASTNode *e = parse_primary(p);
    
    while (e && !p->panic && (check(p, TOKEN_DOT) || check(p, TOKEN_AT))) {
        int line = peek(p).line;
        Token ty, m;
        
        if (match(p, TOKEN_DOT)) {
            if(!expect(p, TOKEN_OBJECT_ID, &m)) break;
            ASTNode *d = ast_make_dispatch(e, m.value, line);
            parse_args(p, d);
            e = d;
        } else {
            advance(p); // skip '@'
            if(!expect(p, TOKEN_TYPE_ID, &ty)) break;
            if(!expect(p, TOKEN_DOT, NULL)) break;
            if(!expect(p, TOKEN_OBJECT_ID, &m)) break;
            ASTNode *d = ast_make_static_dipatch(e, ty.value, m.value, line);
            parse_args(p, d);
            e = d;
        }
    }
    return e;
}

static ASTNode *parse_neg_expr(Parser *p) {
    if(check(p, TOKEN_NEG)) {
        int line = advance(p).line;
        return ast_make_unop(TOKEN_NEG, parse_neg_expr(p), line);
    }
    return parse_dispatch_expr(p);
}

static ASTNode *parse_isvoid_expr(Parser *p) {
    if(check(p, TOKEN_ISVOID)) {
        int line = advance(p).line;
        return ast_make_isvoid(parse_isvoid_expr(p), line);
    }
    return parse_neg_expr(p);
}

static ASTNode *parse_mul_expr(Parser *p) {
    ASTNode *left = parse_isvoid_expr(p);
    while (check(p, TOKEN_MUL) || check(p, TOKEN_DIVIDE)) {
        Token op = advance(p);
        ASTNode *right = parse_isvoid_expr(p);
        left = ast_make_binop(op.type, left, right, op.line);
    }
    return left;
}

static ASTNode *parse_add_expr(Parser *p) {
    ASTNode *left = parse_mul_expr(p);
    while(check(p, TOKEN_PLUS) || check(p, TOKEN_MINUS)) {
        Token op = advance(p);
        ASTNode *right = parse_mul_expr(p);
        left = ast_make_binop(op.type, left, right, op.line);
    }
    return left;
}

static ASTNode *parse_compare_expr(Parser *p) {
    ASTNode *left = parse_add_expr(p);
    if (check(p, TOKEN_LT) || check(p, TOKEN_LEQ) || check(p, TOKEN_EQ)) {
        Token op = advance(p);
        ASTNode *right = parse_add_expr(p);
        left = ast_make_binop(op.type, left, right, op.line);
    }
    return left;
}

static ASTNode *parse_not_expr(Parser *p) {
    if (check(p, TOKEN_NOT)) {
        int line = advance(p).line;
        return ast_make_unop(TOKEN_NOT, parse_not_expr(p), line);
    }
    return parse_compare_expr(p);
}

static ASTNode *parse_assign_expr(Parser *p) {
    if(check(p, TOKEN_OBJECT_ID) && peek_next(p).type == TOKEN_ASSIGN) {
        Token id = advance(p);
        advance(p);
        ASTNode *value = parse_assign_expr(p);
        return ast_make_assign(id.value, value, id.line);
    }
    return parse_not_expr(p);
}

static ASTNode *parse_expr(Parser *p) {
    return parse_assign_expr(p);
}

/*==================== features, classes and program ====================*/

/**
 * @pre the current Token is '('
 */
static ASTNode *parse_method_rest(Parser *p, Token name) {

    advance(p); // skip '('
    ASTNode *m = ast_make_method(name.value, NULL, NULL, name.line);

    if(!check(p, TOKEN_RPAREN)) {
        do {
            Token id, ty;
            if(!expect(p, TOKEN_OBJECT_ID, &id)) return NULL;
            if(!expect(p, TOKEN_COLON, NULL)) return NULL;
            if(!expect(p, TOKEN_TYPE_ID, &ty)) return NULL;
            ast_add_formal(m, ast_make_formal(id.value, ty.value, id.line));
        } while (!p->panic && match(p, TOKEN_COMMA));
    }

    expect(p, TOKEN_RPAREN, NULL);

    Token ret;
    if(expect(p, TOKEN_COLON, NULL) && expect(p, TOKEN_TYPE_ID, &ret)) {
        m->as.method.ret_type = ret.value;
    }

    if(expect(p, TOKEN_LBRACE, NULL)) {
        m->as.method.body = parse_expr(p);
        expect(p, TOKEN_RBRACE, NULL);
    }

    expect(p, TOKEN_SEMICOLON, NULL);
    return m;
}

static ASTNode *parse_features(Parser *p) {
    Token name, ty;
    if(!expect(p, TOKEN_OBJECT_ID, &name)) return NULL;

    if(check(p, TOKEN_LPAREN)) return parse_method_rest(p, name);

    if(!expect(p, TOKEN_COLON, NULL)) return NULL;
    if(!expect(p, TOKEN_TYPE_ID, &ty)) return NULL;

    ASTNode *init = match(p, TOKEN_ASSIGN) ? parse_expr(p) : ast_make_no_expr(name.line);
    expect(p, TOKEN_SEMICOLON, NULL);
    return ast_make_attr(name.value, ty.value, init, name.line);
}

static ASTNode *parse_class(Parser *p) {
    if(!expect(p, TOKEN_CLASS, NULL)) return NULL;

    Token name;
    Entry *parent = NULL;
    if(!expect(p, TOKEN_TYPE_ID, &name)) return NULL;
    if(match(p, TOKEN_INHERITS)) {
        Token parent_tok;
        if(!expect(p, TOKEN_TYPE_ID, &parent_tok)) return NULL;
        parent = parent_tok.value;
    }

    if(!expect(p, TOKEN_LBRACE, NULL)) return NULL;

    ASTNode *cls = ast_make_class(name.value, parent, name.line);
    int body_depth = p->depth;

    while (!at_end(p) && !check(p, TOKEN_RBRACE)) {
        ASTNode *f = parse_features(p);

        if(f) ast_add_feature(cls, f);
        if(p->panic) synchronize_feature(p, body_depth);
    }

    expect(p, TOKEN_RBRACE, NULL);
    expect(p, TOKEN_SEMICOLON, NULL);
    return cls;
}

static ASTNode *parse_program(Parser *p) {
    ASTNode *prog = ast_make_program(peek(p).line);
    
    if(at_end(p)) error_expected(p, TOKEN_CLASS);

    while(!at_end(p)) {
        int before = p->pos;
        ASTNode *cls = parse_class(p);
        if(cls) ast_add_class(prog, cls);
        if(p->panic) synchronize_class(p);
        if(p->pos == before) advance(p);
    }

    return prog;
}

ASTNode *parse(TokenList *tokens, SyntaxErrorList *errors) {
    Parser p;
    parser_init(&p, tokens, errors);
    return parse_program(&p);
}