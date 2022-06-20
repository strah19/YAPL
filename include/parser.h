#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "lexer.h"
#include "ast.h"

#include <map>

struct ParserError {
    Token* token = nullptr;

    ParserError() = default;
    ParserError(Token* token) : token(token) { }
};

class Parser {
public:
    Parser() = default;
    Parser(Lexer* lexer);
    ~Parser();

    void parse();

    Token* peek(int index = 0);
    Token* advance();

    ParserError parser_error(Token* token, const char* msg);
    Token* consume(int type, const char* msg);
    bool match(int type);
    bool check(int type);
    bool is_end();

    Ast_TranslationUnit* translation_unit() { return root; }
private:
    Ast_Expression* expression();
    Ast_Expression* logical();
    Ast_Expression* equality();
    Ast_Expression* bitwise();
    Ast_Expression* comparison();
    Ast_Expression* shifts();
    Ast_Expression* term();
    Ast_Expression* factor();
    Ast_Expression* unary();
    Ast_Expression* primary();

    Ast_ExpressionStatement*  expression_statement();
    Ast_PrintStatement*       print_statement();
    Ast_Expression*           assignment();
    Ast_VarDecleration*       var_decleration(bool semi = true);
    Ast_FuncDecleration*      func_decleration();
    std::vector<Ast_VarDecleration*> func_args();
    Ast_Decleration*          decleration();
    Ast_Statement*            statement(); 
    Ast_Scope*                scope();

    Ast_ReturnStatement*      return_statement();
    Ast_ConditionalStatement* conditional_statement();
    Ast_IfStatement*          if_statement();
    Ast_ElifStatement*        elif_statement();
    Ast_ElseStatement*        else_statement();
    Ast_WhileLoop*            while_loop();

    Ast* default_ast(Ast* ast);

    void synchronize();
    int token_to_ast(Token* token);
    int token_to_ast_unary(Token* token);
    int token_to_equal(Token* token);
private:
    std::vector<Token> tokens;
    uint32_t current = 0;
    Ast_TranslationUnit* root = nullptr;
    const char* filepath = nullptr;
};

#endif // !PARSER_H