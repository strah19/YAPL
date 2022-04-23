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

struct Object {
    Object(double value) : value(value) { }
    Object() { }
    union {
        double value;
    };
};

class Interpreter {
public:
    Interpreter() = default;
    ~Interpreter() = default;

    void run(Ast_TranslationUnit* unit);
private:
    double evaluate_expression(Ast_Expression* expression);
    void define(const char* name, Object object);
    RunTimeError runtime_error(const char* msg);

    std::map<std::string, Object> globals; 
};

#endif // !INTERPRETER_H