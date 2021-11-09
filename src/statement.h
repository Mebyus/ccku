#ifndef KU_STATEMENT_H
#define KU_STATEMENT_H

#include "slice.h"
#include "str.h"
#include "token.h"

typedef enum StatementType StatementType;
typedef enum ExpressionType ExpressionType;
typedef struct Statement Statement;
typedef struct DefineStatement DefineStatement;
typedef struct Expression Expression;
typedef struct CallExpression CallExpression;
typedef struct CallArgument CallArgument;
typedef struct Identifier Identifier;

TYPEDEF_SLICE(Statement)
TYPEDEF_SLICE(Expression)
TYPEDEF_SLICE(CallArgument)

enum StatementType {
    st_Empty,
    st_Block,
    st_Assign,
    st_Defer,
    st_Expression,
    st_FunctionDeclaration,
    st_Define,
    st_Import,
    st_Module,
    st_Return,
    st_StructDeclaration,
    st_TypeDeclaration,
};

enum ExpressionType {
    et_Call,
    et_StringLiteral,
    et_Identifier,
};

struct Statement {
    StatementType type;
    void *ptr;
};

struct DefineStatement {
    slice_of_Expressions left, right;
};

struct Expression {
    ExpressionType type;
    void *ptr;
};

struct Identifier {
    str name;
};

struct CallExpression {
    str function_name;
    slice_of_Expressions args;
};

struct CallArgument {
    Expression expression;
};

Statement get_empty_statement();
Statement get_define_statement(slice_of_Expressions left, slice_of_Expressions right);
Statement get_expression_statement(Expression expr);

Expression get_identifier_expression(Token token);
Expression get_call_expression(Token name_token, slice_of_Expressions args);

#endif // KU_STATEMENT_H
