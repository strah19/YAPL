#ifndef LEXER_H
#define LEXER_H

#include "common.h"

#include <string>
#include <vector>

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
        T_NOT = '~',
        T_LINE = '|',
        
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
        T_QUOTE = '"',

        T_EOF = 255,

        T_AND,
        T_OR,
        T_IF,
        T_ELIF,
        T_ELSE,
        T_WHILE,
        T_BREAK,
        T_FOR,
        T_RETURN,
        T_CONTINUE,
        T_COMPARE_EQUAL,
        T_LTE,
        T_GTE,
        T_NOT_EQUAL,
        T_EQUAL_PLUS,
        T_EQUAL_MINUS,
        T_EQUAL_STAR,
        T_EQUAL_SLASH,
        T_EQUAL_MOD,
        T_FALSE,
        T_REMIT,
        T_TRUE,
        T_CHAR_CONST,
        T_CAST,
        T_FUNC,
        
        T_FLOAT,
        T_BOOLEAN,
        T_CHAR,
        T_INT,
        T_INT_CONST,
        T_STRING,
        T_STRING_CONST,
        T_CONSTANT,
        T_BIT_LEFT,
        T_BIT_RIGHT,

        T_IDENTIFIER,
        T_FLOAT_CONST,
        T_PRINT, //temp??
        T_INPUT, //temp??

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
        double float_const;
        char char_const;
        char* identifier;
        char* string;
    };
};

class Lexer {
public:
    Lexer(const char* filepath);
    void lex();
    void log();
    const char* file() { return filepath; }

    void        log_token(Token& token, uint32_t i);
    static void print_token(Token& token);
    static void print_from_type(int type);
    static void log_keywords_symbols(int type);

    std::vector<Token>* fetch_tokens() { return &tokens; }

    inline uint32_t lines() const { return current_line; }
private:
    void load();

    void newline();
    void multiline_comment_beg();
    void multiline_comment_end();
    void singleline_comment();
    char incr_char(int32_t off = 1); //Get a character from a specifc offset, initally it is just 1.
    bool is_identifier(char c);
    bool is_spec_char(char c);
    void remove_whitespaces(char* s);

    void     create_sym_token();
    void     move();
    uint32_t get_type(char c);
    void     reset();

public:
    std::vector<Token> tokens;
    const char* filepath = nullptr;

    std::string current;

    uint32_t current_type = 0;
    uint32_t current_index = 0;

    std::string stream;
    uint32_t size = 0;
    
    uint32_t nested_comment = 0;

    uint32_t counter = 0;
    uint32_t current_line = 1;
};

#endif // !LEXER_H