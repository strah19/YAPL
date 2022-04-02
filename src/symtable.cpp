/**
 * @file symtable.cpp
 * @author strah19
 * @date March 27 2022
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
 * Defines the symbol table. This table is used for things like identifiers, symbols, etc.
 */

#include "symtable.h"

/**
 * 
 * 
 */
int SymTable::get_index(const char* name) {
    for(int i = 0; i < table.size(); i++) {
        if (name[0] == table[i].name[0]) {
            if (strcmp(name, table[i].name) == 0) {
                return i;
            }
        }
    }

    return -1;
}

/**
 * 
 * 
 */
Entry* SymTable::insert(const char* name, int type) {
    Entry* e = look_up_type(name, type);
    if (e == nullptr) {
        table.push({name, type});
        return &table[table.size() - 1];
    }
    return e;
}

/**
 * 
 * 
 */
Entry* SymTable::look_up(const char* name) {
    for(int i = 0; i < table.size(); i++) {
        if (name[0] == table[i].name[0]) {
            if (strcmp(name, table[i].name) == 0) {
                return &table[i];
            }
        }
    }
    return nullptr;
}

/**
 * 
 * 
 */
Entry* SymTable::look_up_type(const char* name, int type) {
    for(int i = 0; i < table.size(); i++) {
        if (name[0] == table[i].name[0] && table[i].type == type) {
            if (strcmp(name, table[i].name) == 0) {
                return &table[i];
            }
        }
    }
    return nullptr;
}

/**
 * 
 * 
 */
Entry* SymTable::look_up_by_type(int type) {
    for(int i = 0; i < table.size(); i++) {
        if (table[i].type == type) {
             return &table[i];
        }
    }
    return nullptr;
}