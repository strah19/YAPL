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

#define OBJECT_ERRORS(ast, obj) if (obj.found_errors()) throw Interpreter::construct_runtime_error(*ast, OBJ_ERROR_MESSAGES[obj.error]);
#define ENVIRONMENT_ERRORS(ast, err) if (Environment::found_errors(err)) throw Interpreter::construct_runtime_error(*ast, EN_ERROR_MESSAGES[err]);

static bool is_if_or_elif(int type);

void Interpreter::interpret(Ast_TranslationUnit* unit) {
    current_environment = &environment;
    try {
        for (int i = 0; i < unit->declerations.size(); i++)
            execute(unit->declerations[i]);
    }
    catch (RunTimeError error) {
        print_runtime_error(error);
    }
}

void Interpreter::execute(Ast_Decleration* decleration) {
    if (decleration->type == AST_EXPRESSION_STATEMENT) 
        evaluate_expression(AST_CAST(Ast_ExpressionStatement, decleration)->expression);
    else if (decleration->type == AST_SCOPE)
        scope(decleration);
    else if (decleration->type == AST_PRINT) 
        print_statement(AST_CAST(Ast_PrintStatement, decleration));
    else if (decleration->type == AST_VAR_DECLERATION) 
        variable_decleration(AST_CAST(Ast_VarDecleration, decleration));
    else if (decleration->type == AST_IF) 
        if_statement(AST_CAST(Ast_IfStatement, decleration));
    else if (decleration->type == AST_WHILE)
        while_loop(AST_CAST(Ast_WhileLoop, decleration)); 
    else if (decleration->type == AST_FUNC_DECLERATION) 
        function_decleration(AST_CAST(Ast_FuncDecleration, decleration));
}

void Interpreter::function_decleration(Ast_FuncDecleration* func) {
    if (current_environment->func_is_defined(func->ident) == EN_ERROR_NONE) 
        throw construct_runtime_error(*func, "Function can only be defined once");
    current_environment->func_define(func->ident, func);
}

void Interpreter::scope(Ast_Decleration* decleration) {
    Environment* previous = current_environment;
    current_environment->next = new Environment;
    current_environment = current_environment->next;
    current_environment->previous = previous;
    auto scope = AST_CAST(Ast_Scope, decleration);

    for (int i = 0; i < scope->declerations.size(); i++)
        execute(scope->declerations[i]);
    current_environment = current_environment->previous;
    delete current_environment->next;
}

void Interpreter::while_loop(Ast_WhileLoop* loop) {
    Object obj = evaluate_expression(loop->condition);
    OBJECT_ERRORS(loop, obj);

    while (obj.type == BOOLEAN && obj.boolean) {
         execute(loop->scope);

        obj = evaluate_expression(loop->condition);
        OBJECT_ERRORS(loop, obj);
    }
}

void Interpreter::if_statement(Ast_IfStatement* conditional) {
    Ast_ConditionalStatement* current = conditional;
    while (current) {
        if (is_if_or_elif(current->type) && conditional_statement(current))
            break;
        else if (current->type == AST_ELSE) {
            execute(conditional->scope);
            break;
        }
        current = current->next;
    }
}

bool is_if_or_elif(int type) {
    return (type == AST_IF || type == AST_ELIF);
}

bool Interpreter::conditional_statement(Ast_ConditionalStatement* conditional) {
    Object obj = evaluate_expression(conditional->condition);
    OBJECT_ERRORS(conditional, obj);

    if (obj.type == BOOLEAN && obj.boolean) {
        execute(conditional->scope);
        return true;
    }
    return false;
}

