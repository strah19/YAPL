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
#define EXPECTED_ID "Expected an identifier"
#define EXPECTED_SEMI "Expected ';' after statement"
#define EXPECTED_COLON "Expected ':' for variable decleration"
#define EXPECTED_LEFT_CURLY "Expected '{' at the end of scope"
#define EXPECTED_RIGHT_CURLY "Expected '}' at the end of scope"
#define EXPECTED_FUNC "Expected 'func' for a function decleration"
#define EXPECTED_LEFT_PAR "Expected '('"
#define EXPECTED_RIGHT_PAR "Expected ')'"
#define EXPECTED_LARROW "Expected '<'"
#define EXPECTED_RARROW "Expected '>"

#define UNKNOWN_TYPE "Unknown type found in variable decleration"
#define ELIF_WITHOUT_IF "Elif without an if statement found"
#define ELSE_WITHOUT_IF "Else without an if statement found"
#define UNKNOWN_TOKEN "Unknown token found in expression"
#define INVALID_LVALUE "In assignment l-value is not valid"

#define AST_NEW(type, ...) \
    static_cast<type*>(default_ast(new type(__VA_ARGS__)))

static std::map<int, int> TYPES  = {
    { Tok::T_FLOAT,   AST_FLOAT   },
    { Tok::T_INT,     AST_INT     },
    { Tok::T_STRING,  AST_STRING  },
    { Tok::T_BOOLEAN, AST_BOOLEAN },
    { Tok::T_CHAR,    AST_CHAR    }
};

static std::map<int, int> SPECIFIERS = {
    { Tok::T_CONSTANT, AST_SPECIFIER_CONST }
};

Ast* Parser::default_ast(Ast* ast) {
    ast->line = peek()->line;
    ast->file = filepath;

    return ast;
}

Parser::Parser(Lexer* lexer) {
    if (lexer) { 
        tokens = *lexer->fetch_tokens();
        filepath = lexer->file();

        root =  new Ast_TranslationUnit;
        current = 0;
    }
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
    report_error("In file '%s', on line %d: '%s'.\n", filepath, token->line, msg);
    return ParserError(token);
}

bool Parser::is_end() {
    return (current == tokens.size() - 1);
}

Ast_Decleration* Parser::decleration() {
    try {
        if (peek()->type == Tok::T_IDENTIFIER && peek(1)->type == Tok::T_COLON) {
            if (peek(2)->type == Tok::T_FUNC)
                return func_decleration();
            return var_decleration();
        }
        return statement();
    }
    catch (ParserError error) {
        synchronize();
        return nullptr;
    }
}

Ast_FuncDecleration* Parser::func_decleration() {
    consume(Tok::T_IDENTIFIER, EXPECTED_ID);
    auto id = peek(-1)->identifier;
    consume(Tok::T_COLON, EXPECTED_COLON);

    consume(Tok::T_FUNC, EXPECTED_FUNC);

    auto args = func_args();

    int return_type = AST_VOID;
    if (match(Tok::T_DASH_ARROW)) {
        if (TYPES.find(peek()->type) != TYPES.end()) {
            return_type = TYPES[peek()->type];
            match(peek()->type);
        }
        else
            throw parser_error(peek(), UNKNOWN_TYPE);
    }

    consume(Tok::T_LCURLY, EXPECTED_LEFT_CURLY);
    auto s = scope();

    return AST_NEW(Ast_FuncDecleration, id, return_type, args, s);
}

std::vector<Ast_VarDecleration*> Parser::func_args() {
    std::vector<Ast_VarDecleration*> args;
    consume(Tok::T_LPAR, EXPECTED_LEFT_PAR);
    if (match(Tok::T_RPAR))
        return args;

    args.push_back(var_decleration(false));
    while (match(Tok::T_COMMA)) {
        args.push_back(var_decleration(false));
    }

    consume(Tok::T_RPAR, EXPECTED_RIGHT_PAR);
    return args;
}

