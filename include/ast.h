#ifndef AST_H
#define AST_H

#include "common.h"

#include <vector>

enum {
    AST_EXPRESSION,
    AST_UNARY,
    AST_PRIMARY,
    AST_BINARY,
    AST_DECLERATION,
    AST_VAR_DECLERATION,
    AST_ASSIGNMENT,
    AST_STATEMENT,
    AST_PRINT,
    AST_EXPRESSION_STATEMENT,
    AST_SCOPE,
    AST_TRANSLATION_UNIT
};

enum {
    AST_OPERATOR_MULTIPLICATIVE,
    AST_OPERATOR_DIVISION,
    AST_OPERATOR_MODULO,
    AST_OPERATOR_ADD,
    AST_OPERATOR_SUB,
    AST_OPERATOR_COMPARITIVE_EQUAL,
    AST_OPERATOR_COMPARITIVE_NOT_EQUAL,
    AST_OPERATOR_LTE,
    AST_OPERATOR_GTE,
    AST_OPERATOR_LT,
    AST_OPERATOR_GT,
    AST_OPERATOR_AND,
    AST_OPERATOR_OR,
    AST_OPERATOR_POWER,
    AST_OPERATOR_NONE
};

enum {
    AST_UNARY_MINUS,
    AST_UNARY_NONE
};

enum {
    AST_INT,
    AST_FLOAT,
    AST_BOOLEAN,
    AST_STRING,
    AST_NESTED,
    AST_ID,
    AST_TYPE_NONE
};

struct Ast {
	int type = 0;
};

struct Ast_Expression : Ast {
    Ast_Expression() { type = AST_EXPRESSION; }
};

struct Ast_PrimaryExpression : public Ast_Expression {
    Ast_PrimaryExpression() { type = AST_PRIMARY; }
    Ast_PrimaryExpression(const char* ident) : ident(ident), type_value(AST_ID) { type = AST_PRIMARY; }
    int type_value = AST_TYPE_NONE;

    union {
        double float_const;
        int int_const;
        const char* ident;
        const char* string;
        bool boolean;
        
        Ast_Expression* nested;
    };
};

struct Ast_BinaryExpression : public Ast_Expression {
    Ast_BinaryExpression() { type = AST_BINARY; }
    Ast_BinaryExpression(Ast_Expression* left, int op, Ast_Expression* right) : left(left), op(op), right(right) { type = AST_BINARY; }

    int op = AST_OPERATOR_NONE;

    Ast_Expression* left = nullptr;
    Ast_Expression* right = nullptr;

    Ast_BinaryExpression(const Ast_BinaryExpression& bin) {
        op = bin.op;
        left = bin.left;
        right = bin.right;
    }
};

struct Ast_UnaryExpression : public Ast_Expression {
    Ast_UnaryExpression() { type = AST_UNARY; }
    Ast_UnaryExpression(Ast_Expression* next, int op) : op(op), next(next) { type = AST_UNARY; }

    Ast_Expression* next = nullptr;
    int op = AST_UNARY_NONE;
};

struct Ast_Assignment : public Ast_Expression {
    Ast_Assignment() { type = AST_ASSIGNMENT; }
    Ast_Assignment(Ast_Expression* expression, const char* id) : expression(expression), id(id) { type = AST_ASSIGNMENT; }

    const char* id = nullptr;
    Ast_Expression* expression = nullptr;
};

struct Ast_Decleration : public Ast {
    Ast_Decleration() { type = AST_DECLERATION; }
};

struct Ast_Statement : public Ast_Decleration {
    Ast_Statement() { type = AST_STATEMENT; }
};

struct Ast_ExpressionStatement : public Ast_Statement {
    Ast_ExpressionStatement(Ast_Expression* expression) : expression(expression) { type = AST_EXPRESSION_STATEMENT; }

    Ast_Expression* expression = nullptr;
};

struct Ast_PrintStatement : public Ast_Statement {
    Ast_PrintStatement(Ast_Expression* expression) : expression(expression) { type = AST_PRINT; }

    Ast_Expression* expression = nullptr;
};

struct Ast_Scope : public Ast_Statement {
    Ast_Scope() { type = AST_SCOPE; }

    std::vector<Ast_Decleration*> declerations;
};

struct Ast_VarDecleration : public Ast_Decleration {
    Ast_VarDecleration() { type = AST_VAR_DECLERATION; }
    Ast_VarDecleration(const char* ident, Ast_Expression* expression, int type_value) : ident(ident), expression(expression), type_value(type_value) { type = AST_VAR_DECLERATION; }

    int type_value = AST_TYPE_NONE;
    const char* ident = nullptr;

    Ast_Expression* expression = nullptr;
};

struct Ast_TranslationUnit : public Ast {
    Ast_TranslationUnit() { type = AST_TRANSLATION_UNIT; }

    std::vector<Ast_Decleration*> declerations;
};

#define AST_DELETE(type) delete type

#define AST_CAST(type, base) static_cast<type*>(base)

#endif // !AST_H