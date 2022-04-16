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

Ast_TranslationUnit::~Ast_TranslationUnit() {
    delete expression;
}

Ast_UnaryExpression::~Ast_UnaryExpression() {
    delete next;
}

Ast_BinaryExpression::~Ast_BinaryExpression() {
    delete left;
    delete right;
}

Ast_PrimaryExpression::~Ast_PrimaryExpression() {
    delete ident;
    delete nested;
}