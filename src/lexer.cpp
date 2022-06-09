/**
 * @file lexer.cpp
 * @author strah19
 * @date March 25 2022
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
 * Defines the languages lexer and functionality. It also has the tokens for the lexer.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <map>

#include "lexer.h"
#include "err.h"

static std::map<std::string, int> keywords = {
    { "if", Tok::T_IF },
    { "else", Tok::T_ELSE },
    { "elif", Tok::T_ELIF },
    { "while", Tok::T_WHILE },
    { "continue", Tok::T_CONTINUE },
    { "return", Tok::T_RETURN },
    { "break", Tok::T_BREAK },
    { "boolean", Tok::T_BOOLEAN },
    { "float", Tok::T_FLOAT },
    { "return", Tok::T_RETURN },
    { "print", Tok::T_PRINT },
    { "string", Tok::T_STRING },
    { "func", Tok::T_FUNC },
    { "true", Tok::T_TRUE },
    { "false", Tok::T_FALSE },
    { "constant", Tok::T_CONSTANT },
    { "remit", Tok::T_REMIT },
    { "and", Tok::T_AND },
    { "or", Tok::T_OR },
    { "for", Tok::T_FOR }
};

static std::map<std::string, int> symbols = {
    { "<=", Tok::T_LTE },
    { ">=", Tok::T_GTE },
    { "!=", Tok::T_NOT_EQUAL },
    { "==", Tok::T_COMPARE_EQUAL },
    { "->", Tok::T_DASH_ARROW },
    { "+=", Tok::T_EQUAL_PLUS },
    { "-=", Tok::T_EQUAL_MINUS },
    { "*=", Tok::T_EQUAL_STAR },
    { "/=", Tok::T_EQUAL_SLASH }
};

// different types of general tokens used when lexing
enum {
    IDENTIFIER = 1,
    NUMERIC,
    SYMBOL,
    STRING,
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT
};

/**
 * Constructor for the lexer, takes in the source file for analysis.
 * 
 * @param const char* The path to the source file.
 */
Lexer::Lexer(const char* filepath) : filepath(filepath) {
    load();
}

char Lexer::incr_char(int32_t off) {
    return stream[current_index + off];
}

bool Lexer::is_identifier(char c) {
    return (isalpha(c) || c == '_');
}

bool Lexer::is_spec_char(char c) {
    return (c == ' ' || c == '\n' || c == '\t');
}

void Lexer::remove_whitespaces(char* s) {
    char* d = s;
    do while (isspace(*s)) s++; while (*d++ = *s++);
}

void Lexer::newline() {
    if (stream[current_index] == '\n') {
        current_line++;
        current_type = (current_type == SINGLE_LINE_COMMENT) ? 0 : current_type;
    }
}

void Lexer::singleline_comment() {
    if (stream[current_index] == '/' && incr_char() == '/') {
        current_type = (current_type == 0) ? SINGLE_LINE_COMMENT : current_type;
        move();
    }   
}

void Lexer::multiline_comment_beg() {
    if (stream[current_index] == '<' && incr_char() == '/') {
        nested_comment++;
        current_type = MULTI_LINE_COMMENT;
        move();
    }
}

void Lexer::multiline_comment_end() {
    if (stream[current_index] == '/' && incr_char() == '>') {
        nested_comment--;
        current_type = (nested_comment == 0) ? 0 : current_type;
        move();
    }
}

void Lexer::create_sym_token() {
    for (int i = 0; i < current.size(); i++) {
        if (symbols.find(current) != symbols.end()) {
            tokens.push_back(Token(symbols[current], current_line));
            return;
        }
    }

    for (int i = 0; i < current.size(); i++) {
        tokens.push_back(Token(current[i], current_line));
    }
}

void Lexer::reset() {
    current_type = 0;
    current.clear();
}

uint32_t Lexer::get_type(char c) {
    if (isdigit(c))
        return NUMERIC;
    else if(is_identifier(c))
        return IDENTIFIER;
    return (!is_spec_char(c)) ? SYMBOL : 0;
}

void Lexer::move() {
    counter++;
    current_index++;
}

/**
 * This is the run function. Will use the data from the filepath.
 */
