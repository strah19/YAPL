#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

struct Ast_Expression;

enum {
    AST_ID,
    AST_EXPRESSION,
    ASY_UNARY,
    AST_PRIMARY,
    AST_BINARY,
    AST_ASSIGNMENT,
    AST_STATEMENT
};

#define AST_DELETE(type) delete type

#define AST_CAST(type, base) static_cast<type*>(base)

class Parser {
public:
    Parser() = default;
    Parser(Vec<Token>* tokens);

    void run();

    Token* peek();
    Token* advance();
private:
    Vec<Token>* tokens = nullptr;
};

#endif // !PARSER_H