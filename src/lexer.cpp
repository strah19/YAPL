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

#include "lexer.h"
#include "err.h"
#include "symtable.h"

static SymTable keywords;
static SymTable symbols;

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
Lexer::Lexer(const char* filepath) {
    stream = load(filepath, &size);

    backtrack_symbol_pos = 0;

    keywords.insert("if", Tok::T_IF);
    keywords.insert("else", Tok::T_ELSE);
    keywords.insert("elif", Tok::T_ELIF);
    keywords.insert("while", Tok::T_WHILE);
    keywords.insert("continue", Tok::T_CONTINUE);
    keywords.insert("return", Tok::T_RETURN);
    keywords.insert("break", Tok::T_BREAK);
    keywords.insert("int", Tok::T_INT);
    keywords.insert("boolean", Tok::T_BOOLEAN);
    keywords.insert("byte", Tok::T_BYTE);
    keywords.insert("double", Tok::T_DOUBLE);
    keywords.insert("float", Tok::T_FLOAT);
    keywords.insert("foreign", Tok::T_FOREIGN);
    keywords.insert("return", Tok::T_RETURN);
    keywords.insert("from", Tok::T_FROM);
    keywords.insert("constant", Tok::T_CONST);
    keywords.insert("var", Tok::T_VAR);
    keywords.insert("print", Tok::T_PRINT);
    keywords.insert("string", Tok::T_STRING);

    symbols.insert("<=", Tok::T_LTE);
    symbols.insert(">=", Tok::T_GTE);
    symbols.insert("!=", Tok::T_NOT_EQUAL);
    symbols.insert("==", Tok::T_COMPARE_EQUAL);
    symbols.insert("++", Tok::T_INC);
    symbols.insert("--", Tok::T_DEC);
    symbols.insert("->", Tok::T_DASH_ARROW);
}

char Lexer::incr_char(int32_t off) {
    return *(stream + off);
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
    if (*stream == '\n') {
        current_line++;
        current_type = (current_type == SINGLE_LINE_COMMENT) ? 0 : current_type;
    }
}

void Lexer::singleline_comment() {
    if (*stream == '/' && incr_char() == '/') {
        current_type = (current_type == 0) ? SINGLE_LINE_COMMENT : current_type;
        move();
    }   
}

void Lexer::multiline_comment_beg() {
    if (*stream == '<' && incr_char() == '/') {
        nested_comment++;
        current_type = MULTI_LINE_COMMENT;
        move();
    }
}

void Lexer::multiline_comment_end() {
    if (*stream == '/' && incr_char() == '>') {
        nested_comment--;
        current_type = (nested_comment == 0) ? 0 : current_type;
        move();
    }
}

void Lexer::create_sym_token() {
    for (int i = 0; i < current.size(); i++) {
        Entry* ent = symbols.look_up(current.c_str());
        if (ent) {
            tokens.push_back(Token(ent->type, current_line));
            stream = backtrack_symbol_pos + i + 2;
            return;
        }
    }

    tokens.push_back(Token(current[0], current_line));
    stream = backtrack_symbol_pos + 1;    
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
    stream++;
}

/**
 * This is the run function. Will use the data from the filepath.
 */
void Lexer::lex() {
    while (*stream != '\0') {
        singleline_comment();
        multiline_comment_beg();
        if (current_type != SINGLE_LINE_COMMENT && current_type != MULTI_LINE_COMMENT) {
            if (current_type == IDENTIFIER && !is_identifier(*stream)) {
                Entry* ent = keywords.look_up(current.c_str());
                if (ent) {
                    tokens.push_back(Token(ent->type, current_line));
                    reset();
                }
                else if (!isdigit(*stream)) {
                    tokens.push_back(Token(Tok::T_IDENTIFIER, current_line));
                
                    tokens.back().identifier = new char[current.size()];
                    strcpy(tokens.back().identifier, current.c_str());
                    reset();
                }
            }
            else if (!isdigit(*stream) && current_type == NUMERIC) {
                tokens.push_back(Token(Tok::T_INT_CONST, current_line));
                tokens.back().int_const = atoi(current.c_str());

                reset();
            }
            else if (*stream == '"' && current_type == STRING) {
                tokens.push_back(Token(Tok::T_STRING_CONST, current_line));
                tokens.back().string = new char[current.size() - 1];
                strcpy(tokens.back().string, current.c_str() + 1);

                reset();
                move();
                continue;
            }
            else if (current_type == SYMBOL && get_type(*stream) != SYMBOL) {
                create_sym_token();
                reset();
            }
            if (!is_spec_char(*stream)) {
                current.push_back(*stream);
                if (current.size() == 1) {
                    current_type = get_type(*stream);

                    if (current_type == SYMBOL) {
                        backtrack_symbol_pos = stream;
                        if (*stream == '"') 
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
    Entry* ent = keywords.look_up_by_type(token.type);
    if (ent)
        printf("%s", ent->name);

    ent = symbols.look_up_by_type(token.type);
    if (ent)
        printf("%s", ent->name);

    switch (token.type) {
    case Tok::T_IDENTIFIER: {
        printf("%s", token.identifier);
        break;
    }
    case Tok::T_INT_CONST: {
        printf("%d", token.int_const);
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
    default: {
        if (token.type < Tok::T_EOF)
            printf("%c", token.type);
        break;
    }
    }
}

void Lexer::print_from_type(int type) {
    Entry* ent = keywords.look_up_by_type(type);
    if (ent)
        printf("%s", ent->name);

    ent = symbols.look_up_by_type(type);
    if (ent)
        printf("%s", ent->name);

    switch (type) {
    case Tok::T_IDENTIFIER: {
        printf("identifier");
        break;
    }
    case Tok::T_INT_CONST: {
        printf("int-const");
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

uint8_t* Lexer::load(const char* filepath, uint32_t* size) {
    uint8_t* stream;
    FILE* file;

    if(file = fopen(filepath, "r")) {
        struct stat st;

        if (fstat(fileno(file), &st) != -1) {
            stream = (uint8_t *) malloc(st.st_size + 1);
            *size = st.st_size;
            fread((void*) stream, 1, st.st_size, file);
            stream[*size - 1] = '\0';
        }
        else 
            fatal_error("failed to load stats of input file.\n");
    }
    else 
        fatal_error("failed to open '%s' for compilation.\n", filepath);

    return stream;
}