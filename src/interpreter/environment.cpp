/**
 * @file environment.cpp
 * @author strah19
 * @date May 31 2022
 * @version 1.0
 * 
 * @section LICENSE
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License as published 
 * by the Free Software Foundation.
 * 
 * @section DESCRIPTION
 * 
 * Defines the environment for variables in interpreter.
 */

#include "environment.h"
#include "interpreter.h"

void Environment::var_define(const char* name, Object object) {
    values[name] = object;
}

int Environment::var_update(const char* name, Object object) {
    if (var_found(name)) {
        values[name] = object; 
        return EN_ERROR_NONE;
    }
    if (!var_found(name) && previous) {
        if (!found_errors(previous->var_update(name, object)))
            return EN_ERROR_NONE;
    }

    if (!var_found(name)) 
        return EN_ERROR_UNDEFINED_VAR;
    return EN_ERROR_NONE;
}

Object Environment::var_get(const char* name) {
    if (!var_found(name) && previous)
        return previous->var_get(name); 

    if (!var_found(name)) 
        return Object(OBJ_ERROR_UNDEFINED_VAR);

    return values[name];
}

int Environment::var_is_defined(const char* name) {
    if (var_found(name))
        return EN_ERROR_NONE;

    if (!var_found(name) && previous) {
        if (!found_errors(previous->var_is_defined(name)))
            return EN_ERROR_NONE;
    }
    
    if (!var_found(name)) 
        return EN_ERROR_UNDEFINED_VAR;
    return EN_ERROR_NONE;
}

bool Environment::var_found(const char* name) {
    return (values.find(name) != values.end());
}

int Environment::func_is_defined(const char* name) {
    if (func_found(name))
        return EN_ERROR_NONE;

    if (!func_found(name) && previous) {
        if (!found_errors(previous->func_is_defined(name)))
            return EN_ERROR_NONE;
    }
    
    if (!func_found(name)) 
        return EN_ERROR_UNDEFINED_FUNC;
    return EN_ERROR_NONE;
}

void Environment::func_define(const char* name, Ast_FuncDecleration* func) {
    functions[name] = func;
}

bool Environment::func_found(const char* name) {
    return (functions.find(name) != functions.end());
}

Ast_FuncDecleration* Environment::func_get(const char* name) {
    if (!func_found(name) && previous)
        return previous->func_get(name); 

    if (!func_found(name)) 
        return nullptr;

    return functions[name];
}

bool Environment::found_errors(int error) {
    return (error != EN_ERROR_NONE);
}