Ast_VarDecleration* Parser::var_decleration(bool semi) {
    consume(Tok::T_IDENTIFIER, EXPECTED_ID);
    auto id = peek(-1)->identifier;
    consume(Tok::T_COLON, EXPECTED_COLON);

    int specifiers = AST_SPECIFIER_NONE;
    if (SPECIFIERS.find(peek()->type) != SPECIFIERS.end()) {
        specifiers = SPECIFIERS[peek()->type];
        match(peek()->type);
    }

    int var_type = type();

    Ast_Expression* expr = nullptr;
    if (match(Tok::T_EQUAL))
        expr = expression();
    expr = (!expr) ? nullptr : expr;
    if (semi) consume(Tok::T_SEMI, EXPECTED_SEMI);

    return AST_NEW(Ast_VarDecleration, id, expr, var_type, specifiers);
}

Ast_Statement* Parser::statement() {
    //***********************REFACTOR***********************//
    if (match(Tok::T_PRINT)) return print_statement();
    else if (match(Tok::T_IF)) return conditional_statement();
    else if (match(Tok::T_ELIF)) throw parser_error(peek(), ELIF_WITHOUT_IF);
    else if (match(Tok::T_ELSE)) throw parser_error(peek(), ELSE_WITHOUT_IF);
    else if (match(Tok::T_LCURLY)) return scope();
    else if (match(Tok::T_WHILE)) return while_loop();
    else if (match(Tok::T_RETURN)) return return_statement();

    return expression_statement();
}

