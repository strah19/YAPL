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
#include "../config.h"
#include "lexer.h"
#include "parser.h"
#include "bench.h"
#include "err.h"
#include "interpreter.h"

// If USE_VM is defined, YAPL will use the VM otherwise it will use the interpreter.
#ifdef USE_VM
    #include "vm.h"
#endif

int main(int argc, char* argv[]) {
    printf("USING YAPL VERSION %d.%d\n", YAPL_VERSION_MAJOR, YAPL_VERSION_MINOR);
    if (!argv[1])
        fatal_error("no input file found.\n");
    Lexer lex(argv[1]);

    printf("started lexing...\n");
    begin_debug_benchmark();
    lex.lex();
    if (argv[2] && strcmp(argv[2], "-log") == 0)
        lex.log();
    end_debug_benchmark("lexer");
    printf("finished lexing %d lines of code...\n", lex.lines());
    
    Parser parser(&lex);
    parser.parse();

#ifdef USE_VM
    vm::run();
#else
    Interpreter interpreter;
    interpreter.interpret(parser.translation_unit());
#endif

    return 0;
}