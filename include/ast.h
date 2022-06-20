#ifndef AST_H
#define AST_H

#include "common.h"
#include "lexer.h"

#include <vector>

using number = double;

enum {
    AST_EXPRESSION,
    AST_UNARY,
    AST_PRIMARY,
    AST_BINARY,
    AST_DECLERATION,
    AST_VAR_DECLERATION,
    AST_FUNC_DECLERATION,
    AST_ASSIGNMENT,
    AST_STATEMENT,
    AST_PRINT,
    AST_EXPRESSION_STATEMENT,
    AST_SCOPE,
    AST_CONDITIONAL,
    AST_IF,
    AST_ELIF,
    AST_ELSE,
    AST_FOR,
    AST_WHILE,
    AST_RETURN,
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
    AST_UNARY_NOT,
    AST_UNARY_NONE
};

enum {
    AST_EQUAL,
    AST_EQUAL_PLUS,
    AST_EQUAL_MINUS,
    AST_EQUAL_MULTIPLY,
    AST_EQUAL_DIVIDE
};

enum {
    AST_FLOAT,
    AST_BOOLEAN,
    AST_STRING,
    AST_NESTED,
    AST_ID,
    AST_FUNC_CALL,
    AST_VOID,
    AST_TYPE_NONE
};

enum {
    AST_SPECIFIER_NONE  = 0x00,
    AST_SPECIFIER_CONST = 0x01
};

struct Ast;
struct Ast_Expression;
struct Ast_Scope;

struct Ast {
    Ast() { }

	int type = 0;
    uint32_t line = 0;
    const char* file;
};

struct Ast_Expression : Ast {
    Ast_Expression() { type = AST_EXPRESSION; }
};

struct Ast_FunctionCall {
    Ast_FunctionCall(const char* ident, const std::vector<Ast_Expression*>& args) : ident(ident), args(args) { }

    const char* ident;
    std::vector<Ast_Expression*> args;
};

struct Ast_PrimaryExpression : public Ast_Expression {
    Ast_PrimaryExpression() { type = AST_PRIMARY; }
    Ast_PrimaryExpression(const char* ident) : ident(ident), type_value(AST_ID) { type = AST_PRIMARY; }
    Ast_PrimaryExpression(number float_const) : float_const(float_const), type_value(AST_FLOAT) { type = AST_PRIMARY; }
    int type_value = AST_TYPE_NONE;

    union {
        number float_const;
        const char* ident;
        const char* string;
        bool boolean;
        
        Ast_FunctionCall* call;
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
    Ast_Assignment(Ast_Expression* expression, const char* id, int equal_type = AST_EQUAL) : expression(expression), id(id), equal_type(equal_type) { type = AST_ASSIGNMENT; }

    int equal_type = AST_EQUAL;
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
    Ast_PrintStatement(const std::vector<Ast_Expression*>& expressions) : expressions(expressions) { type = AST_PRINT; }

    std::vector<Ast_Expression*> expressions;
};

struct Ast_ConditionalStatement : public Ast_Statement {
    Ast_ConditionalStatement() { type = AST_CONDITIONAL; }
    Ast_ConditionalStatement(Ast_Expression* condition, Ast_Scope* scope) : condition(condition), scope(scope) { type = AST_CONDITIONAL; }

    Ast_Expression* condition = nullptr;
    Ast_Scope* scope = nullptr;
    Ast_ConditionalStatement* next = nullptr;
};

struct Ast_IfStatement : Ast_ConditionalStatement {
    Ast_IfStatement() { type = AST_IF; }
    Ast_IfStatement(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_IF; }
};

struct Ast_ElifStatement : Ast_ConditionalStatement {
    Ast_ElifStatement() { type = AST_ELIF; }
    Ast_ElifStatement(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_ELIF; }
};

struct Ast_ElseStatement : Ast_ConditionalStatement {
    Ast_ElseStatement() { type = AST_ELSE; }
    Ast_ElseStatement(Ast_Scope* scope) : Ast_ConditionalStatement(nullptr, scope) { type = AST_ELSE; }
};

struct Ast_WhileLoop : Ast_ConditionalStatement {
    Ast_WhileLoop() { type = AST_WHILE; }
    Ast_WhileLoop(Ast_Expression* condition, Ast_Scope* scope) : Ast_ConditionalStatement(condition, scope) { type = AST_WHILE; }
};

struct Ast_ForLoop : Ast_ConditionalStatement {
    Ast_ForLoop() { type = AST_FOR; }
    Ast_ForLoop(Ast_Expression* condition, Ast_Scope* scope, Ast_Expression* change, Ast_Decleration* decleration) :
        Ast_ConditionalStatement(condition, scope), change(change), decleration(decleration) { type = AST_FOR; }

    Ast_Expression* change = nullptr;
    Ast_Decleration* decleration = nullptr;
};

struct Ast_Scope : public Ast_Statement {
    Ast_Scope() { type = AST_SCOPE; }

    std::vector<Ast_Decleration*> declerations;
};

struct Ast_VarDecleration : public Ast_Decleration {
    Ast_VarDecleration() { type = AST_VAR_DECLERATION; }
    Ast_VarDecleration(const char* ident, Ast_Expression* expression, int type_value, int specifiers) 
        : ident(ident), expression(expression), type_value(type_value), specifiers(specifiers) { type = AST_VAR_DECLERATION; }

    int type_value = AST_TYPE_NONE;
    int specifiers = AST_SPECIFIER_NONE;
    const char* ident = nullptr;

    Ast_Expression* expression = nullptr;
};

struct Ast_FuncDecleration : public Ast_Decleration {
    Ast_FuncDecleration() { type = AST_FUNC_DECLERATION; }
    Ast_FuncDecleration(const char* ident, int return_type, const std::vector<Ast_VarDecleration*> args, Ast_Scope* scope) : 
        ident(ident), return_type(return_type), args(args), scope(scope) { type = AST_FUNC_DECLERATION; }
        
    const char* ident = nullptr;
    int return_type = AST_VOID;
    std::vector<Ast_VarDecleration*> args;
    Ast_Scope* scope = nullptr;
};

struct Ast_ReturnStatement : Ast_Statement {
    Ast_ReturnStatement(Ast_Expression* expression) : expression(expression) { type = AST_RETURN; }

    Ast_Expression* expression = nullptr;
};

struct Ast_TranslationUnit : public Ast {
    Ast_TranslationUnit() { type = AST_TRANSLATION_UNIT; }

    std::vector<Ast_Decleration*> declerations;
};

#define AST_DELETE(type) delete type

#define AST_CAST(type, base) static_cast<type*>(base)

#endif // !AST_H