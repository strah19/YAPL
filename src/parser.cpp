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
        if (match(Tok::T_VAR)) 
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

    int var_type = AST_TYPE_NONE;
    if (match(Tok::T_INT))
        var_type = AST_INT;
    else if (match(Tok::T_FLOAT))
        var_type = AST_FLOAT;
    else if (match(Tok::T_STRING))
        var_type = AST_STRING;
    else
        throw parser_error(peek(), "unknown variable type");

    current_assignment_type = var_type;
    auto expr = new Ast_Expression;
    if (match(Tok::T_EQUAL))
        expr = expression();
    current_assignment_type = AST_TYPE_NONE;
    consume(Tok::T_SEMI, EXPECTED_SEMI);

    return new Ast_VarDecleration(id, expr, var_type);
}

Ast_Statement* Parser::statement() {
    if (match(Tok::T_PRINT)) return print_statement();
    else if (match(Tok::T_LCURLY)) return scope();

    return expression_statement();
}

Ast_Scope* Parser::scope() {
    Ast_Scope* scope = new Ast_Scope;
    while (!check(Tok::T_RCURLY) && !is_end()) {
        scope->declerations.push_back(decleration());
    }

    consume(Tok::T_RCURLY, "Expected '}' for the end of a block");
    return scope;
}

Ast_ExpressionStatement* Parser::expression_statement() {
    auto expr = expression();
    consume(Tok::T_SEMI, EXPECTED_SEMI);
    return new Ast_ExpressionStatement(expr);
}

Ast_PrintStatement* Parser::print_statement() {
    auto print = new Ast_PrintStatement(expression());
    consume(Tok::T_SEMI, EXPECTED_SEMI);
    return print;
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
    auto expr = equality();

    if (match(Tok::T_EQUAL)) {
        Token* equal = peek(-1);
        auto val = assignment();

        if (expr->type == AST_PRIMARY && AST_CAST(Ast_PrimaryExpression, expr)->type_value == AST_ID) 
            return new Ast_Assignment(val, AST_CAST(Ast_PrimaryExpression, expr)->ident);
    
        parser_error(equal, "l-value in assignment is not valid");
    }

    return expr;
}

Ast_Expression* Parser::equality() {
    auto expr = comparison();

    while (match(Tok::T_COMPARE_EQUAL) || match(Tok::T_NOT_EQUAL)) {
        auto tok = tokens[current - 1];
        auto right = comparison();
        expr = new Ast_BinaryExpression(expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::comparison() {
    auto expr = term();

    while (match(Tok::T_LTE) || match(Tok::T_GTE) || match(Tok::T_LARROW) || match(Tok::T_RARROW)) {
        auto tok = tokens[current - 1];
        auto right = term();
        expr = new Ast_BinaryExpression(expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::term() {
    auto expr = factor();

    while (match(Tok::T_PLUS) || match(Tok::T_MINUS)) {
        auto tok = tokens[current - 1];
        auto right = factor();
        expr = new Ast_BinaryExpression(expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::factor() {
    auto expr = unary();
    
    while (match(Tok::T_SLASH) || match(Tok::T_STAR)) {
        auto tok = tokens[current - 1];
        auto right = unary();
        expr = new Ast_BinaryExpression(expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::unary() {
    if (match(Tok::T_MINUS)) {
        Ast_Expression* right = unary();
        return new Ast_UnaryExpression(right, AST_UNARY_MINUS);
    }

    return primary();
}

Ast_Expression* Parser::primary() {
    auto prime = new Ast_PrimaryExpression();

    switch (peek()->type) {
    case Tok::T_INT_CONST: {
        prime->int_const = peek()->int_const;
        prime->type_value = AST_INT;
        check_assignment(AST_INT);
        match(Tok::T_INT_CONST);
        break;
    }
    case Tok::T_FLOAT_CONST: {
        prime->float_const = peek()->float_const;
        prime->type_value = AST_FLOAT;
        check_assignment(AST_FLOAT);
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
        check_assignment(AST_STRING);
        match(Tok::T_STRING_CONST);
        break;
    }
    }

    return prime;
}

void Parser::check_assignment(int type) {
    if (current_assignment_type == type || current_assignment_type == AST_TYPE_NONE || check_if_types_are_numbers(type)) return;
    throw parser_error(peek(), "Type in assignment or decleration must match primary-expression");
}

bool Parser::check_if_types_are_numbers(int type) {
    return (current_assignment_type == AST_FLOAT && type == AST_INT);
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
    }
    return -1;
}