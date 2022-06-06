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
    EN_ERROR_UNDEFINED_VAR
};

static std::map<int, const char*> EN_ERROR_MESSAGES {
    { EN_ERROR_NONE, "No error found in object" },
    { EN_ERROR_UNDEFINED_VAR, "Undefined variable" }
};

struct Environment {
    Environment() = default;
    ~Environment() = default;

    int is_defined(const char* name);
    void define(const char* name, Object object);
    int update(const char* name, Object object);
    bool found(const char* name);
    static bool found_errors(int error);
    Object get(const char* name);

    std::map<std::string, Object> values; 
    Environment* next = nullptr;
    Environment* previous = nullptr;
    int type = EN_NONE;
};

#endif // !ENVIRONMENT_H