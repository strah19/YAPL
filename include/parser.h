#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "lexer.h"
#include "ast.h"

class Parser {
public:
    Parser() = default;
    Parser(std::vector<Token>* tokens);
    ~Parser();

    void run();

    Token* peek();
    Token* advance();

    void match(int type);
    bool check(int type);
    bool is_end();

    void visualize();
    void visualize_expression(Ast_Expression* expr, int indent = 1);
private:
    Ast_Expression* expression();
    Ast_Expression* equality();
    Ast_Expression* comparison();

    Ast_Expression* term();
    Ast_Expression* factor();
    Ast_Expression* unary();
    Ast_Expression* primary();

    Ast_Identifier* identifier();

    Ast_Decleration* decleration();

    int token_to_ast(Token* token);
    void ident(int indent);
private:
    std::vector<Token> tokens;
    uint32_t current = 0;
    Ast_TranslationUnit* root = nullptr;
};

#endif // !PARSER_H