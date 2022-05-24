#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "lexer.h"
#include "ast.h"

#include <utility>
#include <unordered_map>

struct RunTimeError {
    RunTimeError() = default;
    RunTimeError(uint32_t line, const char* msg) : line(line), msg(msg) { }

    const char* msg;
    uint32_t line;
};

enum {
    NUMBER,
    STRING,
    BOOLEAN,
    NONE,
};

struct Object {
    Object(double number) : number(number), type(NUMBER) { }
    Object(const char* str) : str(str), type(STRING) { }
    Object(bool boolean, int type) : boolean(boolean), type(type) { }
    Object() { }

    union {
        double number;
        const char* str;
        bool boolean;
    };
    int type = NONE;
    bool mutability = true;

    Object operator+(const Object& obj);
    Object operator-(const Object& obj);
    Object operator*(const Object& obj);
    Object operator/(const Object& obj);
    Object operator-();
    Object operator==(const Object& obj);
    Object operator!=(const Object& obj);
    Object operator>(const Object& obj);
    Object operator<(const Object& obj);
    Object operator>=(const Object& obj);
    Object operator<=(const Object& obj);
    Object operator&&(const Object& obj);
    Object operator||(const Object& obj);
    Object operator!();
};

enum {
    EN_CONDITION,
    EN_LOOP,
    EN_FUNC,
    EN_NONE
};

struct Environment {
    Environment() = default;
    ~Environment() = default;

    bool must_be_defined(const char* name);
    void define(const char* name, Object object);
    bool update(const char* name, Object object);
    bool check(const char* name);
    Object get(const char* name);

    std::unordered_map<std::string, Object> values; 
    Environment* next = nullptr;
    Environment* previous = nullptr;
    int type = EN_NONE;
};

class Interpreter {
public:
    Interpreter() = default;
    ~Interpreter() = default;

    void interpret(Ast_TranslationUnit* unit);

    static RunTimeError runtime_error(const char* msg);
    static void check_operators(const Object& left, const Object& right);
    static void division_zero(const Object& right);
private:
    void assignment(Ast_Expression* root);
    void execute(Ast_Decleration* decleration);
    Object evaluate_expression(Ast_Expression* expression);
    void print_statement(Ast_PrintStatement* print);
    void variable_decleration(Ast_VarDecleration* decleration);
    int convert_to_interpreter_type(int ast_type);
    void if_statement(Ast_IfStatement* conditional);
    bool not_else_statement(Ast_ConditionalStatement* conditional);
    void conditional_controller(Ast_ConditionalController* controller);
    void while_loop(Ast_WhileLoop* loop);   
    void execute_conditions(Ast_Scope* scope); 
    void execute_loops(Ast_Scope* scope); 
private:
    Environment environment;
    Environment* current_environment;
    int backtrack_out_env = EN_NONE;
};

#endif // !INTERPRETER_H