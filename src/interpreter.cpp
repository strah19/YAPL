/**
 * @file interpreter.cpp
 * @author strah19
 * @date April 23 2022
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
 * Defines the languages interpreter.
 */
#include "interpreter.h"
#include "err.h"

void Interpreter::run(Ast_TranslationUnit* unit) {
    for (int i = 0; i < unit->declerations.size(); i++) {
        if (unit->declerations[i]->type == AST_STATEMENT) {
            auto statement = AST_CAST(Ast_Statement, unit->declerations[i]);
            if (statement->print)
                printf("%f", evaluate_expression(statement->expression));
        }
        else if (unit->declerations[i]->type == AST_ASSIGNMENT) {
            auto assignment = AST_CAST(Ast_Assignment, unit->declerations[i]);
            define(assignment->ident->id, Object(0));
            if (assignment->expression) 
                globals[assignment->ident->id].value = evaluate_expression(assignment->expression);
        }
    }
}

RunTimeError Interpreter::runtime_error(const char* msg) {
    report_error("runtime error: %s.\n", msg);
    return RunTimeError(msg);
}

void Interpreter::define(const char* name, Object object) {
    if (globals.find(name) == globals.end()) {
        globals[name] = object;
    }
}

double Interpreter::evaluate_expression(Ast_Expression* expression) {
    switch (expression->type) {
    case AST_BINARY: {
        auto bin = AST_CAST(Ast_BinaryExpression, expression);
        auto left = evaluate_expression(bin->left);
        auto right = evaluate_expression(bin->right);
        
        switch (bin->op) {
        case AST_OPERATOR_ADD:            return left + right;
        case AST_OPERATOR_MULTIPLICATIVE: return left * right;
        case AST_OPERATOR_SUB:            return left - right;
        case AST_OPERATOR_DIVISION:       return left / right;
        }  
        break;
    }
    case AST_PRIMARY: {
        auto primary = AST_CAST(Ast_PrimaryExpression, expression);
        if (!primary->nested && !primary->ident)
            return primary->int_const;
        else if (primary->nested)
            return evaluate_expression(primary->nested);
        else if (primary->ident) 
            return globals[primary->ident->id].value;
        break;
    }
    case AST_UNARY:
        auto unary = AST_CAST(Ast_UnaryExpression, expression);
        auto value = evaluate_expression(unary->next);
        switch (unary->op) {
        case AST_UNARY_MINUS: return -value;
        default: return value;
        }
    }

    return 0.0;
}