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

class Environment {
public:
    Environment() = default;
    ~Environment() = default;

    void must_be_defined(const char* name);
    void define(const char* name, Object object);
    Object get(const char* name);
private:
    std::unordered_map<std::string, Object> values; 
    Environment* next = nullptr;
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
    Object evaluate_expression(Ast_Expression* expression);
    Environment environment;
};

#endif // !INTERPRETER_H