#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "ast.h"
#include "object.h"
#include <map>

enum {
    EN_CONDITION,
    EN_LOOP,
    EN_FUNC,
    EN_NONE
};

enum {
    EN_ERROR_NONE,
    EN_ERROR_UNDEFINED_VAR,
    EN_ERROR_UNDEFINED_FUNC,
};

static std::map<int, const char*> EN_ERROR_MESSAGES {
    { EN_ERROR_NONE, "No error found in object" },
    { EN_ERROR_UNDEFINED_VAR, "Undefined variable" },
    { EN_ERROR_UNDEFINED_FUNC, "Undefined function" }
};

struct Environment {
    Environment() = default;
    ~Environment() = default;

    int var_is_defined(const char* name);
    void var_define(const char* name, Object object);
    int var_update(const char* name, Object object);
    bool var_found(const char* name);
    Object var_get(const char* name);

    int func_is_defined(const char* name);
    void func_define(const char* name, Ast_FuncDecleration* func);
    bool func_found(const char* name);
    Ast_FuncDecleration* func_get(const char* name);

    static bool found_errors(int error);
    
    std::map<std::string, Ast_FuncDecleration*> functions;
    std::map<std::string, Object> values; 
    Environment* next = nullptr;
    Environment* previous = nullptr;
    int type = EN_NONE;
};

#endif // !ENVIRONMENT_H