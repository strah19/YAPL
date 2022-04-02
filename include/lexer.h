#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>
#include "vec.h"

namespace Tok {
    enum {
        T_COLON = ':',
        T_PLUS = '+',
        T_SLASH = '/',
        T_STAR = '*',
        T_MINUS = '-',
        T_EQUAL = '=',
        T_CARET = '^',
        T_AMBERSAND = '&',
        
        T_LPAR = '(',
        T_RPAR = ')',
        T_LBRACKET = '[',
        T_RBRACKET = ']',
        T_LCURLY = '{',
        T_RCURLY = '}',
        T_SEMI = ';',
        T_PERCENT = '%',
        T_POUND = '#',
        T_EXCLAMATION = '!',
        T_LARROW = '<',
        T_RARROW = '>',
        T_COMMA = ',', 

        T_EOF = 255,


        T_COLON_ASSIGN,
        T_AND,
        T_OR,
        T_IF,
        T_ELIF,
        T_ELSE,
        T_WHILE,
        T_BREAK,
        T_RETURN,
        T_CONTINUE,
        T_COMPARE_EQUAL,
        T_LTE,
        T_GTE,
        T_FOREIGN,
        T_FROM,
        T_NOT_EQUAL,
        
        T_CONST,
        T_INT,
        T_FLOAT,
        T_DOUBLE,
        T_BYTE,
        T_BOOLEAN,

        T_IDENTIFIER,
        T_INT_CONST,
        T_CHAR_CONST,
        T_FLOAT_CONST,

        T_INC,
        T_DEC,
        T_DASH_ARROW
    };
}

struct Token {
    Token() { }
    Token(int type, uint32_t line) : type(type), line(line) { }
    ~Token() { }

    int type = 0;
    uint32_t line = 0;

    union {
        int int_const;
        float float_const;
        char char_const;
        char* identifier;
    };
};

class Lexer {
public:
    Lexer(const char* filepath);
    void run();
    void log();

    void log_token(Token& token, uint32_t i);
    void print_type(Token& token);

    inline uint32_t lines() const { return current_line; }
private:
    uint8_t* load(const char* filepath, uint32_t* size);

    void newline();
    void multiline_comment();
    void singleline_comment();
    char incr_char(int32_t off = 1); //Get a character from a specifc offset, initally it is just 1.
    bool is_identifier(char c);
    bool is_spec_char(char c);
    void remove_whitespaces(char* s);

    void create_sym_token();
    void move();
    uint32_t get_type(char c);
    void reset();
public:
    Vec<Token> tokens;
    const char* filepath = nullptr;

    String current;

    uint32_t current_type = 0;

    uint8_t* stream = nullptr;
    uint32_t size = 0;
    
    uint8_t* backtrack_symbol_pos = 0;
    uint32_t nested_comment = 0;

    uint32_t counter = 0;
    uint32_t current_line = 1;
};

#endif // !LEXER_H