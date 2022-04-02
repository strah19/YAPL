/**
 * @file main.cpp
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
 * Is the entry point for this project.
 */
#include <stdio.h>
#include "config.h"
#include "lexer.h"
#include "vec.h"
#include "bench.h"

int main() {
    Lexer lex("../tests/3.25.22/lexical_test.yapl");

    printf("started lexing...\n");
    begin_debug_benchmark();
    lex.run();
    lex.log();
    end_debug_benchmark("lexer");
    printf("finished lexing %d lines of code...\n", lex.lines());

    return 0;
}