void Interpreter::variable_decleration(Ast_VarDecleration* decleration) {
    current_environment->var_define(decleration->ident, Object());
    if (decleration->expression) {
        Object obj;
        if (decleration->expression) {
            obj = evaluate_expression(decleration->expression);
            OBJECT_ERRORS(decleration->expression, obj);   
            if (convert_to_interpreter_type(decleration->type_value) != obj.type) {
                throw construct_runtime_error(*decleration, OBJ_ERROR_MESSAGES[OBJ_ERROR_WRONG_TYPE]);
            }        
        }
        else
            obj.type = convert_to_interpreter_type(decleration->type_value);
            
        obj.mutability = (decleration->specifiers & AST_SPECIFIER_CONST) ? false : true;
        current_environment->var_define(decleration->ident, obj);
    }
    else if ((decleration->specifiers & AST_SPECIFIER_CONST)) throw construct_runtime_error(*decleration, "constant variable must have an expression.");
}

void Interpreter::print_statement(Ast_PrintStatement* print) {
    for (int i = 0; i < print->expressions.size(); i++) {
        Object obj = evaluate_expression(print->expressions[i]);
        OBJECT_ERRORS(print, obj);             
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
        default:
            printf("(null)");
        }
    }
    printf("\n");
}

Object Interpreter::assignment(Ast_Assignment* assign) {
    if (assign->expression->type == AST_ASSIGNMENT)
        assignment(AST_CAST(Ast_Assignment, assign->expression));
    ENVIRONMENT_ERRORS(assign, current_environment->var_is_defined(assign->id));

    Object obj;
    if (assign->expression->type == AST_ASSIGNMENT) {
        auto id = new Ast_PrimaryExpression(AST_CAST(Ast_Assignment, assign->expression)->id);
        obj = evaluate_expression(id);
        OBJECT_ERRORS(assign, obj);
        delete id;
    }
    else 
        obj = evaluate_equal(assign);
    ENVIRONMENT_ERRORS(assign, current_environment->var_update(assign->id, obj));
    return obj;
}

Object Interpreter::evaluate_equal(Ast_Assignment* assign) {
    Object obj = current_environment->var_get(assign->id);
    OBJECT_ERRORS(assign, obj);

    switch(assign->equal_type) {
    case AST_EQUAL:          return evaluate_expression(assign->expression); 
    case AST_EQUAL_PLUS:     return obj + evaluate_expression(assign->expression); 
    case AST_EQUAL_MINUS:    return obj - evaluate_expression(assign->expression); 
    case AST_EQUAL_MULTIPLY: return obj * evaluate_expression(assign->expression); 
    case AST_EQUAL_DIVIDE:   return obj / evaluate_expression(assign->expression); 
    case AST_EQUAL_MOD:      return obj % evaluate_expression(assign->expression); 
    default:                 return obj;
    }
}

RunTimeError Interpreter::construct_runtime_error(Ast ast, const char* msg) {
    return RunTimeError(ast, msg);
}

void Interpreter::print_runtime_error(const RunTimeError& runtime_error) {
    report_runtime_error("In file '%s' on line %d: '%s'.\n", runtime_error.ast.file, runtime_error.ast.line, runtime_error.msg);
}

Object Interpreter::evaluate_expression(Ast_Expression* expression) {
    switch (expression->type) {
    case AST_BINARY:     return evaluate_binary(AST_CAST(Ast_BinaryExpression, expression));
    case AST_ASSIGNMENT: return evaluate_assignment(AST_CAST(Ast_Assignment, expression));
    case AST_PRIMARY:    return evaluate_primary(AST_CAST(Ast_PrimaryExpression, expression));
    case AST_UNARY:      return evaluate_unary(AST_CAST(Ast_UnaryExpression, expression));
    }

    return Object();
}

Object Interpreter::evaluate_unary(Ast_UnaryExpression* unary) {
    Object value = evaluate_expression(unary->next);
    switch (unary->op) {
    case AST_UNARY_MINUS:   return -value;
    case AST_UNARY_NOT:     return !value;
    case AST_UNARY_BIT_NOT: return ~value;
    default:                return value;
    }
    return value;
}

