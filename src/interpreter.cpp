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
    current_environment = &environment;
    try {
        for (int i = 0; i < unit->declerations.size(); i++) {
            execute(unit->declerations[i]);
        }
    }
    catch (RunTimeError error) {
        runtime_error(error.msg);
    }
}

void Interpreter::execute(Ast_Decleration* decleration) {
    if (decleration->type == AST_EXPRESSION_STATEMENT) {
        auto expression_statement = AST_CAST(Ast_ExpressionStatement, decleration);
        if (expression_statement->expression->type == AST_ASSIGNMENT) 
            assignment(expression_statement->expression); 
    }
    else if (decleration->type == AST_SCOPE) {
        Environment* previous = current_environment;
        current_environment->next = new Environment;
        current_environment = current_environment->next;
        current_environment->previous = previous;
        auto scope = AST_CAST(Ast_Scope, decleration);
        for (int i = 0; i < scope->declerations.size(); i++) {
            execute(scope->declerations[i]);
        }
        delete current_environment;
        current_environment = previous;
    }
    else if (decleration->type == AST_PRINT) 
        print_statement(AST_CAST(Ast_PrintStatement, decleration));
    else if (decleration->type == AST_VAR_DECLERATION) 
        variable_decleration(AST_CAST(Ast_VarDecleration, decleration));
}

void Interpreter::variable_decleration(Ast_VarDecleration* decleration) {
    current_environment->define(decleration->ident, Object());
    if (decleration->expression) 
        current_environment->define(decleration->ident, evaluate_expression(decleration->expression));
}

void Interpreter::print_statement(Ast_PrintStatement* print) {
    Object obj = evaluate_expression(print->expression);
    switch (obj.type) {
    case NUMBER:
        printf("%f\n", obj.number);
        break;
    case STRING:
        printf("%s\n", obj.str);
        break;
    }   
}

void Interpreter::assignment(Ast_Expression* root) {
    if (AST_CAST(Ast_Assignment, root)->expression->type == AST_ASSIGNMENT)
        assignment(AST_CAST(Ast_Assignment, root)->expression);
    auto assignment = AST_CAST(Ast_Assignment, root);
    current_environment->must_be_defined(assignment->id);

    Object obj;
    if (assignment->expression->type == AST_ASSIGNMENT) {
        auto id = new Ast_PrimaryExpression(AST_CAST(Ast_Assignment, assignment->expression)->id);
        obj = evaluate_expression(id);
        delete id;
    }
    else    
        obj = evaluate_expression(assignment->expression);
    current_environment->define(assignment->id, obj);
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
        case AST_ID:     return current_environment->get(primary->ident);
        case AST_FLOAT: return primary->float_const;
        case AST_INT: return primary->int_const;
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
    if (values.find(name) == values.end() && previous != nullptr)
        return previous->get(name); 

    if (values.find(name) == values.end()) 
        throw RunTimeError("Undefined variable");

    return values[name];
}

void Environment::must_be_defined(const char* name) {
    if (values.find(name) != values.end())  return;
    throw RunTimeError("Undefined variable in assignment");
}

bool Environment::check(const char* name) {
    return (values.find(name) == values.end());
}