// Author: Narottam Viswas

#include <stdint.h>
#include <string.h>

#include "runtime.h"

#include "kernel/auto_correlation2d.h"

#ifndef SPIKE
#include "printf.h"
#else
#include <stdio.h>
#endif

// Run also the scalar benchmark
#define SCALAR 1

// Check the vector results against golden vectors
#define CHECK 1

// Matrix dimensions
extern uint64_t rows;
extern uint64_t cols;

// Matrices for benchmarks
extern int64_t a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t c[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
// Golden matrix
extern int64_t g[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

void print_matrix(const char *name, const int64_t *matrix, uint64_t rows, uint64_t cols)
{
    printf("%s:\n", name);
    for (uint64_t i = 0; i < rows; i++)
    {
        for (uint64_t j = 0; j < cols; j++)
        {
            printf("%ld ", matrix[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main()
{
    printf("\n");
    printf("=========================\n");
    printf("=  auto_correlation_2D  =\n");
    printf("=========================\n");
    printf("\n");
    printf("\n");

    // Print matrix sizes
    printf("Matrix A: %lux%lu\n", rows, cols);
    printf("Matrix C: %lux%lu\n", 2 * rows - 1, 2 * cols - 1);
    printf("\n");

    // Print initial matrices
    print_matrix("Matrix A", a, rows, cols);
    print_matrix("Matrix C (initial)", c, 2 * rows - 1, 2 * cols - 1);
    print_matrix("Matrix G (golden)", g, 2 * rows - 1, 2 * cols - 1);

    for (uint64_t avl_rows = 4; avl_rows <= rows; avl_rows *= 2)
    {
        for (uint64_t avl_cols = 4; avl_cols <= cols; avl_cols *= 2)
        {
            int64_t count_cycle;
            printf("Calculating auto_correlation with rows = %ld, cols = %ld\n", avl_rows, avl_cols);

            // Vector implementation
            count_cycle = -get_cycle_count();
            auto_correlation_2d_v64b(c, a, avl_rows, avl_cols);
            count_cycle += get_cycle_count();
            printf("\nThe vector execution took %ld cycles.\n", count_cycle);

            // print_matrix("Matrix C (vector result)", c, 2 * avl_rows - 1, 2 * avl_cols - 1);

            // Scalar implementation
            count_cycle = -get_cycle_count();
            auto_correlation_2d_s64b(c, a, avl_rows, avl_cols);
            count_cycle += get_cycle_count();
            printf("The scalar execution took %ld cycles.\n\n", count_cycle);

            // print_matrix("Matrix C (scalar result)", c, 2 * avl_rows - 1, 2 * avl_cols - 1);
        }
    }

    return 0;
}
