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
    case NUMBER:  return this->number + obj.number;
    case BOOLEAN: return Object::init_bool(this->boolean + o.boolean);
    case STRING:  return strcat((char*) this->str, (char*) o.str);
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
    case NUMBER:  return this->number - o.number;
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
    case NUMBER:  return this->number * o.number;
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
    case NUMBER:  return (_number) ((int)this->number % (int)o.number);
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
    case NUMBER:  return this->number / o.number;
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
    case NUMBER:  return Object::init_bool(this->number == o.number);
    case BOOLEAN: return Object::init_bool(this->boolean == o.boolean);
    case STRING:  return Object::init_bool(strcmp(this->str, o.str));
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator!=(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object::init_bool(this->number != o.number);
    case BOOLEAN: return Object::init_bool(this->boolean != o.boolean);
    case STRING:  return Object::init_bool(!strcmp(this->str, o.str));
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator>(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object::init_bool(this->number > o.number);
    case BOOLEAN: return Object::init_bool(this->boolean > o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator<(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object::init_bool(this->number < o.number);
    case BOOLEAN: return Object::init_bool(this->boolean < o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator>=(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object::init_bool(this->number >= o.number);
    case BOOLEAN: return Object::init_bool(this->boolean >= o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator<=(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object::init_bool(this->number <= o.number);
    case BOOLEAN: return Object::init_bool(this->boolean <= o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator-() {
    switch (this->type) {
        case NUMBER: return -this->number;
        default: cannot_negate_type_error();
    }
    return *this;
}

Object Object::operator&&(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object::init_bool(this->number && o.number);
    case BOOLEAN: return Object::init_bool(this->boolean && o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator||(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object::init_bool(this->number || o.number);
    case BOOLEAN: return Object::init_bool(this->boolean || o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator!() {
    switch (this->type) {
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
    case NUMBER: return (_number) ((int)this->number & (int)o.number);
    case BOOLEAN: return Object::init_bool(this->boolean & o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator|(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return (_number) ((int)this->number | (int)o.number);
    case BOOLEAN: return Object::init_bool(this->boolean | o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator^(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return (_number) ((int)this->number ^ (int)o.number);
    case BOOLEAN: return Object::init_bool(this->boolean ^ o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator~() {
    switch (this->type) {
    case NUMBER: return (_number) (~(int)(this->number));
    case BOOLEAN: return Object::init_bool(~this->boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator<<(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return (_number) ((int)this->number << (int)o.number);
    case BOOLEAN: return Object::init_bool(this->boolean << o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator>>(const Object& obj) {
    Object o = obj;
    this->error |= check_operators(o);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return (_number) ((int)this->number >> (int)o.number);
    case BOOLEAN: return Object::init_bool(this->boolean >> o.boolean);
    default: unknown_type_error();
    }
    return *this;
}

int Object::check_divide_by_zero(const Object& obj) {
    if (obj.type == NUMBER && obj.number != 0) return OBJ_ERROR_NONE;
    if (obj.type == BOOLEAN && obj.boolean != 0) return OBJ_ERROR_NONE;
    if (obj.type == CHAR && obj.char_const != 0) return OBJ_ERROR_NONE;
    return OBJ_ERROR_DIVIDE_ZERO;
}

int Object::check_operators(Object& obj) {
    if (this->type == obj.type) return OBJ_ERROR_NONE;
    else if (this->type == CHAR && obj.type == NUMBER) {
        obj.type = CHAR;
        obj.char_const = (int) obj.number;
        return OBJ_ERROR_NONE;
    }
    else if (this->type == NUMBER && obj.type == CHAR) {
        this->type = CHAR;
        this->char_const = (int) this->number;
        return OBJ_ERROR_NONE;
    }
    return OBJ_ERROR_MUST_BE_NUMBERS;
}

int Object::convert(Object& obj) {
    if (this->type == obj.type) return OBJ_ERROR_NONE;

    if (obj.type == NUMBER) {
        if (this->type == CHAR) {
            this->type = NUMBER;
            this->number = (_number) ((int) this->char_const);
            return OBJ_ERROR_NONE;
        }
    }
    else if (obj.type == CHAR) {
        if (this->type == NUMBER) {
            this->type = CHAR;
            this->char_const = (char) ((int) this->number);
            return OBJ_ERROR_NONE;
        }
    }

    return OBJ_ERROR_CONVERT;
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