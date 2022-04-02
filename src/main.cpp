#include <iostream>
#include "config.h"
#include "lexer.h"
#include "vec.h"

int main() {
    std::cout << "running YAPL v" << YAPL_VERSION_MAJOR << '.' << YAPL_VERSION_MINOR << std::endl;

    Lexer lex("../tests/3.25.22/lexical_test.yapl");

    printf("started lexing...\n");
    lex.run();
    lex.log();
    printf("finished lexing %d lines of code...\n", lex.lines());

    return 0;
}