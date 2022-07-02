#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "lexer.h"
#include "ast.h"
#include "object.h"
#include "environment.h"

#include <utility>

struct RunTimeError {
    RunTimeError() = default;
    RunTimeError(Ast ast, const char* msg) : ast(ast), msg(msg) { }

    const char* msg;
    Ast ast;
};

class Interpreter {
public:
    Interpreter() = default;
    ~Interpreter() = default;

    void interpret(Ast_TranslationUnit* unit);

    static RunTimeError construct_runtime_error(Ast ast, const char* msg);
    static void         print_runtime_error(const RunTimeError& runtime_error);
private:
    void   execute(Ast_Decleration* decleration);
    void   scope(Ast_Decleration* decleration);
    Object execute_function(Ast_FuncDecleration* function, Ast_FunctionCall* call);

    Object assignment(Ast_Assignment* assign);
    void   print_statement(Ast_PrintStatement* print);
    void   variable_decleration(Ast_VarDecleration* decleration);
    void   function_decleration(Ast_FuncDecleration* func);

    void if_statement(Ast_ConditionalStatement* conditional);
    bool conditional_statement(Ast_ConditionalStatement* conditional);

    void while_loop(Ast_WhileLoop* loop);   

    Object evaluate_expression(Ast_Expression* expression);
    Object evaluate_unary(Ast_UnaryExpression* unary);
    Object evaluate_primary(Ast_PrimaryExpression* primary);
    Object evaluate_binary(Ast_BinaryExpression* binary);
    Object evaluate_assignment(Ast_Assignment* assign);
    Object evaluate_equal(Ast_Assignment* assign);
    Object evaluate_function_call(Ast_FunctionCall* call);

    int convert_to_interpreter_type(int ast_type);
private:
    Environment environment;
    Environment* current_environment;
};

#endif // !INTERPRETER_H