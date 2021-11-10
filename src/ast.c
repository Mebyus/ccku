#include "ast.h"

Statement init_empty_statement() {
    Statement stmt = {
        .type = st_Empty,
        .ptr  = NULL,
    };
    return stmt;
}

Statement init_define_statement(slice_of_Expressions left, slice_of_Expressions right) {
    DefineStatement *dstmt = (DefineStatement *)malloc(sizeof(DefineStatement));
    if (dstmt == NULL) {
        fatal(1, "not enough memory for new define statement");
    }
    dstmt->left  = left;
    dstmt->right = right;

    Statement stmt = {
        .type = st_Define,
        .ptr  = dstmt,
    };
    return stmt;
}

Statement init_expression_statement(Expression expr) {
    Expression *new_expr = (Expression *)malloc(sizeof(Expression));
    if (new_expr == NULL) {
        fatal(1, "not enough memory for new define statement");
    }
    *new_expr = expr;

    Statement stmt = {
        .type = st_Expression,
        .ptr  = new_expr,
    };
    return stmt;
}

Expression init_identifier_expression(Token token) {
    Identifier *ident = (Identifier *)malloc(sizeof(Identifier));
    if (ident == NULL) {
        fatal(1, "not enough memory for new identifier expression");
    }
    ident->name = take_str_from_str(&token.literal);

    Expression expr = {
        .type = et_Identifier,
        .ptr  = ident,
    };
    return expr;
}

Expression init_integer_expression(Token token) {
    Integer *literal = (Integer *)malloc(sizeof(Integer));
    if (literal == NULL) {
        fatal(1, "not enough memory for new integer literal expression");
    }
    literal->token = token;

    Expression expr = {
        .type = et_IntegerLiteral,
        .ptr  = literal,
    };
    return expr;
}

Expression init_string_expression(Token token) {
    String *literal = (String *)malloc(sizeof(String));
    if (literal == NULL) {
        fatal(1, "not enough memory for new string literal expression");
    }
    literal->token = token;

    Expression expr = {
        .type = et_StringLiteral,
        .ptr  = literal,
    };
    return expr;
}

Expression init_call_expression(Token name_token, slice_of_Expressions args) {
    CallExpression *cexpr = (CallExpression *)malloc(sizeof(CallExpression));
    if (cexpr == NULL) {
        fatal(1, "not enough memory for new call expression");
    }
    cexpr->function_name = take_str_from_str(&name_token.literal);
    cexpr->args          = args;

    Expression expr = {
        .type = et_Call,
        .ptr  = cexpr,
    };
    return expr;
}

IMPLEMENT_SLICE(Statement)
IMPLEMENT_SLICE(Expression)
IMPLEMENT_SLICE(CallArgument)
