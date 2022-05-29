/**
 * @file parser.cpp
 * @author strah19
 * @date April 2 2022
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
 * Defines the languages syntax and parsers the tokens from the lexer.
 */

#include "parser.h"
#include "err.h"
#include <stdio.h>
#include <stdlib.h>

//error messages
#define EXPECTED_ID "expected an identifier"
#define EXPECTED_SEMI "expected semicolon after statement"

static std::map<int, int> TYPES  = {
    { Tok::T_FLOAT,   AST_FLOAT   },
    { Tok::T_STRING,  AST_STRING  },
    { Tok::T_BOOLEAN, AST_BOOLEAN }
};

static std::map<int, int> SPECIFIERS = {
    { Tok::T_CONSTANT, AST_SPECIFIER_CONST }
};

#define AST_NEW(type, ...) \
    static_cast<type*>(default_ast(new type(__VA_ARGS__)))

Ast* Parser::default_ast(Ast* ast) {
    ast->line = peek()->line;

    return ast;
}

Parser::Parser(std::vector<Token>* tokens) : tokens(*tokens) { 
    root =  new Ast_TranslationUnit;
    current = 0;
}

Parser::~Parser() {
    delete root;
}

void Parser::parse() {
    while (!is_end()) {
        auto dec = decleration();

        if (dec)
            root->declerations.push_back(dec);
    }
}

Token* Parser::peek(int index) {
    return &tokens[current + index];
}

Token* Parser::advance() {
    return ((!is_end()) ? &tokens[current++] : &tokens[current]);
}