void Lexer::lex() {
    bool num_has_dec = false;
    
    while (current_index < stream.size()) {
        singleline_comment();
        multiline_comment_beg();
        if (current_type != SINGLE_LINE_COMMENT && current_type != MULTI_LINE_COMMENT) {
            if (current_type == IDENTIFIER && !is_identifier(stream[current_index])) {
                if (keywords.find(current) != keywords.end()) {
                    tokens.push_back(Token(keywords[current], current_line));
                    reset();
                }
                else if (!isdigit(stream[current_index])) {
                    tokens.push_back(Token(Tok::T_IDENTIFIER, current_line));
                
                    tokens.back().identifier = new char[current.size()];
                    strcpy(tokens.back().identifier, current.c_str());
                    reset();
                }
            }
            else if (!isdigit(stream[current_index]) && current_type == NUMERIC) {
                if (stream[current_index] == '.' && !num_has_dec) {
                    num_has_dec = true;
                }
                else {
                    if (!num_has_dec) {
                        tokens.push_back(Token(Tok::T_FLOAT_CONST, current_line));
                        tokens.back().float_const = atoi(current.c_str());
                    }
                    else if (num_has_dec) {
                        tokens.push_back(Token(Tok::T_FLOAT_CONST, current_line));
                        tokens.back().float_const = std::stof(current.c_str());
                        num_has_dec = false;
                    }

                    reset();
                }
            }
            else if (stream[current_index] == '"' && current_type == STRING) {
                tokens.push_back(Token(Tok::T_STRING_CONST, current_line));
                current.erase(0, 1);
                tokens.back().string = new char[current.length() + 1];
                strcpy(tokens.back().string, current.c_str());

                reset();
                move();
            }
            else if (current_type == SYMBOL && get_type(stream[current_index]) != SYMBOL) {
                create_sym_token();
                reset();
            }
            if (!is_spec_char(stream[current_index]) || current_type == STRING) {
                current.push_back(stream[current_index]);
                if (current.size() == 1) {
                    current_type = get_type(stream[current_index]);

                    if (current_type == SYMBOL) {
                        if (stream[current_index] == '"') 
                            current_type = STRING;
                    }
                }
            }      
        }
        multiline_comment_end();
        newline();  //Check for newline.
        move();
    }
    tokens.push_back(Token(Tok::T_EOF, current_line));
}

/**
 * Will log all the tokens collected during analysis.
 */
void Lexer::log() {
    for (int i = 0; i < tokens.size(); i++) 
        log_token(tokens[i], i);
}

/**
 * Will log a specific token.
 * 
 * @param Token The token to print.
 * @param uint32_t The index of the token.
 */
void Lexer::log_token(Token& token, uint32_t i) {
    printf("token #%d of type %d: '", i, token.type);
    print_token(token);
    printf("' on line %d.\n", token.line);
}

/**
 * Will print the actual data the token collected.
 * 
 * @param Token The token.
 */
void Lexer::print_token(Token& token) {
    log_keywords_symbols(token.type);

    switch (token.type) {
    case Tok::T_IDENTIFIER: {
        printf("%s", token.identifier);
        break;
    }
    case Tok::T_EOF: {
        printf("EOF");
        break;
    }
    case Tok::T_STRING_CONST: {
        printf("%s", token.string);
        break;
    }
    case Tok::T_FLOAT_CONST: {
        printf("%f", token.float_const);
    }
    default: {
        if (token.type < Tok::T_EOF)
            printf("%c", token.type);
        break;
    }
    }
}

void Lexer::print_from_type(int type) {
    log_keywords_symbols(type);

    switch (type) {
    case Tok::T_IDENTIFIER: {
        printf("identifier");
        break;
    }
    case Tok::T_FLOAT_CONST: {
        printf("float-const");
        break;
    }
    case Tok::T_EOF: {
        printf("EOF");
        break;
    }
    default: {
        if (type < Tok::T_EOF)
            printf("%c", type);
        break;
    }
    }
}

void Lexer::log_keywords_symbols(int type) {
    
}

void Lexer::load() {
    struct stat sb{};

    FILE* input_file = fopen(filepath, "r");
    if (input_file == nullptr) {
        perror("fopen");
    }

    stat(filepath, &sb);
    stream.resize(sb.st_size);
    fread(const_cast<char*>(stream.data()), sb.st_size, 1, input_file);
    fclose(input_file);
}