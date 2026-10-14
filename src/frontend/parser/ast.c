#include "ast.h"
#include "dynarray.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Internal allocator for ast node
 */
static ASTNode  *ast_alloc(ASTNodeType type, int line) {
    ASTNode *n = malloc(sizeof(ASTNode));
    if(!n) {
        perror("Unable to allocate memory to the ASTNode.\n");
        exit(1);
    }
    n->type = type;
    n->line = line;
    return n;
}

ASTNode *ast_make_program(int line) {
    ASTNode *n = ast_alloc(NODE_PROGRAM, line);
    n->as.program.classes = dynarray_create(sizeof(ASTNode*));
    return n;
}

ASTNode *ast_make_class(Entry *name, Entry *parent, int line) {
    ASTNode *n = ast_alloc(NODE_CLASS, line);
    n->as.class_.name = name;
    n->as.class_.parent = parent;
    n->as.class_.features = dynarray_create(sizeof(ASTNode*));
    return n;
}

ASTNode *ast_make_method(Entry *name, Entry *ret_type, ASTNode *body, int line) {
    ASTNode *n = ast_alloc(NODE_METHOD, line);
    n->as.method.name = name;
    n->as.method.ret_type = ret_type;
    n->as.method.body = body;
    n->as.method.formals = dynarray_create(sizeof(ASTNode*));
    return n;
}

ASTNode *ast_make_attr(Entry *name, Entry *decl_type, ASTNode *init, int line) {
    ASTNode *n = ast_alloc(NODE_ATTR, line);
    n->as.attr.name = name;
    n->as.attr.decl_type = decl_type;
    n->as.attr.init = init;
    return n;
}

ASTNode *ast_make_formal(Entry *name, Entry *decl_type, int line) {
    ASTNode *n = ast_alloc(NODE_FORMAL, line);
    n->as.formal.name = name;
    n->as.formal.decl_type = decl_type;
    return n;
}

ASTNode *ast_make_assign(Entry *name, ASTNode *value, int line) {
    ASTNode *n = ast_alloc(NODE_ASSIGN, line);
    n->as.assign.name = name;
    n->as.assign.values = value;
    return n;
}

ASTNode *ast_make_dispatch(ASTNode *recv, Entry *method, int line) {
    ASTNode *n = ast_alloc(NODE_DISPATCH, line);
    n->as.dispatch.recv = recv;
    n->as.dispatch.method = method;
    n->as.dispatch.args = dynarray_create(sizeof(ASTNode *));
    return n;
}

ASTNode *ast_make_static_dipatch(ASTNode *recv, Entry *cast_type, Entry *method, int line) {
    ASTNode *n = ast_alloc(NODE_STATIC_DISPATCH, line);
    n->as.static_dispatch.recv = recv;
    n->as.static_dispatch.cast_type = cast_type;
    n->as.static_dispatch.method = method;
    n->as.static_dispatch.args = dynarray_create(sizeof(ASTNode*));
    return n;
}

ASTNode *ast_make_if(ASTNode *cond, ASTNode *then_b, ASTNode *else_b, int line) {
    ASTNode *n = ast_alloc(NODE_IF, line);
    n->as.if_.cond = cond;
    n->as.if_.then_branch = then_b;
    n->as.if_.else_branch = else_b;
    return n;
}

ASTNode *ast_make_while(ASTNode *cond, ASTNode *body, int line) {
    ASTNode *n = ast_alloc(NODE_WHILE, line);
    n->as.while_.cond = cond;
    n->as.while_.body = body;
    return n;
}

ASTNode *ast_make_block(int line) {
    ASTNode *n = ast_alloc(NODE_BLOCK, line);
    n->as.block.exprs = dynarray_create(sizeof(ASTNode*));
    return n;
}

ASTNode *ast_make_let(Entry *name, Entry *decl_type, ASTNode *init, ASTNode *body, int line) {
    ASTNode *n = ast_alloc(NODE_LET, line);
    n->as.let.name = name;
    n->as.let.decl_type = decl_type;
    n->as.let.init = init;
    n->as.let.body = body; /* comma-separated let bindings are desugared by the
                                   parser into a chain of nested NODE_LET nodes here */
    return n;
}

ASTNode *ast_make_case(ASTNode *expr, int line) {
    ASTNode *n = ast_alloc(NODE_CASE, line);
    n->as.case_.expr = expr;
    n->as.case_.branches = dynarray_create(sizeof(ASTNode*));
    return n;
}

