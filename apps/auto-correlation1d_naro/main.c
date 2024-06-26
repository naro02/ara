// Author: Narottam Viswas

#include <stdint.h>
#include <string.h>

#include "runtime.h"

#include "kernel/auto_correlation1d.h"

#ifndef SPIKE
#include "printf.h"
#else
#include <stdio.h>
#endif

// Run also the scalar benchmark
#define SCALAR 1

// Check the vector results against golden vectors
#define CHECK 1

// Vector size (Byte)
extern uint64_t vsize;

// Vectors for benchmarks
extern int64_t a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t c[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
// Golden vector
extern int64_t g[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

void print_vector(const char *name, int64_t *vector, uint64_t length)
{
    printf("%s:\n", name);
    for (uint64_t i = 0; i < length; i++)
    {
        printf("%ld ", vector[i]);
    }
    printf("\n");
}

int main()
{
    printf("\n");
    printf("=========================\n");
    printf("=  auto_correlation_1D  =\n");
    printf("=========================\n");
    printf("\n");
    printf("\n");

    for (uint64_t avl = 8; avl <= vsize; avl *= 8)
    {
        int64_t count_cycle;
        printf("Calculating auto_correlation with avl = %ld \n", avl);
        count_cycle = -get_cycle_count();
        auto_correlation_v64b(c, a, avl);
        count_cycle += get_cycle_count();
        printf("\nThe vector execution took %ld cycles.\n", count_cycle);

        // print_vector("Vector a", a, avl);
        // print_vector("Vector c (result)", c, 2 * avl - 1);

        count_cycle = -get_cycle_count();
        auto_correlation_s64b(c, a, avl);
        count_cycle += get_cycle_count();
        printf("The scalar execution took %ld cycles.\n\n", count_cycle);

        // // Print vectors
        // print_vector("Vector a", a, avl);
        // print_vector("Vector c (result)", c, 2 * avl - 1);
    }

    return 0;
}
