/**
 * @file ast.cpp
 * @author strah19
 * @date April 16 2022
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
 * Mostly destructors for the Ast objects.
 */

#include "ast.h"
#include "mem.h"

static MemoryManager memory_manager;

void* Ast::operator new (size_t size) {
    return memory_manager.allocate(size);
}

void* Ast::operator new[] (size_t size) {
    return memory_manager.allocate(size);
}

void Ast::operator delete (void* chunk) {
    memory_manager.free(chunk);
}

void Ast::operator delete[] (void* chunk) {
    memory_manager.free(chunk);
}