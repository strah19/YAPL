#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "lexer.h"
#include "ast.h"

#include <unordered_map>

struct RunTimeError {
    RunTimeError() = default;
    RunTimeError(const char* msg) : msg(msg) { }

    const char* msg;
};

enum {
    NUMBER,
    STRING,
    NONE,
};

struct Object {
    Object(double number) : number(number), type(NUMBER) { }
    Object(const char* str) : str(str), type(STRING) { }
    Object() { }
    union {
        double number;
        const char* str;
    };
    int type = NONE;
};

struct Environment {
public:
    Environment() = default;
    ~Environment() = default;

    void must_be_defined(const char* name);
    void define(const char* name, Object object);
    bool check(const char* name);
    Object get(const char* name);

    std::unordered_map<std::string, Object> values; 
    Environment* next = nullptr;
    Environment* previous = nullptr;
};

class Interpreter {
public:
    Interpreter() = default;
    ~Interpreter() = default;

    void interpret(Ast_TranslationUnit* unit);
    static RunTimeError runtime_error(const char* msg);
    void check_operators(const Object& left, const Object& right);
    void assignment(Ast_Expression* root);
    void division_zero(const Object& right);
private:
    void execute(Ast_Decleration* decleration);
    Object evaluate_expression(Ast_Expression* expression);
    void print_statement(Ast_PrintStatement* print);
    void variable_decleration(Ast_VarDecleration* decleration);
private:
    Environment environment;
    Environment* current_environment;
};

#endif // !INTERPRETER_H