ASTNode *ast_make_case_branch(Entry *name, Entry *decl_type, ASTNode *body, int line) {
    ASTNode *n = ast_alloc(NODE_CASE_BRANCH, line);
    n->as.case_branch.name = name;
    n->as.case_branch.decl_type = decl_type;
    n->as.case_branch.body = body;
    return n;
}

ASTNode *ast_make_new(Entry *type_name, int line) {
    ASTNode *n = ast_alloc(NODE_NEW, line);
    n->as.new_.type_name = type_name;
    return n;
}

ASTNode *ast_make_isvoid(ASTNode *expr, int line) {
    ASTNode *n = ast_alloc(NODE_ISVOID, line);
    n->as.isvoid.expr = expr;
    return n;
}

ASTNode *ast_make_binop(TokenType op, ASTNode* left, ASTNode *right, int line) {
    ASTNode *n = ast_alloc(NODE_BINOP, line);
    n->as.binop.op = op;
    n->as.binop.left = left;
    n->as.binop.right = right;
    return n;
}

ASTNode *ast_make_unop(TokenType op, ASTNode *operand, int line) {
    ASTNode *n = ast_alloc(NODE_UNOP, line);
    n->as.unop.op = op;
    n->as.unop.operand = operand;
    return n;
}

ASTNode *ast_make_int_const(Entry *value, int line) {
    ASTNode *n = ast_alloc(NODE_INT_CONST, line);
    n->as.int_const.value = value;
    return n;
}

ASTNode *ast_make_str_const(Entry *value, int line) {
    ASTNode *n = ast_alloc(NODE_STR_CONST, line);
    n->as.string_const.value = value;
    return n;
}

ASTNode *ast_make_bool_const(bool value, int line) {
    ASTNode *n = ast_alloc(NODE_BOOL_CONST, line);
    n->as.bool_const.value = value;
    return n;
}

ASTNode *ast_make_object_id(Entry *name, int line) {
    ASTNode *n = ast_alloc(NODE_OBJECT_ID, line);
    n->as.object_id.name = name;
    return n;
}

ASTNode *ast_make_no_expr(int line) {
    return ast_alloc(NODE_NO_EXPR, line);
}

void ast_add_class(ASTNode *program, ASTNode *class_node) {
    dynarray_push(program->as.program.classes, &class_node);
}

void ast_add_feature(ASTNode *class_node, ASTNode *feature) {
    dynarray_push(class_node->as.class_.features, &feature);
}

void ast_add_formal(ASTNode *method, ASTNode *formal) {
    dynarray_push(method->as.method.formals, &formal);
}

void ast_add_arg(ASTNode *dispatch_node, ASTNode *arg) {
    if(dispatch_node->type == NODE_DISPATCH) {
        dynarray_push(dispatch_node->as.dispatch.args, &arg);
    } else if (dispatch_node->type == NODE_STATIC_DISPATCH) {
        dynarray_push(dispatch_node->as.static_dispatch.args, &arg);
    } else {
        perror("ast_add_arg: node is not a dispatch node.\n");
        exit(1);
    }
}

void ast_add_block_expr(ASTNode *block, ASTNode *expr) {
    dynarray_push(block->as.block.exprs, &expr);
}

void ast_add_branch(ASTNode *case_node, ASTNode *branch) {
    dynarray_push(case_node->as.case_.branches, &branch);
}