Object Interpreter::evaluate_primary(Ast_PrimaryExpression* primary) {
    switch (primary->type_value) {
    case AST_NESTED:  return evaluate_expression(primary->nested);
    case AST_FLOAT:   return primary->float_const;
    case AST_STRING:  return primary->string;
    case AST_BOOLEAN: return Object(primary->boolean, BOOLEAN);
    case AST_ID: {
        Object obj  = current_environment->var_get(primary->ident);
        OBJECT_ERRORS(primary, obj);
        return obj;
    }   
    case AST_FUNC_CALL: {
        Object obj = evaluate_function_call(primary->call);
        OBJECT_ERRORS(primary, obj);
        return obj;
    }
    default: return Object(OBJ_ERROR_UNKNOWN_TYPE);
    }
}

Object Interpreter::evaluate_function_call(Ast_FunctionCall* call) {
    if (current_environment->func_is_defined(call->ident) == EN_ERROR_UNDEFINED_FUNC) 
        return Object(OBJ_ERROR_UNDEFINED_FUNC);
    
    Ast_FuncDecleration* dec = current_environment->func_get(call->ident);
    if (dec) 
        return execute_function(dec, call);

    return Object(OBJ_ERROR_NONE);
}

Object Interpreter::execute_function(Ast_FuncDecleration* function, Ast_FunctionCall* call) {
    Environment* previous = current_environment;
    current_environment->next = new Environment;
    current_environment = current_environment->next;
    current_environment->previous = previous;

    if (function->args.size() != call->args.size())
        return Object(OBJ_ERROR_PARAMS);
        
    for (int i = 0; i < function->args.size(); i++) {
        Object arg = evaluate_expression(call->args[i]);
        OBJECT_ERRORS(call->args[i], arg);
        current_environment->var_define(function->args[i]->ident, arg);
    }

    for (int i = 0; i < function->scope->declerations.size(); i++) {
        if (function->scope->declerations[i]->type == AST_RETURN) {
            auto ret = AST_CAST(Ast_ReturnStatement, function->scope->declerations[i]);
            if (function->return_type == AST_VOID && ret->expression) return Object(OBJ_ERROR_RETURN_FULL);
            else if (function->return_type != AST_VOID && !ret->expression) return Object(OBJ_ERROR_RETURN_IS_NULL);

            if (!ret->expression)
                return Object(OBJ_ERROR_NONE);
            else if (ret->expression) {
                Object obj_return = evaluate_expression(ret->expression);
                if (obj_return.type != function->return_type) 
                    return Object(OBJ_ERROR_WRONG_RET_TYPE);
                OBJECT_ERRORS(ret->expression, obj_return);
                return obj_return;
            }
        }
        execute(function->scope->declerations[i]);
    }
    current_environment = current_environment->previous;
    delete current_environment->next;
    return Object(OBJ_ERROR_NONE);
}

Object Interpreter::evaluate_binary(Ast_BinaryExpression* binary) {
    auto left = evaluate_expression(binary->left);
    auto right = evaluate_expression(binary->right);

    switch (binary->op) {
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
    case AST_OPERATOR_AND:                   return left && right;
    case AST_OPERATOR_OR:                    return left || right;
    case AST_OPERATOR_MODULO:                return left % right;
    case AST_OPERATOR_BIT_AND:               return left & right;
    case AST_OPERATOR_BIT_OR:                return left | right;
    case AST_OPERATOR_BIT_XOR:               return left ^ right;
    case AST_OPERATOR_BIT_LEFT:              return left << right;
    case AST_OPERATOR_BIT_RIGHT:             return left >> right;
    default: return Object(OBJ_ERROR_UNKNOWN_OPERATOR);
    }  
}

Object Interpreter::evaluate_assignment(Ast_Assignment* assign) {
    Object obj = current_environment->var_get(assign->id);
    OBJECT_ERRORS(assign, obj);
    if (!obj.mutability)
        throw construct_runtime_error(*assign, "Can not have assignment on constant variable.");
    return assignment(assign); 
}

int Interpreter::convert_to_interpreter_type(int ast_type) {
    switch (ast_type) {
    case AST_FLOAT:   return NUMBER;
    case AST_STRING:  return STRING;
    case AST_BOOLEAN: return BOOLEAN;
    }
    return NONE;
}
