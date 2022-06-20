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
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return this->number + obj.number;
    case BOOLEAN: return Object(this->boolean + obj.boolean, BOOLEAN);
    case STRING: return strcat((char*) this->str, (char*) obj.str);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator-(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return this->number - obj.number;
    case BOOLEAN: return Object(this->boolean - obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator*(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return this->number * obj.number;
    case BOOLEAN: return Object(this->boolean * obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator%(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return (number) ((int)this->number % (int)obj.number);
    case BOOLEAN: return Object(this->boolean % obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator/(const Object& obj) {
    this->error |= check_operators(obj);
    this->error |= Object::check_divide_by_zero(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER: return this->number / obj.number;
    case BOOLEAN: return Object(this->boolean / obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator==(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object(this->number == obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean == obj.boolean, BOOLEAN);
    case STRING:  return Object(strcmp(this->str, obj.str), BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator!=(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object(this->number != obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean != obj.boolean, BOOLEAN);
    case STRING:  return Object(!strcmp(this->str, obj.str), BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator>(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object(this->number > obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean > obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator<(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object(this->number < obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean < obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator>=(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object(this->number >= obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean >= obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator<=(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object(this->number <= obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean <= obj.boolean, BOOLEAN);
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
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object(this->number && obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean && obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator||(const Object& obj) {
    this->error |= check_operators(obj);
    if (found_errors()) return Object(this->error);
    switch (this->type) {
    case NUMBER:  return Object(this->number || obj.number, BOOLEAN);
    case BOOLEAN: return Object(this->boolean || obj.boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

Object Object::operator!() {
    switch (this->type) {
    case BOOLEAN: return Object(!this->boolean, BOOLEAN);
    default: unknown_type_error();
    }
    return *this;
}

int Object::check_divide_by_zero(const Object& obj) {
    if (obj.type == NUMBER && obj.number != 0) return OBJ_ERROR_NONE;
    if (obj.type == BOOLEAN && obj.boolean != 0) return OBJ_ERROR_NONE;
    return OBJ_ERROR_DIVIDE_ZERO;
}

int Object::check_operators(const Object& obj) {
    if (this->type == obj.type) return OBJ_ERROR_NONE;
    return OBJ_ERROR_MUST_BE_NUMBERS;
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