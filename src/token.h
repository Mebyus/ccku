#ifndef KU_TOKEN_H
#define KU_TOKEN_H

#include <stdint.h>

#include "position.h"
#include "str.h"

typedef enum TokenType TokenType;
typedef struct Token Token;
typedef struct KeywordLookupResult KeywordLookupResult;

enum TokenType {
    // Special tokens
    tt_Illegal,
    tt_EOF,
    tt_Comment,

    // Identifiers and basic type literal classes
    tt_BlankIdentifier, // _
    tt_Identifier,      // e.g: myvar, main, Line, println
    tt_Integer,         // e.g: 5367, 43_432, 1_000_097
    tt_Float,           // e.g: 123.45
    tt_Character,       // e.g: 'a', '\t', 'p'
    tt_String,          // e.g: "abc", "", "\t\n  42Hello\n"

    // Operators and/or punctuators
    tt_Plus,      // +
    tt_Minus,     // -
    tt_Asterisk,  // *
    tt_Slash,     // /
    tt_Percent,   // %
    tt_Ampersand, // &

    tt_Pipe,          // |
    tt_Caret,         // ^
    tt_LeftShift,     // <<
    tt_RightShift,    // >>
    tt_BitwiseAndNot, // &^

    tt_Semicolon, // ;
    tt_Period,    // .
    tt_Colon,     // :
    tt_Comma,     // ,
    tt_Ellipsis,  // ...

    tt_Equal,   // ==
    tt_Less,    // <
    tt_Greater, // >
    tt_Assign,  // =
    tt_Not,     // !

    tt_Define,          // :=
    tt_AddAssign,       // +=
    tt_SubstractAssign, // -=
    tt_MultiplyAssign,  // *=
    tt_QuotientAssign,  // /=
    tt_RemainderAssign, // %=

    tt_LogicalAnd, // &&
    tt_LogicalOr,  // ||
    tt_LeftArrow,  // <-
    tt_Increment,  // ++
    tt_Decrement,  // --

    // Brackets
    tt_LeftCurlyBracket,   // {
    tt_RightCurlyBracket,  // }
    tt_LeftSquareBracket,  // [
    tt_RightSquareBracket, // ]
    tt_LeftRoundBracket,   // (
    tt_RightRoundBracket,  // )

    // Keywords
    tt_begin_keyword,

    tt_Break,
    tt_Case,
    tt_Channel,
    tt_Const,
    tt_Continue,

    tt_If,
    tt_Else,
    tt_ElseIf,
    tt_In,
    tt_For,
    tt_Loop,
    tt_While,

    tt_Defer,
    tt_Function,
    tt_Ku,
    tt_Goto,
    tt_Import,

    tt_Interface,
    tt_Map,
    tt_Package,
    tt_Module,
    tt_Return,

    tt_Select,
    tt_Struct,
    tt_Switch,
    tt_Type,
    tt_Var,

    tt_Dirty,
    tt_Default,
    tt_Immutable,
    tt_Public,

    tt_end_keyword,
};


struct Token {
    TokenType type;
    Position pos;
    str literal;
};

struct KeywordLookupResult {
    bool found;
    TokenType type;
};

Token create_token(TokenType type, Position pos);
Token create_token_with_literal(TokenType type, Position pos, str literal);
KeywordLookupResult lookup_keyword(str s);
void print_token(Token token);
void free_token(Token token);

#endif // KU_TOKEN_H
