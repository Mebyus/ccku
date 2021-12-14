#include "ast.h"

// Language syntax in Backus-Naur extended form
//
// <SourceText> = <ModuleClause> [ <ImportClause> ] { <TopLevelDeclaration> } <EOF>
//
// <ModuleClause> = "module" <ModuleName>
//
// <ModuleName> = <Identifier>
//
// <TopLevelDeclaration> = <ConstDeclaration> | <TypeDeclaration> | <VarDeclaration> | <FunctionDeclaration> |
//                         <FunctionDefinition>
//
// <FunctionDeclaration> = "fn" <FunctionName> <FunctionSignature>
//
// <FunctionDefinition> = <FunctionDeclaration> <FunctionBody>
//
// <FunctionSignature> = <FunctionParameters> [ "=>" <FunctionResult> ]
//
// <FunctionResult> = <FunctionParameters> | <TypeSpecifier>
//
// <FunctionParameters> = "(" [ <ParameterList> [ "," ] ] ")"
//
// <ParameterList>  = <ParameterDeclaration> { "," <ParameterDeclaration> }
//
// <ParameterDeclaration>  = [ <IdentifierList> ] [ "..." ] <TypeSpecifier>
//
// <IdentifierList> = <Identifier> { "," <Identifier> }
//
// <FunctionBody> = <BlockStatement>
//
// <FunctionName> = <Identifier>
//
// <TypeSpecifier> = <TypeName> | <TypeLiteral>
//
// <TypeName> = <Identifier> | <QualifiedIdentifier>
//
// <QualifiedIdentifier> = <ModuleName> "." <Identifier>
//
// <TypeLiteral> = <ArrayTypeLiteral> | <PointerTypeLiteral> | <SliceTypeLiteral> | <MapTypeLiteral> |
//                 <ChannelTypeLiteral>
//
// <ArrayTypeLiteral> = "[" <ArrayLengthSpecifier> "]" <ElementTypeSpecifier>
//
// <ArrayLengthSpecifier> = <Expression>
//
// <ElementTypeSpecifier> = <TypeSpecifier>
//
// <PointerTypeLiteral> = "*" <BaseTypeSpecifier>
//
// <BaseTypeSpecifier> = <TypeSpecifier>
//
// <SliceTypeLiteral> = "[" "]" <ElementTypeSpecifier>
//
// <MapTypeLiteral> = "[" <KeyTypeSpecifier> "]" "=>" <ValueTypeSpecifier>
//
// <KeyTypeSpecifier> = <TypeSpecifier>
//
// <ValueTypeSpecifier> = <TypeSpecifier>
//
// <ConstDeclaration> = "const", <ConstSpec>
//
// <ConstSpec> = <SingleLineConstSpec> | <MultiLineConstSpec>
//
// <SingleLineConstSpec> = <IdentifierList>, [ <Type> ], "=", <ExpressionList>, <TERM>
//
// <MultiLineConstSpec> = <SingleLineConstSpec>, { <SingleLineConstSpec> }
//
// <BlockStatement> = "{", <StatementList>, "}"
//
// <StatementList> = { <Statement> };
//
// <Statement> = <BlockStatement> | <AssignStatement> | <DefineStatement> | <IfStatement> |
//               <DeferStatement> | <ExpressionStatement> | <ReturnStatement> | <MatchStatement> |
//               <LoopStatement>
//
// <DeferStatement> = "defer", <DeferClause>;
//
// <IfStatement> = <IfClause>, { <ElifClause> }, [ <ElseClause> ];
//
// <IfClause> = "if", <Expression>, <BlockStatement>;
//
// <ElifClause> = "elif", <Expression>, <BlockStatement>;
//
// <ElseClause> = "else", <BlockStatement>;
//
// <ExpressionStatement> = <Expression>, ";";
//
// <DeferClause> = <CallExpression>, <TERM> | <BlockStatement>;
//
// <DefineStatement> = [ "imt" ], <Identifier>, { ",", <Identifier> }, ":=", <Expression>, { ",", <Expression> }, ";";
//
// <SelectorExpression> = <Expression>, ".", <Identifier>
//
// <CallExpression> = <Expression>, "(", { <Expression>, "," }, ")";
//
// <DestinationExpression> = <IndexExpression> | <Identifier>
//
// <AssignStatement> = <DestinationExpression>, { ",", <DestinationExpression> }, "=", <Expression>, { ",",
//                     <Expression> }, ";";
//
// <ReturnStatement> = "return", [ <Expression> ], ";";

const StandaloneSourceTree empty_standalone_source_tree = {
    .functions  = EMPTY_SLICE,
    .statements = EMPTY_SLICE,
};

const BlockStatement empty_block_statement = {
    .statements = EMPTY_SLICE,
};

Statement init_empty_statement() {
    Statement stmt = {
        .type = st_Empty,
        .ptr  = nil,
    };
    return stmt;
}

Statement init_define_statement(slice_of_Expressions left, slice_of_Expressions right) {
    DefineStatement *dstmt = (DefineStatement *)malloc(sizeof(DefineStatement));
    if (dstmt == nil) {
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
    if (new_expr == nil) {
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
    if (ident == nil) {
        fatal(1, "not enough memory for new identifier expression");
    }
    ident->token = token;

    Expression expr = {
        .type = et_Identifier,
        .ptr  = ident,
    };
    return expr;
}

Expression init_integer_expression(Token token) {
    Integer *literal = (Integer *)malloc(sizeof(Integer));
    if (literal == nil) {
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
    if (literal == nil) {
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
    if (cexpr == nil) {
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
IMPLEMENT_SLICE(Identifier)
IMPLEMENT_SLICE(ParameterDeclaration)
IMPLEMENT_SLICE(FunctionDefinition)
