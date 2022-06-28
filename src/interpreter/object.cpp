/**
 * @file object.cpp
 * @author strah19
 * @date May 31 2022
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
 * Defines the objects for interpreter.
 */

#include "object.h"
#include "interpreter.h"

Object Object::operator+(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_float(this->float_const + obj.float_const);
    case INT:     return Object::init_int(this->int_const + obj.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean + o.boolean);
    case STRING:  return Object::init_str(strcat((char*) this->str, (char*) o.str));
    case CHAR:    return Object::init_char(this->char_const + o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator-(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_float(this->float_const - o.float_const);
    case INT:     return Object::init_int(this->int_const - obj.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean - o.boolean);
    case CHAR:    return Object::init_char(this->char_const - o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator*(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_float(this->float_const * o.float_const);
    case INT:     return Object::init_int(this->int_const * obj.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean * o.boolean);
    case CHAR:    return Object::init_char(this->char_const * o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator%(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case INT:     return Object::init_int(this->int_const % obj.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean % o.boolean);
    case CHAR:    return Object::init_char(this->char_const % o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator/(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    this->error |= Object::check_divide_by_zero(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_float(this->float_const / o.float_const);
    case INT:     return Object::init_int(this->int_const / obj.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean / o.boolean);
    case CHAR:    return Object::init_char(this->char_const / o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator==(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_bool(this->float_const == o.float_const);
    case INT:     return Object::init_bool(this->int_const == obj.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean == o.boolean);
    case STRING:  return Object::init_bool(strcmp(this->str, o.str));
    case CHAR:    return Object::init_bool(this->char_const == o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator!=(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_bool(this->float_const != o.float_const);
    case INT:     return Object::init_bool(this->int_const != o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean != o.boolean);
    case STRING:  return Object::init_bool(!strcmp(this->str, o.str));
    case CHAR:    return Object::init_bool(this->char_const != o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator>(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_bool(this->float_const > o.float_const);
    case INT:     return Object::init_bool(this->int_const > o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean > o.boolean);
    case CHAR:    return Object::init_bool(this->char_const > o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator<(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_bool(this->float_const < o.float_const);
    case INT:     return Object::init_bool(this->int_const < o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean < o.boolean);
    case CHAR:    return Object::init_bool(this->char_const < o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator>=(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_bool(this->float_const >= o.float_const);
    case INT:     return Object::init_bool(this->int_const >= o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean >= o.boolean);
    case CHAR:    return Object::init_bool(this->char_const >= o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator<=(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_bool(this->float_const <= o.float_const);
    case INT:     return Object::init_bool(this->int_const <= o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean <= o.boolean);
    case CHAR:    return Object::init_bool(this->char_const <= o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator-() {
    switch (this->type) {
        case FLOAT: return -this->float_const;
        case INT:   return -this->int_const;
        default: cannot_negate_type_error();
    }
    return *this;
}

Object Object::operator&&(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_bool(this->float_const && o.float_const);
    case INT:     return Object::init_bool(this->int_const && o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean && o.boolean);
    case CHAR:    return Object::init_bool(this->char_const && o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator||(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case FLOAT:   return Object::init_bool(this->float_const || o.float_const);
    case INT:     return Object::init_bool(this->int_const || o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean || o.boolean);
    case CHAR:    return Object::init_bool(this->char_const || o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator!() {
    switch (this->type) {
    case FLOAT:   return Object::init_bool(!this->float_const);
    case INT:     return Object::init_bool(!this->int_const);
    case BOOLEAN: return Object::init_bool(!this->boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator&(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case INT:     return Object::init_bool(this->int_const & o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean & o.boolean);
    case CHAR:    return Object::init_bool(this->char_const & o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator|(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case INT:     return Object::init_bool(this->int_const | o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean | o.boolean);
    case CHAR:    return Object::init_bool(this->char_const | o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator^(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case INT:     return Object::init_bool(this->int_const ^ o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean ^ o.boolean);
    case CHAR:    return Object::init_bool(this->char_const ^ o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator~() {
    switch (this->type) {
    case INT:     return Object::init_bool(~this->int_const);
    case BOOLEAN: return Object::init_bool(~this->boolean);
    case CHAR:    return Object::init_bool(~this->char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator<<(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case INT:     return Object::init_bool(this->int_const << o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean << o.boolean);
    case CHAR:    return Object::init_bool(this->char_const << o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator>>(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case INT:     return Object::init_bool(this->int_const >> o.int_const);
    case BOOLEAN: return Object::init_bool(this->boolean >> o.boolean);
    case CHAR:    return Object::init_bool(this->char_const >> o.char_const);
    default: unknown_type_error();
    }
    return *this;
}

int Object::check_divide_by_zero(const Object& obj) {
    if (obj.type == FLOAT && obj.float_const != 0) return OBJ_ERROR_NONE;
    if (obj.type == INT && obj.int_const != 0)     return OBJ_ERROR_NONE;
    if (obj.type == BOOLEAN && obj.boolean != 0)   return OBJ_ERROR_NONE;
    if (obj.type == CHAR && obj.char_const != 0)   return OBJ_ERROR_NONE;
    return OBJ_ERROR_DIVIDE_ZERO;
}

int Object::check_operators(Object& obj) {
    if (this->type == obj.type) return OBJ_ERROR_NONE;
    return OBJ_ERROR_MUST_BE_NUMBERS;
}

int Object::convert(Object& obj) {
    if (this->type == obj.type) return OBJ_ERROR_NONE;

    if (obj.type == INT) {
        if (this->type == CHAR) {
            this->type = INT;
            this->int_const = (int) this->char_const;
        }
        else if (this->type == FLOAT) {
            this->type = INT;
            this->int_const = (int) this->float_const;
        }
        else
            return OBJ_ERROR_CONVERT;
    }
    else if (obj.type == CHAR) {
        if (this->type == INT) {
            this->type = CHAR;
            this->char_const = (char) this->int_const;
        }
        else
            return OBJ_ERROR_CONVERT;
    }
    else if (obj.type == FLOAT) {
        if (this->type == INT) {
            this->type = FLOAT;
            this->float_const = (float) this->int_const;
        }
        else
            return OBJ_ERROR_CONVERT;
    }
    else
        return OBJ_ERROR_CONVERT;

    return OBJ_ERROR_NONE;
}

bool Object::found_errors() {
    return (this->error != OBJ_ERROR_NONE);
}

void Object::unknown_type_error() {
    this->error = OBJ_ERROR_UNKNOWN_TYPE;
}

void Object::cannot_negate_type_error() {
    this->error = OBJ_ERROR_NEGATE;
}