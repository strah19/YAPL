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

static uint32_t current_line_interpreting = 0;

Object Object::operator+(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER: return this->number + obj.number;
    case BOOLEAN: return Object(this->boolean + obj.boolean, BOOLEAN);
    case STRING: return strcat((char*) this->str, (char*) obj.str);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator-(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER: return this->number - obj.number;
    case BOOLEAN: return Object(this->boolean - obj.boolean, BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator*(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER: return this->number * obj.number;
    case BOOLEAN: return Object(this->boolean * obj.boolean, BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator/(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    Interpreter::division_zero(obj);
    switch (this->type) {
    case NUMBER: return this->number / obj.number;
    case BOOLEAN: return Object(this->boolean / obj.boolean, BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator==(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER:  return Object(this->number == obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean == obj.boolean, BOOLEAN);
    case STRING:  return Object(strcmp(this->str, obj.str), BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator!=(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER:  return Object(this->number != obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean != obj.boolean, BOOLEAN);
    case STRING:  return Object(!strcmp(this->str, obj.str), BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator>(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER:  return Object(this->number > obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean > obj.boolean, BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator<(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER:  return Object(this->number < obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean < obj.boolean, BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator>=(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER:  return Object(this->number >= obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean >= obj.boolean, BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator<=(const Object& obj) {
    Interpreter::check_operators(*this, obj);
    switch (this->type) {
    case NUMBER:  return Object(this->number <= obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean <= obj.boolean, BOOLEAN);
    default: throw Interpreter::runtime_error("unknown type found.");
    }
}

Object Object::operator-() {
    switch (this->type) {
        case NUMBER: return -this->number;
        default: throw Interpreter::runtime_error("type cannot be negated.");  
    }
}

void Interpreter::interpret(Ast_TranslationUnit* unit) {
    current_environment = &environment;
    try {
        current_line_interpreting = unit->declerations[0]->line;
        for (int i = 0; i < unit->declerations.size(); i++)
            execute(unit->declerations[i]);
    }
    catch (RunTimeError error) {
        //backlog errors?
    }
}

void Interpreter::execute(Ast_Decleration* decleration) {
    if (decleration->type == AST_EXPRESSION_STATEMENT) {
        auto expression_statement = AST_CAST(Ast_ExpressionStatement, decleration);
        if (expression_statement->expression->type == AST_ASSIGNMENT) {
            auto assign = AST_CAST(Ast_Assignment, expression_statement->expression);
            if (!current_environment->get(assign->id).mutability)
                throw runtime_error("Can not have assignment on constant variable.");
            assignment(expression_statement->expression); 
        }
    }
    else if (decleration->type == AST_SCOPE) {
        Environment* previous = current_environment;
        current_environment->next = new Environment;
        current_environment = current_environment->next;
        current_environment->previous = previous;
        auto scope = AST_CAST(Ast_Scope, decleration);
        for (int i = 0; i < scope->declerations.size(); i++)
            execute(scope->declerations[i]);
        delete current_environment;
        current_environment = previous;
    }
    else if (decleration->type == AST_PRINT) 
        print_statement(AST_CAST(Ast_PrintStatement, decleration));
    else if (decleration->type == AST_VAR_DECLERATION) 
        variable_decleration(AST_CAST(Ast_VarDecleration, decleration));
    else if (decleration->type == AST_IF) 
        conditional_statement(AST_CAST(Ast_IfStatement, decleration));
    current_line_interpreting = decleration->line;
}

void Interpreter::variable_decleration(Ast_VarDecleration* decleration) {
    current_environment->define(decleration->ident, Object());
    if (decleration->expression) {
        Object obj;
        if (decleration->expression)
            obj = evaluate_expression(decleration->expression);
        else
            obj.type = convert_to_interpreter_type(decleration->type_value);
        if (obj.type != convert_to_interpreter_type(decleration->type_value))
            throw runtime_error("types do not match.");
        obj.mutability = (decleration->specifiers & AST_SPECIFIER_CONST) ? false : true;
        current_environment->define(decleration->ident, obj);
    }
    else if ((decleration->specifiers & AST_SPECIFIER_CONST))
        throw runtime_error("constant variable must have an expression.");
}

void Interpreter::conditional_statement(Ast_IfStatement* conditional) {
    Ast_ConditionalStatement* current = conditional;
    while (current) {
        if ((current->type == AST_IF || current->type == AST_ELIF) && not_else_statement(current))
            break;
        else if (current->type == AST_ELSE) {
            execute(current->scope);
            break;
        }
        current = current->next;
    }
}

bool Interpreter::not_else_statement(Ast_ConditionalStatement* conditional) {
    Object obj = evaluate_expression(conditional->condition);
    if (obj.type == BOOLEAN && obj.boolean) {
        execute(conditional->scope);
        return true;
    }
    return false;
}

void Interpreter::print_statement(Ast_PrintStatement* print) {
    for (int i = 0; i < print->expressions.size(); i++) {
        Object obj = evaluate_expression(print->expressions[i]);
        switch (obj.type) {
        case NUMBER:
            printf("%f", obj.number);
            break;
        case BOOLEAN:
            printf("%d", obj.boolean);
            break;
        case STRING:
            printf("%s", obj.str);
            break;
        }
    }
    printf("\n");
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
    report_error("runtime error on line %d: %s.\n", current_line_interpreting, msg);
    return RunTimeError(current_line_interpreting, msg);
}

Object Interpreter::evaluate_expression(Ast_Expression* expression) {
    switch (expression->type) {
    case AST_BINARY: {
        auto bin = AST_CAST(Ast_BinaryExpression, expression);

        auto left = evaluate_expression(bin->left);
        auto right = evaluate_expression(bin->right);

        switch (bin->op) {
        case AST_OPERATOR_ADD:                   return left + right;
        case AST_OPERATOR_MULTIPLICATIVE:        return left * right; 
        case AST_OPERATOR_SUB:                   return left - right;
        case AST_OPERATOR_DIVISION:              return left / right;
        case AST_OPERATOR_COMPARITIVE_EQUAL:     return left == right;
        case AST_OPERATOR_COMPARITIVE_NOT_EQUAL: return left != right;
        case AST_OPERATOR_GT:                    return left > right;
        case AST_OPERATOR_LT:                    return left < right;
        case AST_OPERATOR_GTE:                   return left >= right;
        case AST_OPERATOR_LTE:                   return left <= right;
        }  
 
        break;
    }
    case AST_PRIMARY: {
        auto primary = AST_CAST(Ast_PrimaryExpression, expression);

        switch (primary->type_value) {
        case AST_NESTED:  return evaluate_expression(primary->nested);
        case AST_ID:      return current_environment->get(primary->ident);
        case AST_FLOAT:   return primary->float_const;
        case AST_STRING:  return primary->string;
        case AST_BOOLEAN: return Object(primary->boolean, BOOLEAN);
        }
    
        break;
    }
    case AST_UNARY:
        auto unary = AST_CAST(Ast_UnaryExpression, expression);
        Object value = evaluate_expression(unary->next);
        switch (unary->op) {
        case AST_UNARY_MINUS: return -value;
        default: return value;
        }
    }

    return Object();
}

void Interpreter::division_zero(const Object& right) {
    if (right.type == NUMBER && right.number != 0) return;
    if (right.type == BOOLEAN && right.boolean != 0) return;
    throw runtime_error("cannot divide by 0");
}

void Interpreter::check_operators(const Object& left, const Object& right) {
    if (left.type == right.type) return;
    throw runtime_error("operands must be numbers");
}

int Interpreter::convert_to_interpreter_type(int ast_type) {
    switch (ast_type) {
    case AST_FLOAT:   return NUMBER;
    case AST_STRING:  return STRING;
    case AST_BOOLEAN: return BOOLEAN;
    }
    return NONE;
}

void Environment::define(const char* name, Object object) {
    values[name] = object;
}

Object Environment::get(const char* name) {
    if (values.find(name) == values.end() && previous != nullptr)
        return previous->get(name); 

    if (values.find(name) == values.end()) 
        throw Interpreter::runtime_error("Undefined variable");

    return values[name];
}

void Environment::must_be_defined(const char* name) {
    if (values.find(name) != values.end())  return;
    throw Interpreter::runtime_error("Undefined variable in assignment");
}

bool Environment::check(const char* name) {
    return (values.find(name) == values.end());
}