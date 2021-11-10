#ifndef KU_STATEMENT_H
#define KU_STATEMENT_H

#include "slice.h"
#include "str.h"
#include "token.h"

// Language syntax in Backus-Naur form
//
// <SourceText> => <ModuleClause>, [<ImportClause>], { <TopLevelDeclaration> }, <EOF>;
//
// <ModuleClause> => "module", <Identifier>, ";";
//
// <TopLevelDeclaration> => <ConstDeclaration> | <TypeDeclaration> | <VarDeclaration> | <FunctionDeclaration>
//
// <BlockStatement> => "{", <StatementList>, "}";
//
// <StatementList> => { <Statement> };
//
// <Statement> => <BlockStatement> | <AssignStatement> | <DefineStatement> | <IfStatement>
//                <DeferStatement> | <ExpressionStatement> | <ReturnStatement> | <MatchStatement>;
//
// <DeferStatement> => "defer", <DeferClause>;
//
// <IfStatement> => <IfClause>, { <ElifClause> }, [ <ElseClause> ];
//
// <IfClause> => "if", <Expression>, <BlockStatement>;
//
// <ElifClause> => "elif", <Expression>, <BlockStatement>;
//
// <ElseClause> => "else", <BlockStatement>;
//
// <ExpressionStatement> => <Expression>, ";";
//
// <DeferClause> => <CallExpression>, ";" | <BlockStatement>;
//
// <DefineStatement> => [ "imt" ], <Identifier>, { ",", <Identifier> }, ":=", <Expression>, { ",", <Expression> }, ";";
//
// <SelectorExpression> => <Expression>, ".", <Identifier>
//
// <CallExpression> => <Expression>, "(", { <Expression>, "," }, ")";
//
// <DestinationExpression> => <IndexExpression> | <Identifier>
//
// <AssignStatement> => <DestinationExpression>, { ",", <DestinationExpression> }, "=", <Expression>, { ",",
//                      <Expression> }, ";";
//
// <ReturnStatement> => "return", [ <Expression> ], ";";

typedef enum StatementType StatementType;
typedef enum ExpressionType ExpressionType;
typedef struct Statement Statement;
typedef struct DefineStatement DefineStatement;
typedef struct Expression Expression;
typedef struct CallExpression CallExpression;
typedef struct CallArgument CallArgument;
typedef struct Identifier Identifier;
typedef struct Integer Integer;
typedef struct String String;

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
    et_Identifier,
    et_Call,
    et_IntegerLiteral,
    et_StringLiteral,
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

struct Integer {
    Token token;
};

struct String {
    Token token;
};

struct CallExpression {
    str function_name;
    slice_of_Expressions args;
};

struct CallArgument {
    Expression expression;
};

Statement init_empty_statement();
Statement init_define_statement(slice_of_Expressions left, slice_of_Expressions right);
Statement init_expression_statement(Expression expr);

Expression init_identifier_expression(Token token);
Expression init_integer_expression(Token token);
Expression init_call_expression(Token name_token, slice_of_Expressions args);
Expression init_string_expression(Token token);

#endif // KU_STATEMENT_H
