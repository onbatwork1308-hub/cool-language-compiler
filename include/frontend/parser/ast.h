#ifndef AST_H
#define AST_H

#include "table.h"
#include "token.h"
#include "dynarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @enum ASTNodeType
 * @brief Every grammer construct the parser can produce a node for.
 */

typedef enum {
    NODE_PROGRAM, NODE_CLASS, NODE_ATTR, NODE_METHOD, NODE_FORMAL,

    NODE_ASSIGN, NODE_DISPATCH, NODE_STATIC_DISPATCH, NODE_IF, NODE_WHILE,
    NODE_BLOCK, NODE_LET, NODE_CASE, NODE_CASE_BRANCH, NODE_NEW, NODE_ISVOID,
    NODE_BINOP, NODE_UNOP,
    NODE_INT_CONST, NODE_STR_CONST, NODE_BOOL_CONST, NODE_OBJECT_ID,
    NODE_NO_EXPR
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    int line;

    union {
        struct { DynArray *classes; } program;
        struct { Entry *name; Entry *parent; DynArray *features; } class_;
        struct { Entry *name; Entry *decl_type; struct ASTNode *init; } attr;
        struct { Entry *name; DynArray *formals; Entry *ret_type; 
                    struct ASTNode *body; } method;
        struct {Entry *name; Entry *decl_type; } formal;
        
        struct { Entry *name; struct ASTNode *values; } assign;
        struct { struct ASTNode *recv; Entry *method; DynArray *args; } dispatch;
        struct { struct ASTNode *recv; Entry *cast_type; Entry *method; 
                    DynArray *args; } static_dispatch;
        struct { struct ASTNode *cond, *then_branch, *else_branch; } if_;
        struct { struct ASTNode *cond, *body; } while_;
        struct { DynArray *exprs; } block;
        struct { Entry *name; Entry *decl_type; struct ASTNode *init;
                    struct ASTNode *body; } let;
        struct { struct ASTNode *expr; DynArray *branches; } case_;
        struct { Entry *name; Entry *decl_type; struct ASTNode *body; } case_branch;
        
        struct { Entry *type_name; } new_;
        struct { struct ASTNode *expr; } isvoid;
        struct { TokenType op; struct ASTNode *left, *right; } binop;
        struct { TokenType op; struct ASTNode *operand; } unop;

        struct { Entry *value; } int_const;
        struct { Entry *value; } string_const;
        struct { bool value; } bool_const;
        struct { Entry *name; } object_id;
    } as;
} ASTNode;

/**
 * Constructors for each of the ASTNode's union.
 */
ASTNode *ast_make_program(int line);
ASTNode *ast_make_class(Entry *name, Entry *parent, int line);
ASTNode *ast_make_attr(Entry *name, Entry *decl_typr,ASTNode *init, int line);
ASTNode *ast_make_method(Entry *name, Entry *ret_type, ASTNode *body, int line);
ASTNode *ast_make_formal(Entry *name, Entry *decl_type, int line);

ASTNode *ast_make_assign(Entry *name, ASTNode *value, int line);
ASTNode *ast_make_dispatch(ASTNode *recv, Entry *method, int line);
ASTNode *ast_make_static_dipatch(ASTNode *recv, Entry *cast_type, Entry *method, int line);
ASTNode *ast_make_if(ASTNode *cond, ASTNode *then_b, ASTNode *else_b, int line);
ASTNode *ast_make_while(ASTNode *cond, ASTNode *body, int line);
ASTNode *ast_make_block(int line);
ASTNode *ast_make_let(Entry *name, Entry *decl_type, ASTNode *init, ASTNode *body, int line);
ASTNode *ast_make_case(ASTNode *expr, int line);
ASTNode *ast_make_case_branch(Entry *name, Entry *decl_type, ASTNode *body, int line);

ASTNode *ast_make_new(Entry *type_name, int line);
ASTNode *ast_make_isvoid(ASTNode *value, int line);
ASTNode *ast_make_binop(TokenType op, ASTNode *left, ASTNode *right, int line);
ASTNode *ast_make_unop(TokenType op, ASTNode *operand, int line);

ASTNode *ast_make_int_const(Entry *value, int line);
ASTNode *ast_make_str_const(Entry *value, int line);
ASTNode *ast_make_bool_const(bool value, int line);
ASTNode *ast_make_object_id(Entry *name, int line);
ASTNode *ast_make_no_expr(int line);

void ast_add_class(ASTNode *program, ASTNode *class_node);
void ast_add_feature(ASTNode *class_node, ASTNode *feature);
void ast_add_formal(ASTNode *method, ASTNode *formal);
void ast_add_arg(ASTNode *dispatch_or_static_dispatch, ASTNode *arg);
void ast_add_block_expr(ASTNode *block, ASTNode *expr);
void ast_add_branch(ASTNode *case_node, ASTNode *branch);

/**
 * Traversal and other debuging helpers.
 */
void ast_walk(ASTNode *root, void (*visit)(ASTNode *node, void *ctx), void *ctx);
void ast_print(ASTNode *root, FILE *out);
void ast_destroy(ASTNode *root);
#endif
