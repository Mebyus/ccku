#include "ast.h"
#include "xnew.h"

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
// <ParameterDeclaration>  = [ <IdentifierList> ":" ] [ "..." ] <TypeSpecifier>
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
//                 <ChannelTypeLiteral> | <SetTypeLiteral>
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

const Identifier empty_identifier = {
    .token = empty_token,
};

const TypeSpecifier empty_type_specifier = {
    .ptr = nil,
};

const FunctionResult void_result = {
    .type = frt_Void,
    .ptr  = nil,
};

const StandaloneSourceTree empty_standalone_source_tree = {
    .functions  = EMPTY_SLICE,
    .statements = EMPTY_SLICE,
};

const BlockStatement empty_block_statement = {
    .statements = EMPTY_SLICE,
};

const str function_display_title = STR("fn: ");
const str params_display_title   = STR("params: ");
const str result_display_title   = STR("result: ");
const str void_display_title     = STR("void");

const u8 display_indentation = 2;

Identifier init_identifier(Token token) {
    Identifier identifier = {
        .token = token,
    };
    return identifier;
}

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
    CallExpression *call_expression = (CallExpression *)malloc(sizeof(CallExpression));
    if (call_expression == nil) {
        fatal(1, "not enough memory for new call expression");
    }
    call_expression->function_name = take_str_from_str(&name_token.literal);
    call_expression->args          = args;

    Expression expr = {
        .type = et_Call,
        .ptr  = call_expression,
    };
    return expr;
}

TypeSpecifier new_name_type_specifier(Token token) {
    TypeName *type_name = (TypeName *)malloc(sizeof(TypeName));
    if (type_name == nil) {
        fatal(1, "not enough memory for new call expression");
    }
    type_name->name              = init_identifier(token);
    type_name->module_name       = empty_identifier;
    TypeSpecifier type_specifier = {
        .type = tst_Name,
        .ptr  = type_name,
    };
    return type_specifier;
}

FunctionResult new_simple_result(TypeSpecifier type_specifier) {
    SimpleResult *simple_result = (SimpleResult *)malloc(sizeof(SimpleResult));
    if (simple_result == nil) {
        fatal(1, "not enough memory for new simple result");
    }
    simple_result->type_specifier  = type_specifier;
    FunctionResult function_result = {
        .type = frt_Simple,
        .ptr  = simple_result,
    };
    return function_result;
}

FunctionResult new_typed_tuple_result(slice_of_ParameterDeclarations params) {
    TypedTupleResult *tuple_result       = xnew(TypedTupleResult);
    tuple_result->parameter_declarations = params;
    FunctionResult function_result       = {
              .type = frt_TypedTuple,
              .ptr  = tuple_result,
    };
    return function_result;
}

FunctionResult new_tuple_signature_result_from_identifiers(slice_of_Identifiers names) {
    TupleSignatureResult *tuple_signature_result = xnew(TupleSignatureResult);
    tuple_signature_result->type_specifiers      = empty_slice_of_TypeSpecifiers;
    for (u32 i = 0; i < names.len; i++) {
        append_TypeSpecifier_to_slice(
            &tuple_signature_result->type_specifiers, new_name_type_specifier(names.elem[i].token));
    }
    FunctionResult function_result = {
        .type = frt_TupleSignature,
        .ptr  = tuple_signature_result,
    };
    return function_result;
}

FunctionResult new_tuple_signature_result(slice_of_TypeSpecifiers type_specifiers) {
    TupleSignatureResult *tuple_signature_result = xnew(TupleSignatureResult);
    tuple_signature_result->type_specifiers      = type_specifiers;
    FunctionResult function_result               = {
                      .type = frt_TupleSignature,
                      .ptr  = tuple_signature_result,
    };
    return function_result;
}

TypeSpecifier new_slice_type_specifier(TypeSpecifier element_type_specifier) {
    SliceTypeLiteral *slice_type_literal = xnew(SliceTypeLiteral);
    slice_type_literal->element_type     = element_type_specifier;
    TypeLiteral *type_literal            = xnew(TypeLiteral);
    type_literal->type                   = tlt_Slice;
    type_literal->ptr                    = slice_type_literal;
    TypeSpecifier type_specifier         = {
                .type = tst_Literal,
                .ptr  = type_literal,
    };
    return type_specifier;
}

slice_of_TypeSpecifiers new_type_specifiers_from_identifiers(slice_of_Identifiers names) {
    slice_of_TypeSpecifiers type_specifiers = empty_slice_of_TypeSpecifiers;
    for (u32 i = 0; i < names.len; i++) {
        append_TypeSpecifier_to_slice(&type_specifiers, new_name_type_specifier(names.elem[i].token));
    }
    return type_specifiers;
}

void print_type_name(TypeName type_name) {
    print_indent_str(1, type_name.name.token.literal);
}

void print_type_qualified_name(TypeName type_name) {
    print_indent_str(1, type_name.name.token.literal);
    print_str(type_name.module_name.token.literal);
}

void print_type_literal(TypeLiteral type_literal) {
    switch (type_literal.type) {
    case tlt_Slice:
        break;
    default:
        break;
    }
}

void print_type_specifier(TypeSpecifier type_specifier) {
    switch (type_specifier.type) {
    case tst_Name:
        print_type_name(*(TypeName *)type_specifier.ptr);
        break;
    case tst_QualifiedName:
        print_type_name(*(TypeName *)type_specifier.ptr);
        break;
    case tst_Literal:
        print_type_literal(*(TypeLiteral *)type_specifier.ptr);
        break;
    default:
        break;
    }
}

void print_parameter_declaration(u8 spaces, ParameterDeclaration decl) {
    u8 indent = (u8)(spaces + display_indentation);
    for (u32 i = 0; i < decl.names.len; i++) {
        print_indent_str(indent, decl.names.elem[i].token.literal);
        print_type_specifier(decl.type_specifier);
        println();
    }
}

void print_function_parameters(FunctionParameters params) {
    print_indent_str(display_indentation, params_display_title);
    if (params.parameter_declarations.len == 0) {
        print_str(void_display_title);
    } else {
        println();
        for (u32 i = 0; i < params.parameter_declarations.len; i++) {
            print_parameter_declaration(display_indentation, params.parameter_declarations.elem[i]);
        }
    }
    println();
}

void print_function_result(FunctionResult result) {
    print_indent_str(display_indentation, result_display_title);
    if (result.type == frt_Void) {
        print_str(void_display_title);
    }
    println();
}

void print_function_name(Identifier name) {
    print_str(function_display_title);
    println_str(name.token.literal);
}

void print_function_definition(FunctionDefinition def) {
    print_function_name(def.declaration.name);
    print_function_parameters(def.declaration.parameters);
    print_function_result(def.declaration.result);
    println();
}

void print_standalone_source_tree(StandaloneSourceTree tree) {
    for (u32 i = 0; i < tree.functions.len; i++) {
        print_function_definition(tree.functions.elem[i]);
    }
}

IMPLEMENT_SLICE(Statement)
IMPLEMENT_SLICE(Expression)
IMPLEMENT_SLICE(CallArgument)
IMPLEMENT_SLICE(Identifier)
IMPLEMENT_SLICE(TypeSpecifier)
IMPLEMENT_SLICE(ParameterDeclaration)
IMPLEMENT_SLICE(FunctionDefinition)
