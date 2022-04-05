#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

#define AST_NEW(type) \
    static_cast<type*>(default_ast(new type))

class Parser {
public:
    Parser() = default;
    Parser(Vec<Token>* tokens);

	Ast* default_ast(Ast* ast);

    void run();

    Token* peek();
    Token* advance();

    bool end();
private:
    Ast_Expression* expression();
private:
    Vec<Token>* tokens = nullptr;
    uint32_t current = 0;
};

#endif // !PARSER_H