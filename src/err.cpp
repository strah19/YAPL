/**
 * @file err.cpp
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
 * Defines eror messages with specific color and formats.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

void fatal_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    printf("\033[0;31mfatal YAPL error: \033[0m");
    vprintf(fmt, args);

    va_end(args);
    exit(EXIT_FAILURE);
}

void report_warning(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    printf("\033[1;33mYAPL warning: \033[0m");    
    vprintf(fmt, args);

    va_end(args);
}

void report_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    printf("\033[0;31mYAPL error: \033[0m");    
    vprintf(fmt, args);

    va_end(args);
}

void begin_custom_error() {
    printf("\033[1;33m");
}

void end_custom_error() {
    printf("\033[0m");
}