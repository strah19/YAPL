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

void Environment::define(const char* name, Object object) {
    values[name] = object;
}

int Environment::update(const char* name, Object object) {
    if (found(name)) {
        values[name] = object; 
        return EN_ERROR_NONE;
    }
    if (!found(name) && previous) {
        if (!found_errors(previous->update(name, object)))
            return EN_ERROR_NONE;
    }

    if (!found(name)) {
        return EN_ERROR_UNDEFINED_VAR;
    }
    return EN_ERROR_NONE;
}

Object Environment::get(const char* name) {
    if (!found(name) && previous)
        return previous->get(name); 

    if (!found(name)) 
        return Object(OBJ_ERROR_UNDEFINED_VAR);

    return values[name];
}

int Environment::is_defined(const char* name) {
    if (found(name))
        return EN_ERROR_NONE;

    if (!found(name) && previous) {
        if (!found_errors(previous->is_defined(name)))
            return EN_ERROR_NONE;
    }
    
    if (!found(name)) 
        return EN_ERROR_UNDEFINED_VAR;
    return EN_ERROR_NONE;
}

bool Environment::found(const char* name) {
    return (values.find(name) != values.end());
}

bool Environment::found_errors(int error) {
    return (error != EN_ERROR_NONE);
}
