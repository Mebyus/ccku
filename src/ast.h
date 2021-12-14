#ifndef KU_AST_H
#define KU_AST_H

#include "slice.h"
#include "str.h"
#include "token.h"
#include "types.h"

typedef enum FunctionResultType FunctionResultType;
typedef enum StatementType StatementType;
typedef enum ExpressionType ExpressionType;
typedef enum TypeSpecifierType TypeSpecifierType;
typedef enum TypeLiteralType TypeLiteralType;
typedef struct StandaloneSourceText StandaloneSourceText;
typedef struct Statement Statement;
typedef struct DefineStatement DefineStatement;
typedef struct Expression Expression;
typedef struct CallExpression CallExpression;
typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct FunctionDefinition FunctionDefinition;
typedef struct SimpleResult SimpleResult;
typedef struct TupleResult TupleResult;
typedef struct FunctionResult FunctionResult;
typedef struct FunctionParameters FunctionParameters;
typedef struct ParameterDeclaration ParameterDeclaration;
typedef struct TypeSpecifier TypeSpecifier;
typedef struct BlockStatement BlockStatement;
typedef struct CallArgument CallArgument;
typedef struct Identifier Identifier;
typedef struct Integer Integer;
typedef struct String String;

TYPEDEF_SLICE(Identifier)
TYPEDEF_SLICE(ParameterDeclaration)
TYPEDEF_SLICE(Statement)
TYPEDEF_SLICE(Expression)
TYPEDEF_SLICE(CallArgument)
TYPEDEF_SLICE(FunctionDefinition)

enum FunctionResultType {
    frt_Void,
    frt_Simple,
    frt_Tuple,
};

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

enum TypeSpecifierType {
    tst_Name,
    tst_Literal,
};

enum TypeLiteralType {
    tlt_Pointer,
    tlt_Array,
    tlt_Slice,
    tlt_Map,
    tlt_Set,
    tlt_Channel,
};

struct TypeName {
    Identifier module_name;
    Identifier name;
};

struct ArrayTypeLiteral {
    Expression length_specifier;
    TypeSpecifier element_type;
};

struct SliceTypeLiteral {
    TypeSpecifier element_type;
};

struct MapTypeLiteral {
    TypeSpecifier key_type;
    TypeSpecifier value_type;
};

struct SetTypeLiteral {
    TypeSpecifier element_type;
};

struct TypeLiteral {
    TypeLiteralType type;
    void *ptr;
};

struct TypeSpecifier {
    TypeSpecifierType type;
    void *ptr;
};

struct ParameterDeclaration {
    TypeSpecifier type_specifier;
    slice_of_Identifiers names;
};

struct FunctionParameters {
    slice_of_ParameterDeclarations parameter_declarations;
};

struct SimpleResult {
    TypeSpecifier type_specifier;
};

struct TupleResult {
    slice_of_ParameterDeclarations parameter_declarations;
};

struct FunctionResult {
    FunctionResultType type;
    void *ptr;
};

struct Identifier {
    Token token;
};

struct FunctionDeclaration {
    Identifier name;
    FunctionParameters parameters;
    FunctionResult result;
};

struct BlockStatement {
    slice_of_Statements statements;
};

struct FunctionDefinition {
    FunctionDeclaration declaration;
    BlockStatement body;
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

struct StandaloneSourceText {
    slice_of_FunctionDefinitions functions;
    slice_of_Statements statements;
};

Statement init_empty_statement();
Statement init_define_statement(slice_of_Expressions left, slice_of_Expressions right);
Statement init_expression_statement(Expression expr);

Expression init_identifier_expression(Token token);
Expression init_integer_expression(Token token);
Expression init_call_expression(Token name_token, slice_of_Expressions args);
Expression init_string_expression(Token token);

#endif // KU_AST_H