Ast_ReturnStatement* Parser::return_statement() {
    if (match(Tok::T_SEMI))
        return AST_NEW(Ast_ReturnStatement, nullptr);
    auto expr = expression();
    consume(Tok::T_SEMI, EXPECTED_SEMI);

    return AST_NEW(Ast_ReturnStatement, expr);
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

//Does not consume or match LCURLY!
Ast_Scope* Parser::scope() {
    Ast_Scope* s = AST_NEW(Ast_Scope);
    while (!check(Tok::T_RCURLY) && !is_end()) {
        s->declerations.push_back(decleration());
    }

    consume(Tok::T_RCURLY, EXPECTED_RIGHT_CURLY);
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

    if (match(Tok::T_EQUAL) || match(Tok::T_EQUAL_PLUS) || match(Tok::T_EQUAL_MINUS) || 
        match(Tok::T_EQUAL_STAR) || match(Tok::T_EQUAL_SLASH) || match(Tok::T_EQUAL_MOD)) {
        Token* equal = peek(-1);
        auto val = assignment();

        if (expr->type == AST_PRIMARY && AST_CAST(Ast_PrimaryExpression, expr)->type_value == AST_ID) 
            return AST_NEW(Ast_Assignment, val, AST_CAST(Ast_PrimaryExpression, expr)->ident, token_to_equal(equal));
    
        parser_error(equal, INVALID_LVALUE);
    }

    return expr;
}

Ast_Expression* Parser::logical() {
    auto expr = bitwise();

    while (match(Tok::T_AND) || match(Tok::T_OR)) {
        auto tok = tokens[current - 1];
        auto right = bitwise();
        expr = AST_NEW(Ast_BinaryExpression, expr, token_to_ast(&tok), right);      
    }

    return expr;
}

Ast_Expression* Parser::bitwise() {
    auto expr = equality();
    
    while (match(Tok::T_CARET) || match(Tok::T_LINE) || match(Tok::T_AMBERSAND)) {
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
    auto expr = shifts();

    while (match(Tok::T_LTE) || match(Tok::T_GTE) || match(Tok::T_LARROW) || match(Tok::T_RARROW)) {
        auto tok = tokens[current - 1];
        auto right = shifts();
        expr = AST_NEW(Ast_BinaryExpression, expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::shifts() {
    auto expr = term();

    while (match(Tok::T_BIT_LEFT) || match(Tok::T_BIT_RIGHT)) {
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
    
    while (match(Tok::T_SLASH) || match(Tok::T_STAR) || match(Tok::T_PERCENT)) {
        auto tok = tokens[current - 1];
        auto right = unary();
        expr = AST_NEW(Ast_BinaryExpression, expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::unary() {
    if (match(Tok::T_MINUS) || match(Tok::T_EXCLAMATION) || match(Tok::T_NOT)) {
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
    case Tok::T_INT_CONST: {
        prime->int_const = peek()->int_const;
        prime->type_value = AST_INT;
        match(Tok::T_INT_CONST);
        break;
    }
    case Tok::T_CHAR_CONST: {
        prime->char_const = peek()->char_const;
        prime->type_value = AST_CHAR;
        match(Tok::T_CHAR_CONST);
        break;
    }
    case Tok::T_IDENTIFIER: {
        const char* ident = peek()->identifier;
        match(Tok::T_IDENTIFIER);

        prime->type_value = (peek()->type == Tok::T_LPAR) ? AST_FUNC_CALL : AST_ID;
        if (prime->type_value == AST_FUNC_CALL) {
            std::vector<Ast_Expression*> args;
            consume(Tok::T_LPAR, EXPECTED_LEFT_PAR);

            if (!check(Tok::T_RPAR)) {
                args.push_back(expression());
                while (match(Tok::T_COMMA)) {
                    args.push_back(expression());
                }
                consume(Tok::T_RPAR, EXPECTED_RIGHT_PAR);
            }
            else 
                consume(Tok::T_RPAR, EXPECTED_RIGHT_PAR);
            prime->call = new Ast_FunctionCall(ident, args);
        }
        else {
            prime->ident = ident;
        }
        
        break;
    }
    case Tok::T_CAST: {
        match(Tok::T_CAST);
        consume(Tok::T_LARROW, EXPECTED_LARROW);
        int t = type();
        consume(Tok::T_RARROW, EXPECTED_RARROW);
        consume(Tok::T_LPAR, EXPECTED_LEFT_PAR);
        auto expr = expression();
        consume(Tok::T_RPAR, EXPECTED_RIGHT_PAR);

        prime->cast.cast_type = t;
        prime->cast.expression = expr;
        prime->type_value = AST_CAST;

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
    case Tok::T_INPUT: {
        prime->type_value = AST_INPUT;
        match(Tok::T_INPUT);
        consume(Tok::T_LPAR, EXPECTED_LEFT_PAR);
        prime->input_type = type();
        consume(Tok::T_RPAR, EXPECTED_RIGHT_PAR);
        break;
    }
    default:
    printf("OH NO: %d\n", peek()->type);
        parser_error(peek(), UNKNOWN_TOKEN);
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
    case Tok::T_PERCENT:       return AST_OPERATOR_MODULO;
    case Tok::T_AMBERSAND:     return AST_OPERATOR_BIT_AND;
    case Tok::T_LINE:          return AST_OPERATOR_BIT_OR;
    case Tok::T_CARET:         return AST_OPERATOR_BIT_XOR;
    case Tok::T_BIT_LEFT:      return AST_OPERATOR_BIT_LEFT;
    case Tok::T_BIT_RIGHT:     return AST_OPERATOR_BIT_RIGHT;
    }
    return -1;
}

int Parser::token_to_ast_unary(Token* token) {
    switch (token->type) {
    case Tok::T_EXCLAMATION: return AST_UNARY_NOT;
    case Tok::T_MINUS:       return AST_UNARY_MINUS;
    case Tok::T_NOT:         return AST_UNARY_BIT_NOT;
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
    case Tok::T_EQUAL_MOD:   return AST_EQUAL_MOD;
    }
    return -1;
}

int Parser::type() {
    int var_type = AST_TYPE_NONE;
    if (TYPES.find(peek()->type) != TYPES.end()) {
        var_type = TYPES[peek()->type];
        match(peek()->type);
        return var_type;
    }
    else
        throw parser_error(peek(), UNKNOWN_TYPE);
}