bool Parser::match(int type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(int type) {
    return (peek()->type == type);
}

Token* Parser::consume(int type, const char* msg) {
    if (check(type)) return advance();
    throw parser_error(peek(), msg);
}

ParserError Parser::parser_error(Token* token, const char* msg) {
    report_error("on line %d: '%s'.\n", token->line, msg);
    return ParserError(token);
}

bool Parser::is_end() {
    return (current == tokens.size() - 1);
}

Ast_Decleration* Parser::decleration() {
    try {
        if (peek()->type == Tok::T_IDENTIFIER && peek(1)->type == Tok::T_COLON) 
            return var_decleration();
        return statement();
    }
    catch (ParserError error) {
        synchronize();
        return nullptr;
    }
}

Ast_VarDecleration* Parser::var_decleration() {
    consume(Tok::T_IDENTIFIER, EXPECTED_ID);
    auto id = peek(-1)->identifier;
    consume(Tok::T_COLON, "expected : for variable decleration");

    int specifiers = AST_SPECIFIER_NONE;
    if (SPECIFIERS.find(peek()->type) != SPECIFIERS.end()) {
        specifiers = SPECIFIERS[peek()->type];
        match(peek()->type);
    }

    int var_type = AST_TYPE_NONE;
    if (TYPES.find(peek()->type) != TYPES.end()) {
        var_type = TYPES[peek()->type];
        match(peek()->type);
    }
    else
        throw parser_error(peek(), "unknown variable type");

    Ast_Expression* expr = nullptr;
    if (match(Tok::T_EQUAL))
        expr = expression();
    expr = (!expr) ? nullptr : expr;
    consume(Tok::T_SEMI, EXPECTED_SEMI);

    return AST_NEW(Ast_VarDecleration, id, expr, var_type, specifiers);
}

Ast_Statement* Parser::statement() {
    //***********************REFACTOR***********************//
    if (match(Tok::T_PRINT)) return print_statement();
    else if (match(Tok::T_IF)) return conditional_statement();
    else if (match(Tok::T_ELIF)) throw parser_error(peek(), "Elif without an if");
    else if (match(Tok::T_ELSE)) throw parser_error(peek(), "else without an if");
    else if (match(Tok::T_LCURLY)) return scope();
    else if (match(Tok::T_WHILE)) return while_loop();
    else if (match(Tok::T_REMIT) || match(Tok::T_BREAK)) return controller_statement();

    return expression_statement();
}

Ast_ConditionalController* Parser::controller_statement() {
    auto tok = tokens[current - 1];
    consume(Tok::T_SEMI, EXPECTED_SEMI);
    return AST_NEW(Ast_ConditionalController, token_to_controller(&tok));
}

Ast_ConditionalStatement* Parser::conditional_statement() {
    auto if_state = if_statement();

    Ast_ConditionalStatement* current = if_state;
    while (match(Tok::T_ELIF)) {
        current->next = elif_statement();
        current = current->next;
    }

    if (match(Tok::T_ELSE)) 
        current->next = else_statement();

    return if_state;
} 

Ast_IfStatement* Parser::if_statement() {
    Ast_Scope* s;
    Ast_Expression* expr;
    
    expr = expression();
    if (match(Tok::T_LCURLY)) {
        s = scope();
    }
    else {
        s = AST_NEW(Ast_Scope);
        s->declerations.push_back(statement());
    }
    
    return AST_NEW(Ast_IfStatement, expr, s);
}

Ast_ElifStatement* Parser::elif_statement() {
    Ast_Scope* s;
    Ast_Expression* expr;
    
    expr = expression();
    if (match(Tok::T_LCURLY)) {
        s = scope();
    }
    else {
        s = AST_NEW(Ast_Scope);
        s->declerations.push_back(statement());
    }
    return AST_NEW(Ast_ElifStatement, expr, s);
}

Ast_ElseStatement* Parser::else_statement() {
    Ast_Scope* s;
    if (match(Tok::T_LCURLY)) {
        s = scope();
    }
    else {
        s = AST_NEW(Ast_Scope);
        s->declerations.push_back(statement());
    }
    return AST_NEW(Ast_ElseStatement, s);
}

Ast_Scope* Parser::scope() {
    Ast_Scope* s = AST_NEW(Ast_Scope);
    while (!check(Tok::T_RCURLY) && !is_end()) {
        s->declerations.push_back(decleration());
    }

    consume(Tok::T_RCURLY, "Expected '}' for the end of a block");
    return s;
}

Ast_WhileLoop* Parser::while_loop() {
    Ast_Scope* s;
    Ast_Expression* expr;
    
    expr = expression();
    if (match(Tok::T_LCURLY)) {
        s = scope();
    }
    else {
        s = AST_NEW(Ast_Scope);
        s->declerations.push_back(statement());
    }
    return AST_NEW(Ast_WhileLoop, expr, s);
}

Ast_ExpressionStatement* Parser::expression_statement() {
    auto expr = expression();
    consume(Tok::T_SEMI, EXPECTED_SEMI);
    return AST_NEW(Ast_ExpressionStatement, expr);
}

Ast_PrintStatement* Parser::print_statement() {
    std::vector<Ast_Expression*> expressions;
    expressions.push_back(expression());

    while (match(Tok::T_COMMA)) 
        expressions.push_back(expression());

    consume(Tok::T_SEMI, EXPECTED_SEMI);
    return AST_NEW(Ast_PrintStatement, expressions);
}

void Parser::synchronize() {
    advance();

    while (!is_end()) {
        if (peek(-1)->type == Tok::T_SEMI) return;

        advance();
    }   
}

Ast_Expression* Parser::expression() {
    return assignment();
}

Ast_Expression* Parser::assignment() {
    auto expr = logical();

    if (match(Tok::T_EQUAL) || match(Tok::T_EQUAL_PLUS) || match(Tok::T_EQUAL_MINUS) || match(Tok::T_EQUAL_STAR) || match(Tok::T_EQUAL_SLASH)) {
        Token* equal = peek(-1);
        auto val = assignment();

        if (expr->type == AST_PRIMARY && AST_CAST(Ast_PrimaryExpression, expr)->type_value == AST_ID) 
            return AST_NEW(Ast_Assignment, val, AST_CAST(Ast_PrimaryExpression, expr)->ident, token_to_equal(equal));
    
        parser_error(equal, "l-value in assignment is not valid");
    }

    return expr;
}

Ast_Expression* Parser::logical() {
    auto expr = equality();

    while (match(Tok::T_AND) || match(Tok::T_OR)) {
        auto tok = tokens[current - 1];
        auto right = equality();
        expr = AST_NEW(Ast_BinaryExpression, expr, token_to_ast(&tok), right);      
    }

    return expr;
}

Ast_Expression* Parser::equality() {
    auto expr = comparison();

    while (match(Tok::T_COMPARE_EQUAL) || match(Tok::T_NOT_EQUAL)) {
        auto tok = tokens[current - 1];
        auto right = comparison();
        expr = AST_NEW(Ast_BinaryExpression, expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::comparison() {
    auto expr = term();

    while (match(Tok::T_LTE) || match(Tok::T_GTE) || match(Tok::T_LARROW) || match(Tok::T_RARROW)) {
        auto tok = tokens[current - 1];
        auto right = term();
        expr = AST_NEW(Ast_BinaryExpression, expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::term() {
    auto expr = factor();

    while (match(Tok::T_PLUS) || match(Tok::T_MINUS)) {
        auto tok = tokens[current - 1];
        auto right = factor();
        expr = AST_NEW(Ast_BinaryExpression, expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::factor() {
    auto expr = unary();
    
    while (match(Tok::T_SLASH) || match(Tok::T_STAR)) {
        auto tok = tokens[current - 1];
        auto right = unary();
        expr = AST_NEW(Ast_BinaryExpression, expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::unary() {
    if (match(Tok::T_MINUS) || match(Tok::T_EXCLAMATION)) {
        auto tok = tokens[current - 1];
        Ast_Expression* right = unary();
        return AST_NEW(Ast_UnaryExpression, right, token_to_ast_unary(&tok));
    }

    return primary();
}

Ast_Expression* Parser::primary() {
    auto prime = AST_NEW(Ast_PrimaryExpression);
    switch (peek()->type) {
    case Tok::T_TRUE: {
        prime->boolean = true;
        prime->type_value = AST_BOOLEAN;
        match(Tok::T_TRUE);
        break;
    }
    case Tok::T_FALSE: {
        prime->boolean = false;
        prime->type_value = AST_BOOLEAN;
        match(Tok::T_FALSE);
        break;
    }
    case Tok::T_FLOAT_CONST: {
        prime->float_const = peek()->float_const;
        prime->type_value = AST_FLOAT;
        match(Tok::T_FLOAT_CONST);
        break;
    }
    case Tok::T_IDENTIFIER: {
        prime->ident = peek()->identifier;
        prime->type_value = AST_ID;
        match(Tok::T_IDENTIFIER);
        break;
    }
    case Tok::T_LPAR: {
        match(Tok::T_LPAR);
        auto expr = expression();
        match(Tok::T_RPAR);
        prime->nested = expr;
        prime->type_value = AST_NESTED;
        break;
    }
    case Tok::T_STRING_CONST: {
        prime->string = peek()->string;
        prime->type_value = AST_STRING;
        match(Tok::T_STRING_CONST);
        break;
    }
    default:
        parser_error(peek(), "Unknown token inside an expression");
    }

    return prime;
}

int Parser::token_to_ast(Token* token) {
    switch (token->type) {
    case Tok::T_COMPARE_EQUAL: return AST_OPERATOR_COMPARITIVE_EQUAL;
    case Tok::T_NOT_EQUAL:     return AST_OPERATOR_COMPARITIVE_NOT_EQUAL;
    case Tok::T_LTE:           return AST_OPERATOR_LTE;
    case Tok::T_GTE:           return AST_OPERATOR_GTE;
    case Tok::T_LARROW:        return AST_OPERATOR_LT;
    case Tok::T_RARROW:        return AST_OPERATOR_GT;
    case Tok::T_PLUS:          return AST_OPERATOR_ADD;
    case Tok::T_MINUS:         return AST_OPERATOR_SUB;
    case Tok::T_STAR:          return AST_OPERATOR_MULTIPLICATIVE;
    case Tok::T_SLASH:         return AST_OPERATOR_DIVISION;
    case Tok::T_AND:           return AST_OPERATOR_AND;
    case Tok::T_OR:            return AST_OPERATOR_OR;
    }
    return -1;
}

int Parser::token_to_ast_unary(Token* token) {
    switch (token->type) {
    case Tok::T_EXCLAMATION: return AST_UNARY_NOT;
    case Tok::T_MINUS:       return AST_UNARY_MINUS;
    }
    return -1;
}

int Parser::token_to_controller(Token* token) {
    switch (token->type) {
    case Tok::T_REMIT: return AST_CONTROLLER_REMIT;
    case Tok::T_BREAK: return AST_CONTROLLER_BREAK;
    }
    return -1;
}

int Parser::token_to_equal(Token* token) {
    switch (token->type) {
    case Tok::T_EQUAL:       return AST_EQUAL;
    case Tok::T_EQUAL_PLUS:  return AST_EQUAL_PLUS;
    case Tok::T_EQUAL_MINUS: return AST_EQUAL_MINUS;
    case Tok::T_EQUAL_STAR:  return AST_EQUAL_MULTIPLY;
    case Tok::T_EQUAL_SLASH: return AST_EQUAL_DIVIDE;
    }
    return -1;
}