/**
 * @file bench.cpp
 * @author strah19
 * @date April 2 2022
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
 * Defines some simple functions for bench marking the speed of certain 
 * parts of the compiler.
 */

#include <time.h>
#include <stdio.h>

static clock_t bench_clock;

/**
 * 
 * 
 * 
 */
void begin_debug_benchmark() {
    bench_clock = clock();
}

/**
 * 
 * 
 * 
 */
float end_debug_benchmark(const char* label) {
    clock_t end = clock();
    double time_spent = (double)(end - bench_clock);
    printf("Benchmark time for %s is %f ms.\n", label, time_spent);

    return (float) time_spent;
}