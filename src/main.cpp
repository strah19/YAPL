#include <iostream>
#include "config.h"
#include "lexer.h"
#include "vec.h"
#include "bench.h"

int main() {
    std::cout << "running YAPL v" << YAPL_VERSION_MAJOR << '.' << YAPL_VERSION_MINOR << std::endl;

    Lexer lex("../tests/3.25.22/lexical_test.yapl");

    printf("started lexing...\n");
    begin_debug_benchmark();
    lex.run();
    lex.log();
    end_debug_benchmark("lexer");
    printf("finished lexing %d lines of code...\n", lex.lines());

    return 0;
}