void ast_destroy(ASTNode *node) {
    if(!node) return;
    
    switch (node->type) {
        case NODE_PROGRAM: 
            for(int i = 0; i < node->as.program.classes->count; i++) {
                ast_destroy(*(ASTNode **)dynarray_get(node->as.program.classes, i));
            }    
            dynarray_destroy(node->as.program.classes);
            break;
        case NODE_CLASS:
            for(int i = 0; i < node->as.class_.features->count; i++) {
                ast_destroy(*(ASTNode**)dynarray_get(node->as.class_.features, i));
            }
            dynarray_destroy(node->as.class_.features);
            break;
        case NODE_ATTR:
            ast_destroy(node->as.attr.init);
            break;
        case NODE_METHOD:
            for(int i = 0; i < node->as.method.formals->count; i++) {
                ast_destroy(*(ASTNode**)dynarray_get(node->as.method.formals, i));
            }
            dynarray_destroy(node->as.method.formals);
            break;
        case NODE_DISPATCH:
            ast_destroy(node->as.dispatch.recv);
            for(int i = 0; i < node->as.method.formals->count; i++) {
                ast_destroy(*(ASTNode**) dynarray_get(node->as.dispatch.args, i));
            }
            dynarray_destroy(node->as.dispatch.args);
            break;
        case NODE_STATIC_DISPATCH:
            ast_destroy(node->as.static_dispatch.recv);
            for(int i = 0; i < node->as.static_dispatch.args->count; i++) {
                ast_destroy(*(ASTNode**) dynarray_get(node->as.static_dispatch.args, i));
            }
            dynarray_destroy(node->as.static_dispatch.args);
            break;
        case NODE_IF:
            ast_destroy(node->as.if_.cond);
            ast_destroy(node->as.if_.then_branch);
            ast_destroy(node->as.if_.else_branch);
            break;
        case NODE_WHILE:
            ast_destroy(node->as.while_.cond);
            ast_destroy(node->as.while_.body);
            break;
        case NODE_CASE:
            ast_destroy(node->as.case_.expr);
            for(int i = 0; i < node->as.case_.branches->count; i++) {
                ast_destroy(*(ASTNode**) dynarray_get(node->as.case_.branches, i));
            }
            dynarray_destroy(node->as.case_.branches);
            break;
        case NODE_CASE_BRANCH:
            ast_destroy(node->as.case_branch.body);
            break;
        case NODE_ISVOID:
            ast_destroy(node->as.isvoid.expr);
            break;
        case NODE_BINOP:
            ast_destroy(node->as.binop.left);
            ast_destroy(node->as.binop.right);
            break;
        case NODE_UNOP:
            ast_destroy(node->as.unop.operand);
            break;
        default:
            break;
    }

    free(node);
}   

// static void print_indent(FILE *out, int depth) {
//     for (int i = 0; i < depth; i++) fprintf(out, " ");
// }

static const char *op_symbol(TokenType t) {
    switch (t) {
        case TOKEN_PLUS: return "+";   case TOKEN_MINUS: return "-";
        case TOKEN_MUL: return "*";    case TOKEN_DIVIDE: return "/";
        case TOKEN_LT: return "<";     case TOKEN_LEQ: return "<=";
        case TOKEN_EQ: return "=";     case TOKEN_NEG: return "~";
        case TOKEN_NOT: return "not";  default: return "?";
    }
}

static const char *name_of(Entry *e) { return e ? entry_str(e) : "?"; }

static void print_string_literal(FILE *out, const char *s) {
    fputc('"', out);
    for (; *s; s++) {
        switch (*s) {
            case '\n': fputs("\\n", out);  break;
            case '\t': fputs("\\t", out);  break;
            case '\b': fputs("\\b", out);  break;
            case '\f': fputs("\\f", out);  break;
            case '"':  fputs("\\\"", out); break;
            case '\\': fputs("\\\\", out); break;
            default:   fputc(*s, out);
        }
    }
    fputc('"', out);
}

static void ast_print_rec(ASTNode *n, FILE *out, int depth);

static void print_all(DynArray *a, FILE *out, int depth) {
    for (int i = 0; i < a->count; i++)
        ast_print_rec(*(ASTNode **)dynarray_get(a, i), out, depth);
}

