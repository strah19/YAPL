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

void Interpreter::interpret(Ast_TranslationUnit* unit) {
    try {
        for (int i = 0; i < unit->declerations.size(); i++) {
            if (unit->declerations[i]->type == AST_STATEMENT) {
                auto statement = AST_CAST(Ast_Statement, unit->declerations[i]);
                if (statement->print) {
                    Object obj = evaluate_expression(statement->expression);
                    switch (obj.type) {
                    case NUMBER:
                        printf("%f\n", obj.number);
                        break;
                    case STRING:
                        printf("%s\n", obj.str);
                        break;
                    }
                }
            }
            else if (unit->declerations[i]->type == AST_ASSIGNMENT) {
                auto assignment = AST_CAST(Ast_Assignment, unit->declerations[i]);
                environment.define(assignment->ident, Object());
                if (assignment->expression) 
                    environment.define(assignment->ident, evaluate_expression(assignment->expression));
            }
        }
    }
    catch (RunTimeError error) {
        runtime_error(error.msg);
    }
}

RunTimeError Interpreter::runtime_error(const char* msg) {
    report_error("runtime error: %s.\n", msg);
    return RunTimeError(msg);
}

Object Interpreter::evaluate_expression(Ast_Expression* expression) {
    switch (expression->type) {
    case AST_BINARY: {
        auto bin = AST_CAST(Ast_BinaryExpression, expression);

        auto left = evaluate_expression(bin->left);
        auto right = evaluate_expression(bin->right);
        
        if (bin->op != AST_OPERATOR_ADD)
            check_operators(left, right);

        switch (bin->op) {
        case AST_OPERATOR_ADD:            return left.number + right.number;
        case AST_OPERATOR_MULTIPLICATIVE: return left.number * right.number;
        case AST_OPERATOR_SUB:            return left.number - right.number;
        case AST_OPERATOR_DIVISION: 
            division_zero(right); 
            return left.number / right.number;
        }  
        break;
    }
    case AST_PRIMARY: {
        auto primary = AST_CAST(Ast_PrimaryExpression, expression);

        switch (primary->type_value) {
        case AST_NESTED: return evaluate_expression(primary->nested);
        case AST_ID: return environment.get(primary->ident);
        case AST_NUMBER: return primary->int_const;
        case AST_STRING: return primary->string;
        }
    
        break;
    }
    case AST_UNARY:
        auto unary = AST_CAST(Ast_UnaryExpression, expression);
        Object value = evaluate_expression(unary->next);
        switch (unary->op) {
        case AST_UNARY_MINUS: return -value.number;
        default: return value;
        }
    }

    return 0.0;
}

void Interpreter::division_zero(const Object& right) {
    if (right.number != 0) return;
    throw RunTimeError("cannot divide by 0");
}

void Interpreter::check_operators(const Object& left, const Object& right) {
    if (left.type == NUMBER && right.type == NUMBER) return;
    throw RunTimeError("operands must be numbers");
}

void Environment::define(const char* name, Object object) {
    values[name] = object;
}

Object Environment::get(const char* name) {
    if (values.find(name) == values.end()) 
        throw RunTimeError("Undefined variable");
    return values[name];
}