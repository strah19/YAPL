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

void Parser::match(int type) {
    if (peek()->type == type) {
        advance();
    }
    else {
        begin_custom_error();

        printf("expected ");
        Lexer::print_from_type(type);
        printf(" but found '");
        Lexer::print_token(*peek());
        printf("'.\n");

        end_custom_error();
        exit(EXIT_FAILURE);
    }
}

bool Parser::check(int type) {
    if (peek()->type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::is_end() {
    return (current == tokens.size() - 1);
}

Ast_Decleration* Parser::decleration() {
    auto dec = new Ast_Decleration;

    try {
        throw ParserError();
        if (peek()->type == Tok::T_IDENTIFIER && peek(1)->type == Tok::T_COLON) {
            //variable decleration
        }
    }
    catch(ParserError error) {
        synchronize();
        return nullptr;
    }

    return statement();
}

Ast_Statement* Parser::statement() {

    return nullptr;
}

void Parser::synchronize() {
    printf("hahahah\n");
}

Ast_Expression* Parser::expression() {
    return equality();
}

Ast_Expression* Parser::equality() {
    auto expr = comparison();

    while (check(Tok::T_COMPARE_EQUAL) || check(Tok::T_NOT_EQUAL)) {
        auto tok = tokens[current - 1];
        auto right = comparison();
        expr = new Ast_BinaryExpression(expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::comparison() {
    auto expr = term();

    while (check(Tok::T_LTE) || check(Tok::T_GTE) || check(Tok::T_LARROW) || check(Tok::T_RARROW)) {
        auto tok = tokens[current - 1];
        auto right = term();
        expr = new Ast_BinaryExpression(expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::term() {
    auto expr = factor();

    while (check(Tok::T_PLUS) || check(Tok::T_MINUS)) {
        auto tok = tokens[current - 1];
        auto right = factor();
        expr = new Ast_BinaryExpression(expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::factor() {
    auto expr = unary();
    
    while (check(Tok::T_SLASH) || check(Tok::T_STAR)) {
        auto tok = tokens[current - 1];
        auto right = unary();
        expr = new Ast_BinaryExpression(expr, token_to_ast(&tok), right);
    }

    return expr;
}

Ast_Expression* Parser::unary() {
    if (check(Tok::T_MINUS)) {
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
        match(Tok::T_INT_CONST);
        break;
    }
    case Tok::T_IDENTIFIER: {
        prime->ident = identifier();
        break;
    }
    case Tok::T_LPAR: {
        match(Tok::T_LPAR);
        auto expr = expression();
        match(Tok::T_RPAR);
        prime->nested = expr;
        break;
    }
    }

    return prime;
}

Ast_Identifier* Parser::identifier() {
    auto id = new Ast_Identifier;
    id->id = advance()->identifier;
    return id;
}

int Parser::token_to_ast(Token* token) {
    switch (token->type) {
    case Tok::T_COMPARE_EQUAL: return AST_OPERATOR_COMPARITIVE_EQUAL;
    case Tok::T_NOT_EQUAL: return AST_OPERATOR_COMPARITIVE_NOT_EQUAL;
    case Tok::T_LTE: return AST_OPERATOR_LTE;
    case Tok::T_GTE: return AST_OPERATOR_GTE;
    case Tok::T_LARROW: return AST_OPERATOR_LT;
    case Tok::T_RARROW: return AST_OPERATOR_GT;
    case Tok::T_PLUS: return AST_OPERATOR_ADD;
    case Tok::T_MINUS: return AST_OPERATOR_SUB;
    case Tok::T_STAR: return AST_OPERATOR_MULTIPLICATIVE;
    case Tok::T_SLASH: return AST_OPERATOR_DIVISION;
    }
    return -1;
}

void Parser::visualize() {

}

void Parser::visualize_expression(Ast_Expression* expr, int indent) {
    if (expr) {
        switch (expr->type) {
        case AST_UNARY: {
            auto u = AST_CAST(Ast_UnaryExpression, expr);
            ident(indent);
            printf("unary: %d\n", u->op);
            if (u->next) {
                visualize_expression(u->next, indent + 1);
            }
            break;
        }
        case AST_PRIMARY: {
            auto p = AST_CAST(Ast_PrimaryExpression, expr);
            ident(indent);
            if (p->nested) {
                printf("nested: \n");
                visualize_expression(p->nested, indent + 1);
            }
            else
                printf("primary: %f\n", p->int_const);
            break;
        }
        case AST_BINARY: {
            auto b = AST_CAST(Ast_BinaryExpression, expr);
            ident(indent);
            printf("binary: %d\n", b->op);
            visualize_expression(b->left, indent + 1);
            visualize_expression(b->right, indent + 1);
            break;
        }
        }
    }
}

void Parser::ident(int indent) {
    while (indent > 0) {
        printf("\t");
        indent--;
    }
}