static void ast_print_rec(ASTNode *n, FILE *out, int depth) {
    for (int i = 0; i < depth; i++) fputs("  ", out);
    if (!n) { fputs("(missing)\n", out); return; }
    int d = depth + 1;

    switch (n->type) {
    case NODE_PROGRAM:
        fprintf(out, "PROGRAM #%d\n", n->line);
        print_all(n->as.program.classes, out, d);
        break;
    case NODE_CLASS:
        fprintf(out, "CLASS %s", name_of(n->as.class_.name));
        if (n->as.class_.parent) fprintf(out, " inherits %s", name_of(n->as.class_.parent));
        fprintf(out, " #%d\n", n->line);
        print_all(n->as.class_.features, out, d);
        break;
    case NODE_ATTR:
        fprintf(out, "ATTR %s : %s #%d\n", name_of(n->as.attr.name),
                name_of(n->as.attr.decl_type), n->line);
        ast_print_rec(n->as.attr.init, out, d);
        break;
    case NODE_METHOD:
        fprintf(out, "METHOD %s : %s #%d\n", name_of(n->as.method.name),
                name_of(n->as.method.ret_type), n->line);
        print_all(n->as.method.formals, out, d);
        ast_print_rec(n->as.method.body, out, d);
        break;
    case NODE_FORMAL:
        fprintf(out, "FORMAL %s : %s #%d\n", name_of(n->as.formal.name),
                name_of(n->as.formal.decl_type), n->line);
        break;
    case NODE_ASSIGN:
        fprintf(out, "ASSIGN %s #%d\n", name_of(n->as.assign.name), n->line);
        ast_print_rec(n->as.assign.values, out, d);
        break;
    case NODE_DISPATCH:
        fprintf(out, "DISPATCH %s%s #%d\n", name_of(n->as.dispatch.method),
                n->as.dispatch.recv ? "" : " (self)", n->line);
        if (n->as.dispatch.recv) ast_print_rec(n->as.dispatch.recv, out, d);
        print_all(n->as.dispatch.args, out, d);
        break;
    case NODE_STATIC_DISPATCH:
        fprintf(out, "STATIC_DISPATCH @%s.%s #%d\n", name_of(n->as.static_dispatch.cast_type),
                name_of(n->as.static_dispatch.method), n->line);
        ast_print_rec(n->as.static_dispatch.recv, out, d);
        print_all(n->as.static_dispatch.args, out, d);
        break;
    case NODE_IF:                                   /* children: cond, then, else */
        fprintf(out, "IF #%d\n", n->line);
        ast_print_rec(n->as.if_.cond, out, d);
        ast_print_rec(n->as.if_.then_branch, out, d);
        ast_print_rec(n->as.if_.else_branch, out, d);
        break;
    case NODE_WHILE:                                /* children: cond, body */
        fprintf(out, "WHILE #%d\n", n->line);
        ast_print_rec(n->as.while_.cond, out, d);
        ast_print_rec(n->as.while_.body, out, d);
        break;
    case NODE_BLOCK:
        fprintf(out, "BLOCK #%d\n", n->line);
        print_all(n->as.block.exprs, out, d);
        break;
    case NODE_LET:                                  /* children: init, body */
        fprintf(out, "LET %s : %s #%d\n", name_of(n->as.let.name),
                name_of(n->as.let.decl_type), n->line);
        ast_print_rec(n->as.let.init, out, d);
        ast_print_rec(n->as.let.body, out, d);
        break;
    case NODE_CASE:                                 /* children: scrutinee, branches */
        fprintf(out, "CASE #%d\n", n->line);
        ast_print_rec(n->as.case_.expr, out, d);
        print_all(n->as.case_.branches, out, d);
        break;
    case NODE_CASE_BRANCH:
        fprintf(out, "CASE_BRANCH %s : %s #%d\n", name_of(n->as.case_branch.name),
                name_of(n->as.case_branch.decl_type), n->line);
        ast_print_rec(n->as.case_branch.body, out, d);
        break;
    case NODE_NEW:
        fprintf(out, "NEW %s #%d\n", name_of(n->as.new_.type_name), n->line);
        break;
    case NODE_ISVOID:
        fprintf(out, "ISVOID #%d\n", n->line);
        ast_print_rec(n->as.isvoid.expr, out, d);
        break;
    case NODE_BINOP:
        fprintf(out, "BINOP %s #%d\n", op_symbol(n->as.binop.op), n->line);
        ast_print_rec(n->as.binop.left, out, d);
        ast_print_rec(n->as.binop.right, out, d);
        break;
    case NODE_UNOP:
        fprintf(out, "UNOP %s #%d\n", op_symbol(n->as.unop.op), n->line);
        ast_print_rec(n->as.unop.operand, out, d);
        break;
    case NODE_INT_CONST:
        fprintf(out, "INT_CONST %s #%d\n", name_of(n->as.int_const.value), n->line);
        break;
    case NODE_STR_CONST:
        fputs("STR_CONST ", out);
        print_string_literal(out, name_of(n->as.string_const.value));
        fprintf(out, " #%d\n", n->line);
        break;
    case NODE_BOOL_CONST:
        fprintf(out, "BOOL_CONST %s #%d\n", n->as.bool_const.value ? "true" : "false", n->line);
        break;
    case NODE_OBJECT_ID:
        fprintf(out, "OBJECT_ID %s #%d\n", name_of(n->as.object_id.name), n->line);
        break;
    case NODE_NO_EXPR:
        fprintf(out, "NO_EXPR #%d\n", n->line);
        break;
    }
}

void ast_print(ASTNode *root, FILE *out) { ast_print_rec(root, out, 0); }
void ast_dump(ASTNode *root) { ast_print(root, stderr); }