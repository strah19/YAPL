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
#include "bench.h"
#include "parser.h"
#include "err.h"

int main(int argc, char* argv[]) {
    if (!argv[1])
        fatal_error("no input file found.\n");
    Lexer lex(argv[1]);

    printf("started lexing...\n");
    begin_debug_benchmark();
    lex.run();
    if (argv[2] && strcmp(argv[2], "-log") == 0)
        lex.log();
    end_debug_benchmark("lexer");
    printf("finished lexing %d lines of code...\n", lex.lines());
    
    printf("started parsing...\n");
    begin_debug_benchmark();
    Parser parser(lex.fetch_tokens());
    parser.parse();
    parser.visualize();
    end_debug_benchmark("parser");

    return 0;
}