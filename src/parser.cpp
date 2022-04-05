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

#include <stdio.h>
#include <stdlib.h>

Parser::Parser(Vec<Token>* tokens) : tokens(tokens) { }

void Parser::run() {

}

Token* Parser::peek() {
    return &tokens->array()[current];
}

Token* Parser::advance() {
    return ((!is_end()) ? &tokens->array()[current++] : &tokens->array()[current]);
}

Ast* Parser::default_ast(Ast* ast) {
    ast->line = peek()->line;
    return ast;
}

void Parser::match(int type) {
    if (peek()->type == type) 
        advance();
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

bool Parser::is_end() {
    return (current == tokens->size() - 1);
}