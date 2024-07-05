// Author: Narottam Viswas

#include <stdint.h>
#include <string.h>

#include "runtime.h"

#include "kernel/auto_correlation3d.h"

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
extern uint64_t depth;

// Matrices for benchmarks
extern int64_t a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t c[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
// Golden matrix
extern int64_t g[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

void print_matrix_3d(const char *name, const int64_t *matrix, uint64_t rows, uint64_t cols, uint64_t depth)
{
    printf("%s:\n", name);
    for (uint64_t i = 0; i < rows; i++)
    {
        for (uint64_t j = 0; j < cols; j++)
        {
            for (uint64_t k = 0; k < depth; k++)
            {
                printf("%ld ", matrix[i * cols * depth + j * depth + k]);
            }
            printf(" | ");
        }
        printf("\n");
    }
    printf("\n");
}

int main()
{
    printf("\n");
    printf("=========================\n");
    printf("=  auto_correlation_3D  =\n");
    printf("=========================\n");
    printf("\n");
    printf("\n");

    // Print matrix sizes
    printf("Matrix A: %lux%lux%lu\n", rows, cols, depth);
    printf("Matrix C: %lux%lux%lu\n", 2 * rows - 1, 2 * cols - 1, 2 * depth - 1);
    printf("\n");

    // Print initial matrices
    print_matrix_3d("Matrix A", a, rows, cols, depth);
    print_matrix_3d("Matrix C (initial)", c, 2 * rows - 1, 2 * cols - 1, 2 * depth - 1);
    print_matrix_3d("Matrix G (golden)", g, 2 * rows - 1, 2 * cols - 1, 2 * depth - 1);

    for (uint64_t avl_rows = 4; avl_rows <= rows; avl_rows *= 2)
    {
        for (uint64_t avl_cols = 4; avl_cols <= cols; avl_cols *= 2)
        {
            for (uint64_t avl_depth = 4; avl_depth <= depth; avl_depth *= 2)
            {
                int64_t count_cycle;
                printf("Calculating auto_correlation with rows = %ld, cols = %ld, depth = %ld\n", avl_rows, avl_cols, avl_depth);

                // Vector implementation
                count_cycle = -get_cycle_count();
                auto_correlation_3d_v64b(c, a, avl_rows, avl_cols, avl_depth);
                count_cycle += get_cycle_count();
                printf("\nThe vector execution took %ld cycles.\n", count_cycle);

                print_matrix_3d("Matrix C (vector result)", c, 2 * avl_rows - 1, 2 * avl_cols - 1, 2 * avl_depth - 1);

                // Scalar implementation
                count_cycle = -get_cycle_count();
                auto_correlation_3d_s64b(c, a, avl_rows, avl_cols, avl_depth);
                count_cycle += get_cycle_count();
                printf("The scalar execution took %ld cycles.\n\n", count_cycle);

                print_matrix_3d("Matrix C (scalar result)", c, 2 * avl_rows - 1, 2 * avl_cols - 1, 2 * avl_depth - 1);
            }
        }
    }

    return 0;
}
