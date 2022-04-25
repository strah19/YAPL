#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "lexer.h"
#include "ast.h"

#include <map>

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

    void define(const char* name, Object object);
    Object get(const char* name);
private:
    std::map<std::string, Object> values; 
};

class Interpreter {
public:
    Interpreter() = default;
    ~Interpreter() = default;

    void interpret(Ast_TranslationUnit* unit);
    static RunTimeError runtime_error(const char* msg);
    void check_operators(const Object& left, const Object& right);
    void division_zero(const Object& right);
private:
    Object evaluate_expression(Ast_Expression* expression);
    Environment environment;
};

#endif // !INTERPRETER_H