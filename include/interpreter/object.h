#ifndef OBJECT_H
#define OBJECT_H

#include <map>

enum {
    NUMBER,
    STRING,
    BOOLEAN,
    NONE,
};

enum {
    OBJ_ERROR_NONE,
    OBJ_ERROR_UNKNOWN_TYPE,
    OBJ_ERROR_MUST_BE_NUMBERS,
    OBJ_ERROR_DIVIDE_ZERO,
    OBJ_ERROR_NEGATE,
    OBJ_ERROR_UNDEFINED_VAR,
    OBJ_ERROR_UNKNOWN_OPERATOR,
    OBJ_ERROR_UNDEFINED_FUNC,
    OBJ_ERROR_RETURN_IS_NULL,
    OBJ_ERROR_RETURN_FULL,
    OBJ_ERROR_PARAMS,
    OBJ_ERROR_WRONG_TYPE
};

static std::map<int, const char*> OBJ_ERROR_MESSAGES = {
    { OBJ_ERROR_NONE, "No error found in object" },
    { OBJ_ERROR_UNKNOWN_TYPE, "Unknown type found" },
    { OBJ_ERROR_MUST_BE_NUMBERS, "Operands in expression must be numbers" },
    { OBJ_ERROR_DIVIDE_ZERO, "Cannot divide by zero" },
    { OBJ_ERROR_NEGATE, "Type cannot be negated" },
    { OBJ_ERROR_UNDEFINED_VAR, "Undefined variable" }, 
    { OBJ_ERROR_UNKNOWN_OPERATOR, "Unknown operator found in expression" },
    { OBJ_ERROR_UNDEFINED_FUNC, "Undefined function" },
    { OBJ_ERROR_RETURN_IS_NULL, "Function was returned with nothing when it has a return type" },
    { OBJ_ERROR_RETURN_FULL, "Function was returned with expression when return type is void" },
    { OBJ_ERROR_PARAMS, "Function arguments do not match the paramters" },
    { OBJ_ERROR_WRONG_TYPE, "Types do not match" }
};

struct Object {
    Object(double number) : number(number), type(NUMBER), error(OBJ_ERROR_NONE) { }
    Object(const char* str) : str(str), type(STRING), error(OBJ_ERROR_NONE) { }
    Object(bool boolean, int type) : boolean(boolean), type(type), error(OBJ_ERROR_NONE) { }
    Object(int error) : error(error) { }
    Object() { }

    union {
        double number;
        const char* str;
        bool boolean;
    };
    int type = NONE;
    int error = OBJ_ERROR_NONE;

    bool mutability = true;

    Object operator+(const Object& obj);
    Object operator-(const Object& obj);
    Object operator*(const Object& obj);
    Object operator/(const Object& obj);
    Object operator-();
    Object operator==(const Object& obj);
    Object operator!=(const Object& obj);
    Object operator>(const Object& obj);
    Object operator<(const Object& obj);
    Object operator>=(const Object& obj);
    Object operator<=(const Object& obj);
    Object operator&&(const Object& obj);
    Object operator||(const Object& obj);
    Object operator!();

    static int check_divide_by_zero(const Object& obj);
    int check_operators(const Object& obj);
    bool found_errors();
    void unknown_type_error();
    void cannot_negate_type_error();
};

#endif // !OBJECT_H