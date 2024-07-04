#include "auto_correlation2d.h"

#ifndef SPIKE
#include "printf.h"
#else
#include <stdio.h>
#endif

// Helper function to print vector elements
void print_vector_elements(vint64m1_t v, size_t vl)
{
    int64_t elements[vl];
    vse64_v_i64m1(elements, v, vl);
    printf("[");
    for (size_t i = 0; i < vl; i++)
    {
        printf("%ld", elements[i]);
        if (i < vl - 1)
            printf(", ");
    }
    printf("]\n");
}

// Scalar version of 2D auto-correlation
void auto_correlation_2d_s64b(int64_t *c, int64_t *a, int64_t rows, int64_t cols)
{
    int64_t c_rows = 2 * rows - 1;
    int64_t c_cols = 2 * cols - 1;

    // Initialize result matrix c to zero
    memset(c, 0, c_rows * c_cols * sizeof(int64_t));

    // Compute the 2D auto-correlation
    for (int64_t m = -rows + 1; m < rows; m++)
    {
        for (int64_t n = -cols + 1; n < cols; n++)
        {
            int64_t sum = 0;
            for (int64_t i = 0; i < rows; i++)
            {
                for (int64_t j = 0; j < cols; j++)
                {
                    if ((i + m) >= 0 && (i + m) < rows && (j + n) >= 0 && (j + n) < cols)
                    {
                        sum += a[i * cols + j] * a[(i + m) * cols + (j + n)];
                    }
                }
            }
            c[(m + rows - 1) * c_cols + (n + cols - 1)] = sum;
        }
    }
}

// Vectorized version of 2D auto-correlation
void auto_correlation_2d_v64b(int64_t *c, int64_t *a, int64_t rows, int64_t cols)
{
    int64_t c_rows = 2 * rows - 1;
    int64_t c_cols = 2 * cols - 1;
    size_t vl;

    // Compute the 2D auto-correlation

    for (int64_t m = -rows + 1; m < rows; m++)
    {
        for (int64_t n = 0; n < cols; n++)
        {
            int64_t sum = 0;
            // printf("m = %ld, n = %d\n", m, n);
            for (int64_t i = 0; i < rows; i++)
            {
                for (int64_t j = 0; j < cols; j += vl)
                {
                    if ((i + m) < 0 || (i + m) >= rows)
                        continue;
                    if ((j + n) < 0 || (j + n) >= cols)
                        break;

                    // Set the vector configuration
                    vl = vsetvl_e64m1(cols - (j + n));

                    // Load elements to vector registers
                    vint64m1_t va = vle64_v_i64m1(&a[i * cols + j], vl);
                    vint64m1_t vb = vle64_v_i64m1(&a[(i + m) * cols + (j + n)], vl);

                    // Debug Statements
                    // printf("va : ");
                    // print_vector_elements(va, vl);
                    // printf("vb : ");
                    // print_vector_elements(vb, vl);
                    // printf("\n");

                    // Dot product of va, vb
                    vint64m1_t vmul;
                    asm volatile("vmul.vv %0, %1, %2" : "=vr"(vmul) : "vr"(va), "vr"(vb));
                    vint64m1_t vzero = vmv_v_x_i64m1(0, vl);
                    vint64m1_t vsum;
                    // Adding elements of vmul
                    asm volatile("vredsum.vs %0, %1, %2" : "=vr"(vsum) : "vr"(vmul), "vr"(vzero));
                    int64_t partial_sum;
                    asm volatile("vmv.x.s %0, %1" : "=r"(partial_sum) : "vr"(vsum));
                    sum += partial_sum;
                }
            }
            c[(m + rows - 1) * c_cols + (n + cols - 1)] = sum;
            c[(rows - m - 1) * c_cols + (cols - n - 1)] = sum;
        }
    }
}
